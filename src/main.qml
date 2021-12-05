import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import cn.truedei.UPlayer.UPlayer 1.0


Window {

    id : rootWindow
    visible: true
    width: 800
    height: 650
    title: qsTr("万能播放器")
    //去掉默认的标题栏
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "black"
    onTitleChanged: rootWindowToolBarTitleText.text = title

    //全局变量
    property bool isMoveWindow: false
    property point rootWindowTitleMousePos: Qt.point(x,y)

    //标题栏高度
    property int toolBarHeight: 40
    //标题栏按钮高度
    property int toolBarCloseH: 30
    property int toolBarCloseW: 30


    UPlayer{
        id:uPlayer
    }


    //背景
//    Image {
//        id: rootImage
//        objectName: rootImage
//        anchors.topMargin: 46
//        anchors.bottomMargin: 74
//        anchors.fill: parent
//        clip: false
//        Layout.fillHeight: true
//        Layout.fillWidth: true
//        Layout.maximumWidth: rootWindow.width
//        Layout.maximumHeight: rootWindow.height
//        fillMode: Image.PreserveAspectFit
//        source: !uPlayer.getStrBackgroundPath() ? "" : uPlayer.getStrBackgroundPath()
//        visible: true
//        cache: false
//     }


//    CircleHead {     // 接着我们就可以使用"PieChart"这个类型
//        id: aPieChart
////        x: 120
////        y: 120
//        anchors.centerIn: parent
//        width: 300
//        height: 300
//        anchors.topMargin: 100
//        anchors.bottomMargin: 100
//        anchors.fill: parent
////        source: ":/images/cat.jpg"
//        source: !uPlayer.getStrBackgroundPath() ? "" : uPlayer.getStrBackgroundPath()
//    }


//    MYVideoOutput {
//        id: videooutput
//        anchors.fill: parent
//        anchors.topMargin: 46
//        anchors.bottomMargin: 74
//        Layout.fillHeight: true
//        Layout.fillWidth: true
//        Layout.maximumWidth: rootWindow.width
//        Layout.maximumHeight: rootWindow.height
//        visible: true
//    }

    UVideoOutput
    {
        id: uVideoOutput
        width: parent.width
        height: parent.height
        mWidth: parent.width
        mHeight: parent.height
        anchors.topMargin: 46
        anchors.bottomMargin: 74
        anchors.fill: parent
        clip: false
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.maximumWidth: rootWindow.width
        Layout.maximumHeight: rootWindow.height
//        fileUrl: ":/Image/image.jpg"
        fileUrl: !uPlayer.getStrBackgroundPath() ? "" : uPlayer.getStrBackgroundPath()
    }
    property double zoomVlaue: 1.0
    signal drawImage()

    MouseArea
    {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onClicked: {
            if(mouse.button === Qt.LeftButton)
            {
                drawImage();
            }
        }
        onWheel: {
            var m_value = wheel.angleDelta.y/120;
            if(m_value>0)
            {
                if(zoomVlaue<1.0){
                zoomVlaue += 0.1;
                }
            }else
            {
                if(zoomVlaue>0.0){
                zoomVlaue -= 0.1;
                }
            }
            console.log("wheel event=,zoom=",m_value,zoomVlaue);
            uVideoOutput.zoomDraw(zoomVlaue);
        }
    }

    onDrawImage: {
        console.log("mouse clicked!");
        uVideoOutput.zoomDraw();
    }




    //上 标题栏
    //自定义标题栏
    Rectangle {
            id : rootWindowToolBar
            width: rootWindow.width
            height: rootWindow.toolBarHeight
            color: "#262626";

            //播放器小图标
            Image {
                id: image
                width: 43
                height: 40
                fillMode: Image.PreserveAspectFit
                source: "Image/u_player_logo.ico"
            }

            //播放器名字
            Text {
                id: rootWindowToolBarTitleText1
                x: 48
                y: 0
                color: "#ffffff"
                text: "万能播放器"
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 1
            }

            //电视名字
            Text {
                id: rootWindowToolBarTitleText
                //x: rootWindowToolBar.width / 2 - 90
                color: "white"
                text: "超高清航拍大美中国1080P.MP4"
                //垂直居中
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 1
               //水平居中
                anchors.horizontalCenter:  parent.horizontalCenter
            }

            // 为窗口标题栏添加鼠标事件
            MouseArea {
                anchors.rightMargin: 0
                anchors.bottomMargin: 2
                anchors.leftMargin: 0
                anchors.topMargin: -2
                anchors.fill: rootWindowToolBar
                // 只处理鼠标左键
                acceptedButtons: Qt.LeftButton
                // 接收鼠标按下事件
                onPressed: {

                    rootWindow.rootWindowTitleMousePos = Qt.point(mouseX,mouseY)
                    rootWindow.isMoveWindow = true
                }

                // 鼠标释放，关闭窗口移动flag
                onReleased: {
                    if(mouse.button === Qt.LeftButton){
                        rootWindow.isMoveWindow = false
                    }
                }

                //
                onMouseXChanged: {
                    if(rootWindow.isMoveWindow){
                        rootWindow.x += mouseX - rootWindow.rootWindowTitleMousePos.x;
                    }
                }
                onMouseYChanged: {
                    rootWindow.y += mouseY - rootWindow.rootWindowTitleMousePos.y;
                }
            }

            //ToolBar 关闭按钮
            Button {
                id : pushbtnWindowsClose
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.margins: 5
                anchors.right: rootWindowToolBar.right
                anchors.topMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                text: "X"
                contentItem: Image {
                    width: parent.width
                    height: parent.height
                    source: "Image/window_close.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    Qt.quit();
                }
            }

            //ToolBar 最大化按钮
            Button {
                id : pushbtnWindowsMaximize
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.margins: 5
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: pushbtnWindowsClose.left
                text: "口"
                contentItem: Image {
                    width: parent.width
                    height: parent.height
                    source: "Image/window_max.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    //最大化切换
                    if(rootWindow.visibility === Window.Maximized)
                    {
                        rootWindow.visibility = Window.Windowed;
                    }else{
                        rootWindow.visibility = Window.Maximized;
                    }
                }
            }

            //ToolBar 最小化按钮
            Button {
                id : pushbtnWindowsMinimize
                width: toolBarCloseW
                height: toolBarCloseH
                anchors.margins: 5
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: pushbtnWindowsMaximize.left
                text: "一"
                contentItem: Image {
                    width: parent.width
                    height: parent.height
                    source: "Image/window_min.png"
                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    //最小化
                    rootWindow.visibility = Window.Minimized
                }
            }



    }


    //中，内容区域
    Label {
        id: contentArea
        x: 9
        y: 46
        width: rootWindow.width - 15
        height: rootWindow.height - controlBar.height - rootWindowToolBar.height - 10
        anchors.top: rootWindowToolBar.bottom
        anchors.topMargin: 5
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.bottomMargin: 5

    }

    //下,控制栏
    Rectangle {
        id : controlBar
        x: 0
        y: 582
        width: rootWindow.width
        color: "#262626";
        anchors.bottom: contentArea.bottom
        anchors.bottomMargin: -74
        height: 68
        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
        transformOrigin: Item.Bottom


        //自定义进度条
        Slider {
            id: slider
            x: 8
            y: 5
            from: 1
            value: 25
            to: 100
            width: rootWindow.width - 20
            height: 23
            anchors.rightMargin: -20
            anchors.leftMargin: -20
        }

        GroupBox {
            id: timeGroupBox
            x: 166
            y: 27
            width: 399
            height: 0
            title: qsTr("")
            anchors.top: slider.bottom

            //时间
            Text {
                id: element3
                x: -168
                y: 7
                color: "#12e47b"
                text: qsTr("00:01:34")
                font.pixelSize: 12
                Layout.maximumHeight: 10
                Layout.maximumWidth: 200
            }

            Text {
                id: element4
                x: -114
                y: 7
                width: 30
                height: 12
                color: "#f2f1f1"
                text: qsTr("/")
                font.pixelSize: 12
                Layout.maximumHeight: 10
                Layout.maximumWidth: 200
            }

            Text {
                    id: element5
                    x: -103
                    y: 7
                    color: "#f9f9f9"
                    text: qsTr("00:01:34")
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
            anchors.verticalCenterOffset: 10
            //水平居中
            anchors.horizontalCenter:  parent.horizontalCenter
            anchors.topMargin: 21
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
                    source: "Image/play_stop.png"
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

                    //2、显示选择视频文件的提示
                    rectangle.visible = true

                    uVideoOutput.StopPlay();


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
                    source: "Image/play_fast_reverse.png"
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
                    width: 48
                    height: 48
                    transformOrigin: Item.Bottom
                    source: "Image/play_start.png"
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

                    uVideoOutput.StartPlay();
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
                    source: "Image/play_fast_forward.png"
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
                    source: {
                        console.log("volumeCtrl.value="+volumeCtrl.value)

                        if(volumeCtrl.value == 0){
                            "Image/play_volume_close.png"
                        }
                        else if(volumeCtrl.value >= 50){
                            "Image/play_volume_high.png"
                        }else if(volumeCtrl.value <= 50){
                            "Image/play_volume_low.png"
                        }
                    }

                    fillMode: Image.PreserveAspectFit
                }
                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    color: "#262626"
                    radius: 6
                }
                onClicked: {
                    console.log("声音图标")
                }
            }

            //音量控制条
            //自定义进度条
            Slider {
                id: volumeCtrl
                y: 19
                from: 0
                value: 25
                to: 100
                width: 100
                height: 15
                anchors.leftMargin: 16
                anchors.left: btnAudioIcon.right
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
                    source: "Image/theme.png"
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

    //打开关闭图标
    function mouseEnter(flag){ //处理鼠标进入或者移出变色
        element.color = flag ? "cyan":"darkGray"
        rectangle.color = flag ? "#292727":"#333232"
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
            source: "Image/dir.png"
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

//        folder: myplay.videoPath ? "file:///" + myplay.videoPath : shortcuts.desktop     //默认路径桌面
        selectExisting: true
        selectFolder: false
        selectMultiple: false
        nameFilters: [qsTr("Video files(*.mp4 *.flv)"),qsTr("All files(*)")]
        onAccepted: {
//            myplay.urlPass(chooseVideo.fileUrl.toString().substring(8,chooseVideo.fileUrl.length))
//            myplay.changeVideoPath(chooseVideo.fileUrl.toString().substring(8,chooseVideo.fileUrl.length))
//            if(root.isPlaying == true)
//                isPlayBtnImg.source = "qrc:///image/isPause.png"
            console.log("You chose: " + chooseVideo.fileUrl.toString().substring(7,chooseVideo.fileUrl.length));

            //隐藏选择视频文件的提示
            rectangle.visible = false;

//            uPlayer.urlPass(chooseVideo.fileUrl.toString().substring(7,chooseVideo.fileUrl.length));
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
//        folder: myplay.videoPath ? "file:///" + myplay.videoPath : shortcuts.desktop     //默认路径桌面
        selectExisting: true
        selectFolder: false
        selectMultiple: false
        nameFilters: [qsTr("Image files(*.png *.jpg *jpeg)"),qsTr("All files(*)")]
        onAccepted: {
            console.log("You chose: " + chooseTheme.fileUrl.toString().substring(8,chooseTheme.fileUrl.length));

            //file:///C:/Users/Administrator/Pictures/Camera Roll/mv.jpg
            //立即显示
//            rootImage.source = chooseTheme.fileUrl;
//            uVideoOutput.fileUrl = chooseTheme.fileUrl;

//            rootImage.source= "file:///"+chooseTheme.fileUrl.toString().substring(8,chooseTheme.fileUrl.length)

            //保存到配置文件
//            uPlayer.updateBackgroupImage(chooseTheme.fileUrl);
//            uPlayer.updateBackgroupImage(chooseTheme.fileUrl.toString().substring(7,chooseTheme.fileUrl.length));
            uVideoOutput.updateBackgroupImage(chooseTheme.fileUrl.toString().substring(7,chooseTheme.fileUrl.length));

//            rootImage.source= "file:///C:/Users/Administrator/Pictures/Camera Roll/mv.jpg"
        }
        onRejected: {
            console.log("Canceled");
        }
    }


    //内容区域


    //下部区域，开始、暂停，快进，快退，声音大小，进度条等。



}



