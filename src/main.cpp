#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qqml.h>

#include "src/lexer/lexser.h"
#include "src/parser/parser.h"
#include "src/models/tokenTableModel/tokenTableModel.h"
#include "src/models/parserTableModel/parserTableModel.h"
#include "src/regularExpr/regularExpr.h"
#include "textfilestorage.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // QML types
    qmlRegisterType<TextFileStorage>("CompileUI", 1, 0, "TextFileStorage");

    QQmlApplicationEngine engine;
    Lexer lexer;
    Parser parser;
    RegularExpr regExp;
    TokenTableModel tokenTableModel;
    ParserTableModel parserTableModel;

    QObject::connect(&lexer, &Lexer::tokensReady, &tokenTableModel, &TokenTableModel::updateTokens);
    QObject::connect(&parser, &Parser::errorsReady, &parserTableModel, &ParserTableModel::updateErrors);

    engine.rootContext()->setContextProperty("lexer", &lexer);
    engine.rootContext()->setContextProperty("parser", &parser);
    engine.rootContext()->setContextProperty("tokenTableModel", &tokenTableModel);
    engine.rootContext()->setContextProperty("parserTableModel", &parserTableModel);
    engine.rootContext()->setContextProperty("regExp", &regExp);

    QObject::connect(&engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        [] { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );

    engine.loadFromModule("CompileUI", "Main");

    return app.exec();
}