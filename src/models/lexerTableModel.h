#pragma once
#include <QAbstractTableModel>
#include <QObject>
#include <QString>
#include <QVariant>
class LexerAbstractTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LexerAbstractTableModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
};