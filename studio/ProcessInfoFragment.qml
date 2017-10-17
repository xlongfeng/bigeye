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
    id: processInfo

    header: RowLayout {
        width: processInfo.width
        height: 16
        Text {
            text: "Process Name"
            Layout.fillWidth: true
        }

        Text {
            text: "PID"
            Layout.preferredWidth: parent.width * 0.15
        }

        Text {
            text: "CPU"
            Layout.preferredWidth: parent.width * 0.1
        }

        Text {
            text: "%CPU"
            Layout.preferredWidth: parent.width * 0.1
        }

        Text {
            text: "Memory"
            Layout.preferredWidth: parent.width * 0.15
        }
    }

    model: ProcessModel {}
    delegate: RowLayout {
        width: processInfo.width
        Text {
            text: name
            Layout.fillWidth: true
        }

        Text {
            text: pid
            Layout.preferredWidth: parent.width * 0.15
        }

        Text {
            text: smp
            Layout.preferredWidth: parent.width * 0.1
        }

        Text {
            text: cpu
            Layout.preferredWidth: parent.width * 0.1
        }

        Text {
            text: memory
            Layout.preferredWidth: parent.width * 0.15

        }
    }
}
