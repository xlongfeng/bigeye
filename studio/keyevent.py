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

from repeater import *


class KeyEvent(RepeaterDelegate):
    ConnectStatus = Enum('Status', "disconnected connected")
    _status = ConnectStatus.disconnected

    def __init__(self, parent=None):
        super(KeyEvent, self).__init__(parent)

    def open(self):
        if self._status is self.ConnectStatus.connected:
            return
        self._status = self.ConnectStatus.connected
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('keyEventOpen')
        self._repeater.submitRequestBlock(block)

    def close(self):
        if self._status is self.ConnectStatus.disconnected:
            return
        self._status = self.ConnectStatus.disconnected
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('keyEventClose')
        self._repeater.submitRequestBlock(block)

    def report(self, code, down):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('keyEvent')
        ostream.writeInt32(code)
        ostream.writeBool(down)
        self._repeater.submitRequestBlock(block)