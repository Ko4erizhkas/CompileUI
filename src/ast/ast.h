#pragma once
#include <QObject>
#include <QString>
class AST : public QObject
{
    Q_OBJECT
public:
    explicit AST(QObject* parent = nullptr);

private:
    QString Ast_work(const QString text);
};