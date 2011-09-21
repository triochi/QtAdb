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
    id: threadItem
    width: parent.width
    height: 40

    Keys.onReleased:
    {
        if (event.key == Qt.Key_Down || event.key == Qt.Key_Up)
        {
            messageWidget.sort(threadId);
        }
    }

    Rectangle
    {
        id: threadDelegateRectangle
        anchors.fill: parent
        color: "white"
        radius: 5
        border.width: 1
        border.color: "#aaaaaa"
        smooth: true

        Rectangle
        {
            anchors.fill: parent
            color:  read == "0" ? "#2200ff00" : "#00000000"
        }
    }

    Image
    {
        id: image
        source: contactDefault
        width: 35
        height: 35
        smooth: true
        y: 5
    }

    Text
    {
        y: 5
        x: image.width
        id: thread
        text: '<B>'+contactName+ ' (' + messageCount +')</B>'
//        text: '<B>'+"contactName"+ ' (' + messageCount +')</B>'
        smooth: true
    }

    Text
    {
        y: 26
        x: image.width
        id: lastbody
        color: "#aaaaaa";
        text: lastBody ;
//        text: "body of last sms"
        font.pointSize: 7
        width: parent.width - image.width - timestamp.width
        clip: true
        smooth: true
    }

    Text
    {
        y: 26
        id: timestamp
        color: "#aaaaaa";
        text: "      " + messageWidget.getDateFromTimestamp(timeStamp) ;
        font.pointSize: 7
        x: parent.width - timestamp.width - 5
        smooth: true
    }

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked:
        {
            if (mouse.button == Qt.LeftButton)
            {
                ListView.view.currentIndex = index
                threadItem.forceActiveFocus()
                messageWidget.filterMessages(threadId);
                messageList.opacity = 1
                sendMessage.opacity = 1
                if (read == "0")
                {
                    messageWidget.markThreadAsRead(threadId);
                }
            }
            else if (mouse.button == Qt.RightButton )
            {
                ListView.threadContextMenu.x = mouseX;
                ListView.threadContextMenu.y = mouseY;
                ListView.threadContextMenu.opacity = 1;
            }
        }
    }
}



