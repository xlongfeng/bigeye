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
from random import choice

from PyQt5.QtCore import pyqtProperty, pyqtSlot, QObject, QTimer

from fishbone import *
from orm import *
from keyeventmodel import *
from keyevent import *
from process import *
from filetransfer import *
from snapshot import *
from video import *


class Controller(QObject):
    modelChanged = pyqtSignal()
    _model = None

    @pyqtProperty(KeyEventModel, notify=modelChanged)
    def model(self):
        return self._model
    
    @model.setter
    def model(self, model):
        self._model = model
        self.modelChanged.emit()

    previewChanged = pyqtSignal()
    _preview = None

    @pyqtProperty(str, notify=previewChanged)
    def preview(self):
        return self._preview

    @preview.setter
    def preview(self, source):
        self._preview = source
        self.previewChanged.emit()
    
    def __init__(self, parent=None):
        super(Controller, self).__init__(parent)
        self._testCaseId = None
        self._startTime = None

        self._fishboneConnector = FishboneConnector.instance()

        self._keyEvent = KeyEvent.instance()

        self._videoRecorder = VideoRecorder.instance()
        self._videoRecorder.frameChanged.connect(self.onPreviewChanged)

        self.model = KeyEventModel(self)

    @pyqtSlot(str, str)
    def start(self, name, category):
        testcase = TestCase(name=name, category=category, timestamp=datetime.now())
        session.add(testcase)
        session.commit()
        self._testCaseId = testcase.id
        self._startTime = datetime.now()
        self._model.clear()

        self._videoRecorder.setFrameRate(5)
        self._videoRecorder.setResolution(
            self._fishboneConnector.screenWidth,
            self._fishboneConnector.screenHeight)
        self._videoRecorder.setFilename(str(self._testCaseId))
        self._videoRecorder.start()
    
    @pyqtSlot()
    def stop(self):
        self._videoRecorder.stop()

    @pyqtSlot(str, int, bool)
    def report(self, name, code, down):
        delta = datetime.now() - self._startTime
        timestamp = delta.days * 24 * 60 * 60 * 1000
        timestamp += delta.seconds * 1000
        timestamp += round(delta.microseconds / 1000)
        self._model.append(name, code, down, timestamp, self._testCaseId)
        self._keyEvent.report(code, down)

    @pyqtSlot()
    def onPreviewChanged(self):
        self.preview = "image://video/timestamp=" + str(datetime.now().timestamp())


class AutomaticController(Controller):
    def __init__(self, parent=None):
        super(AutomaticController, self).__init__(parent)

        self._automaticKeys = []
        self._automaticTimer = QTimer(self)
        self._automaticTimer.setSingleShot(True)
        self._automaticTimer.timeout.connect(self.automaticKeyReport)
        self._automaticNextKey = None
        self._pressRange = None
        self._releaseRange = None

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

    @pyqtSlot(str, str)
    def start(self, name, category):
        super().start(name, category)
        self._automaticNextKey = None
        self._automaticTimer.start(choice(self._pressRange) * 1000)

    @pyqtSlot()
    def stop(self):
        super().stop()
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


class ReplayController(Controller):
    replayModelChanged = pyqtSignal()
    _replayModel = None

    @pyqtProperty(KeyEventModel, notify=replayModelChanged)
    def replayModel(self):
        return self._replayModel

    @replayModel.setter
    def replayModel(self, model):
        self._replayModel = model
        self.replayModelChanged.emit()

    _replayTestCase = None

    replayTestCaseIdChanged = pyqtSignal()
    _replayTestCaseId = None

    @pyqtProperty(int, notify=replayTestCaseIdChanged)
    def replayTestCaseId(self):
        return self._replayTestCaseId

    @replayTestCaseId.setter
    def replayTestCaseId(self, id):
        self._replayTestCaseId = id
        self.replayTestCaseIdChanged.emit()

    testCaseNameChanged = pyqtSignal()
    _testCaseName = None

    @pyqtProperty(str, notify=testCaseNameChanged)
    def testCaseName(self):
        return self._testCaseName

    @testCaseName.setter
    def testCaseName(self, name):
        self._testCaseName = name
        self.testCaseNameChanged.emit()

    def __init__(self, parent=None):
        super(ReplayController, self).__init__(parent)
        self._replayModel = KeyEventModel(self)
        self.replayTestCaseIdChanged.connect(self.onReplayTestCaseIdChanged)

    @pyqtSlot()
    def onReplayTestCaseIdChanged(self):
        self._replayTestCase = session.query(TestCase).filter(TestCase.id == self.replayTestCaseId).one()
        self.testCaseName = self._replayTestCase.name
        self._replayModel.select(self.replayTestCaseId)
        """
        print(testcase.name, testcase.category, testcase.timestamp)
        for keyevent in testcase.key_event_list:
            print(keyevent.id, keyevent.name, keyevent.code, keyevent.down, keyevent.timestamp)
        """

    @pyqtSlot(str, str)
    def start(self, name, category):
        super().start(name, category)

    @pyqtSlot()
    def stop(self):
        super().stop()