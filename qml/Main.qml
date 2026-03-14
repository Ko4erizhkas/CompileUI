import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 837
    height: 451
    visible: true
    title: "Compiler"

    menuBar: MenuBar {
        Menu { title: "Файл" }
        Menu { title: "Правка" }
        Menu { title: "Текст" }
        Menu { title: "Пуск" }
        Menu { title: "Справка" }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        anchors.margins: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Repeater {
                model: [
                    "FileIcon",
                    "FolderIcon",
                    "SaveIcon",
                    "CutIcon",
                    "CopyIcon",
                    "PasteIcon",
                    "LeftArrowIcon",
                    "RightArrowIcon",
                    "AnalyzeIcon",
                    "ProgrammInfoIcon",
                    "UserInfoIcon"
                ]

                ToolButton {
                    Layout.preferredWidth: 64
                    Layout.preferredHeight: 56
                    icon.source: "qrc:/qt/qml/CompileUI/resources/icons/" + modelData + ".png"
                    icon.width: 32
                    icon.height: 32
                    display: AbstractButton.IconOnly
                }
            }
        }

        TextArea {
            id: sourceEditor
            Layout.fillWidth: true
            Layout.preferredHeight: 160
            placeholderText: "Исходный текст"
            wrapMode: TextEdit.NoWrap
        }

        TextArea {
            id: outputEditor
            Layout.fillWidth: true
            Layout.fillHeight: true
            readOnly: true
            placeholderText: "Результат"
            wrapMode: TextEdit.NoWrap
        }
    }
}
