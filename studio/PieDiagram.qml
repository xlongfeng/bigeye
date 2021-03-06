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


Canvas {
    id: diagram
    property int precent
    property color color: "red"

    onPrecentChanged: diagram.requestPaint()

    onPaint: {
        var pivotX = Math.round(width / 2)
        var pivotY = Math.round(height / 2)
        var radius = Math.round((Math.min(width, height) - 2) / 2)
        var angle = Math.PI * 2 * precent / 100
        if (angle < Math.PI / 2)
            angle += Math.PI * 2 * 3 / 4
        else
            angle -= Math.PI / 2

        var ctx = getContext("2d")

        ctx.save()
        ctx.strokeStyle = Qt.rgba(0.5, 0.5, 0.5, 1)
        ctx.fillStyle = Qt.rgba(1, 1, 1, 1)
        ctx.lineWidth = 2
        ctx.beginPath()
        ctx.arc(pivotX, pivotY, radius, 0, Math.PI * 2, false)
        ctx.closePath()
        ctx.clip()
        ctx.fill()
        ctx.stroke()
        ctx.restore()

        ctx.save()
        ctx.strokeStyle = Qt.rgba(0.3, 0.5, 0.5, 1)
        ctx.fillStyle = color
        ctx.lineWidth = 3
        ctx.beginPath()
        ctx.arc(pivotX, pivotY, 8, angle, Math.PI * 2 * 3 / 4, false)
        ctx.lineTo(pivotX, pivotY - radius)
        ctx.arc(pivotX, pivotY, radius, Math.PI * 2 * 3 / 4, angle, false)
        ctx.closePath()
        ctx.clip()
        ctx.fill()
        ctx.stroke()
        ctx.restore()
    }
}
