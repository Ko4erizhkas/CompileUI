#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include "textfilestorage.h"
#include "src/lexer/lexser.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<TextFileStorage>("CompileUI", 1, 0, "TextFileStorage");
    qmlRegisterType<Lexer>("Lexer", 1, 0, "Lexer");


    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, [] { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.loadFromModule("CompileUI", "Main");

    return app.exec();
}
