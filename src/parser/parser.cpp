#include "src/parser/parser.h"
Parser::Parser(QObject* parent) : QObject{parent} {}

// Parser::Parser(QObject* parent, const QVector<Token>& tokens) : 
//                 QObject{parent}, m_tokens(tokens) {}


QString Parser::parse(const QString& text)
{
    createStates(text);
    QString res = "";
    for (const auto& e : state_errors)
    {
        res += e.errorDesc + " ";
    }
    return res;
}


void Parser::createStates(const QString& text)
{
    Lexer lexer;
    const QVector<Token> tokens = lexer.scanReturns(text).first;

    States state = States::ExpectedTypeFunc;
    int i = 0;

    auto sync = [&](int idx)
    {
        while (idx < tokens.size())
        {
            auto t = tokens[idx].type;
            if (t == TokenType::Comma ||
                t == TokenType::RParen ||
                t == TokenType::Semicolon)
                return idx;
            idx++;
        }
        return idx;
    };

    while (i < tokens.size())
    {
        const Token& token = tokens[i];

        switch (state)
        {
        case States::ExpectedTypeFunc:
            if (isType(token))
            {
                addState(static_cast<int>(TokenType::Type), token);
                state = States::ExpectedIdFunc;
                i++;
            }
            else
            {
                addError(token, "Unsupported function type");
                i++;
            }
            break;

        case States::ExpectedIdFunc:
            if (isId(token))
            {
                addState(static_cast<int>(TokenType::Id), token);
                state = States::ExpectedLParen;
                i++;
            }
            else
            {
                addError(token, "Expected function name");
                i++;
            }
            break;

        case States::ExpectedLParen:
            if (token.type == TokenType::LParen)
            {
                addState(static_cast<int>(TokenType::LParen), token);
                i++;
                States pState = States::ExpectedTypeOrRParen;

                while (i < tokens.size() && pState != States::ExpectedRParen)
                {
                    const Token& t = tokens[i];

                    switch (pState)
                    {
                    case States::ExpectedTypeOrRParen:
                        if (t.type == TokenType::RParen)
                        {
                            pState = States::ExpectedRParen;
                        }
                        else if (isType(t))
                        {
                            addState(static_cast<int>(TokenType::Type), t);
                            pState = States::ExpectedId;
                            i++;
                        }
                        else
                        {
                            addError(t, "Expected type or ')'");
                            i = sync(i);
                        }
                        break;

                    case States::ExpectedId:
                        if (isId(t) && !isType(t))
                        {
                            addState(static_cast<int>(TokenType::Id), t);
                            pState = States::ExpectedParamTail;
                            i++;
                        }
                        else
                        {
                            addError(t, "Expected parameter name");
                            i = sync(i);
                            pState = States::ExpectedParamTail;
                        }
                        break;

                    case States::ExpectedParamTail:
                        if (t.type == TokenType::Comma)
                        {
                            addState(static_cast<int>(TokenType::Comma), t);
                            pState = States::ExpectedTypeOrRParen;
                            i++;
                        }
                        else if (t.type == TokenType::RParen)
                        {
                            pState = States::ExpectedRParen;
                        }
                        else
                        {
                            addError(t, "Expected ',' or ')'");
                            i = sync(i);
                        }
                        break;

                    default:
                        i++;
                        break;
                    }
                }

                // обработка ')'
                if (i < tokens.size() && tokens[i].type == TokenType::RParen)
                {
                    addState(static_cast<int>(TokenType::RParen), tokens[i]);
                    i++;
                    state = States::ExpectedSemicolon;
                }
                else
                {
                    addError(tokens[i], "Expected ')'");
                    return;
                }
            }
            else
            {
                addError(token, "Expected '('");
                i++;
            }
            break;

        case States::ExpectedSemicolon:
            if (token.type == TokenType::Semicolon)
            {
                addState(static_cast<int>(TokenType::Semicolon), token);
                return;
            }
            else
            {
                addError(token, "Expected ';'");
                i++;
            }
            break;

        default:
            i++;
            break;
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
    return token.type == TokenType::Type ? true : false;
}
bool Parser::isId(const Token& token) const
{
    return token.type == TokenType::Id ? true : false;
}
bool Parser::isSpace(const Token& token) const 
{
    return token.type == TokenType::Space ? true : false;
}