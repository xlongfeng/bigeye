/*
 * Bigeye - Accessorial Tool for Daily Test
 * Copyright (C) 2017 xlongfeng <xlongfeng@126.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import Bigeye 1.0

Item {
    property var controller

    Connections {
        target: controller
        onDiskVolumeChanged: {
            var mTotal = controller.diskVolumeTotal
            var mFree = controller.diskVolumeFree
            var mUsed = mTotal - mFree

            var precent = (mUsed * 100 / mTotal).toFixed(1)
            if (isNaN(precent))
                precent = 0.0
            diagram.precent = precent

            mTotal = (mTotal / 1024).toFixed(1)
            mUsed = (mUsed / 1024).toFixed(1)
            used.text = "%1 MB (%2%)".arg(mUsed).arg(precent)
            total.text = "%1 MB".arg(mTotal)
        }
    }

    Text {
        id: used
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    PieDiagram {
        id: diagram
        width: height
        height: parent.height * 2 / 3
        anchors.centerIn: parent
        color: "lime"
    }

    Text {
        id: total
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
}
