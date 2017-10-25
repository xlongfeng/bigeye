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

    AutomaticController {
        id: controller
    }

    GridLayout {
        columns: 2
        anchors.fill: parent

        MonitorScreen {
            source: controller.preview

            Layout.row: 0
            Layout.column: 0
        }

        MonitorCheckBoxPanel {
            id: buttonPanel
            state: randomOperation.state
            controller: controller

            Layout.row: 1
            Layout.column: 0
        }

        ColumnLayout {

            CpuInfoFragment {
                Layout.fillWidth: true
                Layout.minimumHeight: 160
            }

            ProcessInfoFragment {
                Layout.fillWidth: true
                Layout.minimumHeight: 240
            }

            KeyLoggerView {
                model: controller.model
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            RowLayout {
                Label {
                    text: qsTr("Press Interval(s)")
                }
                Label {
                    text: "( " + Math.round(pressRange.first.value)
                          + " - " + Math.round(pressRange.second.value) +" ):"
                    Layout.fillWidth: true
                }

                RangeSlider {
                    id: pressRange
                    from: 1
                    to: 30
                    stepSize: 1
                    first.value: 1
                    second.value: 2
                    Layout.preferredWidth: 256
                }
            }

            RowLayout {
                Label {
                    text: qsTr("Release Interval(ms)")
                }
                Label {
                    text: "( " + Math.round(releaseRange.first.value)
                          + " - " + Math.round(releaseRange.second.value) +" ):"
                    Layout.fillWidth: true
                }

                RangeSlider {
                    id: releaseRange
                    from: 100
                    to: 2000
                    stepSize: 10
                    first.value: 100
                    second.value: 200
                    Layout.preferredWidth: 256
                }
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
                        controller.setPressRange(pressRange.first.value, pressRange.second.value)
                        controller.setReleaseRange(releaseRange.first.value, releaseRange.second.value)
                        buttonPanel.setup()
                        controller.start(testCase.text, "automatic")
                    } else {
                        randomOperation.state = "stopped"
                        controller.stop()
                    }
                }
                Layout.fillWidth: true
                Layout.bottomMargin: 10
            }

            Layout.row: 0
            Layout.column: 1
            Layout.rowSpan: 2
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
