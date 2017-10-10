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

ListView {
    id: testCase

    model: TestCaseModel { }

    /*
    delegate: ItemDelegate {
        width: parent.width
        text: "%1 %2".arg(name).arg(timestamp)
        onClicked: {
        }
    }
    */
    delegate: SwipeDelegate {
        id: delegate

        text: "%1 %2".arg(name).arg(timestamp)
        width: parent.width

        swipe.right: Rectangle {
            width: parent.width
            height: parent.height

            clip: true
            color: SwipeDelegate.pressed ? "#555" : "#666"

            Label {
                /*
                font.family: "Fontello"
                text: delegate.swipe.complete ? "\ue805" // icon-cw-circled
                                              : "\ue801" // icon-cancel-circled-1
                                              */
                text: qsTr("Delete")

                padding: 20
                anchors.fill: parent
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter

                opacity: 2 * -delegate.swipe.position

                color: Material.color(delegate.swipe.complete ? Material.Green : Material.Red, Material.Shade200)
                Behavior on color { ColorAnimation { } }
            }

            Label {
                text: qsTr("Removed")
                color: "white"

                padding: 20
                anchors.fill: parent
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter

                opacity: delegate.swipe.complete ? 1 : 0
                Behavior on opacity { NumberAnimation { } }
            }

            SwipeDelegate.onClicked: delegate.swipe.close()
            SwipeDelegate.onPressedChanged: undoTimer.stop()
        }

        Timer {
            id: undoTimer
            interval: 3600
            onTriggered: listModel.remove(index)
        }

        swipe.onCompleted: undoTimer.start()
    }

    remove: Transition {
        SequentialAnimation {
            PauseAnimation { duration: 125 }
            NumberAnimation { property: "height"; to: 0; easing.type: Easing.InOutQuad }
        }
    }

    displaced: Transition {
        SequentialAnimation {
            PauseAnimation { duration: 125 }
            NumberAnimation { property: "y"; easing.type: Easing.InOutQuad }
        }
    }
}
