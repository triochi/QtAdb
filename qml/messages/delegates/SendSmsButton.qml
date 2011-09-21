import Qt 4.7

Rectangle
{
    id: sendSmsButton
    z:10

    color: "white"
    radius: 5
    border.width: 1
    border.color: "#aaaaaa"
    smooth: true

    Text
    {
        anchors.centerIn: parent
        id: text
        color: "black"
        text: "<B>Send sms</B>"
        opacity: 1
    }

    states: [
        State
        {
            name: "clicked"
            PropertyChanges
            {
                target: sendSmsButton
                color: "#eeeeee"
            }

            PropertyChanges
            {
                target: text
//                color: "gray"
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
