#include "src/parser/parser.h"
#include <QDebug>
#include <initializer_list>
Parser::Parser(QObject* parent) : QObject{parent} {}

// Parser::Parser(QObject* parent, const QVector<Token>& tokens) : 
//                 QObject{parent}, m_tokens(tokens) {}


QString Parser::parse(const QString& text)
{
    states.clear();

    createStates(text);
    QString res = "";
    qDebug() << "Total errors:" << state_errors.count();
    for (const auto& e : state_errors)
    {
        qDebug() 
        << "--------Error--------\n"
        << "line:" << e.line 
        << " pos:" << e.letterPos << 
        " state:" << e.states << 
        " message:" << e.errorDesc;
    }
    return res;
}


void Parser::createStates(const QString& text)
{
    Lexer lexer;
    const QVector<Token> tokens = lexer.scanReturns(text).first;

    States state = States::ExpectedTypeFunc;
    int i = 0;

    state_errors.clear();
    
    auto ironsState = [&](int& idx, States state)
    {
        const auto& sign = signature[state];
        while (idx < tokens.size())
        {
            for (auto s : sign)
            {
                if (tokens[idx].type == s)
                {
                    return;
                }
            }
            idx++;
        }
    };

    while (i < tokens.size())
    {
        const Token& token = tokens[i];
        if (i < 0 || i > tokens.size())
            break;
        if (isSpace(token))
        {
            i++;
            continue;
        }

        switch (state)
        {

        case States::ExpectedTypeFunc:
        {
            if (isType(token))
            {
                addState(static_cast<int>(TokenType::Type), token);
                state = States::ExpectedIdFunc;
                i++;
            }
            else
            {
                addError(token, "Expected function type");
                ironsState(i, state);
                continue;
            }
            break;
        }

        case States::ExpectedIdFunc:
        {
            if (isId(token))
            {
                addState(static_cast<int>(TokenType::Id), token);
                state = States::ExpectedLParen;
                i++;
            }
            else
            {
                addError(token, "Expected function name");
                ironsState(i, state);
                continue;
            }
            break;
        }

        case States::ExpectedLParen:
        {
            if (isLParen(token))
            {
                addState(static_cast<int>(TokenType::LParen), token);
                state = States::ExpectedStartParams;
                i++;
            }
            else
            {
                addError(token, "Expected '('");
                ironsState(i, state);
                continue;
            }
            break;
        }

        case States::ExpectedStartParams:
        {
            if (isType(token))
            {
                addState(static_cast<int>(TokenType::Type), token);
                state = States::ExpectedId;
                i++;
            }
            else if (isRParen(token))
            {
                addState(static_cast<int>(TokenType::RParen), token);
                state = States::ExpectedSemicolon;
                i++;
            }
            else
            {
                addError(token, "Expected type or ')'");
                i++;
            }
            break;
        }

        case States::ExpectedId:
        {
            if (isId(token))
            {
                addState(static_cast<int>(TokenType::Id), token);
                state = States::ExpectedType;
                i++;
            }
            else
            {
                addError(token, "Expected parameter name");
                ironsState(i, state);
                continue;
            }
            break;
        }

        case States::ExpectedType:
        {
            if (isType(token))
            {
                addState(static_cast<int>(TokenType::Type), token);
                state = States::ExpectedId;
                i++;
            }
            else if (isComma(token))
            {
                addState(static_cast<int>(TokenType::Comma), token);
                state = States::ExpectedType;
                i++;
            }
            else if (isRParen(token))
            {
                addState(static_cast<int>(TokenType::RParen), token);
                state = States::ExpectedSemicolon;
                i++;
            }
            else
            {
                addError(token, "Expected type or ',' or ')'");
                ironsState(i, state);
                continue;
            }
            break;
        }

        case States::ExpectedSemicolon:
        {
            if (isSemicolon(token))
            {
                addState(static_cast<int>(TokenType::Semicolon), token);
                state = States::Accepted;
                i++;
            }
            else
            {
                addError(token, "Expected ';'");
                i++;
            }
            break;
        }

        case States::Accepted:
        {
            state = States::ExpectedTypeFunc;
            break;
        }

        default:
        {
            addError(token, "Unknown state");
            i++;
            break;
        }
        }
    }
}






void Parser::addError(const Token& token, QString message)
{
    state_errors.emplace_back(
        ErrorParser(TokenType::Error, token.line,
        token.letterPos, static_cast<int>(token.type), message)
    );
}
void Parser::addState(int numState, const Token& token)
{
    states.insert(static_cast<int>(token.type), token);
}

bool Parser::isType(const Token& token) const
{
    return token.type == TokenType::Type;
}
bool Parser::isId(const Token& token) const
{
    return token.type == TokenType::Id;
}
bool Parser::isSpace(const Token& token) const 
{
    return token.type == TokenType::Space;
}
bool Parser::isSemicolon(const Token& token) const 
{
    return token.type == TokenType::Semicolon;
}
bool Parser::isLParen(const Token& token) const 
{
    return token.type == TokenType::LParen;
}
bool Parser::isRParen(const Token& token) const
{
    return token.type == TokenType::RParen;
} 
bool Parser::isComma(const Token& token) const 
{
    return token.type == TokenType::Comma;
}