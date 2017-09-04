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

    Screenshot {
        id: screenshot
    }

    ColumnLayout {
        anchors.fill: parent

        Image {
            id: thumbnail
            width: 800
            height: 600
            source: screenshot.image

            Image {
                id: snip
                width: 800
                height: 600
                source: screenshot.image
                visible: false
                anchors.centerIn: thumbnail
                Behavior on width {
                    NumberAnimation { duration: 2000 }
                }
                Behavior on height {
                    NumberAnimation { duration: 2000 }
                }
                onWidthChanged: {
                    if (width == 600)
                        visible = false
                }
            }

            NumberAnimation {
                id: animateWidth
                target: snip
                properties: "width"
                from: 800
                to: 600
                duration: 200
            }
            NumberAnimation {
                id: animateHeight
                target: snip
                properties: "height"
                from: 600
                to: 500
                duration: 200
            }

            IconButton {
                text: "snapshot"
                icon: "images/camera.png"
                opacity: 0.4
                onPressed: {
                    animateWidth.start()
                    animateHeight.start()
                    snip.visible = true
                    screenshot.save()
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

        RowLayout {
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
}
