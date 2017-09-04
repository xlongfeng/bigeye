/*
 * Bigeye - Accessorial Tool of Daily Test
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
import QtQuick.Dialogs 1.2

import Bigeye 1.0


Pane {
    id: view

    state: "stopped"

    ScreenRecorder {
        id: recorder
    }

    ColumnLayout {
        anchors.fill: parent

        Image {
            id: thumbnail
            width: 800
            height: 600
            source: recorder.frame

            Label {
                id: timekeeper
                text: "timekeeper"
                color: "red"
                opacity: 0.5
                font.pixelSize: 32
                anchors.top: thumbnail.top
                anchors.topMargin: 32
                anchors.right: thumbnail.right
                anchors.rightMargin: 32

                Timer {
                    id: timer
                    property int  elapse: -1
                    interval: 1000
                    repeat: true
                    onTriggered: {
                        elapse += 1
                    }
                    onElapseChanged: {
                        var sec = elapse % 60
                        var min = (elapse - sec) / 60 % 60
                        var hour = ((elapse - sec) / 60 - min) / 60
                        sec = "0%1".arg(sec).slice(-2)
                        min = "0%1".arg(min).slice(-2)
                        hour = "0%1".arg(hour).slice(-2)
                        timekeeper.text = "%1:%2:%3".arg(hour).arg(min).arg(sec)
                    }
                }
            }

            IconButton {
                id: button
                text: "recorder"
                icon: "images/record-start.png"
                opacity: 0.4
                onPressed: {
                    if (view.state === "stopped") {
                        view.state = "started"
                        recorder.start()
                        timer.elapse = 0
                        timer.start()
                    } else {
                        view.state = "stopped"
                        recorder.stop()
                        timer.stop()
                        timer.elapse = -1
                    }
                }
                onHoveredChanged: {
                    if (hovered) {
                        opacity = 0.8
                    } else {
                        opacity = 0.4
                    }
                }

                anchors.horizontalCenter: thumbnail.horizontalCenter
                anchors.bottom: thumbnail.bottom
                anchors.bottomMargin: 32

                background: Item { }
            }
        }
    }

    states: [
        State {
            name: "started"
            PropertyChanges { target: button; icon: "images/record-stop.png" }
            PropertyChanges { target: timekeeper; visible: true }
        },
        State {
            name: "stopped"
            PropertyChanges { target: button; icon: "images/record-start.png" }
            PropertyChanges { target: timekeeper; visible: false }
        }
    ]
}
