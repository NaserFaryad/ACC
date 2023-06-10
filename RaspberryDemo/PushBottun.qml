import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root
    signal cliked()
    property string button_name: "button"
    property string display: "..."
    property color buttonColr: "#000000"
    property color txtColor: "#000000"
    property int txtSize: 14
    property bool txtBold: true
    property bool visibleNumber: true

    Rectangle {
        id: rectangle
        color: "#ffffff"
        anchors.fill: parent

        Row {
            id: row
            anchors.fill: parent

            Button {
                id: button
                width: parent.width*0.4 + (parent.width*0.6)*(1-visibleNumber)
                height: parent.height
                text: button_name
                font.bold: txtBold
                font.pixelSize: root.txtSize
                anchors.verticalCenter: parent.verticalCenter
                background: Rectangle {
                            color: parent.down ? "#bbbbbb" :
                                  (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                        }
                onClicked: cliked()


            }

            Text {
                id: element
                text: display
                font.bold: txtBold
                font.pixelSize: root.txtSize
                width: (parent.width*0.6)*visibleNumber
                visible: visibleNumber
                height: parent.height
                color: root.txtColor
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }


}

/*##^##
Designer {
    D{i:4;anchors_x:55;anchors_y:18}D{i:3;anchors_height:50}
}
##^##*/
