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

Item {
    id: activity

    TestCaseModel {
        id: testCaseModel
    }

    StackView.onActivated: testCaseModel.select()

    ListView {
        id: testCase

        width: 480
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true
        model: testCaseModel
        delegate: TestCaseDelegate {
            id: delegate
            onClicked: {
                activity.StackView.view.push("ReplayActivity.qml", {"identity": identity})
            }
            onPressAndHold: {
                menu.x = (width - menu.width) / 2
                menu.y = height
                menu.open()
            }

            onRemoved: {
                testCaseModel.remove(index)
            }

            Menu {
                id: menu
                MenuItem {
                    text: "Delete"
                    onTriggered: {
                        testCaseModel.remove(index)
                    }
                }
            }
        }

        remove: Transition {
            SequentialAnimation {
                PauseAnimation { duration: 125 }
                NumberAnimation { property: "height"; to: 0; easing.type: Easing.InOutQuad }
            }
        }

        displaced: Transition {
            SequentialAnimation {
                PauseAnimation { duration: 125 }
                NumberAnimation { property: "y"; easing.type: Easing.InOutQuad }
            }
        }
    }
}
