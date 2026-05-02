#pragma once
#include <QObject>
#include <QString>
#include <QUrl>

class TextFileStorage : public QObject
{
    Q_OBJECT

public:
    explicit TextFileStorage(QObject *parent = nullptr);

    Q_INVOKABLE bool saveToDefaultFile(const QString &text);
    Q_INVOKABLE QString loadFromDefaultFile();
    Q_INVOKABLE QString defaultFilePath() const;
    Q_INVOKABLE QString lastError() const;
    Q_INVOKABLE bool saveToFile(const QString &text, const QUrl &fileUrl);
    Q_INVOKABLE QString loadFromFile(const QUrl &fileUrl);
    Q_INVOKABLE QUrl buildFileUrl(const QUrl &folderUrl, const QString &fileName) const;
    Q_INVOKABLE QString fileNameFromUrl(const QUrl &fileUrl) const;
    Q_INVOKABLE QString loadFromResource(const QString &resourcePath);

private:
    bool saveToPath(const QString &text, const QString &filePath);
    QString loadFromPath(const QString &filePath);
    QString localPathFromUrl(const QUrl &fileUrl) const;

    QString m_lastError;
};
