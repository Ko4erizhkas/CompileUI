#include "src/parser/parser.h"

Parser::Parser(QObject* parent) : QObject{parent} {}

// Parser::Parser(QObject* parent, const QVector<Token>& tokens) : 
//                 QObject{parent}, m_tokens(tokens) {}


void Parser::parse(const QString& text)
{
    Lexer lexer;
    const QVector<Token> tokens = lexer.scanReturns(text).first;
    const QVector<ErrorToken> errors = lexer.scanReturns(text).second;

    for (const auto& token : tokens)
    {
        switch (m_state)
        {
            case 0:
                if (token.type == TokenType::Type);
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            default: break;
        }
    }

}