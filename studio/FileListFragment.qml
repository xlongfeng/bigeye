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

ListView {
    id: fileList
    delegate: FileEntryDelegate {
        width: fileList.width
        onRemoved: {
            fileList.model.remove(index)
        }
    }
    ScrollIndicator.vertical: ScrollIndicator { }

    function addFile() {
        filename.text = ""
        addFileDialog.open()
    }

    Dialog {
        id: addFileDialog
        width: 320
        height: 160

        modal: true
        focus: true
        title: "Add file"

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            var name = filename.text
            name = name.trim()
            if (name !== "") {
                fileList.model.add(name)
            }
        }

        contentItem: TextField {
            id: filename
        }
    }
}
