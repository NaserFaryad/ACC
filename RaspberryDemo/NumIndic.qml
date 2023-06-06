import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root
    width: 100
    height: 50
    property color color: "lightgray"
    property color labelColor: "white"
    property color textColor: "black"
    property string labelText: "Label"
    property string indicText: "0.000"
    property color bgColor: "#2c2d2e"
    property int textSize: 15
    property int labelSize: 10
    property int rectRadius: 2
    property int textMargin: 2
    property int labelMargin: 2
    property bool labelBold: false
    property bool textBold: true

    Rectangle
    {
        id: rectangle
        anchors.fill: parent
        color: root.bgColor
        radius: rectRadius
        Label {
            id: label
            x: 8
            y: 18
            color: root.labelColor
            text: root.labelText
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: root.labelSize
            font.bold: textBold
            anchors.leftMargin: labelMargin
            horizontalAlignment: Label.AlignHCenter
        }



        Rectangle {
            id: textRec
            radius: rectRadius
            width: root.width - label.width - 15
            color: root.color
            anchors.top: parent.top
            anchors.topMargin: textMargin
            anchors.bottom: parent.bottom
            anchors.bottomMargin: textMargin
            anchors.right: parent.right
            anchors.rightMargin: textMargin
            Text {
                id: element
                focus: true
                color: root.textColor
                text: root.indicText
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.bold: textBold
                font.pixelSize: root.textSize
            }
        }
    }

}

/*##^##
Designer {
    D{i:4;anchors_x:55;anchors_y:18}D{i:3;anchors_height:50}
}
##^##*/
