cmake_minimum_required(VERSION 3.21)
project(CompileUIQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.10.2 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

set(QT_QML_NO_CACHEGEN TRUE)

qt_add_executable(CompileUIQt
    src/main.cpp
    src/textfilestorage.cpp
    src/textfilestorage.h
    src/lexer/lexser.cpp
    src/lexer/lexser.h
    src/lexer/token/token.h
    src/lexer/token/errorToken/errorToken.h
)

qt_add_qml_module(CompileUIQt
    URI CompileUI
    VERSION 1.0
    QML_FILES
        qml/Main.qml
    RESOURCES
        resources/icons/AnalyzeIcon.png
        resources/icons/CopyIcon.png
        resources/icons/CutIcon.png
        resources/icons/FileIcon.png
        resources/icons/FolderIcon.png
        resources/icons/LeftArrowIcon.png
        resources/icons/PasteIcon.png
        resources/icons/ProgrammInfoIcon.png
        resources/icons/RightArrowIcon.png
        resources/icons/SaveIcon.png
        resources/icons/UserInfoIcon.png
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(CompileUIQt PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
else()
    set_target_properties(CompileUIQt PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

target_link_libraries(CompileUIQt
    PRIVATE Qt6::Quick
)

install(TARGETS CompileUIQt
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)