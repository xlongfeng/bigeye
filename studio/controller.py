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

    _screenWidth = 0
    _screenHeight = 0

    @pyqtProperty(int)
    def screenWidth(self):
        return self._screenWidth

    @pyqtProperty(int)
    def screenHeight(self):
        return self._screenHeight

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
        self._screenWidth = self._fishboneConnector.screenWidth
        self._screenHeight = self._fishboneConnector.screenHeight

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
        self._videoRecorder.setFilename("data/videos/" + str(self._testCaseId))
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
    _replayModel = None
    replayModelChanged = pyqtSignal()

    @pyqtProperty(KeyEventModel, notify=replayModelChanged)
    def replayModel(self):
        return self._replayModel

    @replayModel.setter
    def replayModel(self, model):
        self._replayModel = model
        self.replayModelChanged.emit()

    _replayTestCase = None
    _replayKeyEvents = None

    _replayKeyEventIndex = -1
    replayKeyEventIndexChanged = pyqtSignal()

    @pyqtProperty(int, notify=replayKeyEventIndexChanged)
    def replayKeyEventIndex(self):
        return self._replayKeyEventIndex

    @replayKeyEventIndex.setter
    def replayKeyEventIndex(self, index):
        self._replayKeyEventIndex = index
        self.replayKeyEventIndexChanged.emit()

    _replayTestCaseId = None
    replayTestCaseIdChanged = pyqtSignal()

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

    _repeatTimes = 0
    repeatTimesChanged = pyqtSignal()

    @pyqtProperty(int, notify=repeatTimesChanged)
    def repeatTimes(self):
        return self._repeatTimes

    @repeatTimes.setter
    def repeatTimes(self, value):
        self._repeatTimes = value
        self.repeatTimesChanged.emit()

    _intervalTime = 0
    intervalTimeChanged = pyqtSignal()

    @pyqtProperty(int, notify=intervalTimeChanged)
    def intervalTime(self):
        return self._intervalTime

    @intervalTime.setter
    def intervalTime(self, value):
        self._intervalTime = value
        self.intervalTimeChanged.emit()

    _loopCounter = 0
    loopCounterChanged = pyqtSignal()

    @pyqtProperty(int, notify=loopCounterChanged)
    def loopCounter(self):
        return self._loopCounter

    @loopCounter.setter
    def loopCounter(self, value):
        self._loopCounter = value
        self.loopCounterChanged.emit()

    _rebootOption = False
    rebootOptionChanged = pyqtSignal()

    @pyqtProperty(bool, notify=rebootOptionChanged)
    def rebootOption(self):
        return self._rebootOption

    @rebootOption.setter
    def rebootOption(self, value):
        self._rebootOption = value
        self.rebootOptionChanged.emit()
        
    _restoreOption = False
    restoreOptionChanged = pyqtSignal()

    @pyqtProperty(bool, notify=restoreOptionChanged)
    def restoreOption(self):
        return self._restoreOption

    @restoreOption.setter
    def restoreOption(self, value):
        self._restoreOption = value
        self.restoreOptionChanged.emit()

    def __init__(self, parent=None):
        super(ReplayController, self).__init__(parent)
        self._replayModel = KeyEventModel(self)
        self.replayTestCaseIdChanged.connect(self.onReplayTestCaseIdChanged)
        self._replayTimer = QTimer(self)
        self._replayTimer.setSingleShot(True)
        self._replayTimer.timeout.connect(self.replayKeyReport)
        self._replayKeyEvent = None

        self._intervalTimer = QTimer(self)
        self._intervalTimer.setSingleShot(True)
        self._intervalTimer.timeout.connect(self.nextCycle)

    @pyqtSlot()
    def onReplayTestCaseIdChanged(self):
        self._replayTestCase = session.query(TestCase).filter(TestCase.id == self.replayTestCaseId).one()
        self._replayKeyEvents = self._replayTestCase.key_event_list
        self.testCaseName = self._replayTestCase.name
        self._replayModel.select(self.replayTestCaseId)

    @pyqtSlot(str, str)
    def start(self, name, category):
        super().start(name, category)
        print(self.repeatTimes, self.intervalTime, self.rebootOption, self.restoreOption)
        self.loopCounter = 1
        self.replayKeyEventIndex = -1
        self.nextReplayKeyReport()

    @pyqtSlot()
    def stop(self):
        super().stop()
        self._replayTimer.stop()

    @pyqtSlot()
    def replayKeyReport(self):
        self.report(self._replayKeyEvent.name, self._replayKeyEvent.code, self._replayKeyEvent.down)
        self.nextReplayKeyReport()

    def nextReplayKeyReport(self):
        self.replayKeyEventIndex += 1
        if self.replayKeyEventIndex == len(self._replayTestCase.key_event_list):
            self.replayKeyEventIndex = -1
            self._replayKeyEvent = None
            if self.intervalTime != self.loopCounter:
                if self.restoreOption:
                    print("restore request")
                if self.rebootOption:
                    print("reboot request")
                self._intervalTimer.start(self.intervalTime * 1000)
            else:
                print("replay is done")
            return
        if self._replayKeyEvent is not None:
            lastKeyEvent = self._replayKeyEvent
            self._replayKeyEvent = self._replayKeyEvents[self.replayKeyEventIndex]
            timestamp = self._replayKeyEvent.timestamp - lastKeyEvent.timestamp
        else:
            self._replayKeyEvent = self._replayKeyEvents[self.replayKeyEventIndex]
            timestamp = self._replayKeyEvent.timestamp
        self._replayTimer.start(timestamp)

    def nextCycle(self):
        # waiting for client connecting
        self.loopCounter += 1
        self.replayKeyEventIndex = -1
        self.nextReplayKeyReport()

