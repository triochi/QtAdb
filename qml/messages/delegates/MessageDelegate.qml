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
    id: messageItem
    width: parent.width
    height: message.height + timestamp.height + 15 < 40 ? 40 : message.height + timestamp.height + 15


    Rectangle
    {
        id: messageDelegateRectangle
        color: toa == "outbox" ? "#eeeeee" : "white"
        anchors.fill: parent
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
        y: 5
        smooth: true
    }

    Text
    {
        y: 5
        x: image.width
        id: message
        width: parent.width - image.width
        text: toa == "outbox" ? '<B>'+myNickname+': </B>' + body : '<B>'+contactName+': </B>' + body
//        text: toa == "outbox" ? '<B>'+myNickname+': </B>' + "some sms" : '<B>'+"contactName"+': </B>' + "some sms"
        wrapMode: Text.WordWrap
        smooth: true
    }

    Text
    {
        x: parent.width - timestamp.width -5
        y: parent.height - timestamp.height -5
        id: timestamp
        color: "#aaaaaa"
        text: "      " + messageWidget.getDateFromTimestamp(timeStamp) ;
        font.pointSize: 7
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
                messageItem.forceActiveFocus()
                if (read == "0")
                {
                    messageWidget.markMessageAsRead(messageId);
                }
            }
            else if (mouse.button == Qt.RightButton)
            {

            }
        }
    }
}

