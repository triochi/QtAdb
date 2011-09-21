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

Item
{
    id: contactItem
    width: parent.width
    height: 20

    Rectangle
    {
        id: contactDelegateRectangle
        anchors.fill: parent
        color: "white"
        border.width: 1
        border.color: "#aaaaaa"
        smooth: true
    }

    Image
    {
        id: image
        source: contactDefault
        width: 20
        height: 20
        smooth: true
    }

    Text
    {
        y: 3
        x: image.width
        id: thread
        text: '<B>'+name+ ' (' + number +')</B>'

//        text: '<B>'+"contactName"+ ' (' + "number" +')</B>'
        smooth: true
    }

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true

        onClicked:
        {
            ListView.view.currentIndex = index
            contactItem.forceActiveFocus()
            textInput.text = name+ ' (' + number +')'
            newMessage.state = ""
        }
    }
}

