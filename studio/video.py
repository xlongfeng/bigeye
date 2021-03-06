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
import cv2
import numpy as np

from PyQt5.QtCore import Qt, pyqtProperty, pyqtSlot, QTimer, QSize, QStandardPaths
from PyQt5.QtGui import QImage
from PyQt5.QtQuick import QQuickImageProvider

from repeater import *
from fishbone import *


class VideoRecorder(RepeaterDelegate):
    _rate = 0
    _width = 0
    _height = 0

    @pyqtProperty(int)
    def width(self):
        return self._width

    @width.setter
    def width(self, value):
        self._width = value

    @pyqtProperty(int)
    def height(self):
        return self._height

    @width.setter
    def height(self, value):
        self._height = value

    _filename = None

    _frame = None

    frameChanged = pyqtSignal()

    @pyqtProperty(QImage, notify=frameChanged)
    def frame(self):
        return self._frame

    @frame.setter
    def frame(self, frame):
        self._frame = frame
        self.frameChanged.emit()

    _filename = None

    def __init__(self, parent=None):
        super(VideoRecorder, self).__init__(parent)
        self._timer = QTimer(self)
        self._timer.timeout.connect(self.onFetchVideoFrame)

    def onFetchVideoFrame(self):
        if self._nextFrame:
            self._nextFrame = False
            block, ostream = self._repeater.getRequestBlock()
            ostream.writeQString('videoFrame')
            self._repeater.submitRequestBlock(block)

    def onExtendedDataArrived(self, category, data):
        if category == 'videoFrame' and self._timer.isActive():
            self._nextFrame = True
            image = QImage(data, self._width,
                           self._height, QImage.Format_RGB16)
            if self._filename:
                image.save(self._filename)
                self._filename = None
            self.frame = image
            bits = image.bits()
            bits.setsize(image.byteCount())
            frame = np.array(bits, np.uint8).reshape(self._height, self._width, 2)
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR5652BGR)
            self._videoWriter.write(frame)
            return True
        else:
            return False

    def setFrameRate(self, rate):
        self._rate = rate

    def setResolution(self, width, height):
        self._width = width
        self._height = height
        self._nextFrame = True

    def setFilename(self, filename):
        self._filename = filename

    def start(self):
        # fourcc = cv2.VideoWriter_fourcc(*'XVID')
        fourcc = cv2.VideoWriter_fourcc(*'X264')
        self._videoWriter = cv2.VideoWriter(self._filename + '.avi',
                                            fourcc, self._rate, (self._width, self._height))
        self._nextFrame = True
        self._timer.start(1000 / self._rate)

    def stop(self):
        self._timer.stop()
        self._videoWriter.release()

    def pause(self):
        self.stop()

    def resume(self):
        self._timer.start()

    def save(self, filename):
        self._filename = filename


class VideoPlayer(SingletonObject):
    def __init__(self, parent=None):
        super(VideoPlayer, self).__init__(parent)


class VideoProvider(QQuickImageProvider):
    def __init__(self):
        super().__init__(QQuickImageProvider.Image)
        self._recorder = VideoRecorder.instance()

    def requestImage(self, id, requestedSize):
        if self._recorder.width == 1024:
            size = QSize(self._recorder.width * 7 / 8, self._recorder.height * 7 / 8)
        elif self._recorder.width == 1280:
            size = QSize(self._recorder.width * 3 / 4, self._recorder.height * 3 / 4)
        else:
            size = QSize(self._recorder.width, self._recorder.height)
        if requestedSize.width() > 0:
            size.setWidth(requestedSize.width())
        if requestedSize.height() > 0:
            size.setHeight(requestedSize.height())
        image = VideoRecorder.instance().frame.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        return image, size


class ScreenRecorder(QObject):
    _width = 0
    _height = 0

    @pyqtProperty(int)
    def width(self):
        return self._width

    @pyqtProperty(int)
    def height(self):
        return self._height

    _scaleWidth = 0
    _scaleHeight = 0

    @pyqtProperty(int)
    def scaleWidth(self):
        return self._scaleWidth

    @pyqtProperty(int)
    def scaleHeight(self):
        return self._scaleHeight

    _fame = None

    frameChanged = pyqtSignal()

    @pyqtProperty(str, notify=frameChanged)
    def frame(self):
        return self._fame

    @frame.setter
    def frame(self, source):
        self._fame = source
        self.frameChanged.emit()

    def __init__(self, parent=None):
        super(ScreenRecorder, self).__init__(parent)
        self._fishboneConnector = FishboneConnector.instance()
        self._width = self._fishboneConnector.screenWidth
        self._height = self._fishboneConnector.screenHeight
        if self._width == 1024:
            self._scaleWidth = self._width * 7 / 8
            self._scaleHeight = self._height * 7 / 8
        elif self._width == 1280:
            self._scaleWidth = self._width * 3 / 4
            self._scaleHeight = self._height  * 3 / 4
        else:
            self._scaleWidth = self._width
            self._scaleHeight = self._height
        print(self._scaleWidth, self._scaleHeight)
        self._recorder = VideoRecorder.instance()
        self._recorder.frameChanged.connect(self.onFrameChanged)

    @pyqtSlot()
    def start(self):
        filename = QStandardPaths.writableLocation(QStandardPaths.MoviesLocation)
        filename += "/bigeye-video-{}".format(datetime.now().strftime("%Y%m%d%H%M%S"))
        self._recorder.setFrameRate(5)
        self._recorder.setResolution(self._width, self._height)
        self._recorder.setFilename(filename)
        self._recorder.start()

    @pyqtSlot()
    def stop(self):
        self._recorder.stop()

    @pyqtSlot()
    def onFrameChanged(self):
        self.frame = "image://video/timestamp={}".format(str(datetime.now().timestamp()))
