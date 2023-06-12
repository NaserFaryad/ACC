import QtQuick 2.9
import QtQuick.Controls 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls 2.12
import QtQuick.Window 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.1
import QtGraphicalEffects 1.15

ApplicationWindow {
    id: applicationWindow
    flags: Qt.FramelessWindowHint
    width: 1280
    height: 800
    visible: true
    color: "#F0EADE"
//    background: BorderImage {
//        id: main_background
//        source: "bg5.jpg"
//        width: 1280; height: 1280
//        border.left: 5; border.top: 5
//        border.right: 5; border.bottom: 5
//    }
    property double gain;
    property double offset;
    property double gp_value;
    property double gn_value;
    property double zp_value;
    property double zn_value;
    property double p180_value;
    property double n180_value;

    property bool gp_flag: false;
    property bool gn_flag: false;
    property bool zp_flag: false;
    property bool zn_flag: false;
    property bool p180_flag: false;
    property bool n180_flag: false;
    property string natural_freq: ""
    property string cutoff_freq: ""
    property string over_shoot: ""
    property bool dynamic_clicked: false
    property string export_xlsx_path: ""


//    LinearGradient {
//        anchors.fill: parent
//        start: Qt.point(0, 0)
//        end: Qt.point(0, 300)
//        gradient: Gradient {
//            GradientStop { position: 0.0; color: "white" }
//            GradientStop { position: 1.0; color: "black" }
//        }
//    }

    Connections {
        target: bridge
        onGp_ready: {
            gp_value = sVolt;
            gp.display = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            gp_flag = true
            col.enabled = true
            if (gp_flag && gn_flag && zp_flag && zn_flag && p180_flag && n180_flag)
                js.sensor_param_calc()

        }
        onGn_ready: {
            gn_value = sVolt;
            gn.display = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            gn_flag = true
            col.enabled = true
            if (gp_flag && gn_flag && zp_flag && zn_flag && p180_flag && n180_flag)
                js.sensor_param_calc()
        }
        onZp_ready: {
            zp_value = sVolt;
            zp.display = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            zp_flag = true
            col.enabled = true
            if (gp_flag && gn_flag && zp_flag && zn_flag && p180_flag && n180_flag)
                js.sensor_param_calc()
        }
        onZn_ready: {
            zn_value = sVolt;
            zn.display = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            zn_flag = true
            col.enabled = true
            if (gp_flag && gn_flag && zp_flag && zn_flag && p180_flag && n180_flag)
                js.sensor_param_calc()
        }
        onP180_ready: {
            p180_value = sVolt;
            p180.display = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            p180_flag = true
            col.enabled = true
            if (gp_flag && gn_flag && zp_flag && zn_flag && p180_flag && n180_flag)
                js.sensor_param_calc()
        }
        onN180_ready: {
            n180_value = sVolt;
            n180.display = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            n180_flag = true
            col.enabled = true
            if (gp_flag && gn_flag && zp_flag && zn_flag && p180_flag && n180_flag)
                js.sensor_param_calc()
        }
        onStatic_real_time_ready: {
            static_real_time.indicText = (Math.round(sVolt*1000000)/1000000).toFixed(6)
        }
        onP1_captured: {
            calibTab.captured_data1 = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            col.enabled = true
        }
        onP2_captured: {
            calibTab.captured_data2 = (Math.round(sVolt*1000000)/1000000).toFixed(6)
            col.enabled = true
        }
        onDate_time_ready: {
            date_time.text =  DateTime;
        }
        onDynamic_test_ready: {
            continousLog.min_value = (Math.round(data[0]*1000000)/1000000).toFixed(6)
            continousLog.max_value = (Math.round(data[1]*1000000)/1000000).toFixed(6)
            continousLog.range_value = (Math.round(data[2]*1000000)/1000000).toFixed(6)
            continousLog.rms_value = (Math.round(data[3]*1000000)/1000000).toFixed(6)

        }
        onPrint_current_temp: {
            sensCurrentP.indicText = (Math.round(current_temp[0]*10000)/10).toFixed(1);
            sensCurrentN.indicText = (Math.round(current_temp[1]*10000)/10).toFixed(1);
             sensTemp.indicText = (Math.round(current_temp[2]*10000)/10000).toFixed(1);
        }
        onOvershoot_ready: {
            applicationWindow.over_shoot = (Math.round(over_shoot*1000000)/1000000).toFixed(6)
            parameterTab.textButton3 = "Over Shoot\n" + (Math.round(over_shoot*1000000)/1000000).toFixed(6)
            dynamic_clicked = false
            paramTab.enabled = true
            col.enabled = true
        }
        onCutoff_ready: {
            applicationWindow.cutoff_freq = (Math.round(cutof*1000000)/1000000).toFixed(6)
            parameterTab.textButton2 = "Cuttoff Freq\n" + (Math.round(cutof*1000000)/1000000).toFixed(6)
            console.log("/////////// cuttoff_freq_calc finished.")
            if (dynamic_clicked)
            {
                bridge.start_over_shoot_calc()
            }
            paramTab.enabled = true
            col.enabled = true
            console.log("/////////// onCutoff_ready finished.")
        }
        onNaturalfreq_ready: {
            applicationWindow.natural_freq = (Math.round(natural_freq*1000000)/1000000).toFixed(6)
            parameterTab.textButton1 = "Natural Freq\n" + (Math.round(natural_freq*1000000)/1000000).toFixed(6)
            if (dynamic_clicked)
            {
                bridge.cuttoff_freq_calc()
            }
            paramTab.enabled = true
            col.enabled = true
        }
        onSet_progress_bar: {
            progressBar.value = percent
        }
        onError_occured: {
            errorDialog.detailedText = msg
            errorDialog.open()
        }

    }

    Item {
        id: js
        function gain_offset_calib(captured_data1, volt_meter1, captured_data2, volt_meter2){
            var gain, offset;
            var x1, x2, y1, y2;
            x1 = parseFloat(captured_data1) // x1
            y1 = parseFloat(volt_meter1) //y1
            x2 = parseFloat(captured_data2) //x2
            y2 = parseFloat(volt_meter2)
            if( Math.abs(x1-x2) < 0.00001 ){
                // Error
                console.log("JS Error: gain is very low")
            }
            else {
                gain = (y1-y2) / (x1-x2);
                offset = y1- gain*x1;
            }
                console.log("JS gain:", gain)
                console.log("JS offset:", offset)
                return {gain:gain, offset:offset};
            }
        function openFile(fileUrl) {
            var request = new XMLHttpRequest();
            request.open("GET", fileUrl, false);
            request.send(null);
            return request.responseText;
        }
        function saveFile(fileUrl, text) {
            var request = new XMLHttpRequest();
            request.open("PUT", fileUrl, false);
            request.send(text);
            return request.status;
        }
        function sensor_param_calc(){
            console.log("Parameter Calculating....")

            // =================== Acceptable ranges :
            const nf_min_acceptable = 300
            const nf_max_acceptable = 550
            const cf_min_acceptable = 800
            const cf_max_acceptable = 2500

            const Os_max_acceptable = 35

            const scale_factor_min_acceptable = 0.7
            const scale_factor_max_acceptable = 2

            const bias_min_acceptable = 0.001
            const bias_max_acceptable = 3

            const mis_max_acceptable = 100
            const non_lin_max_acceptable = 20
            const Load = 350

            var k0=-1,k1=-1,k2=-1;
            var theta1 = 0,theta2 = 0;

            k0 = (zp_value + zn_value) / 2.000;
            k1 = (gp_value - gn_value)/2.000;
            k2 = (gp_value + gn_value-(zp_value + zn_value))/2.000;

            // misalignment calculation in radian :
            if(Math.abs(gp_value - gn_value) > 0.1){
                theta1 = (zp_value - zn_value)/(gp_value - gn_value);
                theta2 = (p180_value - n180_value)/(gp_value - gn_value);
            }
            else{
                // Error, incorrect test
            }
            //

            var bias=-1,scale_factor=-1,non_linearity=-1,mis_out=-1,mis_pen=-1,mis_total=-1;

            if(k1 > 0.001){
                scale_factor = Load/(k1*1000.000);
                bias = k0/k1;
                non_linearity = k2*1000000.000/k1;
            }
            else{
                // Error
            }
            mis_out = theta1 * 180.00 * 3600.00 / 3.14159265;
            mis_pen = theta2 * 180.00 * 3600.00 / 3.14159265;
            mis_total = Math.sqrt((mis_out*mis_out)+(mis_pen*mis_pen));



            // ==================================== scale factor
            if(scale_factor > 99)
                scale_factor = 99;
            else if(scale_factor<-99)
                scale_factor = -99;

            parameterTab.textCell11 = "Scale Factor\n" + (Math.round(scale_factor*1000000)/1000000).toFixed(6)
            if((scale_factor >= scale_factor_min_acceptable) &&(scale_factor <= scale_factor_max_acceptable)){
                console.log("scale factor successfully passed.")
                parameterTab.textColor11 = "#4bd05b"
            }
            else {
                console.log("scale factor failed!!")
                parameterTab.textColor11 = "#fc4236"
            }
            //==================================== bias
            if(bias > 99)
                bias = 99;
            else if(bias<-99)
                bias = -99;

            parameterTab.textCell12 = "Bias\n" + (Math.round(bias*1000000)/1000000).toFixed(6)
            if((bias >= bias_min_acceptable) &&(bias <= bias_max_acceptable)){
                console.log("bias successfully passed.")
                parameterTab.textColor12 = "#4bd05b"
            }
            else {
                console.log("bias failed!!")
                parameterTab.textColor12 = "#fc4236"
            }
            //==================================== non linearity
            if(non_linearity > 99)
                non_linearity = 99;
            else if(mis_out<-99)
                non_linearity = -99;

            parameterTab.textCell13 = "Non-linearity\n" + (Math.round(non_linearity*1000000)/1000000).toFixed(6)
            if((non_linearity <= non_lin_max_acceptable)){
                console.log("on linearity successfully passed.")
                parameterTab.textColor13 = "#4bd05b"
            }
            else {
                console.log("on linearity failed!!")
                parameterTab.textColor13 = "#fc4236"
            }

            //====================================  misalignment output axis
            if(mis_out > 999)
                mis_out = 999;
            else if(mis_out<-999)
                mis_out = -999;

            parameterTab.textCell21 = "Mis-Output\n" + (Math.round(mis_out*1000000)/1000000).toFixed(6)
            if((mis_out <= mis_max_acceptable)){
                console.log("misalignment output axis successfully passed.")
                parameterTab.textColor21 = "#4bd05b"
            }
            else {
                console.log("misalignment output axis failed!!")
                parameterTab.textColor21 = "#fc4236"
            }
            //====================================  misalignment pendolous axis
            if(mis_pen > 999)
                mis_pen = 999;
            else if(mis_pen<-999)
                mis_pen = -999;

            parameterTab.textCell22 = "Mis-Pendolous\n" + (Math.round(mis_pen*1000000)/1000000).toFixed(6)
            if((mis_pen <= mis_max_acceptable)){
                console.log("misalignment pendolous axis successfully passed.")
                parameterTab.textColor22 = "#4bd05b"
            }
            else {
                uconsole.log("misalignment pendolous axis failed!!")
                parameterTab.textColor22 = "#fc4236"
            }
            //====================================  misalignment total axis
            if(mis_total > 999)
                mis_total = 999;
            else if(mis_total<-999)
                mis_total = -999;

            parameterTab.textCell23 = "Mis-Total\n" + (Math.round(mis_total*1000000)/1000000).toFixed(6)
            if((mis_total <= mis_max_acceptable)){
                console.log("misalignment total axis successfully passed.")
                parameterTab.textColor23 = "#4bd05b"
            }
            else {
                console.log("misalignment total axis failed!!")
                parameterTab.textColor23 = "#fc4236"
            }
            gp_flag = false
            gn_flag = false
            zp_flag = false
            zn_flag = false
            p180_flag = false
            n180_flag = false
        }

    }

    Popup {
        id: kybrdPopup
        anchors.centerIn: parent
        width: 300
        height: 400
        modal: true
        focus: true
        property string serialNumValue: ""
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        KeboardPopup {
            id: kbrd
            displayText: serialNum.text
            onKbrdDisplay: {
                serialNum.text = display_number
                kybrdPopup.close()
            }

        }

    }

    Column {
        id: col
        width: 229
        height: 690
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: 10
        spacing: 35
        PushBottun
        {
            id: gp
            width: 220
            height: 60
            button_name: "G+"
            txtSize: 22
            display: "0.00"
            onPb_cliked: {
                console.log("G+ Clicked")
                bridge.pressed_button(0)
                col.enabled = false
            }

        }
        PushBottun
        {
            id: gn
            width: 220
            height: 60
            button_name: "G-"
            txtSize: 22
            display: "0.00"
            onPb_cliked: {
                console.log("G- Clicked")
                bridge.pressed_button(1)
                col.enabled = false
            }


        }
        PushBottun
        {
            id: zp
            width: 220
            height: 60
            button_name: "0+"
            txtSize: 22
            display: "0.00"
            onPb_cliked: {
                console.log("0+ Clicked")
                bridge.pressed_button(2)
                col.enabled = false
            }

        }
        PushBottun
        {
            id: zn
            width: 220
            height: 60
            button_name: "0-"
            txtSize: 22
            display: "0.00"
            onPb_cliked: {
                console.log("0- Clicked")
                bridge.pressed_button(3)
                col.enabled = false
            }

        }
        PushBottun
        {
            id: p180
            width: 220
            height: 60
            button_name: "180+"
            txtSize: 22
            display: "0.00"
            onPb_cliked: {
                console.log("180+ Clicked")
                bridge.pressed_button(4)
                col.enabled = false
            }

        }
        PushBottun
        {
            id: n180
            width: 220
            height: 60
            button_name: "180-"
            txtSize: 22
            display: "0.00"
            onPb_cliked: {
                console.log("180- Clicked")
                bridge.pressed_button(5)
                col.enabled = false
            }

        }
        PushBottun
        {
            id: dynm
            width: 220
            height: 70
            button_name: "DYNAMIC"
            txtSize: 16
            visibleNumber: false
            display: "0.00"
            onPb_cliked: {
                console.log("Dynamic Clicked")
                bridge.natural_freq_calc()
                dynamic_clicked = true
                paramTab.enabled = false
                col.enabled = false

            }

        }
    }

    // Sensor current and Temperature
    NumIndic
    {
        id: sensCurrentP
        labelText: "Current [+15]"
        labelSize: 14
        width: 250
        height: 50
        x: 25
        y: 30

    }
    NumIndic
    {
        id: sensTemp
        labelText: "Temperature"
        labelSize: 14
        width: 250
        height: 50
        x: 971
        y: 30
        indicText: "25.01"

    }

    TabBar {
        id: bar
        x: 250
        y: 100
        width: 1000
        height: 50
        currentIndex: 0
        background: Rectangle {
                color: "#f4d37c"
                radius: 10
            }
        TabButton {
            y: 0
            width: 200
            height: parent.height
            text: qsTr("ACC parameters")
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 14
            clip: false
            visible: true
            background: Rectangle {
                        color: bar.currentIndex == 0 ? "#f4d37c" : "#2F2E2E"
                        radius: 10
                    }
            onClicked: {
                col.enabled = true
                bridge.static_real_time_stop()
            }

        }
        TabButton {
            y: 0
            width: 200
            height: parent.height
            text: qsTr("Continuous")
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 14
            background: Rectangle {
                        color: bar.currentIndex == 1 ? "#f4d37c" : "#2F2E2E"
                        radius: 10
                    }
            onClicked:
            {
                bridge.static_real_time_start()
                col.enabled = false
            }
        }
        TabButton {
            y: 0
            width: 200
            height: parent.height
            text: qsTr("Calibration")
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 14
            background: Rectangle {
                        color: bar.currentIndex == 2 ? "#f4d37c" : "#2F2E2E"
                        radius: 10
                    }
            onClicked: {
                col.enabled = true
                bridge.static_real_time_stop()
            }
        }
        TabButton {
            y: 0
            width: 200
            height: parent.height
            text: qsTr("Debug")
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 14
            background: Rectangle {
                        color: bar.currentIndex == 3 ? "#f4d37c" : "#2F2E2E"
                        radius: 10
                    }
            onClicked: {
                col.enabled = true
                bridge.static_real_time_stop()
            }
        }
        TabButton {
            y: 0
            height: parent.height
            width: 196
            text: qsTr("Read me")
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 14
            background: Rectangle {
                        color: bar.currentIndex == 4 ? "#f4d37c" : "#2F2E2E"
                        radius: 10
                    }
            onClicked: {
                col.enabled = true
                bridge.static_real_time_stop()
            }
        }
    }
    StackLayout
    {
        x: 250
        y: 100
        width: bar.width
        height: 550
        anchors.topMargin: 1
        anchors.top: bar.bottom
        currentIndex: bar.currentIndex
//        currentIndex: bar.currentIndex
        Item {
            id: paramTab
            anchors.fill: parent
            Rectangle
            {
                height: 550
                color: "#F4F4F4"
                anchors.fill: parent
                radius: 10
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
                ParameterTab {
                    id: parameterTab
                    cellWidth: 150
                    cellHeight: 100
                    space: 40
                    anchors.fill: parent
                    textCell11: "Scale Factor\n--"
                    textCell12: "Bias\n--"
                    textCell13: "Non-linearity\n--"
                    textCell21: "Mis-Output\n--"
                    textCell22: "Mis-Pendolous\n--"
                    textCell23: "Mis-Total\n--"
                    textButton1: "Natural Freq\n--"
                    textButton2: "Cuttoff Freq\n--"
                    textButton3: "Over Shoot\n--"
                    onNaturalfreqClicked: {
                        bridge.natural_freq_calc()
                        paramTab.enabled = false
                        col.enabled = false
                    }
                    onCuttoffClicked: {
                        bridge.cuttoff_freq_calc()
                        paramTab.enabled = false
                        col.enabled = false
                    }
                    onOvershootClicked: {
                        bridge.start_over_shoot_calc()
                        paramTab.enabled = false
                        col.enabled = false
                    }

                    Button {
                        id: save
                        x: 814
                        y: 366
                        width: 120
                        height: 40
                        text: "<font color=\"white\">SAVE</font>"
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
                                GradientStop { position: 0.0; color: save.pressed ? "#0cebeb" : "#02AAB0"}
                                GradientStop { position: 0.51; color: save.pressed ?"#20e3b2":"#00CDAC" }
                                 GradientStop { position: 1.0; color: save.pressed ?"#0cebeb": "#02AAB0"}
                                        }
                        }
                        onClicked: saveDialog.open()
                    }

                    Button {
                        id: reset
                        x: 814
                        y: 430
                        width: 120
                        height: 40
                        text: "<font color=\"white\">RESET</font>"
                        font.pixelSize: 14
                        background: Rectangle {
                            width: parent.height
                            height: parent.width
                            anchors.centerIn: parent
                            color: parent.down ? "#fff" :
                                  (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                            border.width: reset.activeFocus ? 2 : 1
                            border.color: "white"
                            radius: 10
                            rotation: 90

                            gradient: Gradient {
                                GradientStop { position: 0.0; color: reset.pressed ? "#DD5E89" : "#EB3349"}
                                GradientStop { position: 0.51; color: reset.pressed ?"#F7BB97":"#F45C43" }
                                 GradientStop { position: 1.0; color: reset.pressed ?"#DD5E89": "#EB3349"}
                                        }
                        }
                        onClicked: {
                            gp_flag = false
                            gn_flag = false
                            zp_flag = false
                            zn_flag = false
                            p180_flag = false
                            n180_flag = false
                            dynamic_clicked = false
                            parameterTab.textCell11 = "Scale Factor\n--"
                            parameterTab.textCell12 = "Bias\n--"
                            parameterTab.textCell13 = "Non-linearity\n--"
                            parameterTab.textCell21 =  "Mis-Output\n--"
                            parameterTab.textCell22 = "Mis-Pendolous\n--"
                            parameterTab.textCell23 = "Mis-Total\n--"
                            parameterTab.textButton1= "Natural Freq\n--"
                            parameterTab.textButton2= "Cuttoff Freq\n--"
                            parameterTab.textButton3= "Over Shoot\n--"

                            parameterTab.textColor11 = "#ffffff"
                            parameterTab.textColor12 = "#ffffff"
                            parameterTab.textColor13 = "#ffffff"
                            parameterTab.textColor21 = "#ffffff"
                            parameterTab.textColor22 = "#ffffff"
                            parameterTab.textColor23 = "#ffffff"

                            progressBar.value = 0

                            gp.display = "--"
                            gn.display = "--"
                            zp.display = "--"
                            zn.display = "--"
                            p180.display = "--"
                            n180.display = "--"

                        }
                    }

                    Button {
                        id: exportFile
                        x: 814
                        y: 485
                        width: 120
                        height: 40
                        text: "<font color=\"white\">EXPORT</font>"
                        font.pixelSize: 14
                        background: Rectangle {
                            width: parent.height
                            height: parent.width
                            anchors.centerIn: parent
                            color: parent.down ? "#fff" :
                                  (parent.hovered ? "#d6d6d6" : "#f6f6f6")
                            border.width: reset.activeFocus ? 2 : 1
                            border.color: "white"
                            radius: 10
                            rotation: 90

                            gradient: Gradient {
                                GradientStop { position: 0.0; color: exportFile.pressed ? "#085078" : "#16222A"}
                                GradientStop { position: 0.51; color: exportFile.pressed ?"#85D8CE":"#3A6073" }
                                 GradientStop { position: 1.0; color: exportFile.pressed ?"#085078": "#16222A"}
                                        }
                        }
                        onClicked: fileDialog.open()
                    }

                }

            }

            TextField {
                id: serialNum
                x: 616
                y: 482
                width: 146
                height: 40
                font.pixelSize: 16
                placeholderText: qsTr("Enter serial")
                horizontalAlignment: Text.AlignHCenter
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: IntValidator {
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: kybrdPopup.open()
                }
            }

            Label {
                id: label
                x: 477
                y: 492
                text: qsTr("Serial Number")
                font.pixelSize: 16
                MouseArea {
                    anchors.fill: parent
                }
            }
        }
        Item {
            id: logTab
            Rectangle
            {
                color: "#F4F4F4"
                anchors.fill: parent
                radius: 10
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
                NumIndic {
                    id: static_real_time
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    width: 450
                    height: 100
                    labelText: "Voltage [V]"
                    indicText: "0.000000"
                    textSize: 42
                    labelSize: 24
                }
            }
        }
        Item {

            Rectangle
            {
                color: "#F4F4F4"
                anchors.fill: parent
                radius: 10
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
                CalibrationTab {
                    id: calibTab
                    anchors.fill: parent
                    captured_data1: "0.0"
                    onP1clicked: {
                        console.log("Point1 Calibration Tab Clicked")
                        bridge.pressed_button(6)
                        col.enabled = false
                    }
                    onP2clicked: {
                        console.log("Point2 Calibration Tab Clicked")
                        bridge.pressed_button(7)
                        col.enabled = false
                    }
                    onCalculate: {
//                        console.log("Read text file: ", files.read_Params())
                        // input arguman: x1, y1, x2, y2
                        var calib_param= js.gain_offset_calib(calibTab.captured_data1, // x1
                                                              calibTab.volt_meter1, //y1
                                                              calibTab.captured_data2, //x2
                                                              calibTab.volt_meter2) //y2
                        console.log("Gain: ", calib_param.gain)
                        console.log("Offset: ", calib_param.offset)
                        gain = calib_param.gain
                        offset = calib_param.offset
                        calculate_m = (Math.round(calib_param.gain*1000000)/1000000).toFixed(6)
                        calculate_d = (Math.round(calib_param.offset*1000000)/1000000).toFixed(6)
                    }
                    onSave: {
                        console.log("Saving calibration params")
                        files.write_Params(gain, offset)
                        bridge.update_gain_offset()
                    }
                    onCalibclicked: bridge.internal_calibration()

                }
            }
        }
        Item {
            id: continueTab
            Rectangle
            {
                color: "#F4F4F4"
                anchors.fill: parent
                radius: 10
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
                ContinousLogging {
                    id: continousLog
                    anchors.fill: parent
//                    min_value: "0.001"
                    onDynclicked: {
                        console.log("Input frequency: ", frequency)
                        console.log("Input wave type: ", wave)
                        if (wave == "Square")
                            bridge.dynamic_test(parseInt(frequency), 0)
                        else
                            bridge.dynamic_test(parseInt(frequency), 1)
                    }
                }
            }
        }
        Item {
            id: readmeTab
            Rectangle
            {
                color: "#F4F4F4"
                anchors.fill: parent
                radius: 10
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
                TextArea {
                    id: txtareaReadme
                    anchors.fill: parent
                    font.pixelSize: 12
                    readOnly: true
                    text:
                        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, " +
                                "sed do eiusmod tempor incididunt ut labore et dolore magna " +
                                "aliqua. Ut enim ad minim veniam, quis nostrud exercitation " +
                                "ullamco laboris nisi ut aliquip ex ea commodo cosnsequat. ";
                }
            }
        }
    }

    ProgressBar {
        id: progressBar
        x: bar.x + bar.width/2 - width/2
        y: 740
        width: 500
        height: 15
        value: 0

    }

    NumIndic {
        id: sensCurrentN
        x: 400
        y: 30
        width: 250
        height: 50
        anchors.horizontalCenter: parent.horizontalCenter
        labelText: "Current [-15]"
        labelSize: 14
    }

    Text {
        id: date_time
        x: 1077
        y: 713
        width: 173
        height: 50
        color: "black"
        text: qsTr("Text")
        font.pixelSize: 16
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    MessageDialog {
        id: saveDialog
        title: "Save File"
        icon: StandardIcon.Question
        text: "Are you sure you want to save the parameters?"
        standardButtons: StandardButton.Yes | StandardButton.No
        modality: Qt.WindowModal
        onYes: {
            const data_list = [gp.display, gn.display, zp.display, zn.display, p180.display, n180.display, natural_freq, cutoff_freq, over_shoot]
            files.xlsx_save(serialNum.text, data_list)
        }
        onNo: console.log("didn't save")
    }
    MessageDialog {
        id: errorDialog
        title: "Error"
        icon: StandardIcon.Critical
        text: "An error has occurred."
        standardButtons: StandardButton.Ok
        detailedText: ""
        modality: Qt.WindowModal

    }
    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        selectExisting: true
        nameFilters: [ "Excel files (*.xlsx)", "All files (*)" ]
        onAccepted: {
//            console.log("Selected file: ",fileDialog.fileUrls[0].toLocalFile())
            export_xlsx_path = decodeURIComponent(fileDialog.fileUrls[0])
            console.log("You chose: " + export_xlsx_path)
            folderDialog.open()
//            Qt.quit()
        }
        onRejected: {
            console.log("Canceled")
            export_xlsx_path = ""
//            Qt.quit()
        }
    }
   FileDialog  {
        id: folderDialog
        title: "Please choose a Directory"
        folder: shortcuts.home
        selectExisting: true
        selectFolder: true
        onAccepted: {
            console.log("You chose: " + folderDialog.fileUrls)
            fileDialog.fileUrls
            files.file_export(fileDialog.fileUrls[0],  folderDialog.fileUrls[0])
//            Qt.quit()
        }
        onRejected: {
            console.log("Canceled")
            export_xlsx_path = ""
//            Qt.quit()
        }
    }




}







/*##^##
Designer {
    D{i:5;anchors_x:5;anchors_y:5}
}
##^##*/
