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
    id: view

    Screenshot {
        id: screenshot
    }

    MonitorScreen {
        id: screen
        source: screenshot.image
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle {
            id: snip
            border.color: "white"
            border.width: 5
            width: image.sourceSize.width
            height: image.sourceSize.height
            anchors.centerIn: screen
            visible: false
            Image {
                id: image
                width: sourceSize.width - snip.border.width
                height: sourceSize.height - snip.border.width
                source: screenshot.image
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
                screenshot.save()
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
            anchors.bottomMargin: (screen.height - screenshot.height) / 2 + 32

            background: Item { }
        }
    }

    /*
    RowLayout {
        width: screen.width
        anchors.top: screen.bottom
        anchors.horizontalCenter: screen.horizontalCenter
        anchors.topMargin: 16

        Label {
            text: qsTr("Save to:")
        }
        TextField {
            id: saveToDirectory
            selectByMouse: true
            text: saveToDialog.folder

            Layout.fillWidth: true
        }
        Button {
            text: "Browse"
            onClicked: saveToDialog.open()
        }
    }

    FileDialog {
        id: saveToDialog
        title: "Save snapshots to"
        folder: shortcuts.pictures
        selectFolder: true
        onAccepted: {
            saveToDirectory.text = saveToDialog.fileUrl
        }
    }
    */
}
