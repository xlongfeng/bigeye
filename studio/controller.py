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


from datetime import datetime
from random import choice
import usb1
import cv2
import numpy as np

from PyQt5.QtCore import pyqtProperty, pyqtSlot, QObject, QTimer

from orm import *
from keyeventmodel import *
from repeater import Repeater
from snapshot import *
from video import *


class Controller(QObject):
    modelChanged = pyqtSignal()

    @pyqtProperty(KeyEventModel, notify=modelChanged)
    def model(self):
        return self._model
    
    @model.setter
    def model(self, model):
        self._model = model
        self.modelChanged.emit()

    previewChanged = pyqtSignal()

    @pyqtProperty(str, notify=previewChanged)
    def preview(self):
        return self._preview

    @preview.setter
    def preview(self, source):
        self._preview = source
        self.previewChanged.emit()
    
    def __init__(self, parent=None):
        super(QObject, self).__init__(parent)
        self._model = None
        self._preview = None
        self._testCaseId = None
        self._startTime = None

        self._automaticKeys = []
        self._automaticMode = False
        self._automaticTimer = QTimer(self)
        self._automaticTimer.setSingleShot(True)
        self._automaticTimer.timeout.connect(self.automaticKeyReport)
        self._automaticNextKey = None
        self._pressRange = None
        self._releaseRange = None

        self._repeater = Repeater.instance()

        self._videoRecorder = VideoRecorder.instance()
        self._videoRecorder.frameChanged.connect(self.onPreviewChanged)

        self.model = KeyEventModel(self)
    
    @pyqtSlot(int, int)
    def setPressRange(self, start, stop):
        self._pressRange = range(start, stop + 1)
    
    @pyqtSlot(int, int)
    def setReleaseRange(self, start, stop):
        self._releaseRange = range(start, stop + 1)

    @pyqtSlot()
    def clearAutomaticKeys(self):
        self._automaticKeys.clear()

    @pyqtSlot(str, int)
    def appendAutomaticKey(self, name, code):
        self._automaticKeys.append(dict(name=name, code=code))
    
    @pyqtSlot(bool)
    def setAutomation(self, enabled):
        self._automaticMode = enabled
    
    @pyqtSlot(str)
    def start(self, name):
        testcase = TestCase(name=name, timestamp=datetime.now())
        session.add(testcase)
        session.commit()
        self._testCaseId = testcase.id
        self._startTime = datetime.now()
        self._model.clear()

        self._videoRecorder.setFrameRate(10)
        self._videoRecorder.setResolution(800, 600)
        self._videoRecorder.setFilename(str(self._testCaseId))
        self._videoRecorder.start()

        if self._automaticMode:
            self._automaticNextKey = None
            self._automaticTimer.start(choice(self._pressRange) * 1000)
    
    @pyqtSlot()
    def stop(self):
        self._videoRecorder.stop()
        if self._automaticMode:
            self._automaticMode = False
            self._automaticTimer.stop()

            if self._automaticNextKey is not None:
                self.report(self._automaticNextKey['name'], self._automaticNextKey['code'], False)
                self._automaticNextKey = None

    @pyqtSlot()
    def automaticKeyReport(self):
        if self._automaticNextKey is None:
            self._automaticNextKey = choice(self._automaticKeys)
            self.report(self._automaticNextKey['name'], self._automaticNextKey['code'], True)
            self._automaticTimer.start(choice(self._releaseRange))
        else:
            self.report(self._automaticNextKey['name'], self._automaticNextKey['code'], False)
            self._automaticNextKey = None
            self._automaticTimer.start(choice(self._pressRange) * 1000)

    @pyqtSlot(str, int, bool)
    def report(self, name, code, down):
        delta = datetime.now() - self._startTime
        timestamp = delta.days * 24 * 60 * 60 * 1000
        timestamp += delta.seconds * 1000
        timestamp += round(delta.microseconds / 1000)
        self._model.append(name, code, down, timestamp, self._testCaseId)
        self._repeater.reqKeyEvent(code, down)
        if not down:
            self._repeater.reqExecuteProgram("date")
            self._repeater.reqExecuteProgram("date", ["-R"], True)
            self._repeater.reqFileTransfer("bigeye.py", "/tmp/bigeye.py", True)
            self._repeater.reqFileTransfer("repeater", "repeater")

    @pyqtSlot()
    def onPreviewChanged(self):
        self.preview = "image://video/timestamp=" + str(datetime.now().timestamp())
