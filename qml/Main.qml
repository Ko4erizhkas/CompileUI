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
    TextFileStorage{
        id: fileStorage
    }
    function handleToolBarAction(actionId)
    {
        switch(actionId)
        {
        case "newFile":
            sourceEditor.clear()
            outputEditor.text = "Создать новый файл"
            break
        case "openFile": {
                    const restored = fileStorage.loadFromDefaultFile()
                    if (fileStorage.lastError().length > 0) {
                        outputEditor.text = fileStorage.lastError()
                        break
                    }

                    sourceEditor.text = restored
                    outputEditor.text = restored.length > 0
                        ? "Текст загружен из файла: " + fileStorage.defaultFilePath()
                        : "Файл пуст или еще не создан: " + fileStorage.defaultFilePath()
                    break
                }
        case "saveFile":
            if (fileStorage.saveToDefaultFile(sourceEditor.text)) {
                outputEditor.text = "Текст сохранен в файл: " + fileStorage.defaultFilePath()
            } else {
                outputEditor.text = fileStorage.lastError()
            }
            break
        case "cut":
            sourceEditor.cut()
            break
        case "copy":
            sourceEditor.copy()
            break
        case "paste":
            sourceEditor.paste()
            break
        case "undo":
            sourceEditor.undo()
            break
        case "redo":
            sourceEditor.redo()
            break
        case "analyze":
            sourceEditor.text = scanner.scan(sourceEditor.text)
            break
        default:
            outputEditor.text = "Неизвестное действие: " + actionId
            break
        }
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
        ToolBar {
            Layout.fillWidth: parent
            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                Repeater {
                    model : [
                        {icon: "FileIcon" , hint: "Новый файл", action: "newFile"},
                        {icon: "FolderIcon", hint: "Открыть", action: "openFile"},
                        {icon: "SaveIcon", hint: "Сохранить", action: "saveFile"},
                        {icon: "CutIcon", hint: "Вырезать", action: "cut"},
                        {icon: "CopyIcon", hint: "Копировать", action: "copy"},
                        {icon: "PasteIcon", hint: "Вставить", action: "paste"},
                        {icon: "LeftArrowIcon", hint: "Вперёд", action: "undo"},
                        {icon: "RightArrowIcon", hint: "Назад", action: "redo"},
                        {icon: "AnalyzeIcon", hint: "Анализ", action: "analyze"},
                        {icon: "ProgrammInfoIcon", hint: "О программе", action: "aboutProgramm"},
                        {icon: "UserInfoIcon", hint: "Руководство пользователя", action: "UserInfo"}
                    ]
                    ToolButton {
                        Layout.preferredWidth: 56
                        Layout.preferredHeight: 56
                        icon.source: "qrc:/qt/qml/CompileUI/resources/icons/" + modelData.icon + ".png"
                        icon.width: 28
                        icon.height: 28
                        display: AbstractButton.IconOnly

                        ToolTip.visible: hovered
                        ToolTip.text: modelData.hint
                    }
                }
        }
    }
        TextArea {
            id: sourceEditor
            Layout.fillWidth: true
            Layout.preferredHeight: 160
            placeholderText: "Введите прототип функции C++ (например: int sum(int a, int b);)"
            wrapMode: TextEdit.NoWrap

            onTextChanged: {
                outputEditor.text = scanner.scan(text)
            }
            Component.onCompleted:
            {
                const restored = fileStorage.loadFromDefaultFile()
                if (fileStorage.lastError().length > 0)
                {
                    outputEditor.text = fileStorage.lastError()
                    return
                }
                if (restored.length > 0)
                {
                    sourceEditor.text = restored
                }
                else
                {
                    outputEditor.text = scanner.scan(text)
                }
            }
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
