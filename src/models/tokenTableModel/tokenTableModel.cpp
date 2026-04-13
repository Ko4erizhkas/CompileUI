#include "src/models/tokenTableModel/tokenTableModel.h"

TokenTableModel::TokenTableModel(QObject* parent) : QAbstractTableModel(parent) {}

int TokenTableModel::rowCount(const QModelIndex& parent) const 
{
    if (parent.isValid()) return 0;
    return static_cast<int>(m_data.size());
}
int TokenTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 4;
}

QVariant TokenTableModel::data(const QModelIndex& index, int role) const 
{
    if (!index.isValid()) return QVariant();

    const Token& token = m_data[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
            case 0: return token.line;
            case 1: return token.letterPos;
            case 2: return token.lexem;
            case 3: return TokenTypeToString(token.type);
            default: return {};
        }
    }
    return QVariant();
}
QVariant TokenTableModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
    if (role != Qt::DisplayRole) return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch(section)
        {
            case 0: return "Линия";
            case 1: return "Позиция";
            case 2: return "Лексема";
            case 3: return "Тип токена";
        }
    }
    return QVariant();
}
QHash<int, QByteArray> TokenTableModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "display"},
        {Qt::UserRole + 1, "Линия"},
        {Qt::UserRole + 2, "Позиция"},
        {Qt::UserRole + 3, "Лексема"},
        {Qt::UserRole + 4, "Тип токена"}
    };
}
void TokenTableModel::updateTokens(const QVector<Token>& tokens) 
{
    beginResetModel();
    m_data = tokens;
    endResetModel();
}
// Private
QString TokenTableModel::TokenTypeToString(TokenType tp)
{
    switch(tp)
    {
        case TokenType::Id: return QString("Id");
        case TokenType::Type: return QString("Type");
        case TokenType::LParen: return QString("(");
        case TokenType::RParen: return QString(")");
        case TokenType::Comma: return QString(",");
        case TokenType::Semicolon: return QString(";");
        case TokenType::Space: return QString("Space");
        case TokenType::Unknown: return QString("Unknown");
        default: return QString("Default Unknown");
    }
}