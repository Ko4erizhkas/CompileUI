#pragma once
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

// Лабораторная работа: рекурсивный спуск по КС-грамматике
//   E -> T A
//   A -> e | + T A | - T A
//   T -> F B
//   B -> e | * F B | / F B | % F B
//   F -> num | id | ( E )
//   id  -> letter { letter | digit | _ }
//   num -> digit { digit }
//
// Модуль выполняет:
//   1) лексический анализ (числа, идентификаторы, операторы, скобки);
//   2) синтаксический анализ методом рекурсивного спуска с поиском
//      лексических и синтаксических ошибок;
//   3) генерацию тетрад (op, arg1, arg2, result) с временными t1, t2, ...
//      (только для корректных строк);
//   4) построение ПОЛИЗ (алгоритм Дейкстры / сортировочной станции);
//   5) вычисление значения ПОЛИЗ (только для выражений из целых чисел).

enum class ExprTokType
{
    Number,   // num
    Ident,    // id
    Operator, // + - * / %
    LParen,   // (
    RParen,   // )
    Bad,      // недопустимый символ
    End       // конец строки
};

struct ExprToken
{
    ExprTokType type;
    QString     text;
    int         pos; // позиция первого символа лексемы (1-based)
};

struct ExprError
{
    int     pos;
    QString message;
};

struct Tetrad
{
    QString op;
    QString arg1;
    QString arg2;
    QString result;
};

class ExprAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit ExprAnalyzer(QObject* parent = nullptr);

    // Полный отчёт по всем этапам анализа выражения.
    Q_INVOKABLE QString analyze(const QString& text);

private:
    // --- Лексический анализ ---
    QVector<ExprToken> tokenize(const QString& src, QVector<ExprError>& lexErrors) const;
    static bool isLetter(QChar c);
    static bool isDigit(QChar c);
    static bool isOperatorChar(QChar c);

    // --- Синтаксический анализ (рекурсивный спуск) ---
    // Состояние разбора держим в полях, чтобы методы оставались краткими.
    QVector<ExprToken> m_tokens;
    int                m_pos = 0;
    int                m_tempCount = 0;
    bool               m_hasIdentifier = false;
    QVector<ExprError> m_synErrors;
    QVector<Tetrad>    m_tetrads;

    const ExprToken& peek() const;
    const ExprToken& advance();
    void addSynError(int pos, const QString& message);
    QString newTemp();

    QString parseE(); // E -> T A
    QString parseA(QString left); // A -> e | (+|-) T A
    QString parseT(); // T -> F B
    QString parseB(QString left); // B -> e | (*|/|%) F B
    QString parseF(); // F -> num | id | ( E )

    // --- ПОЛИЗ + вычисление ---
    QStringList toRpn(const QVector<ExprToken>& tokens) const;
    static int precedence(const QString& op);
    bool evalRpn(const QStringList& rpn, long long& value, QString& evalError) const;

    // --- Форматирование отчёта ---
    QString renderTokens(const QVector<ExprToken>& tokens) const;
    QString renderErrors(const QVector<ExprError>& errors) const;
    QString renderTetrads(const QVector<Tetrad>& tetrads) const;
    static QString tokenTypeName(ExprTokType t);
};
