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
    id: manualOperation

    state: "stopped"

    Controller {
        id: keyLogger
    }

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Image {
                width: 800
                height: 600
                source: keyLogger.preview
            }

            KeyLoggerView {
                model: keyLogger.model

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
                text: qsTr("Manual Operation")

                Layout.fillWidth: true
            }
            Button {
                id: recordButton
                onPressed: {
                    if (manualOperation.state === "stopped") {
                        manualOperation.state = "started"
                        keyLogger.start(testCase.text)
                    } else {
                        manualOperation.state = "stopped"
                        keyLogger.stop()
                    }
                }
                Layout.minimumWidth: 96
            }

            Layout.maximumHeight: recordButton.implicitHeight + 10
        }

        RowLayout {
            IconButton {
                id: buttonPower
                text: "Power"
                icon: "images/power.png"
                onPressed: keyLogger.report(text, Qt.Key_F9, true)
                onReleased: keyLogger.report(text, Qt.Key_F9, false)
                onPressAndHold: {
                    keyLogger.report(text + "Hold", Qt.Key_F10, true)
                    keyLogger.report(text, Qt.Key_F10, false)
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
                onPressed: keyLogger.report(text, Qt.Key_F4, true)
                onReleased: keyLogger.report(text, Qt.Key_F4, false)
            }
            IconButton {
                id: buttonF5
                text: "F5"
                icon: "images/f5.png"
                onPressed: keyLogger.report(text, Qt.Key_F5, true)
                onReleased: keyLogger.report(text, Qt.Key_F5, false)
            }
            IconButton {
                id: buttonF6
                text: "F6"
                icon: "images/f6.png"
                onPressed: keyLogger.report(text, Qt.Key_F6, true)
                onReleased: keyLogger.report(text, Qt.Key_F6, false)
            }
            IconButton {
                id: buttonF7
                text: "F7"
                icon: "images/f7.png"
                onPressed: keyLogger.report(text, Qt.Key_F7, true)
                onReleased: keyLogger.report(text, Qt.Key_F7, false)
            }
            IconButton {
                id: buttonF8
                text: "F8"
                icon: "images/f8.png"
                onPressed: keyLogger.report(text, Qt.Key_F8, true)
                onReleased: keyLogger.report(text, Qt.Key_F8, false)
            }
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            IconButton {
                id: buttonLeft
                text: "Left"
                icon: "images/left.png"
                onPressed: keyLogger.report(text, Qt.Key_Left, true)
                onReleased: keyLogger.report(text, Qt.Key_Left, false)
            }
            IconButton {
                id: buttonRight
                text: "Right"
                icon: "images/right.png"
                onPressed: keyLogger.report(text, Qt.Key_Right, true)
                onReleased: keyLogger.report(text, Qt.Key_Right, false)
            }
            IconButton {
                id: buttonEnter
                text: "Enter"
                icon: "images/enter.png"
                onPressed: keyLogger.report(text, Qt.Key_Enter, true)
                onReleased: keyLogger.report(text, Qt.Key_Enter, false)
            }

            Layout.maximumHeight: buttonPower.implicitHeight + 10
        }
    }

    states: [
        State {
            name: "started"
            PropertyChanges { target: testCase; enabled: false }
            PropertyChanges { target: recordButton; text: qsTr("Stop") }
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
            PropertyChanges { target: testCase; enabled: true }
            PropertyChanges { target: recordButton; text: qsTr("Start") }
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
