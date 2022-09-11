import QtQuick
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Controls

Item {
    id: delegateItem
    height: delegateLoader.height
    Component {
        id: commentDelegate
        ColumnLayout {
            id: commentColumn
            width: commentView.width
            spacing: 10
            Row {
                width: parent.width-20
                spacing: 10
                Layout.topMargin: 8
                Layout.alignment: Qt.AlignHCenter
                Rectangle {
                    id: headPortrait
                    height: 40
                    width: 40
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
                    spacing: 5
                    width: parent.width-headPortrait.width-10
                    Layout.alignment: Qt.AlignVCenter
                    Text {
                        text: nickname
                        color: "grey"
                        font.pixelSize: 14
                    }
                    Text {
                        text: Qt.formatDateTime(commentDate, "yyyy-MM-dd hh:mm")
                        color: "grey"
                        font.pixelSize: 12
                    }
                    Text {
                        Layout.preferredWidth: parent.width-10
                        Layout.topMargin: 5
                        text: comment
                        font.pixelSize: 16
                        wrapMode: Text.WrapAnywhere
                    }
                }
            }
            ToolSeparator {
                Layout.preferredHeight: 1
                Layout.preferredWidth: parent.width
                contentItem: Rectangle{
                    anchors.fill: parent
                    color: "#cccccc"
                }
            }
        }
    }
    Component {
        id: myCommentDelegate
        ColumnLayout {
            id: myCommentColumn
            width: commentView.width
            spacing: 10
            Row {
                width: parent.width-20
                spacing: 10
                Layout.topMargin: 8
                Layout.alignment: Qt.AlignHCenter
                Rectangle {
                    id: headPortrait
                    height: 40
                    width: 40
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
                    spacing: 5
                    width: parent.width-headPortrait.width-50
                    Layout.alignment: Qt.AlignVCenter
                    Text {
                        text: nickname
                        color: "grey"
                        font.pixelSize: 14
                    }
                    Text {
                        text: Qt.formatDateTime(commentDate, "yyyy-MM-dd hh:mm")
                        color: "grey"
                        font.pixelSize: 12
                    }
                    Text {
                        Layout.preferredWidth: parent.width-10
                        Layout.topMargin: 5
                        text: comment
                        font.pixelSize: 16
                        wrapMode: Text.WrapAnywhere
                    }
                }
                Button {
                    width: 30
                    height: 30
                    Layout.alignment: Qt.Horizontal
                    icon.source: "qrc:more_comment.png"
                    icon.width: 30
                    icon.height: 30
                    background: Rectangle {
                        opacity: 1
                        color: "white"
                    }
                    onClicked: {
                        commentView.currentIndex = index
                        deleteRec.visible = true
                    }
                }
            }
            ToolSeparator {
                Layout.preferredHeight: 1
                Layout.preferredWidth: parent.width
                contentItem: Rectangle{
                    anchors.fill: parent
                    color: "#cccccc"
                }
            }
        }
    }
    Loader {
        id: delegateLoader
        sourceComponent: chooseDelegate(isAuthor)
    }

    function chooseDelegate(isAuthor) {
        if (netizen["id"] === mainPage.personalPage.netizen["id"]) {
            return myCommentDelegate
        }
        if (isAuthor === true) {
            return myCommentDelegate
        } else {
            return commentDelegate
        }
    }
}
