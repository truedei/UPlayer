import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4 as Ctrl
import cn.truedei.UPlayer.UPlayer 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2



ApplicationWindow {
    id: rootWindow
    visible: true
    width: 800+200
    height: 650+200
    minimumHeight: 650+200
    minimumWidth: 800+200
    title: titleStr
    color: "black"
//    color: "transparent"

    //全局变量
    //标题栏按钮高度
    property int toolBarCloseH: 30
    property int toolBarCloseW: 30

    property string titleStr: "万能播放器"

    property int pixDens: Math.ceil(Screen.pixelDensity)
    property int itemWidth: 25 * pixDens
    property int itemHeight: 10 * pixDens
    property int scaledMargin: 2 * pixDens
    property int fontSize: 3 * pixDens

    property int play_state: 0

    property string btnAudioIcon_source:""


    menuBar:MenuBar{
        Menu {
            title: qsTr('&文件')
            Action {
                text: "打开文件"
                onTriggered: {
                    chooseVideo.open();
                }
            }

            MenuSeparator { }
            Menu {
                title: qsTr("其他")
                Action {
                    text: "Find Next"
                    id: open_other
                    }
                Action { text: "Find Previous" }
                Action { text: "Replace" }
            }

        }
        Menu {
            title: "帮助";
            MenuItem{
                text: "关于";
                onTriggered: {
                    console.log("guanyu~~~")
                    var dia = Qt.createComponent("CMsgBox.qml")
                    var dlg = dia.createObject(rootWindow)
//                    dlg.btnType = dlg.mb_OK | dlg.mb_NO | dlg.mb_CANCEL
                    dlg.message = "version 1.0.0 \n 作者:郑晖"
                    dlg.closing.connect(function ret(){
                        if(dlg.retValue === dlg.mb_YES)
                        {
                            //do something
                        }
                        else if(dlg.retValue === dlg.mb_NO)
                        {
                            //do something
                        }
                        else if(dlg.retValue === dlg.mb_CANCEL)
                        {
                            //do something
                        }

                        dlg.destroy()
                    })
                    dlg.show()
                }
            }
            MenuItem{
                text: "官网";
                onTriggered: {
                    Qt.openUrlExternally("http://www.truedei.com/");
                }
            }
        }

        Connections {
            target: open_other
            onTriggered: {
                print("open_other ... ")
            }
        }
    }

    header:ToolBar{
        visible: false
        Row{
           ToolButton{
               text: '打开'
                onClicked: {
                    print("打开。。。")
                }
           }
           ToolButton{
               text: '编辑'
           }
        }
    }


/*
    FileOpen {
        id: fileOpen
        state: "collapsed"
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: scaledMargin
        }
//        width: itemHeight + scaledMargin + 100
        z: 2
        opacity: 0.9

        states: [
            State {
                name: "expanded"
                PropertyChanges {
                    target: fileOpen
                    width: itemWidth * 1.5
                    opacity: 0.8
                }
            },
            State {
                name: "collapsed"
                PropertyChanges {
                    target: fileOpen
                    width: itemHeight + scaledMargin
                    opacity: 0.9
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation { target: fileOpen; property: "width"; duration: 100 }
                NumberAnimation { target: fileOpen; property: "opacity"; duration: 100 }
            }
        ]
    }
*/


    UPlayer{
        id:uPlayer
    }

    UVideoOutput
    {
        id: uVideoOutput
        mWidth: parent.width
        mHeight: parent.height
        anchors.fill: parent
        anchors.topMargin:  20
        anchors.bottomMargin: 60
        anchors.rightMargin: 20
        anchors.leftMargin:  20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.centerIn: parent
        clip: false
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.maximumWidth: rootWindow.width
        Layout.maximumHeight: rootWindow.height
        fileUrl: !uPlayer.getStrBackgroundPath() ? "" : uPlayer.getStrBackgroundPath()
        onTotalDurationChanged:{
            playProgressBar.to = uVideoOutput.totalDuration; //s
        }
        onTotalDurationStrChanged:{
            totalDuration.text = uVideoOutput.totalDurationStr; //"00:03:46"
        }
        onCurrPlayTimeChanged:{
            currDuration.text = uVideoOutput.currPlayTimeStr; //"00:00:01"
        }
        onCurrPlayTimeStrChanged:{
            playProgressBar.value = uVideoOutput.currPlayTime; //s
        }
        onUpdateCurrAVTitleChanged:{
            video_title.text = uVideoOutput.currAVTitle; //media title
            if(uVideoOutput.currAVTitle.length != 0)
            {
                rootWindow.title = titleStr + "@" + uVideoOutput.currAVTitle;
            }else{
                rootWindow.title = titleStr;
            }
        }
    }

    //中，内容区域
    Label {
        id: contentArea
        x: 8
        y: 46
        width: rootWindow.width
        height: rootWindow.height - controlBar.height - 10
        horizontalAlignment: Text.AlignLeft
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.bottomMargin: 5
    }


    //下,控制栏
    Rectangle {
        id : controlBar
        x: 0
        y: 576
        width: rootWindow.width
        height: 70
        color: "#262626";
//        anchors.bottomMargin: 500
        anchors.top: uVideoOutput.bottom
//        anchors.bottom: uVideoOutput.bottom
//        anchors.bottomMargin: -18
        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
        transformOrigin: Item.Bottom


        //自定义进度条
        /*
        ProgressBar {
            id: playProgressBar
            x: 0
            y: 2
            value: 0.0
            padding: 2
            width: controlBar.width   //进度条控件占用长度
            height: 10  //进度条控件占用宽度
            anchors.rightMargin: -20
            anchors.leftMargin: -20
            background: Rectangle {
               implicitWidth: 200
               implicitHeight: 6
               color: "#e6e6e6"
               radius: 3
            }
            contentItem: Item {
                y: -4
                implicitWidth: 200
                implicitHeight: 4

               Rectangle {
                   width: playProgressBar.visualPosition * parent.width
                   height: parent.height
                   radius: 2
                   color: "#17a81a"
               }
            }

            onValueChanged: {
                console.log(playProgressBar.value)
            }
            onDataChanged: {
                console.log("onDataChanged")
            }

       }
       */
//        playProgressBar

        Slider {
            id: playProgressBar
            x: 0
            y: 2
            value: 0.0
            padding: 2
            width: controlBar.width   //进度条控件占用长度
            height: 10  //进度条控件占用宽度
            anchors.rightMargin: -20
            anchors.leftMargin: -20
            background: Rectangle {
               implicitWidth: 200
               implicitHeight: 6
               color: "#e6e6e6"
               radius: 3
            }
            contentItem: Item {
                y: -4
                implicitWidth: 200
                implicitHeight: 4

               Rectangle {
                   width: playProgressBar.visualPosition * parent.width
                   height: parent.height
                   radius: 2
                   color: "#17a81a"
               }
            }

//           MouseArea{
//                id:mouseScrollbar
//                anchors.fill: parent
////                onClicked: {
//////                    console.log(playProgressBar.value)
////                    var pt = mouseY < button.height ? 0 : (mouseY < sliderbar.height - button.height ? mouseY : sliderbar.height - button.height)
////                    list.contentY = pt / (sliderbar.height - button.height) * (list.contentHeight - list.visibleArea.heightRatio * list.contentHeight)
////                    console.log(pt)
////                }
//                // 拖动
//               onMouseYChanged: {
//                   console.log(playProgressBar.value)
//                   var pt = playProgressBar.y
////                   var contentY = pt / (playProgressBar.height - playProgressBar.height) * (playProgressBar.contentHeight - playProgressBar.visibleArea.heightRatio * playProgressBar.contentHeight)


//                   console.log(pt)
//               }
//            }

           // 鼠标区域
//          MouseArea {
//              id: mouseArea
//              anchors.fill: playProgressBar
//              drag.target: playProgressBar
//              drag.axis: Drag.YAxis
//              drag.minimumY: 0
//              drag.maximumY: playProgressBar.height - playProgressBar.height
//              // 拖动
//              onMouseYChanged: {
//                  var pt = playProgressBar.y
//                  //list.contentY = pt / (sliderbar.height - button.height) * (list.contentHeight - list.visibleArea.heightRatio * list.contentHeight)
//                  console.log(pt)
//                  console.log(mouseArea.x)
//                  console.log(mouseArea.y)
//              }
//          }

            onValueChanged: {

                if(uVideoOutput.currPlayTime != playProgressBar.value)
                {
                    console.log("gai bian le-->"+playProgressBar.value)
                    uVideoOutput.playSeek(playProgressBar.value)
                }
            }

        }


        GroupBox {
            id: timeGroupBox
            x: 166
            y: 27
            width: 399
            height: 0
            title: qsTr("")
            anchors.top: playProgressBar.bottom

            //时间
            Text {
                id: totalDuration
                x: -168
                y: 12
                color: "#12e47b"
                text: qsTr("00:00:00")
                font.pixelSize: 12
                Layout.maximumHeight: 10
                Layout.maximumWidth: 200
            }

            Text {
                id: element4
                x: -115
                y: 12
                width: 30
                height: 12
                color: "#f2f1f1"
                text: qsTr("/")
                font.pixelSize: 12
                Layout.maximumHeight: 10
                Layout.maximumWidth: 200
            }

            Text {
                    id: currDuration
                    x: -103
                    y: 12
                    color: "#f9f9f9"
                    text: qsTr("00:00:00")
                    font.pixelSize: 12
                    Layout.maximumHeight: 10
                    Layout.maximumWidth: 200
           }

        }


        Label {
            id: groupBox
            x: 166
            y: 30
            width: 399
            height: 44
            anchors.horizontalCenterOffset: -13
            scale: 1
            font.pointSize: 0
            //垂直居中
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 4
            //水平居中
            anchors.horizontalCenter:  parent.horizontalCenter
            anchors.topMargin: 15
            anchors.top: parent.top

            //停止按钮
            Button {
                id: btnStopPlay
                x: 16
                y: 14
                width: toolBarCloseW
                height: toolBarCloseH
                text: qsTr(" ")
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:/images/play_stop.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("停止播放")

                    //1、关闭正在播放的视频
                    uVideoOutput.stopPlay();

                    //2、显示选择视频文件的提示
                    rectangle.visible = true

                    //恢复背景
                    uVideoOutput.updateBackgroupImage(!uPlayer.getStrBackgroundPath() ? "" : uPlayer.getStrBackgroundPath())

                    //恢复时间
                    totalDuration.text = "00:00:00";
                    currDuration.text = "00:00:00";

                }
            }

            //快退按钮
            Button {
                id: btnFastReversePlay
                x: 97
                y: 13
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnStopPlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:/images/play_fast_reverse.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("快退")
                    uVideoOutput.playDown();
                }
            }



            //开始播放
            Button {
                id: btnStartPlay
                x: 97
                y: 13
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnFastReversePlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    id:btnStartPlayIcon
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:/images/play_pause.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("开始播放")

                    console.log("play_state1="+play_state)
                    play_state = uVideoOutput.startPlay();
                    console.log("play_state2="+play_state)

                    if(play_state === 1)
                    {
                        btnStartPlayIcon.source = "qrc:/images/play_pause.png"
                    }else if(play_state === 2){
                        btnStartPlayIcon.source = "qrc:/images/play_start.png"
                    }

                }
            }

            //快进播放
            Button {
                id: btnFastForwardPlay
                x: 97
                y: 13
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnStartPlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:/images/play_fast_forward.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("快进播放")
                    uVideoOutput.playUp();
                }
            }

            //声音图标
            Button {
                id: btnAudioIcon
                x: 97
                y: 9
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.left: btnFastForwardPlay.right
                text: qsTr(" ")
                anchors.leftMargin: 27
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: getVolumeIcon(volumeCtrl.value)
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    btnAudioIcon_source = btnAudioIcon.contentItem.source;

                    if(btnAudioIcon_source.indexOf("play_volume_close.png") !== -1)
                    {
                        //get curr audio volume value
                        btnAudioIcon.contentItem.source = getVolumeIcon(uVideoOutput.getVolume())
                    }

                    if(btnAudioIcon_source.indexOf("play_volume_low.png") !== -1 || btnAudioIcon_source.indexOf("play_volume_high.png") !== -1)
                    {
                        uVideoOutput.updateVolume(0);
                        btnAudioIcon.contentItem.source = "qrc:/images/play_volume_close.png"
                    }
                }
            }

            //音量控制条
            //自定义进度条
            Slider {
                id: volumeCtrl
                y: 19
                from: 0
                value: 0
                to: 128
                width: 100
                height: 15
                anchors.leftMargin: 16
                anchors.left: btnAudioIcon.right
                onMoved: {
                    var volume = this.value;
                    uVideoOutput.updateVolume(volume);
                    btnAudioIcon.contentItem.source = getVolumeIcon(volume)
                }
            }


            Button {
                id: btn_theme
                x: 458
                y: 8
                width: 32
                height: 30
                anchors.left: volumeCtrl.right
                text: qsTr(" ")
                anchors.leftMargin: 50
                anchors.bottomMargin: 10
                contentItem: Image {
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "qrc:/images/theme.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("换皮肤啊")
                    chooseTheme.open();
                }
            }

        }
    }

//    打开关闭图标
    function mouseEnter(flag){ //处理鼠标进入或者移出变色
        element.color = flag ? "cyan":"darkGray"
        rectangle.color = flag ? "#292727":"#333232"
    }


    function getVolumeIcon(volume)
    {
        if(volume === 0){
            return "qrc:/images/play_volume_close.png"
        }
        else if(volume >= 50){
            return "qrc:/images/play_volume_high.png"
        }else if(volume <= 50){
            return "qrc:/images/play_volume_low.png"
        }
        return "qrc:/images/play_volume_close.png"
    }



    Rectangle {
        id: rectangle
        x:contentArea.width / 2 - 120
        y:contentArea.height / 2
        width: 286
        height: 87
        color: "#333232"
        border.color: "#00000000"


        MouseArea {
            id: mouseId
            anchors.fill: parent
            hoverEnabled: true
            onEntered: { //监听鼠标进入
                mouseEnter(true);
            }
            onExited: { //监听鼠标移出
                 mouseEnter(false);
            }
            onClicked: {
                console.log("打开选择爱啦啦阿拉啦！")
                 chooseVideo.open();
            }
        }

        Image {
            id: image1
            x: 19
            y: 12
            width: 84
            height: 63
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/dir.png"
        }

        Text {
            id: element
            x: 109
            y: 19
            width: 162
            height: 50
            color: "#f7f7f7"
            text: qsTr("打开文件")
            font.pixelSize: 37
        }
    }

    //打开视频文件框
    FileDialog {
        id: chooseVideo
        title: qsTr("选择视频文件")
        selectExisting: true
        selectFolder: false
        selectMultiple: false
        nameFilters: [qsTr("Video files(*.mp4 *.flv)"),qsTr("All files(*)")]
        onAccepted: {
            console.log("You chose: " + chooseVideo.fileUrl.toString().substring(7,chooseVideo.fileUrl.length));

            //隐藏选择视频文件的提示
            rectangle.visible = false;

            uVideoOutput.urlPass(chooseVideo.fileUrl.toString().substring(7,chooseVideo.fileUrl.length));
        }
        onRejected: {
            console.log("Canceled");
        }
    }

    //换皮肤
    FileDialog {
        id: chooseTheme
        title: qsTr("选择背景图片")
        selectExisting: true
        selectFolder: false
        selectMultiple: false
        nameFilters: [qsTr("Image files(*.png *.jpg *jpeg)"),qsTr("All files(*)")]
        onAccepted: {
            console.log("You chose: " + chooseTheme.fileUrl.toString().substring(8,chooseTheme.fileUrl.length));
            uVideoOutput.updateBackgroupImage(chooseTheme.fileUrl.toString().substring(7,chooseTheme.fileUrl.length));
        }
        onRejected: {
            console.log("Canceled");
        }
    }


    //内容区域


    //下部区域，开始、暂停，快进，快退，声音大小，进度条等。


    footer : Ctrl.StatusBar{
        visible: false
        Row{
           Label
           {
               text: '@ZhengHui'
               font.italic: true
               color: 'red'
           }
       }
    }
}
