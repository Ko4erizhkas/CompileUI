#pragma once
#include <QObject>
#include <QVector>
#include <QHash>

#include "src/lexer/token/token.h"
#include "src/lexer/lexser.h"
#include "src/parser/errorParser/errorParser.h"
enum class States
{
    ExpectedTypeFunc, 
    ExpectedSpaceAfterTypeFunc,
    ExpectedIdFunc,
    ExpectedLParen,
    ExpectedStartParams,
    ExpectedTypeOrRParen,
    ExpectedType,
    ExpectedSpace,
    ExpectedId,
    ExpectedParamTail,
    ExpectedRParen,
    ExpectedSemicolon
};
class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject* parent = nullptr);
    //explicit Parser(QObject* parent = nullptr, const QVector<Token>& tokens);
    Q_INVOKABLE QString parse(const QString& text);
private:

    void createStates(const QString& text);
    void addError(const Token& token, QString message);
    void addState(int numState, const Token& token);
    int m_pos = 0;
    int m_state = 0;
    QHash<int, Token> states;
    //QHash<int, ErrorParser> state_errors;
    QVector<ErrorParser> state_errors;
    bool isType(const Token& token) const;
    bool isId(const Token& token) const;
    bool isSpace(const Token& token) const;
};