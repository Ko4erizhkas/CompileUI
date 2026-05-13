#pragma once
#include <QObject>
#include <QString>

class RegularExpr : public QObject
{
    Q_OBJECT
public:
    explicit RegularExpr(QObject* parent = nullptr);
    Q_INVOKABLE QString expression1(const QString text);
    Q_INVOKABLE QString expression2(const QString text);
    Q_INVOKABLE QString expression3(const QString text);
};