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
import "messages"
import "messages/delegates"

Rectangle
{
    width: 800; height: 400
    id: rectangle
    color: "grey"
    z: 0

    FocusScope
    {
        id: messageView
        anchors.fill: parent
        z: 1
        opacity: 1
        focus: true

        onActiveFocusChanged:
        {
            parent.state = "showNewMessageViews"
        }

        MouseArea
        {
            anchors.fill: rectangle
            onClicked:
            {
                rectangle.state = ""
            }
        }

        Button
        {
            id: newMessageButton
            height: 30
            x:1
            y:0
            width: parent.width/2
            focus: false
            z: 10

            MouseArea
            {
                anchors.fill: parent
                hoverEnabled: true

                onPressed:
                {
                    newMessageButton.state = "clicked"
                    rectangle.state="showNewMessageView"
                }
                onReleased:
                {
                    newMessageButton.state = ""
                }
            }

            Text
            {
                anchors.centerIn: parent
                id: text
                color: "black"
                text: qsTr("<B>New message</B>")
                opacity: 1
            }

        }

        MessageList
        {
            id: messageList
            x: parent.width/2
            y: sendMessage.height + 30
            height: parent.height - sendMessage.height - 30
            width: parent.width/2
            KeyNavigation.left: threadList
            focus: true
            z: 9
            opacity: 0
        }

        ThreadList
        {
            id:threadList
            height: parent.height - newMessageButton.height - statusBarThread.height
            width: parent.width/2
            x: 1
            z: 9
            y: newMessageButton.height
            KeyNavigation.right: messageList
            focus: true
        }

        SendMessage
        {
            id: sendMessage
            x: parent.width/2
            z: 10
            width: parent.width/2
            opacity: 0
        }

        Rectangle
        {
            id: statusBarThread
            height: 15
            width: parent.width/2
            y: parent.height - statusBarThread.height
            color: "white"
            z: 10
            border.width: 1
            border.color: "#aaaaaa"

            Text
            {
                z: 11
                text: threadList.count + " threads"
                color: "black"
                anchors.centerIn: parent
            }
        }
    }

    Text
    {
        id: textReadingMessages
        z: 0
        anchors.centerIn: parent
        text: qsTr("reading messages")
        font.pointSize: 14
        color: "white"
        opacity: 0
    }

    FocusScope
    {
        id: newMessageView
        opacity: 0.0
        z: 2
        focus: true

        NewMessage
        {
            id: newMessage

            x: rectangle.width/2 - newMessage.width/2
            y: rectangle.height/2 - newMessage.height/2

            z: 10
            width: rectangle.width/2
        }
    }

    states: [
        State
        {
            name: "waitingForMessages"
            when: messageList.count == 0
            PropertyChanges
            {
                target: messageView
                opacity: 0
            }
            PropertyChanges
            {
                target: textReadingMessages
                opacity: 1
            }

        },

        State
        {
            name: "showNewMessageView"

            PropertyChanges
            {
                target: newMessageView
                opacity: 1
                focus:  true
            }

            PropertyChanges
            {
                target: messageView
                opacity: 0.5
                focus: false
            }
        }]

    transitions: [
        Transition
        {
            NumberAnimation { properties: "opacity"; duration: 400 }
        }]

}
