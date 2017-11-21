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
    id: activity

    state: "stopped"

    property int identity

    ReplayController {
        id: controller
        replayTestCaseId: identity
    }

    GridLayout {
        columns: 2
        anchors.fill: parent

        MonitorScreen {
            id: screen
            controller: controller

            Label {
                id: loopCounter
                text: "%1".arg(controller.loopCounter)
                color: "red"
                opacity: 0.5
                font.pixelSize: 32
                anchors.top: screen.top
                anchors.topMargin: (screen.height - controller.screenHeight) / 2 + 32
                anchors.left: screen.left
                anchors.leftMargin: (screen.width - controller.screenWidth) / 2 + 32
            }

            Layout.row: 0
            Layout.column: 0
        }

        MonitorReplayPanel {
            id: optionPanel
            state: activity.state
            controller: controller

            Layout.row: 1
            Layout.column: 0
        }

        ColumnLayout {
            RowLayout {
                GroupBox {
                    title: qsTr("CPU")
                    CpuStatFragment {
                        anchors.fill: parent
                        model: controller.cpuModel
                    }
                    Layout.fillWidth: true

                    Layout.fillHeight: true
                }
                GroupBox {
                    title: qsTr("Memory")
                    MemoryInfoFragment {
                        controller: controller
                        anchors.fill: parent
                    }
                    Layout.preferredWidth: 144
                    Layout.fillHeight: true
                }
                GroupBox {
                    title: qsTr("Disk")
                    DiskVolumeFragment {
                        controller: controller
                        anchors.fill: parent
                    }
                    Layout.preferredWidth: 144
                    Layout.fillHeight: true
                }
                Layout.fillWidth: true
                Layout.minimumHeight: 160
                Layout.maximumHeight: 160
            }

            GroupBox {
                title: qsTr("Processes")
                ProcessInfoFragment {
                    anchors.fill: parent
                }
                Layout.fillWidth: true
                Layout.minimumHeight: 240
            }

            GroupBox {
                title: qsTr("Saved File List")
                FileListFragment {
                    model: controller.fileListModel
                    anchors.fill: parent
                }
                Layout.fillWidth: true
                Layout.minimumHeight: 160
            }

            GroupBox {
                title: qsTr("Keystroke Logger History")
                KeyLoggerView {
                    id: replayKeyLogger
                    currentIndex: controller.replayKeyEventIndex
                    highlightFollowsCurrentItem : true
                    model: controller.replayModel
                    anchors.fill: parent
                }
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            GroupBox {
                title: qsTr("Keystroke Logger")
                KeyLoggerView {
                    id: keyLogger
                    model: controller.model
                    anchors.fill: parent
                }
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            TextField {
                id: testCase
                selectByMouse: true
                text: controller.testCaseName + qsTr(" Replay")
                Layout.fillWidth: true
            }

            Button {
                id: recordButton
                onPressed: {
                    if (activity.state === "stopped") {
                        activity.state = "started"
                        optionPanel.submit()
                        controller.start(testCase.text, "replay")
                    } else {
                        activity.state = "stopped"
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
            PropertyChanges { target: loopCounter; opacity: 0.5 }
            PropertyChanges { target: testCase; enabled: false }
            PropertyChanges { target: recordButton; text: qsTr("Stop") }
        },
        State {
            name: "stopped"
            PropertyChanges { target: loopCounter; opacity: 0.0 }
            PropertyChanges { target: testCase; enabled: true }
            PropertyChanges { target: recordButton; text: qsTr("Replay") }
        }
    ]
}
