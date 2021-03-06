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


import os
from datetime import datetime
from sqlalchemy import desc

from PyQt5.QtCore import pyqtSlot, Qt, QAbstractListModel, QModelIndex, QVariant

from orm import *


class TestCaseModel(QAbstractListModel):
    IdRole = Qt.UserRole + 1
    NameRole = Qt.UserRole + 2
    CategoryRole = Qt.UserRole + 3
    TimestampRole = Qt.UserRole + 4
    
    _roles = { IdRole: b"identity", NameRole: b"name", CategoryRole: b"category", TimestampRole: b"timestamp"}
    
    def __init__(self, parent=None):
        super(TestCaseModel, self).__init__(parent)
        
        self._testCases = []
    
    @pyqtSlot()
    def select(self):
        self.beginResetModel()
        self._testCases.clear()
        for instance in session.query(TestCase).order_by(desc(TestCase.id)):
            self._testCases.append(instance)
        self.endResetModel()

    @pyqtSlot(int)
    def remove(self, row):
        self.removeRow(row)

    def removeRows(self, row, count, parent=QModelIndex()):
        first = row
        last = row+count
        self.beginRemoveRows(parent, first, last-1)
        for testCase in self._testCases[first:last]:
            session.delete(testCase)
            video = "data/videos/" + str(testCase.id) + ".avi"
            if os.path.exists(video):
                os.remove(video)
        session.commit()
        del self._testCases[first:last]
        self.endRemoveRows()
        return True
    
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
        if role == self.CategoryRole:
            return testcase.category
        if role == self.TimestampRole:
            return testcase.timestamp.strftime("%Y-%m-%d %H:%M:%S")
        
        return QVariant()
    
    def roleNames(self):
        return self._roles