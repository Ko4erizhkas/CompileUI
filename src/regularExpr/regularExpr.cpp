#include "src/regularExpr/regularExpr.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

RegularExpr::RegularExpr(QObject* parent) : QObject(parent) {}

QString RegularExpr::expression1(const QString text)
{
    QRegularExpression regExpr(R"(\b9\d*\b)");
    QRegularExpressionMatchIterator it = regExpr.globalMatch(text);

    QString res;
    res += "Найденны:\n";
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        res += m.captured(0) + '\n';
    }
    return res;
}
QString RegularExpr::expression2(const QString text)
{
    QRegularExpression regExpr(R"(\b(?:0[oO][0-7]+|&[oO][0-7]+|0[0-7]*)\b)");
    QRegularExpressionMatchIterator it = regExpr.globalMatch(text);

    QString res;
    res += "Найденны:\n";
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        res += m.captured(0) + '\n';
    }
    return res;
}
QString RegularExpr::expression3(const QString text)
{
    QRegularExpression regExpr(
        R"((?:[A-Z2-7]{8})*(?:[A-Z2-7]{2}={6}|[A-Z2-7]{4}={4}|
        [A-Z2-7]{5}={3}|[A-Z2-7]{7}=|[A-Z2-7]{8}))"
    );
    QRegularExpressionMatchIterator it = regExpr.globalMatch(text);

    QString res;
    res += "Найденны:\n";
    while (it.hasNext()) {
        QRegularExpressionMatch m = it.next();
        res += m.captured(0) + '\n';
    }
    return res;
}