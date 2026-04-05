#pragma once
#include <QString>
#include <src/lexer/token/token.h>
struct ErrorToken
{
	TokenType token;
	QString errorMessage;
	int line;
	int letterPos;
	ErrorToken(TokenType t, const QString& message, int l, int ltr) :
		token(t), errorMessage(message), line(l), letterPos(ltr) {}
};