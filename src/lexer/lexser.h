#pragma once
#include <QObject>
#include <QString>
#include <QHash>
#include <QPair>
#include <QVector>

#include "src/lexer/token/errorToken/errorToken.h"

class Lexer : public QObject
{
    Q_OBJECT
public:
	explicit Lexer(QObject *parent = nullptr);
	Q_INVOKABLE QString scan(const QString& text);

	QPair<QVector<Token>, QVector<ErrorToken>> scanReturns(const QString& text);
signals:
	void tokensReady(const QVector<Token>& tokens);

private:
	bool isLetter(QChar c);
	bool isType(const QString& word);
	inline bool isId(QChar c);

	QHash<QString, int> types =
	{
		{"void", 1},
		{"int", 2},
		{"float", 3},
		{"double", 4},
		{"short", 5}
	};
};