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

    property bool automation: false

    property int timestamp: 0

    property string preview: "images/screenshot.png"

    function setPressRange(start, stop) {
        console.log("Key press range from %1 to %2".arg(start).arg(stop))
    }

    function setReleaseRange(start, stop) {
        console.log("Key release range from %1 to %2".arg(start).arg(stop))
    }

    function clearAutomaticKeys() {

    }

    function appendAutomaticKey(name, code) {
        console.log(("automatic key: %1 %2").arg(name).arg(code))
    }

    function setAutomation(enabled) {
        automation = enabled
    }

    function start(name) {
        console.log("Start Test Case:%1 ".arg(name))
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
