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

    property KeyEventModel model: KeyEventModel { }

    property int timestamp: 0

    property string preview: "images/screenshot.png"

    function start(name, category) {
        console.log("Start Test Case:%1 %2".arg(name).arg(category))
        model.clear()
    }

    function stop() {

    }

    function report(name, code, down) {
        console.log("%1 %2 %3".arg(name).arg(code).arg(down))
        timestamp += 1000
        model.append({"name": name, "code": code, "down": down, "timestamp": timestamp})
    }
}
