import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtMultimedia 5.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

Window {
    objectName: "mainWindow"
    visible: true
    width: 640
    height: 480


    //背景图片
    Image {
        id: backGrounp
        x:0
        y:7
        width: parent.width
        height: parent.height
        source: myPlay.backGround ? "file:///"+myPlay.backGround : ""
        smooth: true
    }

    //换肤按钮
    Button{
        x:10
        y:10
        id:changeSkinBtn
        width: 50
        height: 50
        contentItem: Image {
            source: "qrc:///Image/image.jpg"
            width: 50
            height: 50
            fillMode: Image.PreserveAspectFit
        }
        background: Rectangle{
            implicitWidth: 50
            implicitHeight: 50
            color: changeSkinBtn.down ? "#265F99":"#000000"
            radius:6
        }
        onClicked: {
            chooseSkin.open()
        }
    }

    //换肤对话框
    FileDialog{
        id:chooseSkin
        title:qsTr("换肤")
        folder: myPlay.backGroundChoose ? "file:///" +myPlay.backGroundChoose : shortcuts.desktop
        nameFilters: [qsTr("Image files(*.png *.jpg)"),qsTr("All files(*)")]
        onAccepted: {
            console.log(backGrounp.source)
            backGrounp.source = chooseSkin.fileUrl

            console.log("你选择的是："+chooseSkin.fileUrl.toString().substring(8,chooseSkin.fileUrl.length))
//            myPlay.changeBackground(chooseSkin.fileUrl.toString().substring(8,chooseSkin.fileUrl.length))
            myPlay.changeBackground(chooseSkin.fileUrl.toString().substring(8,chooseSkin.fileUrl.length))

            console.log("你选择的是："+chooseSkin.fileUrl)
        }
        onRejected: {
            console.log("你取消了，并没选择")
        }
    }


}
