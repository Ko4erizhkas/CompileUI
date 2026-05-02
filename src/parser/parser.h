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
    ExpectedIdFunc,
    ExpectedLParen,
    ExpectedStartParams,
    ExpectedTypeOrRParen,
    ExpectedType,
    ExpectedComma,
    ExpectedId,
    ExpectedParamTail,
    ExpectedRParen,
    ExpectedSemicolon,
    Accepted
};

class Parser : public QObject
{
    Q_OBJECT
public:
    explicit Parser(QObject* parent = nullptr);
    Q_INVOKABLE QString parse(const QString& text);

signals:
    void errorsReady(const QVector<ErrorParser>& errors);

private:
    void createStates(const QString& text);
    void addError(const Token& token, QString message);
    void addState(const Token& token);
    QHash<int, Token> states;
    QVector<ErrorParser> state_errors;
    bool isType(const Token& token) const;
    bool isId(const Token& token) const;
    bool isSpace(const Token& token) const;
    bool isLParen(const Token& token) const;
    bool isRParen(const Token& token) const;
    bool isSemicolon(const Token& token) const;
    bool isComma(const Token& token) const;

    const QHash<States, QVector<TokenType>> signature =
    {
        {States::ExpectedTypeFunc,    {TokenType::Type}},
        {States::ExpectedIdFunc,      {TokenType::Id, TokenType::Semicolon}},
        {States::ExpectedLParen,      {TokenType::LParen, TokenType::Semicolon}},
        {States::ExpectedStartParams, {TokenType::Type, TokenType::RParen}},
        {States::ExpectedId,          {TokenType::Id, TokenType::Comma, TokenType::RParen, TokenType::Semicolon}},
        {States::ExpectedComma,       {TokenType::Comma, TokenType::RParen, TokenType::Semicolon}},
        {States::ExpectedType,        {TokenType::Type, TokenType::Semicolon}},
        {States::ExpectedSemicolon,   {TokenType::Semicolon}}
    };
};
