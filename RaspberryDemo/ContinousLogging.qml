import QtQuick 2.9
import QtQuick.Controls 2.9
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4

Item {
    id: root
    signal dynclicked()
    property string frequency: "100"
    property string wave: "Square"
    property string min_value: "0.000"
    property string max_value: "0.000"
    property string range_value: "0.000"
    property string rms_value: "0.000"
    property string mean_value: "0.000"
    Popup {
        id: kybrdPopup
        anchors.centerIn: parent
        width: 300
        height: 400
        modal: true
        focus: true
        property string freqValue: ""
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        KeboardPopup {
            id: kbrd
            displayText: txtinFreq.text
            onKbrdDisplay: {
                txtinFreq.text = display_number
                frequency = display_number
                kybrdPopup.close()
            }

        }

    }
    Grid {
        id: grid
        spacing: 0
        anchors.fill: parent
        rows: 1
        columns: 3

        Item {
            id: element
            width: root.width/3
            height: root.height

            Label {
                id: lblFunctGen
                x: 10
                y: frame.y - height - 5
                width: parent.width
                height: 25
                text: qsTr("Function Generator Setting")
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Label.AlignHCenter
                font.pixelSize: 18
                visible: true
            }

            Frame {
                id: frame
                width: 200
                height: 200
                anchors.verticalCenterOffset: 0
                anchors.horizontalCenterOffset: 0
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                ComboBox {
                    id: cmbWave
                    x: 10
                    y: 25
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 18
                    model: ['Square', 'Sinusoidal']
                    onCurrentTextChanged: wave = cmbWave.currentText
                }

                Label {
                    id: label
                    x: 18
                    y: 0
                    width: 100
                    height: 25
                    text: qsTr("Wave Typ:")
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 16
                }

                Label {
                    id: label1
                    x: 18
                    y: 120
                    width: 132
                    height: 29
                    text: qsTr("Frequency [Hz]:")
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 16
                }
                TextInput {
                    id: txtinFreq
                    x: 0
                    y: 150
                    width: 120
                    height: 30
                    text: qsTr("100")

                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 18
                    verticalAlignment: TextInput.AlignVCenter
                    MouseArea {
                        anchors.fill: parent
                        onClicked: kybrdPopup.open()
                    }
                }
            }
        }

        Item {
            id: element1
            width: root.width/3
            height: root.height

            Button {
                id: btnContinDyn
                x: 20
                y: 50
                width: 200
                height: 60
                anchors.horizontalCenter: parent.horizontalCenter
                text: "<font color=\"white\">Continous Dynamic</font>"
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
                        GradientStop { position: 0.0; color: btnContinDyn.pressed ? "#DD5E89" : "#41295a"}
                        GradientStop { position: 0.51; color: btnContinDyn.pressed ?"#F7BB97":"#2F0743" }
                         GradientStop { position: 1.0; color: btnContinDyn.pressed ?"#DD5E89": "#41295a"}
                                }
                }
                onClicked: dynclicked()
            }

            Frame {
                id: frame1
                x: 8
                y: 81
                width: 200
                height: 250
                anchors.verticalCenterOffset: 0
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Item {
                    id: element3
                    x: 0
                    y: 150
                    width: 176
                    height: 150
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    Column {
                        id: column
                        spacing: 10
                        NumIndic {
                            id: min
                            x: 178
                            y: 85
                            width: 180
                            height: 30
                            indicText: min_value
                            labelText: "Min"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        NumIndic {
                            id: max
                            x: 175
                            y: 80
                            width: 180
                            height: 30
                            indicText: max_value
                            labelText: "Max"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        NumIndic {
                            id: range
                            x: 171
                            y: 85
                            width: 180
                            height: 30
                            indicText: range_value
                            labelText: "Range"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        NumIndic {
                            id: rms
                            x: 171
                            y: 85
                            width: 180
                            height: 30
                            indicText: rms_value
                            labelText: "RMS"
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        anchors.fill: parent
                    }
                }

                Label {
                    id: label2
                    x: 20
                    y: 5
                    width: 150
                    height: 20
                    text: qsTr("Peak-Peak Values")
                    anchors.horizontalCenterOffset: 0
                    font.pixelSize: 16
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Item {
            id: element2
            x: 316
            width: root.width/3
            height: root.height
            visible: false
            Frame {
                id: frame2
                width: 200
                height: 200
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:1;anchors_height:400;anchors_width:400}
D{i:14;anchors_height:129;anchors_width:137;anchors_x:169;anchors_y:83}D{i:16;anchors_height:150;anchors_width:176;anchors_x:0;anchors_y:150}
}
##^##*/
