#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include "src/lexer/lexser.h"
#include "src/models/tokenTableModel/tokenTableModel.h"
#include "textfilestorage.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // QML types
    qmlRegisterType<Lexer>("Lexer", 1, 0, "Lexer");
    qmlRegisterType<TokenTableModel>("TokenTableModel", 1, 0, "TokenTableModel");
    qmlRegisterType<TextFileStorage>("CompileUI", 1, 0, "TextFileStorage");

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        [] { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    engine.loadFromModule("CompileUI", "Main");

    return app.exec();
}