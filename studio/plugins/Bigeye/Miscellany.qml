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

QtObject {
    id: controller

    property var consoleOptions: ["Close", "TTL to RS232", "USB to RS232"]
    property int consoleIndex: 1
    property var screenResolutionOptions: ["800x480", "800x600", "1024x768", "1280x800", "1280x1024"]
    property int screenResolutionIndex: 3

    function  unitTest() {
        console.log("Launch unit test")
    }

    function reboot() {
        console.log("System reboot ...")
    }

    function poweroff() {
        console.log("System poweroff ...")
    }
}
