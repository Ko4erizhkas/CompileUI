#include "lexser.h"
#include "src/lexer/token/token.h"
#include "src/lexer/token/errorToken/errorToken.h"
#include <vector>


Lexer::Lexer(QObject* parent) : QObject{parent} {}

inline bool Lexer::isId(QChar c)
{
	return c.isLetter() || c == '_';
}
bool Lexer::isType(const QString& word)
{
	return types.contains(word);
}


QString Lexer::scan(const QString& text)
{
	QVector<Token> tokens;
	QVector<ErrorToken> errors;

    QString result = "";
	int pos = 0;

	int letter = 1;
	int line = 1;
	bool expectedType = true;

	while (pos < text.size())
	{
		if (isId(text[pos]))
		{
			int start = pos;
			int startLetter = letter;

			while (pos < text.size() && isId(text[pos]))
			{
				pos++;
				letter++;
			}

			QString word = text.mid(start, pos - start);
			
			if (expectedType)
			{
				if (isType(word))
				{
					tokens.emplace_back(TokenType::Type, word, line, startLetter);
				}
				else
				{
					errors.emplace_back(TokenType::Unknown, "Invaild type: " + word, line, startLetter);
					
				}
				expectedType = false;
			}
			else
			{
				tokens.emplace_back(TokenType::Id, word, line, startLetter);
			}
			continue;
		}
		switch (text[pos].unicode())
		{
		case '(':
			tokens.emplace_back( TokenType::LParen, "(", line, letter);
			expectedType = true;
			letter++;
			break;
		case ')':
			tokens.emplace_back( TokenType::RParen, ")", line, letter);
			letter++;
			break;
		case ',':
			tokens.emplace_back( TokenType::Comma, ",", line, letter);
			expectedType = true;
			letter++;
			break;
		case ';':
			tokens.emplace_back(TokenType::Semicolon, ";", line, letter);
			expectedType = true;
			letter++;
			break;
		case ' ':
			tokens.emplace_back(TokenType::Space, "space", line, letter);
			pos++;
			letter++;
			continue;
		case '\n':
			line++;
			letter = 1;
			pos++;
			break;
		case '\t': 
			letter++;	
			break;
		default:
			errors.emplace_back(TokenType::Unknown, QString("Unknown token: ") + QString(1, text[pos]), line, letter);
			pos++;
			letter++;
			break;
		}
		pos++;
	}


	emit tokensReady(tokens);

	if (errors.empty())
	{
		result += "Errors not founds :)";
	}
	for (auto& e : errors)
	{
		result += "Error at: " + QString::number(e.line) + QChar(':') + QString::number(e.letterPos) + QChar(' ')+ e.errorMessage + QChar('\n');
		
	}

	
    
	return result;
}

QPair<QVector<Token>, QVector<ErrorToken>> Lexer::scanReturns(const QString& text)
{
	QVector<Token> tokens;
	QVector<ErrorToken> errors;

    QString result = "";
	int pos = 0;

	int letter = 1;
	int line = 1;
	bool expectedType = true;

	while (pos < text.size())
	{
		if (isId(text[pos]))
		{
			int start = pos;
			int startLetter = letter;

			while (pos < text.size() && isId(text[pos]))
			{
				pos++;
				letter++;
			}

			QString word = text.mid(start, pos - start);
			
			if (expectedType)
			{
				if (isType(word))
				{
					tokens.emplace_back(TokenType::Type, word, line, startLetter);
				}
				else
				{
					errors.emplace_back(TokenType::Unknown, "Invaild type: " + word, line, startLetter);
					
				}
				expectedType = false;
			}
			else
			{
				tokens.emplace_back(TokenType::Id, word, line, startLetter);
			}
			continue;
		}
		switch (text[pos].unicode())
		{
		case '(':
			tokens.emplace_back( TokenType::LParen, "(", line, letter);
			expectedType = true;
			letter++;
			break;
		case ')':
			tokens.emplace_back( TokenType::RParen, ")", line, letter);
			letter++;
			break;
		case ',':
			tokens.emplace_back( TokenType::Comma, ",", line, letter);
			expectedType = true;
			letter++;
			break;
		case ';':
			tokens.emplace_back(TokenType::Semicolon, ";", line, letter);
			expectedType = true;
			letter++;
			break;
		case ' ':
			tokens.emplace_back(TokenType::Space, "space", line, letter);
			pos++;
			letter++;
			continue;
		case '\n':
			line++;
			letter = 1;
			pos++;
			break;
		case '\t': 
			letter++;	
			break;
		default:
			errors.emplace_back(TokenType::Unknown, QString("Unknown token: ") + QString(1, text[pos]), line, letter);
			pos++;
			letter++;
			break;
		}
		pos++;
	}
    return qMakePair(tokens, errors);
}