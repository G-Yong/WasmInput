import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

import ZyCppItems 1.0

Window {
    id: window

    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Manager{
        id: manager
    }

    Column{
        id: column
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20

        spacing: 2

        Repeater{
            model: 10
            delegate: MyTextField{
                id: textField
            }
        }
    }


}
