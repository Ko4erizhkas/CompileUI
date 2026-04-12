import QtQuick
import QtCore
import QtQuick.Controls
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs

import CompileUI 1.0
import Lexer 1.0
import TokenTableModel 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 500
    visible: true
    title: currentFileName.length > 0 ? "Compiler - " + currentFileName : "Compiler"

    Universal.theme: Universal.System

    property url currentFileUrl: ""
    property string currentFileName: ""
    property bool hasUnsavedChanges: false
    property bool closeAfterSave: false
    property bool closeConfirmationActive: false
    property bool closeApproved: false

    TextFileStorage {
        id: fileStorage
    }
    Lexer {
        id: lexer
    }
    function finalExit()
    {
        closeAfterSave = false
        closeConfirmationActive = false
        closeApproved = true
        Qt.quit()
    }
    function updateCurrentFile(fileUrl) {
        currentFileUrl = fileUrl
        currentFileName = fileStorage.fileNameFromUrl(fileUrl)
    }

    function openSelectedFile(fileUrl) {
        const restored = fileStorage.loadFromFile(fileUrl)
        if (fileStorage.lastError().length > 0) {
            outputEditor.text = fileStorage.lastError()
            return
        }

        updateCurrentFile(fileUrl)
        sourceEditor.text = restored
        hasUnsavedChanges = false
        outputEditor.text = "Файл открыт: " + currentFileName
    }

    function saveCurrentFile() {
        if (!currentFileUrl || currentFileUrl.toString().length === 0) {
            saveAsFileDialog.open()
            return
        }

        if (fileStorage.saveToFile(sourceEditor.text, currentFileUrl)) {
            hasUnsavedChanges = false
            outputEditor.text = "Файл сохранен: " + currentFileName

            if (closeAfterSave) {
                finalExit()
            }
        } else {
            outputEditor.text = fileStorage.lastError()
        }
    }

    function createFileInDirectory(folderUrl) {
        const newFileUrl = fileStorage.buildFileUrl(folderUrl, "new_file.txt")
        if (!newFileUrl || newFileUrl.toString().length === 0) {
            outputEditor.text = "Не удалось выбрать директорию для нового файла"
            return
        }

        updateCurrentFile(newFileUrl)
        sourceEditor.clear()
        hasUnsavedChanges = false
        outputEditor.text = "Создан новый файл: " + currentFileName
    }

    function requestExit() {
        if (!hasUnsavedChanges) {
            finalExit()
            return
        }

        closeConfirmationActive = true
        saveBeforeExitDialog.open()
    }

    function handleToolBarAction(actionId)
    {
        switch (actionId)
        {
        case "newFile":
            newFileFolderDialog.open()
            break
        case "openFile":
            openFileDialog.open()
            break
        case "saveFile":
            saveCurrentFile()
            break
        case "saveAsFile":
            saveAsFileDialog.open()
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
            outputEditor.text = lexer.scan(sourceEditor.text)
            break
        case "userInfo":
            outputEditor.text = "Руководство пользователя"
            break
        case "aboutProgramm":
            outputEditor.text = "О программе"
            break
        case "exit":
            requestExit()
            break
        default:
            outputEditor.text = "Неизвестное действие: " + actionId
            break
        }
    }

    onClosing: function(close) {
        if (closeApproved) {
            closeApproved = false
            close.accepted = true
            return
        }
        if (closeConfirmationActive) {
            close.accepted = false
            return
        }

        if (hasUnsavedChanges) {
            close.accepted = false
            requestExit()
            return
        }

        close.accepted = true
    }

    menuBar: MenuBar {
        FileDialog {
            id: openFileDialog
            title: "Открыть файл"
            fileMode: FileDialog.OpenFile
            nameFilters: ["Text file (*.txt)", "C++ file (*.cpp *.h)", "All files (*)"]
            onAccepted: root.openSelectedFile(selectedFile)
        }
        FileDialog {
            id: saveAsFileDialog
            title: "Сохранить файл как"
            fileMode: FileDialog.SaveFile
            currentFile: currentFileUrl
            nameFilters: ["Text file (*.txt)", "C++ file (*.cpp *.h)", "All files (*)"]
            onAccepted: {
                root.updateCurrentFile(selectedFile)
                root.saveCurrentFile()
            }
            onRejected: {
                if (closeAfterSave) {
                    closeAfterSave = false
                }
            }
        }
        FolderDialog {
            id: newFileFolderDialog
            title: "Выберите директорию для нового файла"
            onAccepted: root.createFileInDirectory(selectedFolder)
        }
        Dialog {
            id: saveBeforeExitDialog
            anchors.centerIn: parent
            title: "Сохранить изменения?"
            modal: true
            standardButtons: Dialog.NoButton

            onRejected: {
                closeConfirmationActive = false
                closeAfterSave = false
            }

            contentItem: ColumnLayout {
                spacing: 12

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    text: currentFileName.length > 0
                        ? "Файл \"" + currentFileName + "\" был изменен."
                        : "Текущий файл был изменен."
                }

                Label {
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    text: "Сохранить изменения перед выходом?"
                }
            }

            footer: DialogButtonBox {
                standardButtons: DialogButtonBox.Save | DialogButtonBox.Discard | DialogButtonBox.Cancel

                Component.onCompleted: {
                    const discard = standardButton(DialogButtonBox.Discard)
                    if (discard)
                        discard.text = "Выйти без сохранения"
                }

                onClicked: function(button) {
                    closeConfirmationActive = false
                    if (button === standardButton(DialogButtonBox.Save)) {
                        closeConfirmationActive = false
                        closeAfterSave = true
                        root.saveCurrentFile()
                    } else if (button === standardButton(DialogButtonBox.Discard)) {
                        finalExit()
                    } else {
                        closeConfirmationActive = false
                        closeAfterSave = false
                        saveBeforeExitDialog.close()
                    }
                }
            }
        }
        Menu {
            title: "Файл"
            Action {
                text: qsTr("Создать")
                onTriggered: handleToolBarAction("newFile")
            }
            Action {
                text: qsTr("Открыть")
                onTriggered: handleToolBarAction("openFile")
            }
            Action {
                text: qsTr("Сохранить")
                onTriggered: handleToolBarAction("saveFile")
            }
            Action {
                text: qsTr("Сохранить как")
                onTriggered: handleToolBarAction("saveAsFile")
            }
            Action {
                text: qsTr("Выход")
                onTriggered: handleToolBarAction("exit")
            }
        }
        Menu {
            title: "Правка"
            Action { text: qsTr("Отменить"); onTriggered: handleToolBarAction("undo") }
            Action { text: qsTr("Повторить"); onTriggered: handleToolBarAction("redo") }
            Action { text: qsTr("Вырезать"); onTriggered: handleToolBarAction("cut") }
            Action { text: qsTr("Копировать"); onTriggered: handleToolBarAction("copy") }
            Action { text: qsTr("Вставить"); onTriggered: handleToolBarAction("paste") }
            Action { text: qsTr("Удалить") }
            Action { text: qsTr("Выделить всё") }
        }
        Menu {
            title: "Текст"
            Action { text: qsTr("Постановка задачи") }
            Action { text: qsTr("Грамматика") }
            Action { text: qsTr("Классификация грамматики") }
            Action { text: qsTr("Метод анализа") }
            Action { text: qsTr("Тестовый пример") }
            Action { text: qsTr("Список литературы") }
            Action { text: qsTr("Исходный код программы") }
        }
        Menu { title: "Пуск" }
        Menu {
            title: "Справка"
            Action { text: qsTr("Вызов справки") }
            Action { text: qsTr("О программе") }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        anchors.margins: 8

        ToolBar {
            Layout.fillWidth: true
            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                Repeater {
                    model: [
                        { icon: "FileIcon", hint: "Новый файл", action: "newFile" },
                        { icon: "FolderIcon", hint: "Открыть", action: "openFile" },
                        { icon: "SaveIcon", hint: "Сохранить", action: "saveFile" },
                        { icon: "CutIcon", hint: "Вырезать", action: "cut" },
                        { icon: "CopyIcon", hint: "Копировать", action: "copy" },
                        { icon: "PasteIcon", hint: "Вставить", action: "paste" },
                        { icon: "LeftArrowIcon", hint: "Отменить", action: "undo" },
                        { icon: "RightArrowIcon", hint: "Повторить", action: "redo" },
                        { icon: "AnalyzeIcon", hint: "Анализ", action: "analyze" },
                        { icon: "ProgrammInfoIcon", hint: "О программе", action: "aboutProgramm" },
                        { icon: "UserInfoIcon", hint: "Руководство пользователя", action: "userInfo" }
                    ]
                    ToolButton {
                        Layout.preferredWidth: 56
                        Layout.preferredHeight: 56
                        icon.source: "qrc:/qt/qml/CompileUI/resources/icons/" + modelData.icon + ".png"
                        icon.width: 28
                        icon.height: 28
                        display: AbstractButton.IconOnly
                        onClicked: handleToolBarAction(modelData.action)
                        ToolTip.visible: hovered
                        ToolTip.text: modelData.hint
                    }
                }
            }
        }
        SplitView {

            orientation: Qt.Vertical
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView
            {
                SplitView.fillHeight: true
                TextArea {
                    id: sourceEditor
                    placeholderText: "Введите прототип функции C++ (например: int sum(int a, int b);)"
                    wrapMode: TextEdit.NoWrap
                    onTextChanged: {
                        hasUnsavedChanges = true
                        outputEditor.text = lexer.scan(text)
                    }
                    Component.onCompleted: {
                        hasUnsavedChanges = false
                        outputEditor.text = lexer.scan(text)
                    }
                }
            }
            ScrollView
            {
                SplitView.fillHeight: true
                TextArea {
                    id: outputEditor
                    readOnly: true
                    placeholderText: "Ошибки сканера"
                    wrapMode: TextEdit.Wrap
                }
            }
        }
    }
}
