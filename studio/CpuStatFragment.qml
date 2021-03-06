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

ListView {
    id: cpuInfo

    orientation: ListView.Horizontal

    delegate: ColumnLayout {
        width: identify.width + 8
        height: cpuInfo.height
        Text {
            text: 100 - idle
        }

        Rectangle {
            width: identify.width
            gradient: Gradient {
                GradientStop { position: 0.0; color: "red" }
                GradientStop { position: 0.5; color: "yellow" }
                GradientStop { position: 1.0; color: "lime" }
            }

            Rectangle {
                color: "white"
                width: parent.width
                height: parent.height * idle / 100
                anchors.top: parent.top
            }

            Layout.fillHeight: true
        }

        Text {
            id: identify
            text: name
        }
    }
}
