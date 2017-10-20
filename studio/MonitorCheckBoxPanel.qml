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
    id: buttonPanel

    property var controller
    width: buttonBox.width + 20
    height: buttonBox.height + 20

    function setup() {
        controller.clearAutomaticKeys()
        for (var i in buttonBox.children) {
            var item = buttonBox.children[i]
            if (item.objectName === "checkbox" && item.checked) {
                controller.appendAutomaticKey(item.text, item.key)
            }
        }
    }

    RowLayout {
        id: buttonBox
        width: 960
        height: 96
        anchors.centerIn: parent

        IconCheckBox {
            id: buttonPower
            text: "Power"
            icon: "images/power.png"
            key: Qt.Key_F9
            objectName: "checkbox"
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        IconCheckBox {
            id: buttonF4
            checked: true
            text: "F4"
            icon: "images/f4.png"
            key: Qt.Key_F4
            objectName: "checkbox"
        }
        IconCheckBox {
            id: buttonF5
            checked: true
            text: "F5"
            icon: "images/f5.png"
            key: Qt.Key_F5
            objectName: "checkbox"
        }
        IconCheckBox {
            id: buttonF6
            checked: true
            text: "F6"
            icon: "images/f6.png"
            key: Qt.Key_F6
            objectName: "checkbox"
        }
        IconCheckBox {
            id: buttonF7
            checked: true
            text: "F7"
            icon: "images/f7.png"
            key: Qt.Key_F7
            objectName: "checkbox"
        }
        IconCheckBox {
            id: buttonF8
            checked: true
            text: "F8"
            icon: "images/f8.png"
            key: Qt.Key_F8
            objectName: "checkbox"
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
        IconCheckBox {
            id: buttonLeft
            checked: true
            text: "Left"
            icon: "images/left.png"
            key: Qt.Key_Left
            objectName: "checkbox"
        }
        IconCheckBox {
            id: buttonRight
            checked: true
            text: "Right"
            icon: "images/right.png"
            key: Qt.Key_Right
            objectName: "checkbox"
        }
        IconCheckBox {
            id: buttonEnter
            checked: true
            text: "Enter"
            icon: "images/enter.png"
            key: Qt.Key_Enter
            objectName: "checkbox"
        }
    }

    states: [
        State {
            name: "started"
            PropertyChanges { target: buttonPower; enabled: false }
            PropertyChanges { target: buttonF4; enabled: false }
            PropertyChanges { target: buttonF5; enabled: false }
            PropertyChanges { target: buttonF6; enabled: false }
            PropertyChanges { target: buttonF7; enabled: false }
            PropertyChanges { target: buttonF8; enabled: false }
            PropertyChanges { target: buttonLeft; enabled: false }
            PropertyChanges { target: buttonRight; enabled: false }
            PropertyChanges { target: buttonEnter; enabled: false }
        },
        State {
            name: "stopped"
            PropertyChanges { target: buttonPower; enabled: true }
            PropertyChanges { target: buttonF4; enabled: true }
            PropertyChanges { target: buttonF5; enabled: true }
            PropertyChanges { target: buttonF6; enabled: true }
            PropertyChanges { target: buttonF7; enabled: true }
            PropertyChanges { target: buttonF8; enabled: true }
            PropertyChanges { target: buttonLeft; enabled: true }
            PropertyChanges { target: buttonRight; enabled: true }
            PropertyChanges { target: buttonEnter; enabled: true }
        }
    ]
}
