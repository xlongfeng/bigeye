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
    property var controller
    width: optionBox.width + 20
    height: optionBox.height + 20

    GridLayout {
        id: optionBox
        width: 960
        height: 96
        anchors.centerIn: parent

        rows: 2
        columns: 8

        RowLayout {
            SpinBox {
                editable: true
                from: 1
                to: 10000
                value: 10
            }

            Label {
                text: "Replay times"
            }
        }

        CheckBox {
            text: "Reboot after completion"
        }

        CheckBox {
            text: "Restore configuration"
        }
    }

    states: [
        State {
            name: "started"
            PropertyChanges { target: optionBox; enabled: false }
        },
        State {
            name: "stopped"
            PropertyChanges { target: optionBox; enabled: true }
        }
    ]
}
