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
        source: screenshot.image
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        controller: screenshot
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
