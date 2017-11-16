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
    width: 980
    height: 116

    property var controller

    Canvas {
        anchors.fill: parent
        onPaint: {
            var ctx = getContext('2d');
            ctx.clearRect(0, 0, width, height);
            var systemInfoHistory = controller.systemInfoHistoryModel
            for (var index = 0; index < systemInfoHistory.count; index++) {
                var info = systemInfoHistory.get(index)
                var data = systemInfoHistory.getData(index)
                ctx.save()
                ctx.strokeStyle = info.color
                ctx.beginPath()
                ctx.setTransform(width / (data.length - 1), 0, 0, height / 100, 0, 0)
                var xpos = 0
                ctx.moveTo(xpos,data[0]);
                xpos += 1
                for (var i = 1; i < data.length; i += 3) {
                    ctx.bezierCurveTo(xpos,data[i+0],xpos+1,data[i+1],xpos+2,data[i+2])
                    xpos += 3
                }
                ctx.setTransform(1, 0, 0, 1, 0, 0)
                ctx.stroke()
                ctx.restore()
            }
        }
    }
}
