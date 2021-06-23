import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.1

Window {
    id: mainWindow
    objectName: "mainWindow"
    width: 320
    height: 320
    visible: true
    title: qsTr("ChangeRes")

    SystemTrayIcon {
        visible: true
        icon.source: "qrc:/img/jpg/trayicon.jpg"

        onActivated: {
            mainWindow.show()
            mainWindow.raise()
            mainWindow.requestActivate()
        }
    }

    Text {
        id: monitorTitle
        text: qsTr("Main Monitor: ")
        anchors.left: parent.left
        anchors.top: parent.top
        font.pixelSize: 14
        anchors.leftMargin: 10
        anchors.topMargin: 10
    }

    TextArea {
        id: resData
        //text: qsTr("3440 1440\n2560 1440")
        text: backEnd.resolutionsText
        selectByMouse: true
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: monitorTitle.bottom
        anchors.bottom: apply.top
        font.pixelSize: 12
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 10

        background: Rectangle {
            anchors.fill: parent
            border.color: "#333333"
        }

    }

    Button {
        id: apply
        text: qsTr("Apply")
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: runAtStartup.top
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0

        onClicked: backEnd.setResolutionsFromText(resData.text)
    }

    CheckBox {
        id: runAtStartup
        text: qsTr("Run when system starts")
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: quit.top
        anchors.bottomMargin: 0
        anchors.rightMargin: 0
        anchors.leftMargin: 0

        checked: backEnd.autorunProperty

        onCheckedChanged: {
            if (checked) {
                backEnd.addToAutorun()
            } else {
                backEnd.removeFromAutorun()
            }
        }
    }

    Button {
        id: quit
        text: qsTr("quit")
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0


        onClicked: Qt.quit()
    }
}
