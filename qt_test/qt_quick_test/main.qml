import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.5

Window {
    visible: true
    width: 640
    height: 480
    title: "qt_quick_test"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            id: displayText
            text: "Hello World!"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
            font.pointSize: 42
        }

        Button {
            text: "Click Me"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                displayText.text = "Button Clicked!";
                buttonHandler.onButtonClicked(); // C++ ΩΩ∑‘ »£√‚
            }
        }
    }
}
