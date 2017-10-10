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
import QtQuick.Dialogs 1.2

import Bigeye 1.0


Pane {
    SwipeView {
        id: view

        currentIndex: 1
        anchors.fill: parent

        Rectangle {
            id: firstPage
            color: "red"
        }
        ListView {
            id: secondPage

            header: RowLayout {
                width: secondPage.width
                height: 16
                Text {
                    text: "Process Name"
                    Layout.fillWidth: true
                }

                Text {
                    text: "PID"
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "CPU"
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "%CPU"
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "Memory"
                    Layout.preferredWidth: 80
                }
            }

            model: ProcessModel {}
            delegate: RowLayout {
                width: secondPage.width
                Text {
                    text: name
                    Layout.fillWidth: true
                }

                Text {
                    text: pid
                    Layout.preferredWidth: 80
                }

                Text {
                    text: smp
                    Layout.preferredWidth: 80
                }

                Text {
                    text: cpu
                    Layout.preferredWidth: 80
                }

                Text {
                    text: memory
                    Layout.preferredWidth: 80
                }
            }
        }
        Rectangle {
            id: thirdPage
            color: "blue"
        }
    }

    PageIndicator {
        id: indicator

        count: view.count
        currentIndex: view.currentIndex

        anchors.bottom: view.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
