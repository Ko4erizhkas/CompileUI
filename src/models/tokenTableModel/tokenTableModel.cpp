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
    return 3;
}

QVariant TokenTableModel::data(const QModelIndex& index, int role) const 
{
    if (!index.isValid()) return QVariant();

    const Token& token = m_data[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
            case Qt::UserRole + 1: return token.line;
            case Qt::UserRole + 2: return token.letterPos;
            case Qt::UserRole + 3: return token.lexem;
//            case Qt::UserRole + 4: return TokenTypeToString(token.type);
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
            case 0: return "Line";
            case 1: return "LetterPos";
            case 2: return "Lexeme";
//            case 3: return "Token_type";
        }
    }
    return QVariant();
}
QHash<int, QByteArray> TokenTableModel::roleNames() const
{
    return {
        {Qt::UserRole + 1, "Line"},
        {Qt::UserRole + 2, "LetterPos"},
        {Qt::UserRole + 3, "Lexeme"},
//        {Qt::UserRole + 4, "Token_Type"}
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