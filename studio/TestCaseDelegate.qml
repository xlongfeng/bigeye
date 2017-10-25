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

ItemDelegate {
    id: container
    width: ListView.view.width
    hoverEnabled: true

    contentItem: Item {
        id: content
        width: container.width
        implicitHeight: button.implicitHeight + 8

        Image {
            id: image
            source: {
                switch (category) {
                case "automatic":
                    return "images/robot.png"
                case "manual":
                    return "images/man.png"
                case "replay":
                    return "images/replay.png"
                }
            }

            sourceSize: "48x48"
            anchors {
                left: parent.left
                leftMargin: 16
                verticalCenter: parent.verticalCenter
            }
        }

        Item {
            id: button
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: image.right
                right: parent.right
            }
            implicitHeight: col.height
            height: implicitHeight
            width: buttonLabel.width + 20

            Column {
                spacing: 2
                id: col
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                Text {
                    id: buttonLabel
                    anchors {
                        left: parent.left
                        leftMargin: 10
                        right: parent.right
                        rightMargin: 10
                    }
                    text: name
                    color: "black"
                    font.pixelSize: 22
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    styleColor: "white"
                    style: Text.Raised
                }
                Text {
                    id: buttonLabel2
                    anchors{
                        left: parent.left
                        leftMargin: 10
                    }
                    text: timestamp
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "#666"
                    font.pixelSize: 12
                }
            }
        }
    }

    background: Rectangle {
        gradient: Gradient {
            GradientStop {
                position: 0
                Behavior on color {ColorAnimation { duration: 100 }}
                color: container.pressed ? "#e0e0e0" : "#fff"
            }
            GradientStop {
                position: 1
                Behavior on color {ColorAnimation { duration: 100 }}
                color: container.pressed ? "#e0e0e0" : container.hovered ? "#f5f5f5" : "#eee"
            }
        }

        Rectangle {
            height: 1
            color: "#ccc"
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
