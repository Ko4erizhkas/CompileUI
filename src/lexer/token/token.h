#pragma once
#include <QString>
enum class TokenType : int
{
	Error = -1,
	Unknown = 0,

	Id = 1,
	Type = 2,

	LParen = 3,		// '('
	RParen = 4,		// ')'
	Comma = 5,		// ','
	Semicolon = 6,	// ';'
	Space = 7		// ' '
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
