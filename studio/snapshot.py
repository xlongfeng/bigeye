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

from PyQt5.QtCore import QObject, QMutex, QSize, QTimer
from PyQt5.QtGui import QImage
from PyQt5.QtQuick import QQuickImageProvider

from singletonobject import *

from repeater import *


class Snapshot(SingletonObject):
    _mutex = QMutex()

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
        self._repeater = Repeater.instance()
        self._repeater.snapshotArrived.connect(self.result)

    def take(self):
        self._repeater.reqSnapshot()

    def result(self, image):
        self._mutex.lock()
        self.image = image
        self._mutex.unlock()

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
