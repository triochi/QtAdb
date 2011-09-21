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

Rectangle
{
    id: button
    z:10

    color: "white"
    radius: 5
    border.width: 1
    border.color: "#aaaaaa"
    smooth: true

    states: [
        State
        {
            name: "clicked"
            PropertyChanges
            {
                target: button
                color: "#eeeeee"
            }

            PropertyChanges
            {
                target: text
                opacity: 0.2
            }
        }
    ]

    transitions: [
        Transition
        {
            ColorAnimation { from: "white"; to: "#eeeeee"; duration: 200 }
            NumberAnimation { target: text; property: "opacity"; to: 0.2; duration: 200 }
        },

        Transition
        {
            ColorAnimation { from: "#eeeeee"; to: "white"; duration: 200 }
            NumberAnimation { target: text; property: "opacity"; to: 1; duration: 200 }
        }
    ]
}
