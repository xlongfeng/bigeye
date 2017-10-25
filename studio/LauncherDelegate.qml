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

Rectangle {
    id: container
    width: ListView.view.width
    height: button.implicitHeight + 32

    signal clicked()

    gradient: Gradient {
        GradientStop {
            position: 0
            Behavior on color {ColorAnimation { duration: 100 }}
            color: mouseArea.pressed ? "#e0e0e0" : "#fff"
        }
        GradientStop {
            position: 1
            Behavior on color {ColorAnimation { duration: 100 }}
            color: mouseArea.pressed ? "#e0e0e0" : mouseArea.containsMouse ? "#f5f5f5" : "#eee"
        }
    }

    Image {
        id: image
        width: height
        height: button.height - 16
        source: icon
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 16
    }

    Item {
        id: button
        anchors.top: parent.top
        anchors.left: image.right
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        implicitHeight: col.height
        height: implicitHeight
        width: buttonLabel.width + 20

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: container.clicked()
            hoverEnabled: true
        }

        Column {
            spacing: 2
            id: col
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            Text {
                id: buttonLabel
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.right: parent.right
                anchors.rightMargin: 10
                text: name
                color: "black"
                font.pixelSize: 22
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                styleColor: "white"
                style: Text.Raised
            }
            Text {
                id: buttonLabel2
                anchors.left: parent.left
                anchors.leftMargin: 10
                text: description
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: "#666"
                font.pixelSize: 12
            }
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
