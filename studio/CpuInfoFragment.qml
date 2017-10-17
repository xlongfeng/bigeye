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
    id: cpuInfo

    orientation: ListView.Horizontal

    model: ListModel {
        ListElement {
            cpu: 0
            cpuUsage: 60
        }

        ListElement {
            cpu: 1
            cpuUsage: 20
        }
    }

    delegate: ColumnLayout {
        width: cpuInfo.width / 4
        height: cpuInfo.height
        Text {
            id: usage
            text: cpuUsage
        }

        Rectangle {
            width: 30
            gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: (101 - cpuUsage) / 100; color: {
                        var colorHigh = Math.round(0xff * cpuUsage / 100).toString(16)
                        var colorLow = Math.round(0xff * (100 - cpuUsage) / 100).toString(16)
                        colorHigh = "0%1".arg(colorHigh).slice(-2)
                        colorLow = "0%1".arg(colorLow).slice(-2)
                        return '#' + colorHigh + colorLow + '00'
                    }
                }
                GradientStop { position: 1.0; color: "#00FF00" }
                GradientStop { position: (101 - cpuUsage - 1) / 100; color: "transparent" }
            }
            Layout.fillHeight: true
        }

        Text {
            id: identify
            text: 'CPU' + cpu
        }
    }
}
