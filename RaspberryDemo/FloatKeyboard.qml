import QtQuick 2.9
import QtQuick.Controls 2.9

Item {
    id: root
    anchors.fill: parent
    signal kbrdDisplay(string display_number)
    property alias displayText: numberDisplay.text
    property int gridMarging: 2
    property int gridSpace: 2
    property int topMarging: height/4
    property int numSize: 11
    Rectangle {
        id: rectangle
        color: "#ffffff"
        anchors.fill: parent

        Grid {
            id: grid
            width: parent.width
            anchors.horizontalCenterOffset: 1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height/4
            anchors.bottom: parent.bottom
            anchors.bottomMargin: gridMarging
            spacing: gridSpace
            rows: 5
            columns: 3
//            x: 0
//            y: numberDisplay.height
            Button {
                id: button
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("1")
                font.pointSize: numSize
                onClicked: displayText= displayText + "1"
            }

            Button {
                id: button1
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("2")
                font.pointSize: numSize
                onClicked: displayText= displayText + "2"
            }

            Button {
                id: button2
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("3")
                font.pointSize: numSize
                onClicked: displayText= displayText + "3"
            }

            Button {
                id: button3
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("4")
                font.pointSize: numSize
                onClicked: displayText= displayText + "4"
            }

            Button {
                id: button4
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("5")
                font.pointSize: numSize
                onClicked: displayText= displayText + "5"
            }

            Button {
                id: button5
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("6")
                font.pointSize: numSize
                onClicked: displayText= displayText + "6"
            }

            Button {
                id: button6
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("7")
                font.pointSize: numSize
                onClicked: displayText= displayText + "7"
            }

            Button {
                id: button7
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("8")
                font.pointSize: numSize
                onClicked: displayText= displayText + "8"
            }

            Button {
                id: button8
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("9")
                font.pointSize: numSize
                onClicked: displayText= displayText + "9"
            }

            Button {
                id: button9
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("C")
                font.pointSize: 11
                onClicked: displayText= ""
            }

            Button {
                id: button10
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("0")
                font.pointSize: numSize
                onClicked: displayText= displayText + "0"
            }

            Button {
                id: button11
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("Enter")
                font.pointSize: 11
                onClicked: kbrdDisplay(numberDisplay.text)


            }

            Button {
                id: button12
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("-")
                font.pointSize: 20
                onClicked: {
                    if (displayText.length === 0)
                    {
                     displayText= displayText + "-"
                    }
                }
            }

            Button {
                id: button13
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr("<")
                font.pointSize: 18
                onClicked: displayText= displayText.slice(0, -1)
            }

            Button {
                id: button14
                width: parent.width/3 - 2*gridSpace
                height: (root.height - numberDisplay.height)/5 - gridSpace
                text: qsTr(".")
                font.bold: true
                font.pointSize: 16
                onClicked: {
                    if (displayText.indexOf('.') === -1)
                    {
                        displayText = displayText + ".";
                    }
                }
            }
        }

        Text {
            id: numberDisplay
            width: parent.width - gridSpace
            height: parent.height/4
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: displayText
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 24
            elide: Text.ElideRight
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:3;anchors_height:35;anchors_width:48}
D{i:4;anchors_height:35;anchors_width:48}D{i:5;anchors_height:35;anchors_width:48}
D{i:2;anchors_height:146;anchors_width:150;anchors_y:54}
}
##^##*/
