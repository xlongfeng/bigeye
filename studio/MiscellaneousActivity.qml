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
    id: activity

    GridLayout {
        width: 240
        height: parent.height
        anchors.centerIn: parent
        columns: 2

        Label {
            text: qsTr("Console")
        }

        ComboBox {
            model: ["Close", "TTL to RS232", "USB to RS232"]
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Screen resolution")
        }

        ComboBox {
            model: ["800x480", "800x600", "1024x768", "1280x800", "1280x1024"]
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Reboot")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Poweroff")
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Item {
            Layout.columnSpan: 2
            Layout.fillHeight: true
        }
    }
}
