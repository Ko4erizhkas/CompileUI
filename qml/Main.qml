import QtQuick
import QtCore
import QtQuick.Controls
import QtQuick.Controls.Universal
import QtQuick.Layouts
import QtQuick.Dialogs

import CompileUI 1.0

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    title: currentFileName.length > 0 ? "Compiler - " + currentFileName : "Compiler"
    Universal.theme: Universal.System

    property url currentFileUrl: ""
    property string currentFileName: ""
    property bool hasUnsavedChanges: false
    property bool closeAfterSave: false
    property bool closeConfirmationActive: false
    property bool closeApproved: false

    property string tokenOutputText: ""
    property string parserOutputText: ""


    TextFileStorage {
        id: fileStorage
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
            parserOutputText = parser.parse(sourceEditor.text)
            outputEditor.text = parserOutputText
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
            nameFilters: [ "All files (*)", "Text file (*.txt)", "C++ file (*.cpp *.h)"]
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
            Action {
                text: qsTr("Грамматика")
                onTriggered: {
                    outputEditor.text = fileStorage.loadFromResource(":/qt/qml/CompileUI/resources/src_for_text_menu/grammatic.txt")
                }
            }
            Action { 
                text: qsTr("Классификация грамматики") 
                onTriggered: {
                    outputEditor.text = fileStorage.loadFromResource(":/qt/qml/CompileUI/resources/src_for_text_menu/classification.txt")
                }                
            }
            Action {
                text: qsTr("Метод анализа")
                onTriggered: {
                    outputEditor.text = fileStorage.loadFromResource(":/qt/qml/CompileUI/resources/src_for_text_menu/methodAnalyze.txt")
                
                }            
            }
            Action { text: qsTr("Тестовый пример") }
            Action { 
                text: qsTr("Список литературы") 
                onTriggered: {
                    outputEditor.text = fileStorage.loadFromResource(":/qt/qml/CompileUI/resources/src_for_text_menu/literature.txt")
                }                
            }
            Action { 
                text: qsTr("Исходный код программы") 
                onTriggered: {
                    outputEditor.text = fileStorage.loadFromResource(":/qt/qml/CompileUI/resources/src_for_text_menu/sourceCode.txt")
                } 
            }
        }
        Menu { 
            title: "Пуск"
            Action {
                text: "Запуск лексера"
                onTriggered: {
                    tokenOutputText = lexer.scan(sourceEditor.text)
                    outputEditor.text = tokenOutputText
                }
            }
            Action {
                text: "Запуски парсера"
                onTriggered: {
                    parserOutputText = parser.parse(sourceEditor.text)
                    outputEditor.text = parserOutputText
                }
            }
        }
        Menu {
            title: "Справка"
            Action { 
                text: qsTr("Вызов справки") 
                onTriggered: {
                    outputEditor.text = fileStorage.loadFromResource(":/qt/qml/CompileUI/resources/src_for_text_menu/forUsers.txt")
                }
            }
            Action {
                text: qsTr("О программе")
                onTriggered: {
                    outputEditor.text = fileStorage.loadFromResource(":/qt/qml/CompileUI/resources/src_for_text_menu/aboutProgram.txt")
                }
            }
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

            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal
            ScrollView
            {
                SplitView.fillHeight: true
                SplitView.preferredHeight: root * 0.4
                TextArea {
                    id: sourceEditor
                    placeholderText: "Введите прототип функции C++ (например: int sum(int a, int b);)"
                    wrapMode: TextEdit.NoWrap
                    font.pixelSize: 14
                    onTextChanged: {
                        hasUnsavedChanges = true
                    }
                    Component.onCompleted: {
                        hasUnsavedChanges = false
                    }
                }

            }
            ScrollView
            {
                SplitView.preferredWidth: parent.width * 0.3
                TextArea {
                    id: outputEditor
                    readOnly: true
                    placeholderText: "Общий вывод программы"
                    wrapMode: TextEdit.Wrap
                    font.pixelSize: 14
                }
            }
            SplitView {
                id: rightSplitView
                SplitView.fillWidth: true
                SplitView.preferredWidth: root.width * 0.45
                orientation: Qt.Vertical

                Frame {
                    SplitView.fillWidth: true
                    SplitView.preferredHeight: rightSplitView.height / 2
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 4

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: "Таблица токенов"
                                font.bold: true
                            }
                            Item { Layout.fillWidth: true }
                            TabBar {
                                id: tokenViewTab
                                implicitHeight: 30
                                TabButton {
                                    text: "Таблица"
                                    implicitHeight: 30
                                    font.pixelSize: 13
                                }
                                TabButton {
                                    text: "Текст"
                                    implicitHeight: 30
                                    font.pixelSize: 13
                                }
                            }
                        }

                        HorizontalHeaderView {
                            syncView: tokenTableView
                            model: ["Линия", "Позиция", "Лексема", "Тип токена"]
                            Layout.fillWidth: true
                            visible: tokenViewTab.currentIndex === 0
                        }

                        StackLayout {
                            currentIndex: tokenViewTab.currentIndex
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            TableView {
                                id: tokenTableView
                                model: tokenTableModel
                                clip: true
                                columnSpacing: 1
                                rowSpacing: 1
                                columnWidthProvider: function(column) {
                                    const rest = width - 60 - 60 - 110 - columnSpacing * 3
                                    switch (column) {
                                        case 0: return 60
                                        case 1: return 60
                                        case 2: return Math.max(110, rest)
                                        case 3: return 110
                                    }
                                }
                                onWidthChanged: forceLayout()
                                delegate: Rectangle {
                                    implicitHeight: 26
                                    color: row % 2 === 0 ? "#ffffff" : "#f5f5f5"
                                    border.color: "#d0d0d0"
                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: 8
                                        text: display ?? ""
                                        color: "#202020"
                                        font.pixelSize: 14
                                        elide: Text.ElideRight
                                        width: parent.width - 12
                                    }
                                }
                            }

                            ScrollView {
                                clip: true
                                TextArea {
                                    readOnly: true
                                    text: tokenOutputText
                                    font.family: "Consolas, Courier New"
                                    font.pixelSize: 14
                                    wrapMode: TextEdit.Wrap
                                    placeholderText: "Нет данных"
                                }
                            }
                        }
                    }
                }

                Frame {
                    SplitView.fillWidth: true
                    SplitView.preferredHeight: rightSplitView.height / 2
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 4

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: "Ошибки парсера"
                                font.bold: true
                            }
                            Item { Layout.fillWidth: true }
                            TabBar {
                                id: parserViewTab
                                implicitHeight: 30
                                TabButton {
                                    text: "Таблица"
                                    implicitHeight: 30
                                    font.pixelSize: 13
                                }
                                TabButton {
                                    text: "Текст"
                                    implicitHeight: 30
                                    font.pixelSize: 13
                                }
                            }
                        }

                        HorizontalHeaderView {
                            syncView: parserTableView
                            model: ["Линия", "Позиция", "Тип токена", "Описание ошибки"]
                            Layout.fillWidth: true
                            visible: parserViewTab.currentIndex === 0
                        }

                        StackLayout {
                            currentIndex: parserViewTab.currentIndex
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            TableView {
                                id: parserTableView
                                model: parserTableModel
                                clip: true
                                columnSpacing: 1
                                rowSpacing: 1
                                columnWidthProvider: function(column) {
                                    const rest = width - 60 - 60 - 110 - columnSpacing * 3
                                    switch (column) {
                                        case 0: return 60
                                        case 1: return 60
                                        case 2: return 110
                                        case 3: return Math.max(150, rest)
                                    }
                                }
                                onWidthChanged: forceLayout()
                                delegate: Rectangle {
                                    implicitHeight: 26
                                    color: row % 2 === 0 ? "#ffffff" : "#f5f5f5"
                                    border.color: "#d0d0d0"
                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: 8
                                        text: display ?? ""
                                        color: "#202020"
                                        font.pixelSize: 14
                                        elide: Text.ElideRight
                                        width: parent.width - 12
                                    }
                                }
                            }

                            ScrollView {
                                clip: true
                                TextArea {
                                    readOnly: true
                                    text: parserOutputText
                                    font.family: "Consolas, Courier New"
                                    font.pixelSize: 14
                                    wrapMode: TextEdit.Wrap
                                    placeholderText: "Нет данных"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
