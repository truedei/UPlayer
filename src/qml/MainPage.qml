import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import cn.truedei.UPlayer.UPlayer 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

import "./mediaplay/"
import "./record/"
import "./director/"

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 800+200
    height: 650+200
    minimumHeight: 650+200
    minimumWidth: 800+200
    title: titleStr
//    color: "black"
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


    // 1. Loader加载不同组件，实现切换页面的功能
    Loader{
        id:myLoader
//        anchors.centerIn: parent // 弹出的界面都居中显示
    }

    // 界面启动时信号
    Component.onCompleted:
    {
        myLoader.sourceComponent = mediaPlayMainPage // 一开始显示登录页面
//        myLoader.sourceComponent = recordMain // 一开始显示登录页面
//        myLoader.sourceComponent = directorMain
    }

    // 界面销毁时信号
    Component.onDestruction: {
        console.log("xiaohui manpage")
    }

    // c
    menuBar:MenuBar{
        Menu {
            title: qsTr('功能列表')
            Action {
                text: "1.简单播放器"
                onTriggered: {
                    print("打开 MediaPlay Page 。。。")
                    rootWindow.title = "简单播放器"
                    myLoader.sourceComponent = mediaPlayMainPage
                }
            }
            Action {
                text: "2.录像拍照"
                onTriggered: {
                    print("打开 录像拍照 。。。")
                    rootWindow.title = "录像拍照"
                    myLoader.sourceComponent = recordMain
                }
            }

            Action {
                text: "3.导播器"
                onTriggered: {
                    print("打开 导播器 。。。")
                    rootWindow.title = "导播器"
                    myLoader.sourceComponent = directorMain
//                    myLoader.sourceComponent = recordMain

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

//        Connections {
//            target: open_other
//            onTriggered: {
//                print("open_other ... ")
//            }
//        }
    }

    header:ToolBar{
        id:h_toobar
        visible: false
        Row{
           ToolButton{
               text: 'MediaPlay'
                onClicked: {
                    print("打开 MediaPlay Page 。。。")
                    myLoader.sourceComponent = mediaPlayMainPage
                }
           }
           ToolButton{
               text: '编辑'
               onClicked: {
                   print("编辑...1111")
                   myLoader.sourceComponent = loginPage
               }
           }
        }
    }


    //内容区域
    function getPageHeight()
    {
        if(h_toobar.visible)
        {
            return rootWindow.height - 90;
        }

        return rootWindow.height - 50;
    }

    function getPageWidth()
    {
        return rootWindow.width;
    }

    // Media Play Page
    Component{
        id:mediaPlayMainPage
        MediaPlayMain {
            width: {return getPageWidth();}
            height: {return getPageHeight();}
        }
    }

    // Record Page
    Component{
        id:recordMain

        RecordMain {
            width: {return getPageWidth();}
            height: {return getPageHeight();}
        }
    }

    // Director Page
    Component{
        id:directorMain

        DirectorMain {
            width: {return getPageWidth();}
            height: {return getPageHeight();}
        }
    }


    //下部区域，开始、暂停，快进，快退，声音大小，进度条等。


//    footer : StatusBar{
//        visible: false
//        Row{
//           Label
//           {
//               text: '@ZhengHui'
//               font.italic: true
//               color: 'red'
//           }
//       }
//    }
}
