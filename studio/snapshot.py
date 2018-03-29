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

from PyQt5.QtCore import Qt, QObject, QMutex, QSize, QTimer, QStandardPaths
from PyQt5.QtGui import QImage
from PyQt5.QtQuick import QQuickImageProvider

from repeater import *
from fishbone import *


class Snapshot(RepeaterDelegate):
    _width = 0
    _height = 0
    _bitdepth = 0

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

    _image = None

    imageChanged = pyqtSignal()

    @pyqtProperty(QImage, notify=imageChanged)
    def image(self):
        return self._image

    @image.setter
    def image(self, image):
        self._image = image
        self.imageChanged.emit()

    _filename = None

    def __init__(self, parent=None):
        super(Snapshot, self).__init__(parent)

    def setResolution(self, width, height):
        self._width = width
        self._height = height
        self._image = QImage(QSize(width, height), QImage.Format_RGB16)
        self._nextFrame = True

    def take(self):
        if self._nextFrame:
            self._nextFrame = False
            block, ostream = self._repeater.getRequestBlock()
            ostream.writeQString('snapshot')
            self._repeater.submitRequestBlock(block)

    def onExtendedDataArrived(self, category, data):
        if category == 'snapshot':
            self._nextFrame = True
            self.image = QImage(data, self._width,
                                self._height, QImage.Format_RGB16)
            if self._filename:
                self.image.save(self._filename)
                self._filename = None
            return True
        else:
            return False

    def save(self, filename):
        self._filename = filename



class SnapshotProvider(QQuickImageProvider):
    def __init__(self):
        super().__init__(QQuickImageProvider.Image)
        self._snapshot = Snapshot.instance()

    def requestImage(self, id, requestedSize):
        image = self._snapshot.image
        if self._snapshot.width == 1024:
            size = QSize(self._snapshot.width * 7 / 8, self._snapshot.height * 7 / 8)
        elif self._snapshot.width == 1280:
            size = QSize(self._snapshot.width * 3 / 4, self._snapshot.height * 3 / 4)
        else:
            size = QSize(self._snapshot.width, self._snapshot.height)
        if requestedSize.width() > 0:
            size.setWidth(requestedSize.width())
        if requestedSize.height() > 0:
            size.setHeight(requestedSize.height())
        image = image.scaled(size, Qt.KeepAspectRatio, Qt.SmoothTransformation)
        return image, size


class Screenshot(QObject):
    _width = 0
    _height = 0

    @pyqtProperty(int)
    def width(self):
        return self._width

    @pyqtProperty(int)
    def height(self):
        return self._height

    _screenWidth = 0
    _screenHeight = 0

    @pyqtProperty(int)
    def screenWidth(self):
        return self._screenWidth

    @pyqtProperty(int)
    def screenHeight(self):
        return self._screenHeight

    _image = None

    imageChanged = pyqtSignal()

    @pyqtProperty(str, notify=imageChanged)
    def image(self):
        return self._image

    @image.setter
    def image(self, source):
        self._image = source
        self.imageChanged.emit()

    def __init__(self, parent=None):
        super(Screenshot, self).__init__(parent)
        self._fishboneConnector = FishboneConnector.instance()
        self._width = self._fishboneConnector.screenWidth
        self._height = self._fishboneConnector.screenHeight
        if self._width == 1024:
            self._screenWidth = self._width * 7 / 8
            self._screenHeight = self._height * 7 / 8
        elif self._width == 1280:
            self._screenWidth = self._width * 3 / 4
            self._screenHeight = self._height  * 3 / 4
        else:
            self._screenWidth = self._width
            self._screenHeight = self._height
        self._snapshot = Snapshot.instance()
        self._snapshot.setResolution(self._width, self._height)
        self._snapshot.imageChanged.connect(self.onImageChanged)
        self._timer = QTimer()
        self._timer.timeout.connect(self.snip)
        self._timer.start(1000)

    @pyqtSlot()
    def snip(self):
        self._snapshot.take()

    @pyqtSlot()
    def screenshot(self):
        filename = QStandardPaths.writableLocation(QStandardPaths.PicturesLocation)
        filename += "/bigeye-screenshot-{}.png".format(datetime.now().strftime("%Y%m%d%H%M%S"))
        self._snapshot.save(filename)

    @pyqtSlot()
    def onImageChanged(self):
        self.image = "image://snapshot/timestamp={}".format(str(datetime.now().timestamp()))
