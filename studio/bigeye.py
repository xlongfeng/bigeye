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


from PyQt5.QtCore import Qt, QCoreApplication, QUrl
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQml import qmlRegisterType, QQmlApplicationEngine

from fishbone import *
from snapshot import *
from video import *
from testcasemodel import *
from keyeventmodel import *
from processmodel import *
from controller import *
from miscellany import *


if __name__ == '__main__':
    import os
    import sys

    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    app = QGuiApplication(sys.argv)

    qmlRegisterType(Fishbone, "Bigeye", 1, 0, "Fishbone")
    qmlRegisterType(TestCaseModel, "Bigeye", 1, 0, "TestCaseModel")
    qmlRegisterType(KeyEventModel, "Bigeye", 1, 0, "KeyEventModel")
    qmlRegisterType(ProcessModel, "Bigeye", 1, 0, "ProcessModel")

    qmlRegisterType(Controller, "Bigeye", 1, 0, "Controller")
    qmlRegisterType(LiteController, "Bigeye", 1, 0, "LiteController")
    qmlRegisterType(AutomaticController, "Bigeye", 1, 0, "AutomaticController")
    qmlRegisterType(ReplayController, "Bigeye", 1, 0, "ReplayController")

    qmlRegisterType(Screenshot, "Bigeye", 1, 0, "Screenshot")
    qmlRegisterType(ScreenRecorder, "Bigeye", 1, 0, "ScreenRecorder")

    qmlRegisterType(Miscellany, "Bigeye", 1, 0, "Miscellany")

    engine = QQmlApplicationEngine()
    engine.addImageProvider('snapshot', SnapshotProvider())
    engine.addImageProvider('video', VideoProvider())

    bigeyeQml = "bigeye-lite.qml" if 'lite' in sys.argv else "bigeye.qml"

    if 'noqrc' in sys.argv:
        engine.load(QUrl.fromLocalFile(os.path.join(os.path.dirname(__file__), bigeyeQml)))
    else:
        import bigeye_rc
        engine.load(QUrl("qrc:/" + bigeyeQml))
    
    sys.exit(app.exec_())
