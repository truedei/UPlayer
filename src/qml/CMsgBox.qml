//CMsgBox.qml
import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3


Window {
    id: root

    property var mb_OK      : 0x01 << 2//0x00000400
    property var mb_SAVE    : 0x01 << 3//0x00002000
    property var mb_YES     : 0x01 << 4//0x00004000
    property var mb_NO      : 0x01 << 5//0x00010000
    property var mb_CANCEL  : 0x01 << 6//0x0008000
    property var mb_RELOGIN : 0x01 << 7

    property var message: ""    //消息框的消息文本
    property var btnType: 0x01  //按钮组合类型
    property var retValue: 0x01 //关闭对话框后点击的按钮类型

    modality: Qt.ApplicationModal
    flags: Qt.FramelessWindowHint | Qt.Window

    width: 313
    height: 150
    color: "transparent"
    Rectangle{
        anchors.fill: parent
        anchors.margins: 4
//        color: "#23518b"
        radius: 5
        Item{
            id:id_item_header
            anchors.left:parent.left
            anchors.top:parent.top
            anchors.right: parent.right
            height: 40
            Text {
                id: id_img_icon
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
                text: qsTr("关于")
//                color: "white"
            }
            Button {
                id: img
                text: "x"
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                width: 30
                height: 30
                onClicked: checkd(mb_CANCEL)
            }
            //拖动窗口
            MouseArea {
                property point clickPos
                property bool isPressed: false
                anchors.left: parent.left
                anchors.top:parent.top
                anchors.bottom: parent.bottom
                anchors.right: img.left
                onPressed: {
                    isPressed = true
                    clickPos = Qt.point(mouse.x, mouse.y)
                }
                onReleased: isPressed = false
                onPositionChanged: {
                    var delta = Qt.point(mouse.x - clickPos.x,
                                         mouse.y - clickPos.y)

                    var tmpX = root.x + delta.x
                    var tmpY = root.y + delta.y
                    if(tmpX + root.width > 25 && Screen.desktopAvailableWidth - tmpX > 25)
                        root.x = tmpX
                    if(tmpY + root.height > 25 && Screen.desktopAvailableHeight - tmpY > 25)
                        root.y = tmpY

                }
            }
        }
        Rectangle {
            id:id_item_body
            anchors.left: parent.left
            anchors.top: id_item_header.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            Keys.onPressed: root.close()
//            color: "#3568a7"
            radius:parent.radius
            Item{
                id:id_rect_msg
                anchors.fill: parent
                anchors.bottomMargin: 55
                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 30
                    anchors.rightMargin: 30
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
//                    color: "white"
                    text: qsTr(message)
                }
            }
            RowLayout{
                property var objs: []
                id:id_layout
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: id_rect_msg.bottom
                anchors.bottom: parent.bottom
            }
        }
    }
    onBtnTypeChanged: create(root.btnType)

    Component
    {
        id:id_btn_comp
        Button{
            property var type: 0x01
            onClicked: checkd(type)
        }
    }

    function clear()
    {
        var len = id_layout.objs.length
        for(var i=0;i<len;i++)
        {
            id_layout.objs[i].destroy()
        }
        id_layout.objs = []
        root.btnType = 0x01
    }

    function checkd(_type)
    {
        root.retValue = _type
        //clear()//可不要，因为对话框关闭后会被销毁释放,单例模式下对话框关闭不会被销毁释放
        root.close()
    }

    function add_one(type,text)
    {
        var obj = id_btn_comp.createObject(id_layout, {"Layout.alignment" : Qt.AlignHCenter, "Layout.preferredWidth" : 60, "Layout.preferredHeight" : 30,"text" : text})
        obj.type = type
        //id_layout.objs.push(obj)//可不要，这句是在单例模式下使用的（对话框只创建一次，后面每次打开都更新按钮）
    }

    function create(type)
    {
        if((type & root.mb_OK) === root.mb_OK)
        {
            add_one(root.mb_OK,"ok")
        }
        if((type & root.mb_SAVE) === root.mb_SAVE)
        {
            add_one(root.mb_SAVE,"save")
        }
        if((type & root.mb_YES) === root.mb_YES)
        {
            add_one(root.mb_YES,"yes")
        }
        if((type & root.mb_NO) === root.mb_NO)
        {
            add_one(root.mb_NO,"no")
        }
        if((type & root.mb_CANCEL) === root.mb_CANCEL)
        {
            add_one(root.mb_CANCEL,"cancel")
        }
        if((type & root.mb_RELOGIN) === root.mb_RELOGIN)
        {
            add_one(root.mb_RELOGIN,"relogin")
        }
    }
}

/*##^##
Designer {
    D{i:0;height:222;width:392}
}
##^##*/


