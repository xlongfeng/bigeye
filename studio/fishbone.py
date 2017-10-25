#!/usr/bin/env python
# -*- coding: utf-8 -*-


#############################################################################
#
# Bigeye - Accessorial Tool for Daily Test
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

from PyQt5.QtCore import pyqtProperty, pyqtSignal, QObject, QTimer

from repeater import *
from keyevent import *


class FishboneConnector(RepeaterDelegate):
    ConnectStatus = Enum('Status', "disconnected connecting connected")
    _status = ConnectStatus.disconnected

    statusChanged = pyqtSignal(int)

    @pyqtProperty(ConnectStatus, notify=statusChanged)
    def status(self):
        return self._status

    @status.setter
    def status(self, status):
        self._status = status
        self.statusChanged.emit(status.value)

    _program = None
    _device = None
    _screenWidth = 0
    _screenHeight = 0
    _screenBitDepth = 0

    @pyqtProperty(int)
    def screenWidth(self):
        return self._screenWidth

    @screenWidth.setter
    def screenWidth(self, value):
        self._screenWidth = value

    @pyqtProperty(int)
    def screenHeight(self):
        return self._screenHeight

    @screenHeight.setter
    def screenHeight(self, value):
        self._screenHeight = value

    @pyqtProperty(int)
    def screenBitDepth(self):
        return self._screenBitDepth

    @screenBitDepth.setter
    def screenBitDepth(self, value):
        self._screenBitDepth = value

    deviceChanged = pyqtSignal()

    @pyqtProperty(str, notify=deviceChanged)
    def device(self):
        return self._device

    @device.setter
    def device(self, device):
        if self._device != device:
            self._device = device
            self.deviceChanged.emit()

    def __init__(self, parent=None):
        super(FishboneConnector, self).__init__(parent)
        self._queryDeviceTimer = QTimer(self)
        self._queryDeviceTimer.setSingleShot(True)
        self._queryDeviceTimer.timeout.connect(self.onQueryDevice)
        self._queryDeviceTimeoutTimer = QTimer(self)
        self._queryDeviceTimeoutTimer.setSingleShot(True)
        self._queryDeviceTimeoutTimer.timeout.connect(self.onQeuryDeviceTimeout)
        self._launchDaemonTimeoutTimer = QTimer(self)
        self._launchDaemonTimeoutTimer.setSingleShot(True)
        self._launchDaemonTimeoutTimer.timeout.connect(self.launchDaemonFailed)
        self.onRepeaterStatusChanged()
        self._repeater.statusChanged.connect(self.onRepeaterStatusChanged)

    @pyqtSlot()
    def onRepeaterStatusChanged(self):
        self.status = self.ConnectStatus.connecting if self._repeater.isConnected() else self.ConnectStatus.disconnected
        if self.status == self.ConnectStatus.connecting:
            self._queryDeviceTimer.start(1000)
        elif self.status == self.ConnectStatus.disconnected:
            self._queryDeviceTimer.stop()
            self._queryDeviceTimeoutTimer.stop()

    @pyqtSlot()
    def onQueryDevice(self):
        self._queryDeviceTimeoutTimer.start(1000)
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('queryDevice')
        self._repeater.submitRequestBlock(block)

    def respQueryDevice(self, istream):
        self._queryDeviceTimeoutTimer.stop()
        self.program = istream.readQString()
        if self.program != 'daemon':
            self.startDaemon()
        else:
            self._launchDaemonTimeoutTimer.stop()
            version = istream.readQString()
            self.device = istream.readQString()
            self.screenWidth = istream.readInt()
            self.screenHeight = istream.readInt()
            self.screenBitDepth = istream.readInt()
            self.status = self.ConnectStatus.connected
            self._queryDeviceTimer.start()

    @pyqtSlot()
    def onQeuryDeviceTimeout(self):
        self.status = self.ConnectStatus.connecting
        self._queryDeviceTimer.start()

    def startDaemon(self):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('startDaemon')
        with open("bigeye-daemon", "rb") as f:
            ostream.writeBytes(f.read())
        self._repeater.submitRequestBlock(block)
        # start a start daemon timeout timer to detect daemon launched failed
        self._launchDaemonTimeoutTimer.start(3000)
        self._queryDeviceTimer.start()

    @pyqtSlot()
    def launchDaemonFailed(self):
        # what can I do with this situation
        print("daemon launched failed")

    def stopDaemon(self):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('stopDaemon')
        self._repeater.submitRequestBlock(block)

class Fishbone(QObject):
    _status = 0

    statusChanged = pyqtSignal()

    @pyqtProperty(int, notify=statusChanged)
    def status(self):
        return self._status

    @status.setter
    def status(self, status):
        self._status = status
        self.statusChanged.emit()

    def __init__(self, parent=None):
        super(Fishbone, self).__init__(parent)
        self._connector = FishboneConnector.instance()
        self.status = self._connector.status.value
        self._connector.statusChanged.connect(self.onStatusChanged)

    @pyqtSlot(int)
    def onStatusChanged(self, status):
        self.status = status
        if status == 3:
            KeyEvent.instance().open()
        else:
            KeyEvent.instance().close()