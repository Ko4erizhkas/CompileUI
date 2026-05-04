#pragma once
#include <QString>
enum class TokenType : int
{
	Error = -1,
	Unknown = 0,
	End_of_token = 9,

	Id = 1,
	Type = 2,

	LParen = 3,		// '('
	RParen = 4,		// ')'
	Comma = 5,		// ','
	Semicolon = 6,	// ';'
	Space = 7,		// ' '
	InvalidType = 8	// valid identifier but not in types list
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
