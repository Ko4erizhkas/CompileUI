#pragma once
#include <QObject>
#include <QString>

class TextFileStorage : public QObject
{
    Q_OBJECT

public:
    explicit TextFileStorage(QObject *parent = nullptr);

    Q_INVOKABLE bool saveToDefaultFile(const QString &text);
    Q_INVOKABLE QString loadFromDefaultFile();
    Q_INVOKABLE QString defaultFilePath() const;
    Q_INVOKABLE QString lastError() const;

private:
    QString m_lastError;
};
