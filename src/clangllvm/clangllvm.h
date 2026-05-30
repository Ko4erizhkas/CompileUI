#pragma once
#include <QObject>
#include <QString>
#include <QStringList>

class ClangLlvm : public QObject
{
    Q_OBJECT
public:
    explicit ClangLlvm(QObject* parent = nullptr);

    Q_INVOKABLE QString dumpAst();
    Q_INVOKABLE QString emitIr(int optLevel);

    Q_INVOKABLE QString buildCfg();

    Q_INVOKABLE bool clangAvailable() const;
    Q_INVOKABLE bool dotAvailable() const;
    Q_INVOKABLE QString toolchainInfo() const;

signals:
    void cfgImagesReady(const QStringList& pngUrls);
    void toolError(const QString& message);

private:
    struct ProcResult
    {
        bool    started  = false;
        int     exitCode = -1;
        QString out;
        QString err;
    };

    QString clangPath() const;
    QString dotPath() const;

    QString materializeSource() const;

    static QString workDir();

    ProcResult run(const QString& program,
                   const QStringList& args,
                   const QString& workingDir = QString(),
                   const QStringList& removeFromPath = {}) const;

    QStringList commonClangArgs() const;

    QString missingToolMessage() const;

    static constexpr const char* kCppStandard = "-std=c++17";
};
