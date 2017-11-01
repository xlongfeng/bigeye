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


from PyQt5.QtCore import pyqtProperty, pyqtSlot, QObject

from repeater import *
from process import *


class MiscellanyRepeater(RepeaterDelegate):
    optionArrived = pyqtSignal(str, str)

    def __init__(self, parent=None):
        super(MiscellanyRepeater, self).__init__(parent)

    def setOption(self, name, value=None):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('setOption')
        ostream.writeQString(name)
        ostream.writeQString(value)
        self._repeater.submitRequestBlock(block)

    def getOption(self, name):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('getOption')
        ostream.writeQString(name)
        self._repeater.submitRequestBlock(block)

    def respGetOption(self, istream):
        name = istream.readQString()
        value = istream.readQString()
        self.optionArrived.emit(name, value)


class Miscellany(QObject):

    _consoleOptionValues = [
        "null",
        "ttymxc0,115200",
        "ttyUSB0,115200"
    ]

    @pyqtProperty(list)
    def consoleOptions(self):
        return ["Close", "TTL to RS232", "USB to RS232"]

    _consoleIndex = 0
    consoleIndexChanged = pyqtSignal()

    @pyqtProperty(int, notify=consoleIndexChanged)
    def consoleIndex(self):
        return self._consoleIndex

    @consoleIndex.setter
    def consoleIndex(self, model):
        self._consoleIndex = model
        self.consoleIndexChanged.emit()

    _screenResolutionOptionValues = [
        "mxcfb0:dev=ldb,SEIKO-WVGA,if=RGB24 video=mxcfb1:dev=hdmi,1280x1024M@60,if=RGB24",
        "mxcfb0:dev=ldb,LDB-SVGA,if=RGB666",
        "mxcfb0:dev=ldb,LDB-XGA,if=RGB666",
        "mxcfb0:dev=ldb,LDB-WXGA,if=RGB24 video=mxcfb1:dev=hdmi,1280x1024M@60,if=RGB24",
        "mxcfb0:dev=ldb,LDB-SXGA,if=RGB666"
    ]

    @pyqtProperty(list)
    def screenResolutionOptions(self):
        return ["800x480 (P7)", "800x600 (G60)", "1024x768 (G70)", "1280x800 (P10)", "1280x1024 (G80)"]
    
    _screenResolutionIndex = 0
    screenResolutionIndexChanged = pyqtSignal()

    @pyqtProperty(int, notify=screenResolutionIndexChanged)
    def screenResolutionIndex(self):
        return self._screenResolutionIndex

    @screenResolutionIndex.setter
    def screenResolutionIndex(self, model):
        self._screenResolutionIndex = model
        self.screenResolutionIndexChanged.emit()

    def __init__(self, parent=None):
        super(Miscellany, self).__init__(parent)
        self._miscellanyRepeater = MiscellanyRepeater.instance()
        self.consoleIndexChanged.connect(self.onConsoleIndexChanged)
        self.screenResolutionIndexChanged.connect(self.onScreenResolutionChanged)

        self._miscellanyRepeater.optionArrived.connect(self.onOptionArrived)
        self._miscellanyRepeater.getOption("console")
        self._miscellanyRepeater.getOption("video")

    def onOptionArrived(self, name, value):
        print("option arrived", name, value)
        if name == "console":
            self.consoleIndex = self.getConsoleOptionIndex(value)
        elif name == "video":
            self.screenResolutionIndex = self.getScreenResolutionIndex(value)
        else:
            print("Unhandled option", name, value)

    def getConsoleOptionValue(self, index):
        return self._consoleOptionValues[index]

    def getConsoleOptionIndex(self, value):
        if value not in self._consoleOptionValues:
            return -1
        return self._consoleOptionValues.index(value)

    def getScreenResolutionValue(self, index):
        return self._screenResolutionOptionValues[index]

    def getScreenResolutionIndex(self, value):
        if value not in self._screenResolutionOptionValues:
            return -1
        return self._screenResolutionOptionValues.index(value)

    @pyqtSlot()
    def onConsoleIndexChanged(self):
        self._miscellanyRepeater.setOption("console", self.getConsoleOptionValue(self.consoleIndex))

    @pyqtSlot()
    def onScreenResolutionChanged(self):
        self._miscellanyRepeater.setOption("video", self.getScreenResolutionValue(self.screenResolutionIndex))

    @pyqtSlot()
    def unitTest(self):
        Process.instance().executeRemote("apps/unittest", detached=True, preCommand="killall unittest")

    @pyqtSlot()
    def reboot(self):
        Process.instance().execute('reboot')

    @pyqtSlot()
    def poweroff(self):
        Process.instance().execute('poweroff')