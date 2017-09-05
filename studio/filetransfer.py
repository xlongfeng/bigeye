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


from repeater import *


class FileTransfer(RepeaterDelegate):
    def __init__(self, parent=None):
        super(FileTransfer, self).__init__(parent)

    def put(self, src, dst):
        buffer = None
        with open(src, "rb") as f:
            buffer = f.read()
        if buffer is not None:
            block, ostream = self._repeater.getRequestBlock()
            ostream.writeQString('fileTransferPut')
            ostream.writeQString(src)
            ostream.writeQString(dst)
            ostream.writeBytes(buffer)
            self._repeater.submitRequestBlock(block)

    def get(self, src, dst):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('fileTransferGet')
        ostream.writeQString(src)
        ostream.writeQString(dst)
        self._repeater.submitRequestBlock(block)

    def respFileTransferGet(self, istream):
        src = istream.readQString()
        dst = istream.readQString()
        data = istream.readBytes()
        with open(dst, "wb") as f:
            f.write(data)
