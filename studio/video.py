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
import cv2
import numpy as np

from PyQt5.QtCore import pyqtProperty, pyqtSlot, QTimer, QSize
from PyQt5.QtGui import QImage
from PyQt5.QtQuick import QQuickImageProvider

from repeater import *


class VideoRecorder(RepeaterDelegate):
    _rate = 0
    _width = 0
    _height = 0
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

    def __init__(self, parent=None):
        super(VideoRecorder, self).__init__(parent)
        self._timer = QTimer(self)
        self._timer.timeout.connect(self.onFetchVideoFrame)

    def onFetchVideoFrame(self):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('videoFrame')
        self._repeater.submitRequestBlock(block)

    def respVideoFrame(self, istream):
        self._videoFrameWidth = istream.readInt()
        self._videoFrameHeight = istream.readInt()
        self._videoFrameBitDepth = istream.readInt()

    def onExtendedDataArrived(self, category, data):
        if category == 'videoFrame' and self._timer.isActive():
            image = QImage(data, self._width,
                           self._height, QImage.Format_RGB16)
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

    def setFilename(self, filename):
        self._filename = filename

    def start(self):
        # fourcc = cv2.VideoWriter_fourcc(*'XVID')
        fourcc = cv2.VideoWriter_fourcc(*'X264')
        self._videoWriter = cv2.VideoWriter(self._filename + '.avi',
                                            fourcc, self._rate, (self._width, self._height))
        self._timer.start(1000 / self._rate)

    def stop(self):
        self._timer.stop()
        self._videoWriter.release()

    def pause(self):
        self.stop()

    def resume(self):
        self._timer.start()


class VideoPlayer(SingletonObject):
    def __init__(self, parent=None):
        super(VideoPlayer, self).__init__(parent)


class VideoProvider(QQuickImageProvider):
    def __init__(self):
        super().__init__(QQuickImageProvider.Image)

    def requestImage(self, id, requestedSize):
        size = QSize(800, 600)
        if requestedSize.width() > 0:
            size.setWidth(requestedSize.width())
        if requestedSize.height() > 0:
            size.setHeight(requestedSize.height())
        image = VideoRecorder.instance().frame
        return image, size


class ScreenRecorder(QObject):
    def __init__(self, parent=None):
        super(ScreenRecorder, self).__init__(parent)

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
        self._recorder = VideoRecorder.instance()
        self._recorder.frameChanged.connect(self.onFrameChanged)

    @pyqtSlot()
    def start(self):
        self._recorder.setFrameRate(10)
        self._recorder.setResolution(800, 600)
        self._recorder.setFilename("bigeye-video-{}".format(datetime.now().strftime("%Y%m%d%H%M%S")))
        self._recorder.start()

    @pyqtSlot()
    def stop(self):
        self._recorder.stop()

    @pyqtSlot()
    def onFrameChanged(self):
        self.frame = "image://video/timestamp={}".format(str(datetime.now().timestamp()))
