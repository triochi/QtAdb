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

Item
{
    id: newMessage
    height: textInputRectangle.height + textEditRectangle.height + newMessageSendSmsButton.height
    y: parent.height - newMessage.height - newMessageSendSmsButton.height

    Rectangle
    {
        id: textInputRectangle
        color: "white"
        radius: 5
        border.width: 1
        border.color: "#aaaaaa"
        smooth: true

        height: 25
        width: parent.width


        TextInput
        {
            anchors.margins: 5
            id: textInput
            anchors.fill: parent
            smooth: true
            text: ""

            Keys.onReleased:
            {
                messageWidget.filterContacts(textInput.text);
            }
        }

        MouseArea
        {
            anchors.fill: parent

            onClicked:
            {
                if (!textInput.focus)
                {
                    newMessage.state = "showContactList"
                    textInput.forceActiveFocus()
                }
                else
                {
                    newMessage.state = ""
                }
            }
        }

    }

    Rectangle
    {
        id: textEditRectangle
        color: "white"
        radius: 5
        border.width: 1
        border.color: "#aaaaaa"
        smooth: true

        y: textInputRectangle.height + 3
        height: textEdit.contentsSize.height +10 < 40 ? 40 : textEdit.contentsSize.height+10;
        width: parent.width

        TextEdit
        {
            anchors.margins: 5
            id: textEdit
            width: parent.width - 40
            anchors.fill: parent
            wrapMode: Text.WordWrap
            smooth: true
            text: ""
        }

        MouseArea
        {
            anchors.fill: parent

            onClicked:
            {
                textEdit.forceActiveFocus()
                newMessage.state = ""
            }
        }
    }

    Button
    {
        id: newMessageSendSmsButton
        width: parent.width
        height: 30
        y: textEditRectangle.height + textEditRectangle.y

        Text
        {
            color: "#7a7a7a"
            anchors.margins: 5
            text: textEdit.text.length
            anchors.right: parent.right
        }

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true

            onPressed:
            {
                newMessageSendSmsButton.state = "clicked"
                messageWidget.sendSms(textInput.text,textEdit.text);
                rectangle.state = ""
                textEdit.text = ""
                textInput.text = ""
            }
            onReleased:
            {
                newMessageSendSmsButton.state = ""
            }
        }

        Text
        {
            anchors.centerIn: parent
            id: text
            color: "black"
            text: qsTr("<B>Send message</B>")
            opacity: 1
        }
    }

    ContactList
    {
        id: contactList
        opacity: 0
        z: 2
        y: textInput.y + textInput.height + 5
        height: contactList.count * 20 < 200 ? contactList.count * 20 : 200
        width: parent.width
    }

    states: [
        State
        {
            name: "showContactList"

            PropertyChanges
            {
                target: contactList
                opacity: 1
                focus:  true
            }
        }]

    transitions: [
        Transition
        {
            NumberAnimation { properties: "opacity"; duration: 400 }
        }]
}
