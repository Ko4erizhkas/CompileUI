#include "functionprototypescanner.h"

#include <QRegularExpression>
#include <QStringList>

FunctionPrototypeScanner::FunctionPrototypeScanner(QObject *parent)
    : QObject(parent)
{
}

QString FunctionPrototypeScanner::scan(const QString &prototype) const
{
    const QString input = prototype.trimmed();
    QStringList errors;

    if (input.isEmpty()) {
        return QStringLiteral("Введите прототип функции.");
    }

    const bool hasSemicolon = input.endsWith(';');
    if (!hasSemicolon) {
        errors << QStringLiteral("В конце прототипа должна быть ';'.");
    }

    const int leftParen = input.indexOf('(');
    const int rightParen = input.lastIndexOf(')');

    if (leftParen < 0 || rightParen < 0 || rightParen < leftParen) {
        errors << QStringLiteral("Некорректные скобки параметров.");
        return errors.join('\n');
    }

    const QString header = input.left(leftParen).trimmed();
    const QString paramsPart = input.mid(leftParen + 1, rightParen - leftParen - 1).trimmed();

    const QStringList headerTokens = header.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (headerTokens.size() != 2) {
        errors << QStringLiteral("Ожидается: <тип> <имя_функции>(...).");
    } else {
        if (!isTypeToken(headerTokens[0])) {
            errors << QStringLiteral("Неизвестный тип возвращаемого значения: %1.").arg(headerTokens[0]);
        }

        if (!isIdentifier(headerTokens[1])) {
            errors << QStringLiteral("Некорректное имя функции: %1.").arg(headerTokens[1]);
        }
    }

    if (!paramsPart.isEmpty() && paramsPart != QStringLiteral("void")) {
        const QStringList params = paramsPart.split(',', Qt::KeepEmptyParts);

        for (int i = 0; i < params.size(); ++i) {
            const QString param = params[i].trimmed();
            if (param.isEmpty()) {
                errors << QStringLiteral("Параметр %1 пустой.").arg(i + 1);
                continue;
            }

            const QStringList paramTokens = param.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            if (paramTokens.size() != 2) {
                errors << QStringLiteral("Параметр %1 должен быть в формате: <тип> <имя>.").arg(i + 1);
                continue;
            }

            if (!isTypeToken(paramTokens[0])) {
                errors << QStringLiteral("Параметр %1: неизвестный тип %2.").arg(i + 1).arg(paramTokens[0]);
            }

            if (!isIdentifier(paramTokens[1])) {
                errors << QStringLiteral("Параметр %1: некорректное имя %2.").arg(i + 1).arg(paramTokens[1]);
            }
        }
    }

    if (errors.isEmpty()) {
        return QStringLiteral("Ошибок нет. Прототип выглядит корректно.");
    }

    return errors.join('\n');
}

bool FunctionPrototypeScanner::isTypeToken(const QString &token) const
{
    static const QStringList allowedTypes {
        QStringLiteral("void"),
        QStringLiteral("bool"),
        QStringLiteral("char"),
        QStringLiteral("short"),
        QStringLiteral("int"),
        QStringLiteral("long"),
        QStringLiteral("float"),
        QStringLiteral("double")
    };

    return allowedTypes.contains(token);
}

bool FunctionPrototypeScanner::isIdentifier(const QString &token) const
{
    static const QRegularExpression identifierRegex(QStringLiteral(R"(^[A-Za-z_][A-Za-z0-9_]*$)"));
    return identifierRegex.match(token).hasMatch();
}
