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
from sqlalchemy import desc

from PyQt5.QtCore import pyqtSlot, Qt, QAbstractListModel, QModelIndex, QVariant

from orm import *


class TestCaseModel(QAbstractListModel):
    IdRole = Qt.UserRole + 1
    NameRole = Qt.UserRole + 2
    TimestampRole = Qt.UserRole + 3
    
    _roles = { IdRole: b"id", NameRole: b"name", TimestampRole: b"timestamp"}
    
    def __init__(self, parent=None):
        super(QAbstractListModel, self).__init__(parent)
        
        self._testCases = []
        self.select()
    
    @pyqtSlot()
    def select(self):
        self.beginResetModel()
        self._testCases.clear()
        for instance in session.query(TestCase).order_by(desc(TestCase.id)):
            self._testCases.append(instance)
        self.endResetModel()
    
    def rowCount(self, parent=QModelIndex()):
        return len(self._testCases)
    
    def data(self, index, role=Qt.DisplayRole):
        try:
            testcase = self._testCases[index.row()]
        except IndexError:
            return QVariant()
            
        if role == self.IdRole:
            return testcase.id
        if role == self.NameRole:
            return testcase.name
        if role == self.TimestampRole:
            return testcase.timestamp.isoformat()
        
        return QVariant()
    
    def roleNames(self):
        return self._roles