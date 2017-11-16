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
import Bigeye 1.0

Item {
    id: controller

    property ListModel cpuModel: ListModel {
        ListElement {
            processor: 0
            name: "CPU0"
            idle: 25
        }

        ListElement {
            processor: 1
            name: "CPU1"
            idle: 75
        }
    }

    property ListModel systemInfoHistoryModel: ListModel {
        id: systemInfoHistory
        ListElement {
            name: "CPU0"
            color: "red"
        }
        ListElement {
            name: "CPU1"
            color: "yellow"
        }
        ListElement {
            name: "MEMORY"
            color: "magenta"
        }
        ListElement {
            name: "DISK"
            color: "lime"
        }

        function getData(index) {
            var data = [
                        [10, 10, 10, 10, 20, 30, 20, 30, 25, 22, 30, 28, 40, 50, 60, 20, 20, 20, 20, 30, 10, 5, 40, 80, 0],
                        [20, 20, 30, 10, 20, 30, 80, 30, 25, 100, 30, 58, 40, 50, 60, 30, 30, 10, 10, 70, 10, 50, 60, 80, 90],
                        [50, 50, 30, 10, 60, 70, 80, 40, 25, 0, 30, 58, 60, 70, 80, 90, 30, 80, 10, 20, 50, 65, 70, 90, 10],
                        [90, 90, 80, 75, 65, 50, 40, 30, 20, 30, 40, 50, 65, 75, 80, 90, 60, 10, 30, 40, 20, 25, 0, 40, 10]
            ]
            return data[index]
        }
    }

    signal memInfoChanged
    property int memTotal: 102400
    property int memFree: 100000

    signal diskVolumeChanged
    property int diskVolumeTotal: 2048000
    property int diskVolumeFree: 1000000

    property int timestamp: 0

    property int screenWidth: 800
    property int screenHeight: 600

    property string preview: "images/screenshot.png"

    property FileListModel fileListModel: FileListModel { }

    property KeyEventModel model: KeyEventModel { }

    Timer {
        id: timer
        interval: 1000
        repeat: true
        onTriggered: {
            memFree -= 8196
            if (memFree <= 0) {
                memFree = 100000
            }
            memInfoChanged()

            diskVolumeFree -= 40960
            if (diskVolumeFree <= 0) {
                diskVolumeFree = 1000000
            }
            diskVolumeChanged()
        }
    }

    function start(name, category) {
        console.log("Start Test Case:%1 %2".arg(name).arg(category))
        model.clear()
        timer.start()
    }

    function stop() {
        timer.stop()
    }

    function report(name, code, down) {
        console.log("%1 %2 %3".arg(name).arg(code).arg(down))
        timestamp += 1000
        model.append({"name": name, "code": code, "down": down, "timestamp": timestamp})
    }
}
