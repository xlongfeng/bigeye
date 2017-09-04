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

CheckBox {
    id: control
    property alias icon: image.source
    property int key

    indicator.x: control.width / 2 - indicator.width / 2
    indicator.y: control.topPadding

    contentItem: ColumnLayout {
        anchors.centerIn: parent
        Item {
            height: indicator.height
        }
        Rectangle {
            border.color: "black"
            border.width: 2
            width: image.implicitWidth
            height: image.implicitHeight
            Image {
                id: image
                sourceSize: "80x80"
                opacity: enabled ? 1.0 : 0.25
                Behavior on opacity {
                    NumberAnimation { duration: 200 }
                }
                anchors.fill: parent
            }
        }
    }
}
