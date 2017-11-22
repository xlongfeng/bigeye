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

ListView {
    id: panel
    spacing: 8

    delegate: Frame {
        width: panel.width
        height: panel.height / panel.count - panel.spacing
        Canvas {
            anchors.fill: parent
            Text {
                anchors {
                    top: parent.top
                    left: parent.left
                }
                text: name
            }

            property var paint: values
            onPaintChanged: requestPaint()

            onPaint: {
                var ctx = getContext('2d');
                var ypos = values
                if (typeof ypos == "string") {
                    ypos = ypos.split(",")
                    for (var i in ypos)
                        ypos[i] = parseInt(ypos[i])
                }
                ctx.clearRect(0, 0, width, height);
                ctx.save()
                ctx.strokeStyle = color
                ctx.beginPath()
                ctx.setTransform(width / ((ypos.length - 1) * 2), 0, 0, height / 100, 0, 0)
                var xpos = 0
                ctx.moveTo(xpos, ypos[0]);
                xpos += 1
                for (var i = 1; i < ypos.length; i += 1) {
                    var y2 = ypos[i]
                    var y1 = ypos[i-1]
                    var y0 = ypos[i-0]
                    if (isNaN(y1))
                        y1 = y
                    if (isNaN(y0))
                        y0 = y
                    y1 = ((y2 - y0) / 2 + y2) * 1 / 3 + y1 * 2 / 3
                    ctx.bezierCurveTo(xpos,y1,xpos,y1,xpos+1,y2)
                    xpos += 2
                }
                ctx.setTransform(1, 0, 0, 1, 0, 0)
                ctx.stroke()
                ctx.restore()
            }
        }
    }
}
