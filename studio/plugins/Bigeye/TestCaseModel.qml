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
       identity: 1
       name: "view npb list test"
       timestamp: "2017-7-13 09:32:36"
    }

    ListElement {
       identity: 2
       name: "button random press test"
       timestamp: "2017-7-13 10:42:49"
    }

    ListElement {
       identity: 3
       name: "trend dialog open repetition"
       timestamp: "2017-7-14 10:32:26"
    }

    ListElement {
       identity: 4
       name: "Test Case 4"
       timestamp: "2017-7-14 11:30:56"
    }

    ListElement {
       identity: 5
       name: "Test Case 5"
       timestamp: "2017-7-15 15:42:16"
    }

    ListElement {
       identity: 6
       name: "Test Case 6"
       timestamp: "2017-7-15 16:32:25"
    }

    ListElement {
       identity: 7
       name: "Test Case 7"
       timestamp: "2017-7-16 13:32:43"
    }

    function select() { }
}
