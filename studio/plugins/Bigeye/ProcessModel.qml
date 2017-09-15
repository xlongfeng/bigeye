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

ListModel {
    ListElement {
       name: "/appdir/ut5000"
       pid: 1
       smp: 0
       cpu: 10
       memory: 3000
    }

    ListElement {
       name: "Xfbdev"
       pid: 2
       smp: 0
       cpu: 3
       memory: 2000
    }

    ListElement {
       name: "top"
       pid: 3
       smp: 0
       cpu: 1
       memory: 1000
    }

    ListElement {
       name: "/usr/sbin/inetd"
       pid: 4
       smp: 0
       cpu: 0
       memory: 1000
    }

    ListElement {
       name: "klogd -c 7"
       pid: 5
       smp: 0
       cpu: 0
       memory: 1000
    }

    ListElement {
       name: "syslogd -l 8 -S -b 20 -O /logdir/m"
       pid: 6
       smp: 0
       cpu: 0
       memory: 1000
    }
}
