#pragma once
#include "src/lexer/token/token.h"
#include <QString>
struct ErrorParser
{
    TokenType errorType;
    int line;
    int letterPos;
    int states;
    QString errorDesc;

    ErrorParser(TokenType error, int ln, int ltrPos, int stts, QString errorMessage) :
        errorType(error), line(ln), letterPos(ltrPos), states(stts), errorDesc(errorMessage) {}

};