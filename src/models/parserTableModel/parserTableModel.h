#pragma once
#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QString>
#include <QHash>
#include <QVector>

#include "src/parser/errorParser/errorParser.h"

class ParserTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ParserTableModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
public slots:
    void updateErrors(const QVector<ErrorParser>& errors);
private:
    QVector<ErrorParser> m_data;
    static QString tokenTypeToString(int type);
};
