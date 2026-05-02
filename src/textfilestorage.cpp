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
    return saveToPath(text, defaultFilePath());
}

QString TextFileStorage::loadFromDefaultFile()
{
    return loadFromPath(defaultFilePath());
}

QString TextFileStorage::defaultFilePath() const
{
    const QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    return appDataDir + QStringLiteral("/sourceEditor.txt");
}

QString TextFileStorage::lastError() const
{
    return m_lastError;
}

bool TextFileStorage::saveToFile(const QString &text, const QUrl &fileUrl)
{
    return saveToPath(text, localPathFromUrl(fileUrl));
}

QString TextFileStorage::loadFromFile(const QUrl &fileUrl)
{
    return loadFromPath(localPathFromUrl(fileUrl));
}

QUrl TextFileStorage::buildFileUrl(const QUrl &folderUrl, const QString &fileName) const
{
    const QString folderPath = localPathFromUrl(folderUrl);
    if (folderPath.isEmpty()) {
        return QUrl();
    }

    const QString targetPath = QDir(folderPath).filePath(fileName);
    return QUrl::fromLocalFile(targetPath);
}

QString TextFileStorage::fileNameFromUrl(const QUrl &fileUrl) const
{
    return QFileInfo(localPathFromUrl(fileUrl)).fileName();
}

bool TextFileStorage::saveToPath(const QString &text, const QString &filePath)
{
    if (filePath.isEmpty()) {
        m_lastError = QStringLiteral("Не выбран файл для сохранения");
        return false;
    }

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

QString TextFileStorage::loadFromPath(const QString &filePath)
{
    if (filePath.isEmpty()) {
        m_lastError = QStringLiteral("Не выбран файл для открытия");
        return QString();
    }

    QFile file(filePath);

    if (!file.exists()) {
        m_lastError = QStringLiteral("Файл не найден: %1").arg(filePath);
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

QString TextFileStorage::loadFromResource(const QString &resourcePath)
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QStringLiteral("Не удалось открыть ресурс: %1").arg(resourcePath);
        return QString();
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    m_lastError.clear();
    return in.readAll();
}

QString TextFileStorage::localPathFromUrl(const QUrl &fileUrl) const
{
    if (!fileUrl.isValid()) {
        return QString();
    }

    if (fileUrl.isLocalFile()) {
        return fileUrl.toLocalFile();
    }

    return fileUrl.toString(QUrl::PreferLocalFile);
}
