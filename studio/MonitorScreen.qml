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
    id: screen

    property alias source: image.source
    property var controller

    color: "gray"
    border.color: "dimgray"
    border.width: 5
    radius: 10
    width: 960 + 20
    height: 768 + 20

    Image {
        id: image
        source: controller.preview
        anchors.centerIn: parent
    }

    Rectangle {
        id: snip
        border.color: "white"
        border.width: 5
        width: preview.sourceSize.width
        height: preview.sourceSize.height
        anchors.centerIn: screen
        visible: false
        Image {
            id: preview
            width: sourceSize.width - snip.border.width
            height: sourceSize.height - snip.border.width
            source: image.source
            anchors.centerIn: parent
        }
    }

    SequentialAnimation {
        id: shutterAnimation
        PropertyAction { target: shutter; property: "enabled"; value: false }
        PropertyAction { target: shutter; property: "visible"; value: false }
        PropertyAction { target: snip; property: "visible"; value: true }
        NumberAnimation { target: snip; property: "scale"; from: 1.0; to: 0.9; duration: 200 }
        PauseAnimation { duration: 800 }
        PropertyAction { target: snip; property: "visible"; value: false }
        PropertyAction { target: shutter; property: "visible"; value: true }
        PropertyAction { target: shutter; property: "enabled"; value: true }
    }

    IconButton {
        id: shutter
        text: "snapshot"
        icon: "images/shutter.png"
        opacity: 0.4
        onPressed: {
            shutterAnimation.running = true
            controller.screenshot()
        }
        onHoveredChanged: {
            if (hovered) {
                opacity = 0.8
            } else {
                opacity = 0.4
            }
        }

        anchors.horizontalCenter: screen.horizontalCenter
        anchors.bottom: screen.bottom
        anchors.bottomMargin: (screen.height - controller.screenHeight) / 2 + 32

        background: Item { }
    }
}
