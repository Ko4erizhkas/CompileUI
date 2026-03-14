import QtQuick
import QtCore
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CompileUI 1.0

ApplicationWindow {
    id: root
    width: 837
    height: 451
    visible: true
    title: "Compiler"

    FunctionPrototypeScanner {
        id: scanner
    }
    menuBar: MenuBar
    {
        FileDialog
        {
            id: saveFileDialog
            currentFolder: StandardPaths.standardLocations()[0]
            fileMode: FileDialog.SaveFile
        }
        FileDialog
        {
            id: openFileDialog
            currentFolder: StandardPaths.standardLocations()[0]
            fileMode: FileDialog.OpenFile
        }
        FileDialog
        {
            id: saveAsFileDiolog
            currentFolder: StandardPaths.standardLocations()[0]
            fileMode: FileDialog.SaveFile
            nameFilters: ["Text file (*.txt)", "Cpp file (*.cpp)"]
        }
                Menu
                {
                    title: "Файл"
                    Action {
                        text : qsTr("Создать")
                        onTriggered: openFileDialog.open()
                    }
                    Action
                    {
                        text: qsTr("Открыть")
                        onTriggered: openFileDialog.open()
                    }
                    Action
                    {
                        text: qsTr("Сохранить")
                        onTriggered: saveFileDialog.open()
                    }
                    Action
                    {
                        text: qsTr("Сохранить как")
                        onTriggered: saveAsFileDiolog.open()
                    }
                    Action
                    {
                        text: qsTr("Выход")
                    }
                }
                Menu
                {
                    title: "Правка"
                    Action {text: qsTr("Отменить")}
                    Action {text: qsTr("Повторить")}
                    Action {text: qsTr("Вырезать")}
                    Action {text: qsTr("Копировать")}
                    Action {text: qsTr("Вставить")}
                    Action {text: qsTr("Удалить")}
                    Action {text: qsTr("Выделить всё")}
                }
                Menu
                {
                    title: "Текст"
                    Action {text: qsTr("Постановка задачи")}
                    Action {text: qsTr("Грамматика")}
                    Action {text: qsTr("Классификация грамматики")}
                    Action {text: qsTr("Метод анализа")}
                    Action {text: qsTr("Тестовый пример")}
                    Action {text: qsTr("Список литературы")}
                    Action {text: qsTr("Исходный код программы")}
                }
                Menu { title: "Пуск" }
                Menu
                {
                    title: "Справка"
                    Action {text: qsTr("Вызов справки")}
                    Action {text: qsTr("О программе")}
                }
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
            placeholderText: "Введите прототип функции C++ (например: int sum(int a, int b);)"
            wrapMode: TextEdit.NoWrap

            onTextChanged: outputEditor.text = scanner.scan(text)
            Component.onCompleted: outputEditor.text = scanner.scan(text)
        }

        TextArea {
            id: outputEditor
            Layout.fillWidth: true
            Layout.fillHeight: true
            readOnly: true
            placeholderText: "Ошибки сканера"
            wrapMode: TextEdit.Wrap
        }
    }
}
