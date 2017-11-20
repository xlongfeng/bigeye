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

    height: content.implicitHeight
    hoverEnabled: true

    signal removed

    contentItem: Item {
        id: content
        width: container.width
        implicitHeight: filename.implicitHeight + 12

        Text {
            id: filename
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            text: name
        }

        Button {
            id: deleteButton
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            contentItem: Image {
                source: "images/delete.png"
                sourceSize: "16x16"
                opacity: container.hovered ? 1.0 : 0
                Behavior on opacity {
                    NumberAnimation { duration: 100 }
                }
            }
            background: Item {
            }

            onClicked: container.removed()
        }
    }
}
