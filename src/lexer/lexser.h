#pragma once 
#include <QString>
#include <QObject>
#include <QHash>
class Lexer : public QObject
{
    Q_OBJECT
public:
	explicit Lexer(QObject *parent = nullptr);
	Q_INVOKABLE QString scan(const QString& text);
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