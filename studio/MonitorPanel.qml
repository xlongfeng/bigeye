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

Rectangle {
    property var controller
    /*
    border.color: "lightyellow"
    border.width: 5
    color: "lightyellow"
    radius: 10
    */
    width: buttonBox.width + 20
    height: buttonBox.height + 20

    RowLayout {
        id: buttonBox
        width: 960
        height: 96
        anchors.centerIn: parent
        IconButton {
            id: buttonPower
            text: "Power"
            icon: "images/power.png"
            onPressed: controller.report(text, Qt.Key_F9, true)
            onReleased: controller.report(text, Qt.Key_F9, false)
            onPressAndHold: {
                controller.report(text + "Hold", Qt.Key_F10, true)
                controller.report(text, Qt.Key_F10, false)
            }
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        IconButton {
            id: buttonF4
            text: "F4"
            icon: "images/f4.png"
            onPressed: controller.report(text, Qt.Key_F4, true)
            onReleased: controller.report(text, Qt.Key_F4, false)
        }
        IconButton {
            id: buttonF5
            text: "F5"
            icon: "images/f5.png"
            onPressed: controller.report(text, Qt.Key_F5, true)
            onReleased: controller.report(text, Qt.Key_F5, false)
        }
        IconButton {
            id: buttonF6
            text: "F6"
            icon: "images/f6.png"
            onPressed: controller.report(text, Qt.Key_F6, true)
            onReleased: controller.report(text, Qt.Key_F6, false)
        }
        IconButton {
            id: buttonF7
            text: "F7"
            icon: "images/f7.png"
            onPressed: controller.report(text, Qt.Key_F7, true)
            onReleased: controller.report(text, Qt.Key_F7, false)
        }
        IconButton {
            id: buttonF8
            text: "F8"
            icon: "images/f8.png"
            onPressed: controller.report(text, Qt.Key_F8, true)
            onReleased: controller.report(text, Qt.Key_F8, false)
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        IconButton {
            id: buttonLeft
            text: "Left"
            icon: "images/left.png"
            onPressed: controller.report(text, Qt.Key_Left, true)
            onReleased: controller.report(text, Qt.Key_Left, false)
        }
        IconButton {
            id: buttonRight
            text: "Right"
            icon: "images/right.png"
            onPressed: controller.report(text, Qt.Key_Right, true)
            onReleased: controller.report(text, Qt.Key_Right, false)
        }
        IconButton {
            id: buttonEnter
            text: "Enter"
            icon: "images/enter.png"
            onPressed: controller.report(text, Qt.Key_Enter, true)
            onReleased: controller.report(text, Qt.Key_Enter, false)
        }
    }

    states: [
        State {
            name: "started"
            PropertyChanges { target: buttonPower; enabled: true }
            PropertyChanges { target: buttonF4; enabled: true }
            PropertyChanges { target: buttonF5; enabled: true }
            PropertyChanges { target: buttonF6; enabled: true }
            PropertyChanges { target: buttonF7; enabled: true }
            PropertyChanges { target: buttonF8; enabled: true }
            PropertyChanges { target: buttonLeft; enabled: true }
            PropertyChanges { target: buttonRight; enabled: true }
            PropertyChanges { target: buttonEnter; enabled: true }
        },
        State {
            name: "stopped"
            PropertyChanges { target: buttonPower; enabled: false }
            PropertyChanges { target: buttonF4; enabled: false }
            PropertyChanges { target: buttonF5; enabled: false }
            PropertyChanges { target: buttonF6; enabled: false }
            PropertyChanges { target: buttonF7; enabled: false }
            PropertyChanges { target: buttonF8; enabled: false }
            PropertyChanges { target: buttonLeft; enabled: false }
            PropertyChanges { target: buttonRight; enabled: false }
            PropertyChanges { target: buttonEnter; enabled: false }
        }
    ]
}
