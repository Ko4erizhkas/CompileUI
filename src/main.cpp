#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqml.h>

#include "functionprototypescanner.h"
#include "textfilestorage.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<FunctionPrototypeScanner>("CompileUI", 1, 0, "FunctionPrototypeScanner");
    qmlRegisterType<TextFileStorage>("CompileUI", 1, 0, "TextfileStorage");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, [] { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.loadFromModule("CompileUI", "Main");

    return app.exec();
}
