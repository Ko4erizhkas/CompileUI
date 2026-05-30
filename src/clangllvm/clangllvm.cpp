#include "src/clangllvm/clangllvm.h"

#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QRegularExpression>

namespace
{
    const char* kSourceResource =
        ":/qt/qml/CompileUI/resources/clang_src/program.cpp";

    const char* kClangFallback =
        "C:/Program Files/LLVM/bin/clang.exe";
    const char* kDotFallback =
        "C:/Program Files/Graphviz/bin/dot.exe";
}

ClangLlvm::ClangLlvm(QObject* parent) : QObject(parent) {}

QString ClangLlvm::workDir()
{
    const QString dir = QDir::tempPath() + "/compileui_clang";
    QDir().mkpath(dir);
    return dir;
}

QString ClangLlvm::clangPath() const
{
    QString p = QStandardPaths::findExecutable("clang");
    if (p.isEmpty() && QFile::exists(kClangFallback))
        p = kClangFallback;
    return p;
}

QString ClangLlvm::dotPath() const
{
    QString p = QStandardPaths::findExecutable("dot");
    if (p.isEmpty() && QFile::exists(kDotFallback))
        p = kDotFallback;
    return p;
}

bool ClangLlvm::clangAvailable() const { return !clangPath().isEmpty(); }
bool ClangLlvm::dotAvailable()   const { return !dotPath().isEmpty(); }

QString ClangLlvm::toolchainInfo() const
{
    const QString c = clangPath();
    const QString d = dotPath();
    QString s;
    s += "clang: " + (c.isEmpty() ? QStringLiteral("не найден") : c) + "\n";
    s += "dot:   " + (d.isEmpty() ? QStringLiteral("не найден") : d) + "\n";
    return s;
}

QString ClangLlvm::missingToolMessage() const
{
    return QStringLiteral(
        "Не найден clang.exe.\n"
        "Установите LLVM и добавьте C:\\Program Files\\LLVM\\bin в PATH.\n\n")
        + toolchainInfo();
}

QStringList ClangLlvm::commonClangArgs() const
{
    return { QString::fromLatin1(kCppStandard) };
}

QString ClangLlvm::materializeSource() const
{
    QFile res(QString::fromLatin1(kSourceResource));
    if (!res.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray data = res.readAll();
    res.close();

    const QString path = workDir() + "/program.cpp";
    QFile out(path);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return QString();
    out.write(data);
    out.close();
    return path;
}

ClangLlvm::ProcResult ClangLlvm::run(const QString& program,
                                     const QStringList& args,
                                     const QString& workingDir,
                                     const QStringList& removeFromPath) const
{
    ProcResult r;
    QProcess proc;

    if (!workingDir.isEmpty())
        proc.setWorkingDirectory(workingDir);

    if (!removeFromPath.isEmpty())
    {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        const QString sep = QString(QDir::listSeparator());
        QStringList parts = env.value("PATH").split(sep, Qt::SkipEmptyParts);
        for (const QString& bad : removeFromPath)
        {
            parts.removeIf([&](const QString& p) {
                return QFileInfo(p).canonicalFilePath()
                       == QFileInfo(bad).canonicalFilePath();
            });
        }
        env.insert("PATH", parts.join(sep));
        proc.setProcessEnvironment(env);
    }

    proc.start(program, args);
    if (!proc.waitForStarted(5000))
        return r;

    r.started = true;
    proc.waitForFinished(30000);
    r.exitCode = proc.exitCode();
    r.out = QString::fromUtf8(proc.readAllStandardOutput());
    r.err = QString::fromUtf8(proc.readAllStandardError());
    return r;
}

QString ClangLlvm::dumpAst()
{
    const QString clang = clangPath();
    if (clang.isEmpty()) { emit toolError(missingToolMessage()); return missingToolMessage(); }

    const QString src = materializeSource();
    if (src.isEmpty())
        return QStringLiteral("Не удалось подготовить исходный файл программы.");

    QStringList args;
    args << "-Xclang" << "-ast-dump" << "-fsyntax-only";
    args << commonClangArgs();
    args << src;

    const ProcResult res = run(clang, args);
    if (!res.started) { emit toolError(missingToolMessage()); return missingToolMessage(); }

    QString out = "=== Clang AST ===\n";
    out += res.out;
    if (!res.err.trimmed().isEmpty())
        out += "\n=== Диагностика clang ===\n" + res.err;
    return out;
}

QString ClangLlvm::emitIr(int optLevel)
{
    const QString clang = clangPath();
    if (clang.isEmpty()) { emit toolError(missingToolMessage()); return missingToolMessage(); }

    const QString src = materializeSource();
    if (src.isEmpty())
        return QStringLiteral("Не удалось подготовить исходный файл программы.");

    const QString opt = (optLevel == 0) ? QStringLiteral("-O0") : QStringLiteral("-O2");

    QStringList args;
    args << "-S" << "-emit-llvm" << opt;
    args << commonClangArgs();
    args << "-o" << "-" << src;

    const ProcResult res = run(clang, args);
    if (!res.started) { emit toolError(missingToolMessage()); return missingToolMessage(); }

    if (res.out.trimmed().isEmpty())
        return QStringLiteral("Ошибка генерации LLVM IR (%1):\n").arg(opt) + res.err;

    QString out = QStringLiteral("=== LLVM IR (%1) ===\n").arg(opt);
    out += res.out;
    if (!res.err.trimmed().isEmpty())
        out += "\n=== Диагностика clang ===\n" + res.err;
    return out;
}

QString ClangLlvm::buildCfg()
{
    const QString clang = clangPath();
    if (clang.isEmpty()) { emit toolError(missingToolMessage()); return missingToolMessage(); }

    const QString dot = dotPath();
    if (dot.isEmpty())
    {
        const QString msg = QStringLiteral(
            "Не найден dot.exe (Graphviz).\n"
            "Установите Graphviz и добавьте C:\\Program Files\\Graphviz\\bin в PATH.\n\n")
            + toolchainInfo();
        emit toolError(msg);
        return msg;
    }

    const QString src = materializeSource();
    if (src.isEmpty())
        return QStringLiteral("Не удалось подготовить исходный файл программы.");

    const QString dir = workDir();

    {
        QDir d(dir);
        const QStringList stale = d.entryList({ "*.dot", "*.png" }, QDir::Files);
        for (const QString& f : stale)
            d.remove(f);
    }

    const QString graphvizDir = QFileInfo(dot).absolutePath();

    QStringList args;
    args << "-Xclang" << "-analyze"
         << "-Xclang" << "-analyzer-checker=debug.ViewCFG"
         << "-fsyntax-only";
    args << commonClangArgs();
    args << src;

    const ProcResult res = run(clang, args, dir, { graphvizDir });
    if (!res.started) { emit toolError(missingToolMessage()); return missingToolMessage(); }

    static const QRegularExpression re(
        QStringLiteral("Writing '([^']+\\.dot)'"));
    QStringList dotFiles;
    auto it = re.globalMatch(res.err);
    while (it.hasNext())
        dotFiles << it.next().captured(1);

    if (dotFiles.isEmpty())
    {
        QDir d(dir);
        const QStringList found = d.entryList({ "*.dot" }, QDir::Files);
        for (const QString& f : found)
            dotFiles << d.absoluteFilePath(f);
    }

    if (dotFiles.isEmpty())
    {
        const QString msg = QStringLiteral(
            "CFG не построен: clang не создал .dot файлов.\n\n") + res.err;
        emit toolError(msg);
        return msg;
    }

    QStringList pngUrls;
    int idx = 0;
    for (const QString& dotFile : dotFiles)
    {
        const QString png = QStringLiteral("%1/cfg_%2.png").arg(dir).arg(idx++);
        const ProcResult dr = run(dot, { "-Tpng", dotFile, "-o", png });
        if (dr.started && QFile::exists(png))
            pngUrls << QUrl::fromLocalFile(png).toString();
    }

    if (pngUrls.isEmpty())
    {
        const QString msg = QStringLiteral("Не удалось отрендерить CFG в PNG (dot).");
        emit toolError(msg);
        return msg;
    }

    emit cfgImagesReady(pngUrls);
    return QStringLiteral("CFG построен. Графов функций: %1.\n"
                          "Изображение открыто в отдельном окне.")
        .arg(pngUrls.size());
}
