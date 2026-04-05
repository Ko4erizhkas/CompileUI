#pragma once
#include <QString>
enum class TokenType
{
	Unknown,

	Id,
	Type,

	LParen,		// '('
	RParen,		// ')'
	Comma,		// ','
	Semicolon,	// ';'
	Space		// ' '
};
struct Token
{
	TokenType type;
	QString lexem;
	int line;
	int letterPos;

	Token(TokenType t, const QString& l, int li, int ltr) :
		type(t), lexem(l), line(li), letterPos(ltr) {}
};
