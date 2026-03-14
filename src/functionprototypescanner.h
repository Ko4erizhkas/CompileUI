#pragma once

#include <QObject>
#include <QString>

class FunctionPrototypeScanner : public QObject
{
    Q_OBJECT

public:
    explicit FunctionPrototypeScanner(QObject *parent = nullptr);

    Q_INVOKABLE QString scan(const QString &prototype) const;

private:
    bool isTypeToken(const QString &token) const;
    bool isIdentifier(const QString &token) const;
};
