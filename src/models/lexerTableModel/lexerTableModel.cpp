#include "src/models/lexerTableModel/lexerTableModel.h"

LexerAbstractTableModel::LexerAbstractTableModel(QObject* parent) : QObject{parent} {}

int LexerAbstractTableModel::rowCount(const QModelIndex& parent = QModelIndex())
{
    Q_UNUSED(parent);
    return data_.size();
}