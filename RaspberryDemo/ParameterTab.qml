import QtQuick 2.9
import QtQuick.Controls 2.9

Item {
    id: root
    width: 1000
    height: 550
    signal overshootClicked()
    signal cuttoffClicked()
    signal naturalfreqClicked()
    property int space: 10
    property int cellWidth: 140
    property int cellHeight: 50
    property int cellFontSize: 14
    property color cellColor: "#2c2d2e"
    property string textCell11: "Text11"
    property string textCell12: "Text12"
    property string textCell13: "Text13"
    property string textCell21: "Text21"
    property string textCell22: "Text22"
    property string textCell23: "Text23"
    property string textButton1: "Button1"
    property string textButton2: "Button2"
    property string textButton3: "Button3"

    property string textColor11: "#ffffff"
    property string textColor12: "#ffffff"
    property string textColor13: "#ffffff"
    property string textColor21: "#ffffff"
    property string textColor22: "#ffffff"
    property string textColor23: "#ffffff"

    Grid {
        id: grid
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
//        width: parent.width
//        height: parent.height
        columns: 3
        rows: 3
        spacing: space
        Rectangle {
            id: rectangle
            width: cellWidth
            height: cellHeight
            color: cellColor

            Text {
                id: text11
                color: textColor11
                text: textCell11
                anchors.fill: parent
                font.pixelSize: cellFontSize
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            id: rectangle1
            width: cellWidth
            height: cellHeight
            color: cellColor

            Text {
                id: text12
                color: textColor12
                text: textCell12
                anchors.fill: parent
                font.pixelSize: cellFontSize
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }

        }

        Rectangle {
            id: rectangle2
            width: cellWidth
            height: cellHeight
            color: cellColor

            Text {
                id: text13
                color: textColor13
                text: textCell13
                transformOrigin: Item.Center
                anchors.fill: parent
                font.pixelSize: cellFontSize
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            id: rectangle3
            width: cellWidth
            height: cellHeight
            color: cellColor

            Text {
                id: text21
                color: textColor21
                text: textCell21
                anchors.fill: parent
                font.pixelSize: cellFontSize
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            id: rectangle4
            width: cellWidth
            height: cellHeight
            color: cellColor

            Text {
                id: text22
                color: textColor22
                text: textCell22
                anchors.fill: parent
                font.pixelSize: cellFontSize
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Rectangle {
            id: rectangle5
            width: cellWidth
            height: cellHeight
            color: cellColor

            Text {
                id: text23
                color: textColor23
                text: textCell23
                anchors.fill: parent
                font.pixelSize: cellFontSize
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Button {
            id: button1
            width: cellWidth
            height: cellHeight
            text: textButton1
            font.pixelSize: cellFontSize
            hoverEnabled: true
            background: Rectangle {
                            color: parent.down ? "#bbbbbb" :
                                    (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                    }
            onClicked: {
                naturalfreqClicked()
            }
        }

        Button {
            id: button2
            width: cellWidth
            height: cellHeight
            text: textButton2
            font.pixelSize: cellFontSize
            hoverEnabled: true
            flat: true
//            highlighted: true
            background: Rectangle {
                            color: parent.down ? "#bbbbbb" :
                                    (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                    }
            onClicked: {
                cuttoffClicked()
            }
        }

        Button {
            id: button3
            width: cellWidth
            height: cellHeight
            text: textButton3
            font.pixelSize: cellFontSize
            hoverEnabled: true
            background: Rectangle {
                            color: parent.down ? "#bbbbbb" :
                                    (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                    }
            onClicked: {
                overshootClicked()
            }
        }
    }

}
