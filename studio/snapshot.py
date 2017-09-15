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

from PyQt5.QtCore import QObject, QMutex, QSize, QTimer
from PyQt5.QtGui import QImage
from PyQt5.QtQuick import QQuickImageProvider

from repeater import *


class Snapshot(RepeaterDelegate):
    _mutex = QMutex()

    _snapshotWidth = 0
    _snapshotHeight = 0
    _snapshotBitDepth = 0

    _image = QImage(QSize(800, 600), QImage.Format_RGB16)

    imageChanged = pyqtSignal()

    @pyqtProperty(QImage, notify=imageChanged)
    def image(self):
        return self._image

    @image.setter
    def image(self, image):
        self._image = image
        self.imageChanged.emit()

    def __init__(self, parent=None):
        super(Snapshot, self).__init__(parent)

    def take(self):
        block, ostream = self._repeater.getRequestBlock()
        ostream.writeQString('snapshot')
        self._repeater.submitRequestBlock(block)

    def respSnapshot(self, istream):
        self._snapshotWidth = istream.readInt()
        self._snapshotHeight = istream.readInt()
        self._snapshotBitDepth = istream.readInt()

    def onExtendedDataArrived(self, category, data):
        if category == 'snapshot':
            self._mutex.lock()
            self.image = QImage(data, self._snapshotWidth,
                           self._snapshotHeight, QImage.Format_RGB16)
            self._mutex.unlock()
            return True
        else:
            return False

    def save(self, filename):
        self._mutex.lock()
        self.image.save(filename)
        self._mutex.unlock()


class SnapshotProvider(QQuickImageProvider):
    def __init__(self):
        super().__init__(QQuickImageProvider.Image)

    def requestImage(self, id, requestedSize):
        image = Snapshot.instance().image
        size = image.size()
        if requestedSize.width() > 0:
            size.setWidth(requestedSize.width())
        if requestedSize.height() > 0:
            size.setHeight(requestedSize.height())
        return image, size


class Screenshot(QObject):
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
        self._snapshot = Snapshot.instance()
        self._snapshot.imageChanged.connect(self.onImageChanged)
        self._timer = QTimer()
        self._timer.timeout.connect(self.snip)
        self._timer.start(1000)

    @pyqtSlot()
    def snip(self):
        self._snapshot.take()

    @pyqtSlot()
    def save(self):
        self._snapshot.save("bigeye-screenshot-{}.png".format(datetime.now().strftime("%Y%m%d%H%M%S")))

    @pyqtSlot()
    def onImageChanged(self):
        self.image = "image://snapshot/timestamp={}".format(str(datetime.now().timestamp()))