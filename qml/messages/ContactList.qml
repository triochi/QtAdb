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
    id: contactListView

    model: contactModel
    delegate: ContactDelegate{}
    highlight: highlight
    highlightFollowsCurrentItem: true

    states: [
        State
        {
            name: "ShowBars"
            when: contactListView.movingVertically
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
        width: 10; height: contactListView.height
        anchors.right: contactListView.right
        opacity: 0
        z: 10
        orientation: Qt.Vertical
        position: contactListView.visibleArea.yPosition
        pageSize: contactListView.visibleArea.heightRatio
    }
}
