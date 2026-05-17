#include "src/models/astTableModel/astTableModel.h"

AstTableModel::AstTableModel(QObject* parent) : QAbstractTableModel(parent) {}

int AstTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_data.size();
}

int AstTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return 4;
}

QVariant AstTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();
    const SemanticError& e = m_data[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case 0: return e.rule;
            case 1: return e.line;
            case 2: return e.letterPos;
            case 3: return e.message;
            default: return {};
        }
    }
    return QVariant();
}

QVariant AstTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0: return "Правило";
            case 1: return "Линия";
            case 2: return "Позиция";
            case 3: return "Описание ошибки";
        }
    }
    return QVariant();
}

QHash<int, QByteArray> AstTableModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "display"},
        {Qt::UserRole + 1, "Правило"},
        {Qt::UserRole + 2, "Линия"},
        {Qt::UserRole + 3, "Позиция"},
        {Qt::UserRole + 4, "Описание"}
    };
}

void AstTableModel::updateErrors(const QVector<SemanticError>& errors)
{
    beginResetModel();
    m_data = errors;
    endResetModel();
}
