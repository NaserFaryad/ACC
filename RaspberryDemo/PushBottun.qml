import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4

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
        color: "#fafbfb"
        anchors.fill: parent
        radius: 10
        Row {
            id: row
            anchors.fill: parent
            Button {
                id: roundButton
                width: parent.width*0.4 + (parent.width*0.6)*(1-visibleNumber)
                height: parent.height
//                text: button_name
                font.bold: txtBold
                font.pixelSize: root.txtSize
                anchors.verticalCenter: parent.verticalCenter
                text: "<font color=\"white\">" + button_name + "</font>"
                background: Rectangle {
                    width: parent.height
                    height: parent.width
                    anchors.centerIn: parent
                    color: parent.down ? "#fff" :
                          (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                    border.width: 1
                    border.color: "#eee"
                    radius: 10
                    rotation: 90
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: roundButton.pressed ? "#DD5E89" : "#41295a"}
                        GradientStop { position: 0.51; color: roundButton.pressed ?"#F7BB97": "#2F0743" }
                         GradientStop { position: 1.0; color: roundButton.pressed ?"#DD5E89": "#41295a" }
                                }
                }
            }
            Text {
                id: element
                text: display
                font.bold: txtBold
                font.pixelSize: root.txtSize
                width: (parent.width*0.6)*visibleNumber
                visible: visibleNumber
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }


        }
        layer.enabled: true
        layer.effect: DropShadow {
            transparentBorder: true
            color: "gray"
            samples: 17
            radius: 4
            spread: 0.0
            horizontalOffset: 2
            verticalOffset: 2
                    }
    }


}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
