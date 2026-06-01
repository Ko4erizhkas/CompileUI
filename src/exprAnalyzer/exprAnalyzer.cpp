#include "src/exprAnalyzer/exprAnalyzer.h"

#include <QStringList>
#include <algorithm>

ExprAnalyzer::ExprAnalyzer(QObject* parent) : QObject(parent) {}

// ============================ Лексический анализ ============================

bool ExprAnalyzer::isLetter(QChar c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool ExprAnalyzer::isDigit(QChar c)
{
    return c >= '0' && c <= '9';
}

bool ExprAnalyzer::isOperatorChar(QChar c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
}

QVector<ExprToken> ExprAnalyzer::tokenize(const QString& src, QVector<ExprError>& lexErrors) const
{
    QVector<ExprToken> tokens;
    int i = 0;
    const int n = src.size();

    while (i < n)
    {
        const QChar c = src[i];

        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            ++i;
            continue;
        }

        const int start = i + 1; // позиция 1-based

        if (isDigit(c))
        {
            // num -> digit { digit }
            QString lex;
            while (i < n && isDigit(src[i])) lex += src[i++];
            tokens.push_back({ ExprTokType::Number, lex, start });
            continue;
        }

        if (isLetter(c))
        {
            // id -> letter { letter | digit | _ }
            QString lex;
            while (i < n && (isLetter(src[i]) || isDigit(src[i]) || src[i] == '_'))
                lex += src[i++];
            tokens.push_back({ ExprTokType::Ident, lex, start });
            continue;
        }

        if (isOperatorChar(c))
        {
            tokens.push_back({ ExprTokType::Operator, QString(c), start });
            ++i;
            continue;
        }

        if (c == '(')
        {
            tokens.push_back({ ExprTokType::LParen, "(", start });
            ++i;
            continue;
        }

        if (c == ')')
        {
            tokens.push_back({ ExprTokType::RParen, ")", start });
            ++i;
            continue;
        }

        // Недопустимый символ (в т.ч. '_' не в составе идентификатора).
        tokens.push_back({ ExprTokType::Bad, QString(c), start });
        lexErrors.push_back({ start, QString("Недопустимый символ '%1'").arg(c) });
        ++i;
    }

    tokens.push_back({ ExprTokType::End, "", n + 1 });
    return tokens;
}

// ===================== Синтаксический анализ (спуск) ========================

const ExprToken& ExprAnalyzer::peek() const
{
    return m_tokens[m_pos];
}

const ExprToken& ExprAnalyzer::advance()
{
    const ExprToken& t = m_tokens[m_pos];
    if (m_tokens[m_pos].type != ExprTokType::End)
        ++m_pos;
    return t;
}

void ExprAnalyzer::addSynError(int pos, const QString& message)
{
    m_synErrors.push_back({ pos, message });
}

QString ExprAnalyzer::newTemp()
{
    return QStringLiteral("t%1").arg(++m_tempCount);
}

// E -> T A
QString ExprAnalyzer::parseE()
{
    QString left = parseT();
    return parseA(left);
}

// A -> e | (+|-) T A
QString ExprAnalyzer::parseA(QString left)
{
    while (peek().type == ExprTokType::Operator &&
           (peek().text == "+" || peek().text == "-"))
    {
        const QString op = advance().text;
        const QString right = parseT();
        const QString temp = newTemp();
        m_tetrads.push_back({ op, left, right, temp });
        left = temp;
    }
    return left;
}

// T -> F B
QString ExprAnalyzer::parseT()
{
    QString left = parseF();
    return parseB(left);
}

// B -> e | (*|/|%) F B
QString ExprAnalyzer::parseB(QString left)
{
    while (peek().type == ExprTokType::Operator &&
           (peek().text == "*" || peek().text == "/" || peek().text == "%"))
    {
        const QString op = advance().text;
        const QString right = parseF();
        const QString temp = newTemp();
        m_tetrads.push_back({ op, left, right, temp });
        left = temp;
    }
    return left;
}

// F -> num | id | ( E )
QString ExprAnalyzer::parseF()
{
    const ExprToken& t = peek();

    switch (t.type)
    {
    case ExprTokType::Number:
        advance();
        return t.text;

    case ExprTokType::Ident:
        m_hasIdentifier = true;
        advance();
        return t.text;

    case ExprTokType::LParen:
    {
        advance();
        const QString inner = parseE();
        if (peek().type == ExprTokType::RParen)
            advance();
        else
            addSynError(peek().pos, "Пропущена закрывающая скобка ')'");
        return inner;
    }

    case ExprTokType::RParen:
        addSynError(t.pos, "Пропущен операнд перед ')'");
        return QStringLiteral("?");

    case ExprTokType::Operator:
        addSynError(t.pos, QString("Пропущен операнд перед оператором '%1'").arg(t.text));
        return QStringLiteral("?");

    case ExprTokType::Bad:
        // Лексическая ошибка уже зафиксирована; пропускаем символ как операнд.
        advance();
        return QStringLiteral("?");

    case ExprTokType::End:
    default:
        addSynError(t.pos, "Пропущен операнд (неожиданный конец выражения)");
        return QStringLiteral("?");
    }
}

// ============================ ПОЛИЗ (Дейкстра) =============================

int ExprAnalyzer::precedence(const QString& op)
{
    if (op == "*" || op == "/" || op == "%") return 2;
    if (op == "+" || op == "-")              return 1;
    return 0;
}

QStringList ExprAnalyzer::toRpn(const QVector<ExprToken>& tokens) const
{
    QStringList output;
    QVector<ExprToken> stack;

    for (const ExprToken& t : tokens)
    {
        switch (t.type)
        {
        case ExprTokType::Number:
        case ExprTokType::Ident:
            output << t.text;
            break;

        case ExprTokType::Operator:
            // Левоассоциативные операторы: выталкиваем не меньшего приоритета.
            while (!stack.isEmpty() &&
                   stack.last().type == ExprTokType::Operator &&
                   precedence(stack.last().text) >= precedence(t.text))
            {
                output << stack.takeLast().text;
            }
            stack.push_back(t);
            break;

        case ExprTokType::LParen:
            stack.push_back(t);
            break;

        case ExprTokType::RParen:
            while (!stack.isEmpty() && stack.last().type != ExprTokType::LParen)
                output << stack.takeLast().text;
            if (!stack.isEmpty()) // снимаем '('
                stack.removeLast();
            break;

        default:
            break;
        }
    }

    while (!stack.isEmpty())
        output << stack.takeLast().text;

    return output;
}

bool ExprAnalyzer::evalRpn(const QStringList& rpn, long long& value, QString& evalError) const
{
    QVector<long long> st;

    for (const QString& tok : rpn)
    {
        if (tok == "+" || tok == "-" || tok == "*" || tok == "/" || tok == "%")
        {
            if (st.size() < 2)
            {
                evalError = "Недостаточно операндов для вычисления";
                return false;
            }
            const long long b = st.takeLast();
            const long long a = st.takeLast();
            long long r = 0;
            if (tok == "+") r = a + b;
            else if (tok == "-") r = a - b;
            else if (tok == "*") r = a * b;
            else if (tok == "/")
            {
                if (b == 0) { evalError = "Деление на ноль"; return false; }
                r = a / b;
            }
            else // %
            {
                if (b == 0) { evalError = "Деление на ноль (остаток)"; return false; }
                r = a % b;
            }
            st.push_back(r);
        }
        else
        {
            bool ok = false;
            const long long num = tok.toLongLong(&ok);
            if (!ok)
            {
                evalError = QString("Не удалось преобразовать '%1' в число").arg(tok);
                return false;
            }
            st.push_back(num);
        }
    }

    if (st.size() != 1)
    {
        evalError = "Некорректное выражение (стек ПОЛИЗ не свёлся к одному значению)";
        return false;
    }

    value = st.last();
    return true;
}

// ============================== Форматирование =============================

QString ExprAnalyzer::tokenTypeName(ExprTokType t)
{
    switch (t)
    {
    case ExprTokType::Number:   return "число (num)";
    case ExprTokType::Ident:    return "идентификатор (id)";
    case ExprTokType::Operator: return "оператор";
    case ExprTokType::LParen:   return "скобка '('";
    case ExprTokType::RParen:   return "скобка ')'";
    case ExprTokType::Bad:      return "НЕДОПУСТИМЫЙ СИМВОЛ";
    case ExprTokType::End:      return "конец";
    default:                    return "?";
    }
}

QString ExprAnalyzer::renderTokens(const QVector<ExprToken>& tokens) const
{
    QString out = "=== Лексический анализ ===\n";
    out += QString("%1  %2  %3\n").arg("Поз", -5).arg("Лексема", -12).arg("Тип");
    for (const ExprToken& t : tokens)
    {
        if (t.type == ExprTokType::End) continue;
        out += QString("%1  %2  %3\n")
                   .arg(t.pos, -5)
                   .arg(t.text, -12)
                   .arg(tokenTypeName(t.type));
    }
    return out;
}

QString ExprAnalyzer::renderErrors(const QVector<ExprError>& errors) const
{
    QString out = QString("Найдено ошибок: %1\n").arg(errors.size());
    QVector<ExprError> sorted = errors;
    std::stable_sort(sorted.begin(), sorted.end(),
                     [](const ExprError& a, const ExprError& b) { return a.pos < b.pos; });
    for (const ExprError& e : sorted)
        out += QString("  позиция %1 — %2\n").arg(e.pos).arg(e.message);
    return out;
}

QString ExprAnalyzer::renderTetrads(const QVector<Tetrad>& tetrads) const
{
    QString out = "=== Тетрады (op, arg1, arg2, result) ===\n";
    out += QString("%1  %2  %3  %4  %5\n")
               .arg("№", -3).arg("op", -4).arg("arg1", -8).arg("arg2", -8).arg("result", -8);
    for (int i = 0; i < tetrads.size(); ++i)
    {
        const Tetrad& q = tetrads[i];
        out += QString("%1  %2  %3  %4  %5\n")
                   .arg(i + 1, -3)
                   .arg(q.op, -4)
                   .arg(q.arg1, -8)
                   .arg(q.arg2, -8)
                   .arg(q.result, -8);
    }
    if (tetrads.isEmpty())
        out += "  (нет операций — выражение является одиночным операндом)\n";
    return out;
}

// ================================ Главный API ==============================

QString ExprAnalyzer::analyze(const QString& text)
{
    // Сброс состояния.
    m_pos = 0;
    m_tempCount = 0;
    m_hasIdentifier = false;
    m_synErrors.clear();
    m_tetrads.clear();

    if (text.trimmed().isEmpty())
        return "Пустое выражение. Введите арифметическое выражение, например: (2 + 3) * 4";

    // 1. Лексический анализ.
    QVector<ExprError> lexErrors;
    m_tokens = tokenize(text, lexErrors);

    QString out;
    out += renderTokens(m_tokens);
    out += "\n";

    // При лексических ошибках к синтаксическому разбору и тетрадам не переходим.
    if (!lexErrors.isEmpty())
    {
        out += "=== Лексические ошибки ===\n";
        out += renderErrors(lexErrors);
        out += "\nВНИМАНИЕ: обнаружены лексические ошибки — "
               "разбор на тетрады и ПОЛИЗ не выполняется.\n";
        return out;
    }

    // 2. Синтаксический анализ методом рекурсивного спуска.
    const QString rootResult = parseE();

    // Проверка лишних символов в конце (например, лишняя ')').
    if (peek().type != ExprTokType::End)
    {
        const ExprToken& t = peek();
        if (t.type == ExprTokType::RParen)
            addSynError(t.pos, "Лишняя закрывающая скобка ')'");
        else
            addSynError(t.pos, QString("Лишний символ '%1' после выражения").arg(t.text));
    }

    out += "=== Синтаксический анализ (рекурсивный спуск) ===\n";

    if (!m_synErrors.isEmpty())
    {
        out += renderErrors(m_synErrors);
        out += "\nВНИМАНИЕ: обнаружены синтаксические ошибки — "
               "разбор на тетрады и ПОЛИЗ не выполняется.\n";
        return out;
    }

    out += "Ошибок не обнаружено. Выражение соответствует грамматике.\n\n";

    // 3. Тетрады (генерируются во время разбора).
    out += renderTetrads(m_tetrads);
    out += QString("Результат выражения: %1\n\n").arg(rootResult);

    // 4. ПОЛИЗ (алгоритм Дейкстры / сортировочной станции).
    const QStringList rpn = toRpn(m_tokens);
    out += "=== ПОЛИЗ (польская инверсная запись) ===\n";
    out += rpn.join(' ') + "\n\n";

    // 5. Вычисление — только для выражений из целых чисел.
    out += "=== Вычисление значения ===\n";
    if (m_hasIdentifier)
    {
        out += "Выражение содержит идентификаторы — числовое значение не вычисляется "
               "(вычисление доступно только для выражений из целых чисел).\n";
    }
    else
    {
        long long value = 0;
        QString evalError;
        if (evalRpn(rpn, value, evalError))
            out += QString("Значение выражения = %1\n").arg(value);
        else
            out += QString("Не удалось вычислить: %1\n").arg(evalError);
    }

    return out;
}
