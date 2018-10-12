import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import XVideo 1.0

ApplicationWindow {
    visible: true
    id: main;

    //用来适配手机屏幕的一些转换函数
    property real multiplierH: (main.height/640)*1000;
    property real multiplierW: (main.width/360)*1000;
     function dpH(numbers) {
        return numbers*multiplierH/1000;
     }
     function dpW(numbers) {
        return numbers*multiplierW/1000;
     }
     function dpX(numbers){
         return (dpW(numbers)+dpH(numbers))/2;
     }
    width: 640;
    height: 360;

    XVideo{
        id: player
        anchors.centerIn: parent;
        nWidth :640;//cpp中的宽和高
        nHeight: 360;
        width: 640;
        height: 360;
        strVideoPath: "d:/music.avi"
        onSig_currentTimeChanged: {
            text_current_time.text = str
             play_progress.value = parseFloat( player.nPosition ) / parseFloat( player.nDuration )
        }
        onSig_totalTimeChanged: {
            text_total_time.text = str
        }
    }
    Slider {
        id: play_progress
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        width: parent.width
        maximumValue: 1
        minimumValue: 0
        value: 0
        style: SliderStyle {
            groove: Rectangle {
                width: parent.width
                height: 4
                color: "gray"
                Rectangle {
                    height: 4
                    width: styleData.handlePosition
                    color: "dodgerblue"
                }
            }
            handle: Rectangle {
                width: 15
                height: 15
                radius: 7.5
                color: "white"
            }
        }
    }
    Button {
        id: btn_start
        text: "start/stop"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 20
        onClicked: {
            player.startPlay()
        }
    }

    Text {
        id: text_current_time
        anchors.verticalCenter: btn_start.verticalCenter
        anchors.left: btn_start.right
        anchors.leftMargin: 50
    }

    Text {
        text: "/"
        anchors.verticalCenter: btn_start.verticalCenter
        anchors.left: text_current_time.right
    }

    Text {
        id: text_total_time
        anchors.verticalCenter: btn_start.verticalCenter
        anchors.left: text_current_time.right
        anchors.leftMargin: 5
    }

    Button {
        text: "pause"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 20
        onClicked: {
            player.pausePlay()
        }
    }
}
