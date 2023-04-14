import QtQuick 2.15
import QtQuick.Controls 2.15

import ZyCppItems 1.0

TextField {
    id: root

    MouseArea{
        id: mouseArea
        anchors.fill: parent

        visible: Qt.platform.os == "wasm"

//        onClicked: {
//            //计算原来控件的位置，给原生html控件提供x，y，width，height
//            var pos = mapToItem(Overlay.overlay, Qt.point(0, 0))

////            console.log("position", pos)

//            // 调用函数，动态创建Input控件
//            ZyHtmlUtil.showTextInput(root, root.text, pos.x, pos.y, root.width, root.height)
//        }

        hoverEnabled: true
        onContainsMouseChanged: {
            if(mouseArea.containsMouse == false)
            {
                return
            }

            //计算原来控件的位置，给原生html控件提供x，y，width，height
            var pos = mapToItem(Overlay.overlay, Qt.point(0, 0))

            // 调用函数，动态创建Input控件
            ZyHtmlUtil.showTextInput(root, root.text, pos.x, pos.y, root.width, root.height)
        }
    }

}
