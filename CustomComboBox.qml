import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
ComboBox {
    id: recipientAccountComboBox
    model: ["", "账户1", "账户2", "账户3"]
    font.pointSize: 18
    // Layout.fillWidth: true
    Layout.alignment: Qt.AlignVCenter
    onCurrentIndexChanged: {
        textField.enabled = currentIndex === 0;
    }
    width: transferPage.width / 4

    TextField {
        id: textField
        width: parent.width - 30 // 调整宽度
        height: parent.height
        enabled: recipientAccountComboBox.currentIndex === 0
        text: enabled ? text : ""
    }
}