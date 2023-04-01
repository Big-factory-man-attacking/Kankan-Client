import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

Rectangle {
    anchors.fill: parent
    color: "white"
    property string manuscript_id: ""
    property var comments: ({})

    function getComment() {
        commentListModel.clear()
        for (var i = 0; i < comments.length; i++) {
            console.log("123")
            commentListModel.append({headSource: comments[i]["netizenInfo"]["headPortrait"], nickname: comments[i]["netizenInfo"]["nickname"], comment: comments[i]["text"], commentDate: new Date()});
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 5
        RowLayout {
            id: titleRow
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 40
            spacing: 15
            Button {
                id: to_leftButton
                icon.source: "qrc:to_left.png"
                icon.width: 35
                icon.height: 30
                background: Rectangle {
                    color: "white"
                }
                onClicked: {
                    commentManger.visible = false
                }
            }
            Text {
                text: qsTr("评论管理")
                font.pixelSize: 16
                Layout.alignment: Qt.AlignHCenter
            }
            Button {
                id: publishButton
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.rightMargin: 10
                icon.source: "qrc:catalog.png"
                icon.width: 25
                icon.height: 30
                background: Rectangle {
                    color: "white"
                    radius: 5
                }
            }
        }
        ToolSeparator {
            Layout.preferredHeight: 1
            Layout.preferredWidth: parent.width
            Layout.alignment: Qt.AlignHCenter
            contentItem: Rectangle{
                anchors.fill: parent
                color: "#cccccc"
            }
        }
        ScrollView {
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height-titleRow.height-11
            clip: true
            ScrollBar.horizontal: ScrollBar{ visible: true }
            ListView {
                id: commentListView
                anchors.fill: parent
                boundsBehavior: Flickable.StopAtBounds
                model: commentListModel
                delegate: commentDelegate
                spacing: 10
            }
        }
    }
    Component {
        id: commentDelegate
        ColumnLayout {
            width: commentListView.width
            spacing: 8
            Rectangle {
                Layout.preferredWidth: parent.width-20
                Layout.preferredHeight: 70
                Layout.alignment: Qt.AlignHCenter

                Rectangle {
                    id: img
                    height: 40
                    width: 40
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    radius: width/2
                    border.width: 1
                    border.color: "#707070"
                    Image {
                        id: image
                        smooth: true
                        visible: false
                        anchors.fill: parent
                        source: headSource
                        antialiasing: true
                    }
                    Rectangle {
                        id: mask
                        anchors.fill: parent
                        radius: width/2
                        visible: false
                        antialiasing: true
                        smooth: true
                    }
                    OpacityMask {
                        id:mask_image
                        anchors.fill: image
                        source: image
                        maskSource: mask
                        visible: true
                        antialiasing: true
                    }
                }
                ColumnLayout {
                    anchors.left: parent.left
                    anchors.leftMargin: 20+mask_image.width
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 10
                    Text {
                        id: nicknameT
                        text: nickname
                        font.pointSize: 12
                    }
                    Text {
                        id: commentTime
                        text: Qt.formatDateTime(commentDate, "yyyy-MM-dd hh:mm")
                    }
                }
            }
            Text {
                Layout.preferredWidth: parent.width-30
                Layout.alignment: Qt.AlignHCenter
                wrapMode: Text.WrapAnywhere
                text: comment
                font.pixelSize: 14
            }
            Button {
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 15
                icon.source: "qrc:delete.png"
                icon.width: 16
                icon.height: 20
                background: Rectangle {
                    color: "white"
                }
                onClicked: {
                    //删除该稿件中的这条评论
                    videoSocialControl.deleteComment(manuscript_id, comments[index]["id"])
                    comments.splice(index, 1)
                    for (var i = 0; i < manuscripts.length; i++) {
                        if (manuscripts[index]["id"] === manuscript_id) {
                            manuscripts[index]["comments"] = comments
                            mainPage.personalPage.netizen["videos"] = manuscripts
                        }
                    }
                    for (var j = 0; j < mainPage.homePage.manuscripts.length; j++) {
                        if (mainPage.homePage.manuscripts[j]["manuscript"]["id"] === manuscript_id) {
                            mainPage.homePage.manuscripts[j]["manuscript"] = manuscripts[index]
                            mainPage.homePage.getVideoInfo()
                        }
                    }
                    commentListModel.remove(commentListView.currentIndex, 1)
                }
            }
        }
    }
    ListModel {
        id: commentListModel
    }
}
