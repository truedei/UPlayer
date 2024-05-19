import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

import QtQuick.Layouts 1.12
import cn.truedei.UPlayer.UPlayer 1.0

ApplicationWindow {
    id: rootWindow
    visible: true
    property UVideoOutput uVideoOutput: null
    width: 400-20
    height: 400 + 20
    minimumHeight: 400
    minimumWidth: 400
    title: "首选项"
//    color: "#262626";
    flags: Qt.WindowMinimizeButtonHint | Qt.WindowCloseButtonHint

    AVRecordConnect{
        id:avRecordConnect
    }


    function initLelectList()
    {
//        for (var i = 0; i < 10; i++) {
//            camDevicesList.append({text: "i="+i})
//        }

        var jsonStr = avRecordConnect.getAllVideoDeviceList();

        console.log("jsonStr="+ jsonStr);

        var camData = JSON.parse(jsonStr);

        for(var index in camData)
        {
            let camObj = camData[index];
            camDevicesList.append(
                        {text: camObj["camName"]}
                        )


            console.log("camName="+ camObj["camName"]);
            var vfmts = camObj["vfmts"];

            for(var v_i in vfmts)
            {
                var description = vfmts[v_i]["description"];
                console.log("description="+ description);

                encoderList.append({text:description});
            }

            //
            var oneFrmsizes = vfmts[0]["frmsizes"];
            for(var v_j in oneFrmsizes)
            {
                var height = oneFrmsizes[v_j]["height"];
                var width = oneFrmsizes[v_j]["width"];

                videoFrmsizesList.append({text:( width+"x"+height)});
                snapFrmsizesList.append({text:( width+"x"+height)});
            }
        }

        camDevicesBox.currentIndex = 0;
        encoderBox.currentIndex = 0;
        snapFrmsizesBox.currentIndex =0;
        videoFrmsizesBox.currentIndex=0;
    }


    function saveData()
    {
        console.log("########## save data ############")

        console.log("camDevicesBox.currentIndex="+camDevicesBox.currentIndex)
        console.log("encoderBox.currentIndex="+encoderBox.currentIndex)
        console.log("snapFrmsizesBox.currentIndex="+snapFrmsizesBox.currentIndex)
        console.log("videoFrmsizesBox.currentIndex="+videoFrmsizesBox.currentIndex)
    }

    // init page
    Component.onCompleted: {
        //init select list
        initLelectList();
    }

    Frame {
        id: frame
        x: 18
        y: 15
        width: 368
        height: rootWindow.height - 30

        TabBar {
            id: bar
            width: parent.width

            Repeater {
                model: ["摄像头", "图像", "拍照"]
                TabButton {
                    text: modelData
                    width: Math.max(100, bar.width / 5)
                }
            }
        }

        StackLayout {
            id: sLayout
            width: parent.width
            anchors.top: bar.bottom
            currentIndex: bar.currentIndex
            Item {
                id: homeTab

                Text {
                    id: shebei
                    x: 8
                    y: 42
                    width: 60
                    height: 36
                    text: qsTr("视频设备")
                    lineHeight: 1.1
                    font.pointSize: 13
                    minimumPointSize: 17
                    minimumPixelSize: 16
                }

                ComboBox {
                    id: camDevicesBox
                    x: 125
                    y: 38
                    width: 177
                    height: 40
                    font.pointSize: 13
                    editable: false

                    model: ListModel {
                        id: camDevicesList
                    }

                    onAccepted: {

                    }
                    onCurrentIndexChanged: {
                        console.log("视频设备 selection index="+currentIndex)
                        saveData();
                    }
                }

                Text {
                    id: shebei22
                    x: 8
                    y: 42 + 40 + 20
                    width: 60
                    height: 36
                    text: qsTr("编码格式")
                    lineHeight: 1.1
                    font.pointSize: 13
                    minimumPointSize: 17
                    minimumPixelSize: 16
                }

                ComboBox {
                    id: encoderBox
                    x: 125
                    y: 38 + 40 + 20
                    width: 177
                    height: 40
                    font.pointSize: 13
                    editable: false

                    model: ListModel {
                        id: encoderList
                    }

                    onAccepted: {
                        console.log("请选择编码格式")
                    }

                    onCurrentIndexChanged: {
                        console.log("编码格式 selection index="+currentIndex)
                        saveData();

                        var camData = JSON.parse(avRecordConnect.getAllVideoDeviceList());

                        videoFrmsizesList.clear();
                        snapFrmsizesList.clear();

                        var vfmts = camData[camDevicesBox.currentIndex]["vfmts"];

                        var v_i = 0;
                        for(v_i in vfmts)
                        {
                            if(v_i == currentIndex)
                            {
                                break;
                            }
                        }

                        var oneFrmsizes = vfmts[v_i]["frmsizes"];

                        for(var v_j in oneFrmsizes)
                        {
                            var height = oneFrmsizes[v_j]["height"];
                            var width = oneFrmsizes[v_j]["width"];

                            videoFrmsizesList.append({text:( width+"x"+height)});
                            snapFrmsizesList.append({text:( width+"x"+height)});
                        }

                        videoFrmsizesBox.currentIndex=0;
                        snapFrmsizesBox.currentIndex =0;
                    }
                }

                Text {
                    id: shebei1
                    x: 8
                    y: 103+ 40+ 20
                    width: 99
                    height: 36
                    text: qsTr("视频分辨率")
                    lineHeight: 1.1
                    font.pointSize: 13
                    minimumPointSize: 17
                    minimumPixelSize: 16
                }

                ComboBox {
                    id: videoFrmsizesBox
                    x: 125
                    y: 103+ 40+ 20
                    width: 177
                    height: 40
                    font.pointSize: 13
                    editable: false
                    model: ListModel {
                        id: videoFrmsizesList
                    }
                    onAccepted: {
                    }
                    onCurrentIndexChanged: {
                        console.log("视频分辨率 selection index="+currentIndex)
                        saveData();
                    }
                }

                Text {
                    id: shebei2
                    x: 8
                    y: 166+ 40+ 20
                    width: 99
                    height: 36
                    text: qsTr("照片分辨率")
                    lineHeight: 1.1
                    font.pointSize: 13
                    minimumPointSize: 17
                    minimumPixelSize: 16
                }

                ComboBox {
                    id: snapFrmsizesBox
                    x: 125
                    y: 164+ 40+ 20
                    width: 177
                    height: 40
                    font.pointSize: 13
                    editable: false
                    model: ListModel {
                        id: snapFrmsizesList
                    }
                    onAccepted: {
                    }
                    onCurrentIndexChanged: {
                        console.log("照片分辨率 selection index="+currentIndex)
                        saveData();
                    }
                }
            }
            Item {
                id: discoverTab
                Text {
                    id: discoverTab2
                    text: qsTr("图像")
                }
            }
            Item {
                id: activityTab
                Text {
                    id: activityTa3
                    text: qsTr("拍照")
                }
            }
        }

    }




}



