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

CheckBox {
    id: control
    property alias icon: image.source
    property int key

    implicitWidth: 96
    implicitHeight: 96

    indicator.z: contentItem.z + 1
    indicator.x: contentItem.x + contentItem.width - contentItem.border.width - indicator.width
    indicator.y: contentItem.y + contentItem.border.width

    contentItem: Rectangle {
        border.color: "black"
        border.width: 2
        width: control.width
        height: control.height
        Image {
            id: image
            sourceSize: "64x64"
            opacity: enabled ? 1.0 : 0.25
            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }
            anchors.left: parent.left
            anchors.bottom: parent.bottom
        }
    }
}
