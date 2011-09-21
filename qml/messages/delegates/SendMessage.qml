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
    id: newMessage
    height: textEdit.contentsSize.height +10 < 40 ? 40 : textEdit.contentsSize.height+10;
    focus: true

    Rectangle
    {
        anchors.fill: parent
        color: "white"
        radius: 5
        border.width: 1
        border.color: "#aaaaaa"
        smooth: true
    }

    TextEdit
    {
        x: 5
        anchors.top: parent.top
        anchors.bottom: sendSmsButton.top
        anchors.margins: 5
        id: textEdit
        height: contentsSize.height
        width: parent.width - 10
        wrapMode: Text.WordWrap
        smooth: true

    }

    Button
    {
        id: sendSmsButton
        width: parent.width
        height: 30
        y: parent.height

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

            onPressed:
            {
                sendSmsButton.state = "clicked"
                if (textEdit.text != "")
                {
                    messageWidget.sendSmsThread(textEdit.text);
                }
            }
            onReleased:
            {
                sendSmsButton.state = ""
                textEdit.text = ""
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
}
