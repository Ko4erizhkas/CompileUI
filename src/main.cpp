#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qqml.h>

#include "src/lexer/lexser.h"
#include "src/parser/parser.h"
#include "src/models/tokenTableModel/tokenTableModel.h"
#include "textfilestorage.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // QML types
    qmlRegisterType<TextFileStorage>("CompileUI", 1, 0, "TextFileStorage");

    QQmlApplicationEngine engine;
    Lexer lexer;
    Parser parser;
    TokenTableModel tokenTableModel;

    QObject::connect(&lexer, &Lexer::tokensReady, &tokenTableModel, &TokenTableModel::updateTokens);

    engine.rootContext()->setContextProperty("lexer", &lexer);
    engine.rootContext()->setContextProperty("parser", &parser);
    engine.rootContext()->setContextProperty("tokenTableModel", &tokenTableModel);
    QObject::connect(&engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        [] { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    engine.loadFromModule("CompileUI", "Main");

    return app.exec();
}