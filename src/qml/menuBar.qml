import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Controls 1.4 as Ctrl14

ApplicationWindow {
    id: root
    width: 960
    height: 720
    visible: true
    title: qsTr("QML MainWindow Temp")

    property var aboutDlg: null
    function showAbout()
    {
        if(aboutDlg == null)
        {
            aboutDlg = Qt.createQmlObject(
                        'import QtQuick 2.15;
                        import QtQuick.Dialogs 1.3;
                        MessageDialog
                        {
                            icon: StandardIcon.Information;
                            text: "这是一个MainWindow示例";
                            visible: true;
                        }',
                        root,
                        "aboutDlg");
            aboutDlg.accepted.connect(onAboutDlgClosed);
        }
    }
    function onAboutDlgClosed()
    {
        aboutDlg.destroy();
        aboutDlg = null;
    }
    //菜单栏
    menuBar: MenuBar
    {
        Menu
        {
            title: qsTr("&File")
            Action { text: qsTr("&New...") }
            Action { text: qsTr("&Open...") }
            Action { text: qsTr("&Save") }
            Action { text: qsTr("Save &As...") }
            MenuSeparator { }   //分割线
            Action
            {
                text: qsTr("&Quit")
                onTriggered: close()
            }
        }
        Menu
        {
            title: qsTr("&Edit")
            Action { text: qsTr("Cu&t") }
            Action { text: qsTr("&Copy") }
            Action { text: qsTr("&Paste") }
        }
        Menu
        {
            title: qsTr("&Help")
            Action
            {
                text: qsTr("&About")
                onTriggered:
                {
                    root.showAbout()
                }
            }
        }
    }
    //工具栏
    header: ToolBar
    {
        Row
        {
            spacing: 10
            ToolButton
            {
                text: 'ToolButton1'
                onClicked: console.log('Button1 Clicked.')
            }
            ToolButton
            {
                text: 'ToolButton2'
                onClicked: console.log('Button2 Clicked.')
            }
            ToolButton
            {
                text: 'ToolButton3'
                onClicked: console.log('Button3 Clicked.')
            }
        }
        background:Rectangle
        {
            anchors.fill:parent
            color:'#008888'
        }
    }
    //状态栏
    footer: Ctrl14.StatusBar
    {
        Row
        {
            Text
            {
                id: statusBar
                text: qsTr("MainWindow StatusBar")
                font.family: 'Arial'
                font.pixelSize: 16
                font.italic: true
                color: '#ff0088'
            }
        }
    }
}
