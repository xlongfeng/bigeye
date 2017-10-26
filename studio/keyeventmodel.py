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


from datetime import datetime

from PyQt5.QtCore import pyqtSignal, pyqtSlot, Qt, QAbstractListModel, QModelIndex, QVariant

from orm import *


class KeyEventModel(QAbstractListModel):
    IdRole = Qt.UserRole + 1
    NameRole = Qt.UserRole + 2
    KeyRole = Qt.UserRole + 3
    DownRole = Qt.UserRole + 4
    TimestampRole = Qt.UserRole + 5
    
    _roles = { IdRole: b"identity", NameRole: b"name",
               KeyRole: b"key", DownRole: b"down", 
               TimestampRole: b"timestamp"}
    
    rowAppended = pyqtSignal()
    
    def __init__(self, parent=None):
        super(KeyEventModel, self).__init__(parent)
        
        self._keyEvents = []
    
    def append(self, name, code, down, timestamp, test_case_id):
        keyevent = KeyEvent(name=name, code=code, down=down, timestamp=timestamp, test_case_id=test_case_id)
        session.add(keyevent)
        session.commit()
        self.beginInsertRows(QModelIndex(), self.rowCount(), self.rowCount())
        self._keyEvents.append(keyevent)
        self.endInsertRows()
        self.rowAppended.emit()

    @pyqtSlot()
    def clear(self):
        self.removeRows(0, self.rowCount())

    def select(self, test_case_id):
        self.beginResetModel()
        self._keyEvents.clear()
        for instance in session.query(KeyEvent).filter(KeyEvent.test_case_id == test_case_id):
            self._keyEvents.append(instance)
        self.endResetModel()
    
    def removeRows(self, row, count, parent=QModelIndex()):
        first = row
        last = row+count
        self.beginRemoveRows(parent, first, last-1)
        del self._keyEvents[first:last]
        self.endRemoveRows()
    
    def rowCount(self, parent=QModelIndex()):
        return len(self._keyEvents)
    
    def data(self, index, role=Qt.DisplayRole):
        try:
            keyevent = self._keyEvents[index.row()]
        except IndexError:
            return QVariant()
            
        if role == self.IdRole:
            return keyevent.id
        if role == self.NameRole:
            return keyevent.name
        if role == self.KeyRole:
            return keyevent.key
        if role == self.DownRole:
            return keyevent.down
        if role == self.TimestampRole:
            return keyevent.timestamp
        
        return QVariant()
    
    def roleNames(self):
        return self._roles
