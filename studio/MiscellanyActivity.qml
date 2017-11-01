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

    Miscellany {
        id: controller
    }

    GridLayout {
        width: 240
        height: parent.height
        anchors.centerIn: parent
        columns: 2

        Label {
            text: qsTr("Console")
        }

        ComboBox {
            model: controller.consoleOptions
            currentIndex: controller.consoleIndex
            onActivated: {
                controller.consoleIndex = index
            }
            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Screen resolution")
        }

        ComboBox {
            model: controller.screenResolutionOptions
            currentIndex: controller.screenResolutionIndex
            onActivated: {
                controller.screenResolutionIndex = index
            }
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Reboot")
            onClicked: controller.reboot()
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Poweroff")
            onClicked: controller.poweroff()
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Item {
            Layout.columnSpan: 2
            Layout.fillHeight: true
        }
    }
}
