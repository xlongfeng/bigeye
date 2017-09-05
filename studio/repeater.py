#!/usr/bin/env python
# -*- coding: utf-8 -*-


#############################################################################
#
# Bigeye - Accessorial Tool of Daily Test
# Copyright (C) 2017 xlongfeng <xlongfeng@126.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################


from enum import Enum
import usb1

from PyQt5.QtCore import (pyqtProperty, pyqtSignal, pyqtSlot,
                          QTimer, QThread, QIODevice, QByteArray,
                          QDataStream, QMutex, qCompress, qUncompress)
from PyQt5.QtGui import QImage

from singletonobject import *


class _RepeaterLoop(QThread):
    def __init__(self, parent=None):
        super(_RepeaterLoop, self).__init__(parent)

    def run(self):
        context = self.parent().getUSBContext()
        transfer_list = self.parent().getTransferList()

        while any(x.isSubmitted() for x in transfer_list):
            try:
                context.handleEvents()
            except usb1.USBErrorInterrupted:
                print(" exception: usb error interrupted")


class Repeater(SingletonObject):
    dataArrived = pyqtSignal(bytearray, name='dataArrived')
    snapshotArrived = pyqtSignal(QImage)
    videoFrameArrived = pyqtSignal(QImage)

    _delegates = []

    _loop = None
    _context = None
    _handle = None
    _device = None
    _in_ep = None
    _out_ep = None
    _in_packet_size = 0
    _out_packet_size = 0
    _transfer_list = []
    _mutex = QMutex()
    _datagram = QByteArray()

    _extendedDataBuffer = QByteArray()
    _extendedDataCategory = None
    _extendedDataCompressed = False
    _extendedDataSize = 0

    def __init__(self, parent=None):
        super(Repeater, self).__init__(parent)
        context = usb1.USBContext()
        context.open()
        self._context = context
        self._loop = _RepeaterLoop(self)
        handle = context.openByVendorIDAndProductID(
            0x2009,
            0x0805,
            skip_on_error=True,
        )
        if handle is not None:
            self._handle = handle
            device = handle.getDevice()
            self._device = device
            for endpoint in device[0][0][0]:
                address = endpoint.getAddress()
                if address & usb1.ENDPOINT_DIR_MASK == usb1.ENDPOINT_IN:
                    self._in_ep = address
                    self._in_packet_size = endpoint.getMaxPacketSize()
                else:
                    self._out_ep = address
                    self._out_packet_size = endpoint.getMaxPacketSize()
            self._interface = handle.claimInterface(0)
            self.startReceive()

    def register(self, delegate):
        self._delegates.append(delegate)

    def getRequestBlock(self):
        block = QByteArray()
        ostream = QDataStream(block, QIODevice.WriteOnly)
        ostream.setVersion(QDataStream.Qt_4_8)
        ostream.writeQString('Bigeye')
        return block, ostream

    def submitRequestBlock(self, block):
        block = self._escape(block)
        transfer = self._handle.getTransfer()
        transfer.setBulk(self._out_ep, block, callback=self.sendDataCallback, user_data=self)
        transfer.submit()
        self._transfer_list.append(transfer)
        self._loop.start()

    def getUSBContext(self):
        return self._context

    def getUSBHandle(self):
        return self._handle

    def getTransferList(self):
        return self._transfer_list

    def onDataArrived(self, data):
        self._mutex.lock()
        start = 0
        while True:
            if self._extendedDataSize > 0:
                size = self._extendedDataBuffer.size()
                remainder = self._extendedDataSize - size
                length = len(data) - start
                if length >= remainder:
                    self._extendedDataBuffer.append(data[start:(start + length)])
                    start += (length + 1)
                    if self._extendedDataCompressed:
                        self._extendedDataBuffer = qUncompress(self._extendedDataBuffer)
                    self.onDisposedExtendedData()
                    self._extendedDataSize = 0
                else:
                    self._extendedDataBuffer.append(data[start:])
                    break
            else:
                end = data.find(b'\x7e', start)
                if end == 0:
                    self.onDisposed(self._datagram)
                    self._datagram.clear()
                elif end > 0:
                    self._datagram.append(data[start:end])
                    self.onDisposed(self._datagram)
                    self._datagram.clear()
                else:
                    self._datagram.append(data[start:])
                    break

                if end == len(data) - 1:
                    break
                else:
                    start = end + 1
        self._mutex.unlock()

    def onDisposed(self, datagram):
        if datagram.size() == 0:
            return

        block = self._unescape(datagram)
        istream = QDataStream(block)
        istream.setVersion(QDataStream.Qt_4_8)
        magic = istream.readQString()
        response = istream.readQString()
        if istream.status() == QDataStream.Ok:
            if response == "extendedData":
                self._extendedDataCategory = istream.readQString()
                self._extendedDataCompressed = istream.readBool()
                self._extendedDataSize = istream.readInt()
                self._extendedDataBuffer.clear()
            else:
                handled = False
                for delegate in self._delegates:
                    if hasattr(delegate, response):
                        getattr(delegate, response)(istream)
                        handled = True
                        if istream.status() != QDataStream.Ok:
                            print("eeeeeeeeeeebbbbbbbeeeeeeeeeeeeee")
                        break
                if not handled:
                    print("Unhandled response:", response)
        else:
            print("onDisposed eeeeeeeeeeeeeee")

    def onDisposedExtendedData(self):
        for delegate in self._delegates:
            extendedDataHandel = "onExtendedDataArrived"
            handled = False
            if hasattr(delegate, extendedDataHandel):
                res = getattr(delegate, extendedDataHandel)(self._extendedDataCategory, self._extendedDataBuffer)
                if res is True:
                    handled = True
                    break
        if not handled:
            print("Unhandled extended data:", self._extendedDataCategory)

    def receiveData(self, data):
        self.dataArrived.emit(data)

    @staticmethod
    def receiveDataCallback(transfer):
        repeater = transfer.getUserData()
        status = transfer.getStatus()
        if status == usb1.TRANSFER_COMPLETED:
            repeater.receiveData(transfer.getBuffer()[:transfer.getActualLength()])
            transfer.submit()
        elif status == usb1.TRANSFER_ERROR or status == usb1.TRANSFER_TIMED_OUT:
            transfer.submit()
        elif status == usb1.TRANSFER_CANCELLED:
            pass
        elif status == usb1.TRANSFER_STALL:
            repeater.getUSBHandle().clearHalt(transfer.getEndpoint())
            transfer.submit()
        elif status == usb1.TRANSFER_NO_DEVICE:
            pass
        elif status == usb1.TRANSFER_OVERFLOW:
            pass

    def startReceive(self):
        self.dataArrived.connect(self.onDataArrived)
        transfer = self._handle.getTransfer()
        transfer.setBulk(self._in_ep, self._in_packet_size * 32, callback=self.receiveDataCallback, user_data=self)
        transfer.submit()
        self._transfer_list.append(transfer)
        self._loop.start()

    @staticmethod
    def sendDataCallback(transfer):
        repeater = transfer.getUserData()
        status = transfer.getStatus()
        if status == usb1.TRANSFER_COMPLETED:
            print("the transfer need to be free")
        elif status == usb1.TRANSFER_ERROR or status == usb1.TRANSFER_TIMED_OUT:
            transfer.submit()
        elif status == usb1.TRANSFER_CANCELLED:
            pass
        elif status == usb1.TRANSFER_STALL:
            repeater.getUSBHandle().clearHalt(transfer.getEndpoint())
            transfer.submit()
        elif status == usb1.TRANSFER_NO_DEVICE:
            pass
        elif status == usb1.TRANSFER_OVERFLOW:
            pass

    def reqDaemonVersion(self):
        block = QByteArray()
        ostream = QDataStream(block, QIODevice.WriteOnly)
        ostream.setVersion(QDataStream.Qt_4_8)
        ostream.writeQString('Bigeye')
        ostream.writeQString('daemonVersion')

        block = self._escape(block)
        transfer = self._handle.getTransfer()
        transfer.setBulk(self._out_ep, block, callback=self.sendDataCallback, user_data=self)
        transfer.submit()
        self._transfer_list.append(transfer)
        self._loop.start()

    def startDaemon(self):
        block = QByteArray()
        ostream = QDataStream(block, QIODevice.WriteOnly)
        ostream.setVersion(QDataStream.Qt_4_8)
        ostream.writeQString('Bigeye')
        ostream.writeQString('startDaemon')
        with open("bigeyeDaemon", "rb") as f:
            ostream.writeBytes(f.read())

        block = self._escape(block)
        transfer = self._handle.getTransfer()
        transfer.setBulk(self._out_ep, block, callback=self.sendDataCallback, user_data=self)
        transfer.submit()
        self._transfer_list.append(transfer)
        self._loop.start()

    def stopDaemon(self):
        block = QByteArray()
        ostream = QDataStream(block, QIODevice.WriteOnly)
        ostream.setVersion(QDataStream.Qt_4_8)
        ostream.writeQString('Bigeye')
        ostream.writeQString('stopDaemon')

        block = self._escape(block)
        transfer = self._handle.getTransfer()
        transfer.setBulk(self._out_ep, block, callback=self.sendDataCallback, user_data=self)
        transfer.submit()
        self._transfer_list.append(transfer)
        self._loop.start()

    @staticmethod
    def _escape(data):
        escape = QByteArray()
        escape.append(chr(0x7e))
        """
        for i in range(data.size()):
            ch = data.at(i)
            if ch == chr(0x7e):
                escape.append(chr(0x7d))
                escape.append(chr(0x5e))
            elif ch == chr(0x7d):
                escape.append(chr(0x7d))
                escape.append(chr(0x5d))
            else:
                escape.append(ch)
        """
        data.replace(b"\x7d", b"\x7d\x5d")
        data.replace(b"\x7e", b"\x7d\x5e")
        escape.append(data)
        escape.append(chr(0x7e))
        return escape

    @staticmethod
    def _unescape(data):
        """
        unescape = QByteArray()
        is_escaped = False
        for i in range(data.size()):
            ch = data.at(i)
            if is_escaped:
                is_escaped = False
                if ch == chr(0x5e):
                    unescape.append(chr(0x7e))
                elif ch == chr(0x5d):
                    unescape.append(chr(0x7d))
                else:
                    print("data corrupted")
                continue
            if ch == chr(0x7d):
                is_escaped = True
                continue
            else:
                unescape.append(ch)
        return unescape
        """
        data.replace(b"\x7d\x5e", b"\x7e")
        data.replace(b"\x7d\x5d", b"\x7d")
        return data

    def _dumpUSBInfo(self):
        handle = self._handle
        print("bConfigurationValue ", handle.getConfiguration())

        device = handle.getDevice()
        print("Device:")
        print("Bus Number ", device.getBusNumber())
        print("Port Number ", device.getPortNumber())
        print("Device Address ", device.getDeviceAddress())
        print("Device Speed ", device.getDeviceSpeed())
        print("Max Packet Size0 ", device.getMaxPacketSize0())
        print("Vendor ID ", device.getVendorID())
        print("Product ID ", device.getProductID())
        print("Manufacturer ", device.getManufacturer())
        print("Product ", device.getProduct())
        print("Serial Number ", device.getSerialNumber())
        print("Num Configurations ", device.getNumConfigurations())
        print("Configurations:")
        for configuration in device:
            print(' ' * 2, " Configuration Value ", configuration.getConfigurationValue())
            print(' ' * 2, " Descriptor ", configuration.getDescriptor())
            print(' ' * 2, " Interfaces:")
            for interface in configuration:
                print(' ' * 4, "NumSettings ", interface.getNumSettings())
                print(' ' * 4, "Interface Settings:")
                for interfaceSetting in interface:
                    print(' ' * 6, "Number ", interfaceSetting.getNumber())
                    print(' ' * 6, "Endpoints:")
                    for endpoint in interfaceSetting:
                        print(' ' * 8, "Address ", endpoint.getAddress())
                        print(' ' * 8, "Attributes ", endpoint.getAttributes())
                        print(' ' * 8, "Max Packet Size ", endpoint.getMaxPacketSize())


class RepeaterDelegate(SingletonObject):
    def __init__(self, parent=None):
        super(RepeaterDelegate, self).__init__(parent)
        self._repeater = Repeater.instance()
        self._repeater.register(self)