#pragma once
#include <QObject>
#include <QVector>
#include <QHash>

#include "src/lexer/token/token.h"
#include "src/lexer/lexser.h"

class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject* parent = nullptr);
    //explicit Parser(QObject* parent = nullptr, const QVector<Token>& tokens);
    void parse(const QString& text);
private:

    void createStates(const QString& text);
    
    int m_pos = 0;
    int m_state = 0;
    QHash<int, QString> states;
    const QVector<Token> m_tokens;
    const QVector<ErrorToken> m_errors;
};