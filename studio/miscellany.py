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
    def __init__(self, parent=None):
        super(MiscellanyRepeater, self).__init__(parent)

    def setBootOption(self, name, value=None):
        print(name, value)


class Miscellany(QObject):

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

    @pyqtProperty(list)
    def screenResolutionOptions(self):
        return ["800x480", "800x600", "1024x768", "1280x800", "1280x1024"]
    
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
        self.consoleIndexChanged.connect(self.onConsoleIndexChanged)
        self.screenResolutionIndexChanged.connect(self.onScreenResolutionChanged)

    @pyqtSlot()
    def onConsoleIndexChanged(self):
        if self.consoleIndex == 0:
            MiscellanyRepeater.instance().setBootOption("console", "null")
        elif self.consoleIndex == 1:
            MiscellanyRepeater.instance().setBootOption("console", "ttymxc0,115200")
        elif self.consoleIndex == 2:
            MiscellanyRepeater.instance().setBootOption("console", "ttyUSB0,115200")
        else:
            print("Wrong console option")

    @pyqtSlot()
    def onScreenResolutionChanged(self):
        if self.screenResolutionIndex == 0:
            MiscellanyRepeater.instance().setBootOption("video", "null")
        elif self.screenResolutionIndex == 1:
            MiscellanyRepeater.instance().setBootOption("video", "ttymxc0,115200")
        elif self.screenResolutionIndex == 2:
            MiscellanyRepeater.instance().setBootOption("video", "ttyUSB0,115200")
        elif self.screenResolutionIndex == 3:
            MiscellanyRepeater.instance().setBootOption("video", "ttyUSB0,115200")
        elif self.screenResolutionIndex == 4:
            MiscellanyRepeater.instance().setBootOption("video", "ttyUSB0,115200")
        else:
            print("Wrong console option")

    @pyqtSlot()
    def reboot(self):
        Process.instance().execute('reboot')

    @pyqtSlot()
    def poweroff(self):
        Process.instance().execute('poweroff')