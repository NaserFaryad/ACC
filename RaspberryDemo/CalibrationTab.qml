import QtQuick 2.9
import QtQuick.Controls 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4

Item {
    id: root
    width: 1000
    height: 550
    signal p1clicked()
    signal p2clicked()
    signal calibclicked()
    signal calculate()
    signal save()
    property string volt_meter1: "0.00"
    property string volt_meter2: "0.00"
    property string captured_data1: "0.00"
    property string captured_data2: "0.00"
    property string calculate_m: "0.00"
    property string calculate_d: "0.00"
    property string which_voltageText: "1"

    Grid {
        id: grid
        anchors.topMargin: 0
        spacing: 0
        anchors.fill: parent
        rows: 2
        columns: 1
        Popup {
            id: kybrdPopup
            anchors.centerIn: parent
            width: 400
            height: 600
            modal: true
            focus: true

            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            FloatKeyboard {
                id: kbrd
                displayText: ""
                onKbrdDisplay: {
                    if(which_voltageText=="1")
                    {
                        voltage1.text = display_number
                        volt_meter1 = display_number
                    }
                    if(which_voltageText=="2")
                    {
                        voltage2.text = display_number
                        volt_meter2 = display_number
                    }
                    kybrdPopup.close()
                }
            }


        }

        Grid {
            id: grid_capture
            width: parent.width
            height: parent.height*0.65
            spacing: 5
            rows: 1
            columns: 2

            Column {
                id: columnPoint1
                width: (parent.width - grid.spacing)/2
                height: parent.height
                spacing: parent.height / 6
                Button {
                    id: buttonP1
                    width: 150
                    height: 50
                    anchors.top: parent.top
                    anchors.topMargin: parent.height / 6
                    transformOrigin: Item.Center
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "<font color=\"white\">Point1 Capture</font>"
                    font.pixelSize: 14
                    background: Rectangle {
                        width: parent.height
                        height: parent.width
                        anchors.centerIn: parent
                        color: parent.down ? "#fff" :
                              (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                        border.width: 1
                        border.color: "white"
                        radius: 10
                        rotation: 90

                        gradient: Gradient {
                            GradientStop { position: 0.0; color: buttonP1.pressed ? "#DD5E89" : "#41295a"}
                            GradientStop { position: 0.51; color: buttonP1.pressed ?"#F7BB97": "#2F0743" }
                             GradientStop { position: 1.0; color: buttonP1.pressed ?"#DD5E89": "#41295a" }
                                    }
                    }
                    onClicked: p1clicked()
                }

                NumIndic {
                    id: numIndic
                    y: parent.height * 3 / 6
                    width: 250
                    anchors.horizontalCenter: parent.horizontalCenter
                    labelText: "Captured Data"
                    indicText: captured_data1
                }

                TextField {
                    id: voltage1
                    y: parent.height * 5 / 6
                    width: 250
                    placeholderText: qsTr("Enter voltmeter value")
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24
                    text: kybrdPopup.kybrdValue
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            kybrdPopup.open()
                            which_voltageText="1"
                        }

                    }
                }
            }

            Column {
                id: columnPoint2
                width: (parent.width - grid.spacing-1)/2
                height: parent.height
                spacing: 20
                Button {
                    id: buttonP2
                    width: 150
                    height: 50
                    anchors.top: parent.top
                    anchors.topMargin: parent.height / 6
                    transformOrigin: Item.Center
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "<font color=\"white\">Point2 Capture</font>"
                    font.pixelSize: 14
                    background: Rectangle {
                        width: parent.height
                        height: parent.width
                        anchors.centerIn: parent
                        color: parent.down ? "#fff" :
                              (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                        border.width: 1
                        border.color: "white"
                        radius: 10
                        rotation: 90

                        gradient: Gradient {
                            GradientStop { position: 0.0; color: buttonP2.pressed ? "#DD5E89" : "#41295a"}
                            GradientStop { position: 0.51; color: buttonP2.pressed ?"#F7BB97": "#2F0743" }
                             GradientStop { position: 1.0; color: buttonP2.pressed ?"#DD5E89": "#41295a" }
                                    }
                    }
                    onClicked: p2clicked()
                }

                NumIndic {
                    id: numIndic2
                    width: 250
                    y: parent.height * 3 / 6
                    anchors.horizontalCenter: parent.horizontalCenter
                    labelText: "Captured Data"
                    indicText: captured_data2
                }

                TextField {
                    id: voltage2
                    width: 250
                    y: parent.height * 5 / 6
                    placeholderText: qsTr("Enter voltmeter value")
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24
                    text: kybrdPopup.kybrdValue
                    MouseArea {
                        anchors.fill: parent
                        onClicked:
                        {
                            kybrdPopup.open()
                            which_voltageText="2"
                        }


                    }
                }
            }
        }

        Grid {
            id: grid_calc
            y: 0
            width: 1000
            height: 192.5
            rows: 1
            columns: 2

            Item {
                id: element_left
                width: parent.width*0.75
                height: parent.height

                Row {
                    id: row
                    anchors.leftMargin: 50
                    anchors.fill: parent
                    spacing: 10

                    Label {
                        id: label
                        x: 0
                        width: 170
                        height: 25
                        text: qsTr("Actual Data =")
                        font.pixelSize: 18
                        horizontalAlignment: label.AlignHCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Label {
                        id: label1
                        width: 170
                        height: 25
                        text: qsTr("Captured Data   X ")
                        font.pixelSize: 18
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        id: rectangle_m
                        x: 130
                        y: 20
                        width: 100
                        height: 50
                        color: "#fafbfb"
                        Text {
                            id: element3
                            color: "#3f1d28"
                            text: calculate_m
                            anchors.fill: parent
                            font.pixelSize: 18
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        anchors.verticalCenter: parent.verticalCenter
                        layer.enabled: true
                        layer.effect: DropShadow {
                            transparentBorder: true
                            color: "gray"
                            samples: 17
                            radius: 6
                            spread: 0.0
                            horizontalOffset: 0
                            verticalOffset: 0
                                    }
                    }

                    Text {
                        id: label2
                        x: 217
                        y: 21
                        width: 20
                        height: 25
                        text: qsTr("+")
                        anchors.verticalCenterOffset: 0
                        anchors.verticalCenter: parent.verticalCenter
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 18
                    }

                    Rectangle {
                        id: rectangle_d
                        x: 250
                        y: 20
                        width: 100
                        height: 50
                        color: "#fafbfb"
                        Text {
                            id: element4
                            text: calculate_d
                            anchors.fill: parent
                            font.pixelSize: 18
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        layer.enabled: true
                        layer.effect: DropShadow {
                            transparentBorder: true
                            color: "gray"
                            samples: 17
                            radius: 6
                            spread: 0.0
                            horizontalOffset: 0
                            verticalOffset: 0
                                    }
                        anchors.verticalCenter: parent.verticalCenter
                    }

                }
            }

            Item {
                id: element_right
                width: parent.width*0.25
                height: parent.height

                Column {
                    id: column3
                    anchors.topMargin: 20
                    anchors.fill: parent
                    spacing: 10
                    Button {
                        id: button2
                        height: 50
                        width: 150
                        checkable: false
                        text: "<font color=\"white\">CALCULATE</font>"
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 18
                        background: Rectangle {
                            width: parent.height
                            height: parent.width
                            anchors.centerIn: parent
                            color: parent.down ? "#fff" :
                                  (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                            border.width: 1
                            border.color: "white"
                            radius: 10
                            rotation: 90

                            gradient: Gradient {
                                GradientStop { position: 0.0; color: button2.pressed ? "#0cebeb" : "#ff7e5f"}
                                GradientStop { position: 0.51; color: button2.pressed ?"#20e3b2":"#feb47b" }
                                 GradientStop { position: 1.0; color: button2.pressed ?"#0cebeb": "#ff7e5f"}
                                        }
                        }
                        onClicked: calculate()
                    }

                    Button {
                        id: button3
                        height: 50
                        width: 150
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "<font color=\"white\">SAVE</font>"
                        font.pixelSize: 18
                        background: Rectangle {
                            width: parent.height
                            height: parent.width
                            anchors.centerIn: parent
                            color: parent.down ? "#fff" :
                                  (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                            border.width: 1
                            border.color: "white"
                            radius: 10
                            rotation: 90

                            gradient: Gradient {
                                GradientStop { position: 0.0; color: button3.pressed ? "#0cebeb" : "#02AAB0"}
                                GradientStop { position: 0.51; color: button3.pressed ?"#20e3b2":"#00CDAC" }
                                 GradientStop { position: 1.0; color: button3.pressed ?"#0cebeb": "#02AAB0"}
                                        }
                        }
                        onClicked: save()
                    }

                    Button {
                        id: button4
                        width: 150
                        height: 50
                        enabled: false
                        visible: false
                        text: qsTr("Internal Calibration")
                        anchors.horizontalCenter: parent.horizontalCenter
                        checkable: false
                        font.pixelSize: 12
                        background: Rectangle {
                            color: parent.down ? "#bbbbbb" :
                                                                    (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                        }
                        onClicked: calibclicked()
                    }
                }
            }
        }
    }


}



/*##^##
Designer {
    D{i:17;anchors_height:400;anchors_width:200}D{i:21;anchors_x:50}D{i:24;anchors_x:200}
D{i:23;anchors_height:192.5;anchors_width:1000;anchors_y:0}D{i:15;anchors_height:192.5;anchors_width:1000;anchors_y:0}
D{i:4;anchors_y:0}D{i:29;anchors_x:50}D{i:33;anchors_x:200}D{i:35;anchors_height:70;anchors_width:175;anchors_x:0;anchors_y:0}
D{i:27;anchors_y:0}D{i:26;anchors_height:70;anchors_width:175;anchors_x:0;anchors_y:0}
D{i:37;anchors_y:0}D{i:25;anchors_height:400;anchors_width:200}D{i:1;anchors_height:400;anchors_width:400}
}
##^##*/