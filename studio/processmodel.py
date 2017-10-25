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


from PyQt5.QtCore import pyqtSlot, Qt, QAbstractListModel, QModelIndex, QVariant, QTimer

from orm import *

from repeater import *


class ProcessModel(QAbstractListModel):
    PidRole = Qt.UserRole + 1
    NameRole = Qt.UserRole + 2
    SmpRole = Qt.UserRole + 3
    CpuRole = Qt.UserRole + 4
    MemoryRole = Qt.UserRole + 5

    _roles = {PidRole: b"pid", NameRole: b"name", SmpRole: b"smp", CpuRole: b"cpu", MemoryRole: b"memory"}

    def __init__(self, parent=None):
        super(ProcessModel, self).__init__(parent)
        self._processes = []
        self._repeater = Repeater.instance()
        self._timer = QTimer()
        self._timer.timeout.connect(self.onUpdate)
        self._timer.start(1000)

        self._processes.append([1000, "/appdir/ut5000", 1, 20, 1000])
        self._processes.append([1001, "Xfbdev", 1, 10, 2000])
        self._processes.append([1002, "top", 1, 20, 3000])
        self._processes.append([1003, "/usr/sbin/inetd", 1, 0, 4000])
        self._processes.append([1004, "klogd -c 7", 1, 0, 5000])
        self._processes.append([1005, "syslogd -l 8 -S -b 20 -O /logdir/m", 1, 0, 6000])

    @pyqtSlot()
    def onUpdate(self):
        pass

    @pyqtSlot()
    def select(self):
        self.beginResetModel()
        self._processes.clear()
        self._processes.append(instance)
        self.endResetModel()

    def rowCount(self, parent=QModelIndex()):
        return len(self._processes)

    def data(self, index, role=Qt.DisplayRole):
        try:
            process = self._processes[index.row()]
        except IndexError:
            return QVariant()

        if self.PidRole <= role <= self.MemoryRole:
            return process[role - self.PidRole]
        """
        if role == self.PidRole:
            return process.id
        if role == self.NameRole:
            return process.name
        if role == self.SmpRole:
            return process.smp
        if role == self.CpuRole:
            return process.cpu
        if role == self.MemoryRole:
            return process.memory
        """

        return QVariant()

    def roleNames(self):
        return self._roles