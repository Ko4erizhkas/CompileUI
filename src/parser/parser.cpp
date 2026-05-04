#include "src/parser/parser.h"

Parser::Parser(QObject* parent) : QObject{parent} {}

QString Parser::parse(const QString& text)
{
    states.clear();
    createStates(text);

    emit errorsReady(state_errors);

    if (state_errors.isEmpty())
        return "Ошибок нет :)";

    QString res;
    for (const auto& e : state_errors)
    {
        res += "Ошибка в " + QString::number(e.line) + ":" +
               QString::number(e.letterPos) + " - " + e.errorDesc + "\n";
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

    auto ironsState = [&](int& idx, States s)
    {
        const auto& sign = signature[s];
        while (idx < tokens.size())
        {
            for (auto t : sign)
            {
                if (tokens[idx].type == t)
                    return;
            }
            idx++;
        }
    };

    while (i < tokens.size())
    {
        const Token& token = tokens[i];

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
                addState(token);
                state = States::ExpectedIdFunc;
                i++;
            }
            else if (token.type == TokenType::Unknown)
            {
                addError(token, "Ожидался тип возвращаемого значения функции");
                state = States::ExpectedIdFunc;
                i++;
            }
            else
            {
                addError(token, "Ожидался тип возвращаемого значения функции");
                ironsState(i, state);
                continue;
            }
            break;
        }

        case States::ExpectedIdFunc:
        {
            if (isId(token))
            {
                addState(token);
                state = States::ExpectedLParen;
                i++;
            }
            else if (token.type == TokenType::Unknown)
            {
                addError(token, "Недопустимый символ");
                state = States::ExpectedLParen;
                i++;
            }
            else if (isSemicolon(token))
            {
                addError(token, "Ожидалось имя функции");
                state = States::ExpectedTypeFunc;
                i++;
            }
            else
            {
                addError(token, "Ожидалось имя функции");
                ironsState(i, state);
                if (i < tokens.size() && isSemicolon(tokens[i]))
                {
                    state = States::ExpectedTypeFunc;
                    i++;
                }
            }
            break;
        }

        case States::ExpectedLParen:
        {
            if (isLParen(token))
            {
                addState(token);
                state = States::ExpectedStartParams;
                i++;
            }
            else if (isSemicolon(token))
            {
                addError(token, "Ожидался '('");
                state = States::ExpectedTypeFunc;
                i++;
            }
            else
            {
                addError(token, "Ожидался '('");
                ironsState(i, state);
                if (i < tokens.size() && isSemicolon(tokens[i]))
                {
                    state = States::ExpectedTypeFunc;
                    i++;
                }
            }
            break;
        }

        case States::ExpectedStartParams:
        {
            if (isType(token))
            {
                addState(token);
                state = States::ExpectedId;
                i++;
            }
            else if (token.type == TokenType::Unknown)
            {
                addError(token, "Недопустимый символ");
                state = States::ExpectedId;
                i++;
            }
            else if (isRParen(token))
            {
                addState(token);
                state = States::ExpectedSemicolon;
                i++;
            }
            else if (isSemicolon(token))
            {
                addError(token, "Ожидался тип параметра или ')'");
                state = States::ExpectedTypeFunc;
                i++;
            }
            else
            {
                addError(token, "Ожидался тип параметра или ')'");
                i++;
            }
            break;
        }

        case States::ExpectedId:
        {
            if (isId(token))
            {
                addState(token);
                state = States::ExpectedComma;
                i++;
            }
            else if (token.type == TokenType::Unknown)
            {
                addError(token, "Недопустимый символ");
                state = States::ExpectedComma;
                i++;
            }
            else if (isComma(token))
            {
                addError(token, "Ожидалось имя параметра");
                state = States::ExpectedType;
                i++;
            }
            else if (isRParen(token))
            {
                addError(token, "Ожидалось имя параметра");
                state = States::ExpectedSemicolon;
                i++;
            }
            else if (isSemicolon(token))
            {
                addError(token, "Ожидалось имя параметра");
                state = States::ExpectedTypeFunc;
                i++;
            }
            else
            {
                addError(token, "Ожидалось имя параметра");
                ironsState(i, state);
                if (i < tokens.size())
                {
                    const Token& sync = tokens[i];
                    if (isComma(sync))          { state = States::ExpectedType;      i++; }
                    else if (isRParen(sync))    { state = States::ExpectedSemicolon; i++; }
                    else if (isSemicolon(sync)) { state = States::ExpectedTypeFunc;  i++; }
                }
            }
            break;
        }

        case States::ExpectedComma:
        {
            if (isComma(token))
            {
                addState(token);
                state = States::ExpectedType;
                i++;
            }
            else if (isRParen(token))
            {
                addState(token);
                state = States::ExpectedSemicolon;
                i++;
            }
            else if (isSemicolon(token))
            {
                addError(token, "Ожидался ',' или ')'");
                state = States::ExpectedTypeFunc;
                i++;
            }
            else
            {
                addError(token, "Ожидался ',' или ')'");
                ironsState(i, state);
                if (i < tokens.size())
                {
                    const Token& sync = tokens[i];
                    if (isComma(sync))          { state = States::ExpectedType;      i++; }
                    else if (isRParen(sync))    { state = States::ExpectedSemicolon; i++; }
                    else if (isSemicolon(sync)) { state = States::ExpectedTypeFunc;  i++; }
                }
            }
            break;
        }

        case States::ExpectedType:
        {
            if (isType(token))
            {
                addState(token);
                state = States::ExpectedId;
                i++;
            }
            else if (token.type == TokenType::Unknown)
            {
                addError(token, "Недопустимый символ");
                state = States::ExpectedId;
                i++;
            }
            else if (isSemicolon(token))
            {
                addError(token, "Ожидался тип параметра");
                state = States::ExpectedTypeFunc;
                i++;
            }
            else
            {
                addError(token, "Ожидался тип параметра");
                ironsState(i, state);
                if (i < tokens.size() && isSemicolon(tokens[i]))
                {
                    state = States::ExpectedTypeFunc;
                    i++;
                }
            }
            break;
        }

        case States::ExpectedSemicolon:
        {
            if (isSemicolon(token))
            {
                addState(token);
                state = States::Accepted;
                i++;
            }
            else
            {
                addError(token, "Ожидался ';'");
                state = States::ExpectedTypeFunc;
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
            addError(token, "Неизвестное состояние");
            i++;
            break;
        }
        }
    }

    if (!tokens.isEmpty() && state != States::Accepted && state != States::ExpectedTypeFunc)
    {
        const Token& last = tokens.last();
        Token eofToken(TokenType::End_of_token, "", last.line, last.letterPos + 1);
        switch (state)
        {
        case States::ExpectedIdFunc:
            addError(eofToken, "Ожидалось имя функции");
            addError(eofToken, "Ожидался ';'");
            break;
        case States::ExpectedLParen:
            addError(eofToken, "Ожидался '('");
            addError(eofToken, "Ожидался ';'");
            break;
        case States::ExpectedStartParams:
            addError(eofToken, "Ожидался тип параметра или ')'");
            addError(eofToken, "Ожидался ';'");
            break;
        case States::ExpectedId:
            addError(eofToken, "Ожидалось имя параметра");
            addError(eofToken, "Ожидался ';'");
            break;
        case States::ExpectedComma:
            addError(eofToken, "Ожидался ',' или ')'");
            addError(eofToken, "Ожидался ';'");
            break;
        case States::ExpectedType:
            addError(eofToken, "Ожидался тип параметра");
            addError(eofToken, "Ожидался ';'");
            break;
        case States::ExpectedSemicolon:
            addError(eofToken, "Ожидался ';'");
            break;
        default:
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

void Parser::addState(const Token& token)
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
