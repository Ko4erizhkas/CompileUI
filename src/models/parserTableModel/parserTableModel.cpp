#include "src/models/parserTableModel/parserTableModel.h"
#include "src/lexer/token/token.h"

ParserTableModel::ParserTableModel(QObject* parent) : QAbstractTableModel(parent) {}

int ParserTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_data.size();
}

int ParserTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 4;
}

QVariant ParserTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    const ErrorParser& error = m_data[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case 0: return error.line;
            case 1: return error.letterPos;
            case 2: return tokenTypeToString(error.states);
            case 3: return error.errorDesc;
            default: return {};
        }
    }
    return QVariant();
}

QVariant ParserTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0: return "Линия";
            case 1: return "Позиция";
            case 2: return "Тип токена";
            case 3: return "Описание ошибки";
        }
    }
    return QVariant();
}

QHash<int, QByteArray> ParserTableModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "display"},
        {Qt::UserRole + 1, "Линия"},
        {Qt::UserRole + 2, "Позиция"},
        {Qt::UserRole + 3, "Тип токена"},
        {Qt::UserRole + 4, "Описание ошибки"}
    };
}

void ParserTableModel::updateErrors(const QVector<ErrorParser>& errors)
{
    beginResetModel();
    m_data = errors;
    endResetModel();
}

QString ParserTableModel::tokenTypeToString(int type)
{
    switch (static_cast<TokenType>(type))
    {
        case TokenType::Id:        return "Id";
        case TokenType::Type:      return "Type";
        case TokenType::LParen:    return "(";
        case TokenType::RParen:    return ")";
        case TokenType::Comma:     return ",";
        case TokenType::Semicolon: return ";";
        case TokenType::Space:     return "Space";
        case TokenType::Unknown:   return "Unknown";
        default:                   return QString::number(type);
    }
}
