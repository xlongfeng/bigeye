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
    id: logger
    delegate: Text {
        text: {
            var t = timestamp
            var ms = t % 1000
            t = (t - ms) / 1000     // unit: second
            var s = t % 60
            t = t - s
            var m = t % 3600 / 60
            t = t - (t % 3600)
            var h = t / 3600
            h = "00%1".arg(h).slice(-3)
            m = "0%1".arg(m).slice(-2)
            s = "0%1".arg(s).slice(-2)
            ms = "00%1".arg(ms).slice(-3)
            return "%1:%2:%3.%4 -- Key %5 <%6>".arg(h).arg(m).arg(s).arg(ms).arg(down ? " Pressed" : " Release").arg(name)
        }
    }

    Connections {
        target: model
        onRowAppended: {
            logger.positionViewAtEnd()
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}
