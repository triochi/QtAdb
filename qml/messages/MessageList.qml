/***********************************************************************
*Copyright 2010-20XX by 7ymekk
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   @author 7ymekk (7ymekk@gmail.com)
*
************************************************************************/


import Qt 4.7
import "delegates"

ListView
{
    id: messageListView

    model: messageModel
    delegate: MessageDelegate{}
    highlight: highlight
    highlightFollowsCurrentItem: true
    Component
    {
        id: highlight
        Rectangle
        {
            width: messageListView.width
            height: 40
            radius: 5
            color: "lightsteelblue"
            y: messageListView.currentItem.y
            z: 10
            opacity: 0.5
            Behavior on y
            {
                SpringAnimation
                {
                    spring: 3
                    damping: 0.2
                }
            }
        }
    }

    Keys.onPressed:
    {
        if (event.key == Qt.Key_Down || event.key == Qt.Key_Up)
        {
            messageListView.state = "ShowBars"
        }
    }

    Keys.onReleased:
    {
        messageListView.state = "HideBars"
    }



    states: [
        State
        {
            name: "ShowBars"
            when: messageListView.movingVertically
            PropertyChanges { target: verticalScrollBar; opacity: 0.5 }
        },

        State
        {
            name: "HideBars"
            PropertyChanges { target: verticalScrollBar; opacity: 0 }
        }]

    transitions: [
        Transition
        {
            NumberAnimation { properties: "opacity"; duration: 400 }
        }]

    ScrollBar
    {
        id: verticalScrollBar
        width: 10; height: messageListView.height
        anchors.right: messageListView.right
        opacity: 0
        z: 10
        orientation: Qt.Vertical
        position: messageListView.visibleArea.yPosition
        pageSize: messageListView.visibleArea.heightRatio
    }
}
