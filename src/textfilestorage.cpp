#include "textfilestorage.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>

TextFileStorage::TextFileStorage(QObject *parent)
    : QObject(parent)
{
}

bool TextFileStorage::saveToDefaultFile(const QString &text)
{
    const QString filePath = defaultFilePath();
    const QFileInfo fileInfo(filePath);
    const QString folder = fileInfo.absolutePath();

    if (!QDir().mkpath(folder)) {
        m_lastError = QStringLiteral("Не удалось создать папку: %1").arg(folder);
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        m_lastError = QStringLiteral("Не удалось открыть файл на запись: %1").arg(file.errorString());
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << text;
    file.close();

    m_lastError.clear();
    return true;
}

QString TextFileStorage::loadFromDefaultFile()
{
    const QString filePath = defaultFilePath();
    QFile file(filePath);

    if (!file.exists()) {
        m_lastError.clear();
        return QString();
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QStringLiteral("Не удалось открыть файл на чтение: %1").arg(file.errorString());
        return QString();
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    const QString result = in.readAll();
    file.close();

    m_lastError.clear();
    return result;
}

QString TextFileStorage::defaultFilePath() const
{
    const QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appDataDir + QStringLiteral("/sourceEditor.txt");
}

QString TextFileStorage::lastError() const
{
    return m_lastError;
}
