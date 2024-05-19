import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import cn.truedei.UPlayer.UPlayer 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

Rectangle {
    id: recordMain
    visible: true
    width: 800+100
    height: 650+100
    color: "black"
//    color: "transparent"

    //全局变量
    //标题栏按钮高度
    property int toolBarCloseH: 30
    property int toolBarCloseW: 30

    property string titleStr: "记录器"

    property int pixDens: Math.ceil(Screen.pixelDensity)
    property int itemWidth: 25 * pixDens
    property int itemHeight: 10 * pixDens
    property int scaledMargin: 2 * pixDens
    property int fontSize: 3 * pixDens

    property int play_state: 0

    property string btnAudioIcon_source:""

    UPlayer{
        id:uPlayer
    }

    AVRecordPainted
    {
        id: avRecordPainted
        mWidth: parent.width
        mHeight: parent.height
//        mShowType:123
        anchors.fill: parent
        anchors.topMargin:  20
        anchors.bottomMargin: 100
        anchors.rightMargin: 20
        anchors.leftMargin:  20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.centerIn: parent
        clip: false
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.maximumWidth: recordMain.width
        Layout.maximumHeight: recordMain.height
//        onTotalDurationChanged:{
////            playProgressBar.to = avRecordPainted.totalDuration; //s
//        }
    }


    // init page
    // 界面启动时信号
    Component.onCompleted:
    {

        console.log("jinru Recordmain.qml")
        //init select list
//        initLelectList();
//        avRecordPainted.startPlay();
//        avRecordPainted.setFileUrl("/dev/video0");
    }

    // 界面销毁时信号
    Component.onDestruction: {
        console.log("xiaohui Recordmain.qml")
        avRecordPainted.exit();
    }

//    AVRecordPainted{
//        id: avRecordPainted
//        mWidth: parent.width
//        mHeight: parent.height
//        anchors.fill: parent
//        anchors.topMargin:  20
//        anchors.bottomMargin: 100
//        anchors.rightMargin: 20
//        anchors.leftMargin:  20
//        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.centerIn: parent
//        clip: false
//        Layout.fillHeight: true
//        Layout.fillWidth: true
//        Layout.maximumWidth: recordMain.width
//        Layout.maximumHeight: recordMain.height
//        fileUrl: !uPlayer.getStrBackgroundPath() ? "" : uPlayer.getStrBackgroundPath()
//    }

    //下,控制栏
    Rectangle {
        id : controlBar
        width: recordMain.width
        height: 70+40
        color: "#262626";
//        color: "black"
//        anchors.bottomMargin: 500
        anchors.top: avRecordPainted.bottom
//        anchors.bottom: avRecordPainted.bottom
//        anchors.bottomMargin: -18
//        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
//        transformOrigin: Item.Bottom

        RowLayout {
            x:20
            y:20
            Button {
                text: "首选项"
                onClicked: {
                    var recordSettings_page = recordSettings.createObject(recordMain);
                    recordSettings_page.title = qsTr("首选项");
//                    recordSettings_page.contentText = qsTr("测试通过");
//                    recordSettings_page.open();
                }
            }
        }



        Label {
            id: groupBox
            width: 399
            height: 44
//            anchors.horizontalCenterOffset: -13
//            scale: 1
//            font.pointSize: 0
            //垂直居中
//            anchors.verticalCenter: parent.verticalCenter
//            anchors.verticalCenterOffset: 4
            //水平居中
            anchors.horizontalCenter:  parent.horizontalCenter
//            anchors.topMargin: 15
//            anchors.top: parent.top

            //开始录像
            Button {
                id: btnStartPlay
                width: toolBarCloseW
                height: toolBarCloseH
//                anchors.left: btnFastReversePlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    id:btnStartPlayIcon
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:/images/record_1.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("开始录像")
                    avRecordPainted.record();
                }
            }

            Button {
                id: btnStartSnap
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnStartPlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    id:btnStartSnapIcon
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:/images/snapshot_2.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("开始拍照")
                    avRecordPainted.photo();
                }
            }


        }


        GroupBox {
            id: timeGroupBox
//            anchors.left: groupBox.right
            x : recordMain.width - 100
            width: 399
            height: 0
            title: qsTr("")

            //时间
            Text {
                id: totalDuration
                color: "#12e47b"
                text: qsTr("00:00:00")
                font.pixelSize: 12
                Layout.maximumHeight: 10
                Layout.maximumWidth: 200
            }

        }

    }



    /* 提示对话框 */
     Component {
       id: recordSettings
       RecordSettings {
           width: 400-20-50
           height: 400
       }
     }

}
