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

    def __init__(self, parent=None):
        super(FishboneConnector, self).__init__(parent)
        self.onStatusChanged()
        self._repeater.statusChanged.connect(self.onStatusChanged)

    @pyqtSlot()
    def onStatusChanged(self):
        self.status = self.ConnectStatus.connecting if self._repeater.isConnected() else self.ConnectStatus.disconnected
        print("repeater connected status", self._repeater.isConnected())

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
        super(QObject, self).__init__(parent)
        self._connector = FishboneConnector.instance()
        self.status = self._connector.status.value
        self._connector.statusChanged.connect(self.onStatusChanged)

    @pyqtSlot(int)
    def onStatusChanged(self, status):
        self.status = status
        print("daemon connected status", status)