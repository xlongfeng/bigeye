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

ApplicationWindow {
    id: window

    visible: true
    minimumWidth: 1440
    minimumHeight: 960
    title: qsTr("Bigeye Studio")

    property int connectStatus

    Fishbone {
        id: fishbone
    }

    header: ToolBar {
        RowLayout {
            spacing: 8
            anchors.fill: parent

            ToolButton {
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: stackView.depth > 1 ? "images/back.png" : "images/drawer.png"
                }
                onClicked: {
                    if (stackView.depth > 1) {
                        if (stackView.currentItem.state == "started") {
                            warningMessage.text = "The current operation is in progress, please stop first."
                            warningDialog.open()
                        } else {
                            stackView.pop()
                            if (stackView.depth == 1)
                                listView.currentIndex = -1
                        }
                    } else {
                        drawer.open()
                    }
                }
            }

            Label {
                id: titleLabel
                text: listView.currentItem ? listView.currentItem.text : qsTr("Bigeye Studio")
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                id: status
                property var statusImageSource: [
                    "images/status/cancel.png",
                    "images/status/disconnected.png",
                    "images/status/connecting.png",
                    "images/status/connected.png"
                ]
                property var statusBackgroudColor: [
                    "transparent",
                    "magenta",
                    "cyan",
                    "lime"
                ]

                contentItem: Image {
                    sourceSize.width: 24
                    sourceSize.height: 24
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                }

                background: Rectangle {
                    color: "transparent"
                }

                Connections {
                    target: fishbone
                    onStatusChanged: {
                        connectStatus = fishbone.status
                        status.contentItem.source = status.statusImageSource[fishbone.status]
                        status.background.color = status.statusBackgroudColor[fishbone.status]
                    }
                }

                Component.onCompleted: {
                    connectStatus = fishbone.status
                    status.contentItem.source = status.statusImageSource[fishbone.status]
                    status.background.color = status.statusBackgroudColor[fishbone.status]
                }
            }

            /*
            ToolButton {
                id: optionToolButton
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "images/menu.png"
                }
                onClicked: optionsMenu.open()

                Menu {
                    id: optionsMenu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight

                    MenuItem {
                        text: "Settings"
                        highlighted: true
                        onTriggered: settingsDialog.open()
                    }
                    MenuItem {
                        text: "About"
                        onTriggered: aboutDialog.open()
                    }
                }
            }
            */
        }
    }

    ListModel {
        id: launcherModel
    }

    Drawer {
        id: drawer
        width: Math.min(window.width, 480) / 3 * 2
        height: window.height
        dragMargin: stackView.depth > 1 ? 0 : undefined

        ListView {
            id: listView
            focus: true
            currentIndex: -1
            anchors.fill: parent
            delegate: ItemDelegate {
                width: parent.width
                text: model.name
                highlighted: ListView.isCurrentItem
                onClicked: {
                    listView.currentIndex = index
                    stackView.push(model.url)
                    drawer.close()
                }
            }
            model: launcherModel
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    Item {
        id: launcherList
        ListView {
            width: 480
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            clip: true
            delegate: LauncherDelegate{
                onClicked: {
                    if (connectStatus !== 3) {
                        warningMessage.text = "Wating for monitor device connection."
                        warningDialog.open()
                        return
                    }

                    listView.currentIndex = index
                    stackView.push(model.url)
                }
            }
            model: launcherModel
            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    function addLauncher(name, desc, icon, url)
    {
        launcherModel.append({"name":name, "description":desc, "icon": icon, "url":url})
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: launcherList
    }

    Component.onCompleted: {
        addLauncher("Test Case", "Manage and replay test cases", Qt.resolvedUrl("images/testcase.png"), Qt.resolvedUrl("TestCaseActivity.qml"));
        addLauncher("Manual Operation", "Manual manipulation and record test procedure", Qt.resolvedUrl("images/man.png"),  Qt.resolvedUrl("ManualOperationActivity.qml"));
        addLauncher("Automatic Operation", "Random manipulation and record test procedure", Qt.resolvedUrl("images/robot.png"), Qt.resolvedUrl("AutomaticOperationActivity.qml"));
        addLauncher("Video Recoder", "Screen record for all-purpose", Qt.resolvedUrl("images/video.png"), Qt.resolvedUrl("VideoRecorderActivity.qml"));
        addLauncher("Screenshot", "Screen capture for all-purpose", Qt.resolvedUrl("images/camera.png"), Qt.resolvedUrl("ScreenshotActivity.qml"));
        addLauncher("System Monitor", "Supervise system status", Qt.resolvedUrl("images/system.png"), Qt.resolvedUrl("SystemMonitorActivity.qml"));
        addLauncher("File Browser", "Filesystem Management", Qt.resolvedUrl("images/file-browser.png"), Qt.resolvedUrl("SystemMonitorActivity.qml"));
        addLauncher("Miscellany", "Handy tools and commands", Qt.resolvedUrl("images/miscellany.png"), Qt.resolvedUrl("MiscellanyActivity.qml"));
    }

    Dialog {
        id: warningDialog
        modal: true
        focus: true
        title: "Warning"
        x: (window.width - width) / 2
        y: window.height / 6
        width: Math.min(window.width, window.height) / 3 * 2
        contentHeight: warningColumn.height

        Column {
            id: warningColumn
            spacing: 20

            Label {
                id: warningMessage
                width: warningDialog.availableWidth
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }

    Dialog {
        id: settingsDialog
        x: Math.round((window.width - width) / 2)
        y: Math.round(window.height / 6)
        width: Math.round(Math.min(window.width, window.height) / 3 * 2)
        modal: true
        focus: true
        title: "Settings"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            settings.style = styleBox.displayText
            settingsDialog.close()
        }
        onRejected: {
            styleBox.currentIndex = styleBox.styleIndex
            settingsDialog.close()
        }

        contentItem: ColumnLayout {
            id: settingsColumn
            spacing: 20

            RowLayout {
                spacing: 10

                Label {
                    text: "Port:"
                }

                ComboBox {
                    id: styleBox
                    property int styleIndex: -1
                    textRole: "portName"
                    // model: KeyRepeater.availablePorts
                    Component.onCompleted: {
                        /*
                        styleIndex = find(settings.style, Qt.MatchFixedString)
                        if (styleIndex !== -1)
                            currentIndex = styleIndex
                        */
                    }
                    Layout.fillWidth: true
                }
            }

            Label {
                text: "Restart required"
                color: "#e41e25"
                opacity: styleBox.currentIndex !== styleBox.styleIndex ? 1.0 : 0.0
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Dialog {
        id: aboutDialog
        modal: true
        focus: true
        title: "About"
        x: (window.width - width) / 2
        y: window.height / 6
        width: Math.min(window.width, window.height) / 3 * 2
        contentHeight: aboutColumn.height

        Column {
            id: aboutColumn
            spacing: 20

            Label {
                width: aboutDialog.availableWidth
                text: "The Autotest External Keyboard provide an extra keyboard input for freescale i.mx6 hardware platform."
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }
}
