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

Pane {
    id: randomOperation

    state: "stopped"

    KeyEventModel {
        id: keyEventModel
    }

    Controller {
        id: keyLogger
        model: keyEventModel
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Rectangle {
                color: "blue"
                width: 600
                height: 450 // 360 for polar
            }

            KeyLoggerView {
                model: keyEventModel

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.maximumWidth: 256
                Layout.bottomMargin: 10
            }
        }

        RowLayout {
            Label {
                text: qsTr("Test Case:")
            }
            TextField {
                id: testCase
                selectByMouse: true
                text: qsTr("Random Operation")

                Layout.fillWidth: true
            }
            Button {
                id: recordButton
                onPressed: {
                    if (randomOperation.state === "stopped") {
                        randomOperation.state = "started"
                        keyLogger.setPressRange(pressRange.first.value, pressRange.second.value)
                        keyLogger.setReleaseRange(releaseRange.first.value, releaseRange.second.value)
                        keyLogger.clearAutomaticKeys()
                        for (var i in automaticKeys.children) {
                            var item = automaticKeys.children[i]
                            if (item.objectName === "checkbox" && item.checked) {
                                keyLogger.appendAutomaticKey(item.text, item.key)
                            }
                        }
                        keyLogger.setAutomation(true)
                        keyEventModel.clear()
                        keyLogger.start(testCase.text)
                    } else {
                        randomOperation.state = "stopped"
                        keyLogger.stop()
                    }
                }
                Layout.minimumWidth: 96
            }

            Layout.maximumHeight: recordButton.implicitHeight + 10
        }

        RowLayout {
            Label {
                text: qsTr("Press Interval(s)")
            }
            Label {
                text: "( " + Math.round(pressRange.first.value)
                      + " - " + Math.round(pressRange.second.value) +" ):"
                Layout.preferredWidth: 96
            }

            RangeSlider {
                id: pressRange
                from: 1
                to: 100
                stepSize: 1
                first.value: 1
                second.value: 10

                Layout.fillWidth: true
            }
            Label {
                text: qsTr("Release Interval(ms)")
            }
            Label {
                text: "( " + Math.round(releaseRange.first.value)
                      + " - " + Math.round(releaseRange.second.value) +" ):"
                Layout.preferredWidth: 96
            }

            RangeSlider {
                id: releaseRange
                from: 100
                to: 5000
                stepSize: 10
                first.value: 100
                second.value: 500

                Layout.fillWidth: true
            }
        }

        RowLayout {
            id: automaticKeys
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

            Layout.maximumHeight: buttonPower.implicitHeight + 10
        }
    }

    states: [
        State {
            name: "started"
            PropertyChanges { target: testCase; enabled: false }
            PropertyChanges { target: recordButton; text: qsTr("Stop") }
        },
        State {
            name: "stopped"
            PropertyChanges { target: testCase; enabled: true }
            PropertyChanges { target: recordButton; text: qsTr("Start") }
        }
    ]
}
