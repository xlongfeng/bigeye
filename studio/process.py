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


from repeater import *


class Process(RepeaterDelegate):
    def __init__(self, parent=None):
        super(Process, self).__init__(parent)

    def execute(self, program, arguments=[], detached=False):
        block, ostream = self._repeater.getRequestBlock()
        if detached:
            ostream.writeQString('executeProgramDetached')
        else:
            ostream.writeQString('executeProgram')
        ostream.writeQString(program)
        ostream.writeQStringList(arguments)
        self._repeater.submitRequestBlock(block)