#pragma once
#include <QAbstractTableModel>
#include <QVector>
#include <QHash>
#include <QString>

#include "src/ast/ast.h"

class AstTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AstTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void updateErrors(const QVector<SemanticError>& errors);

private:
    QVector<SemanticError> m_data;
};
