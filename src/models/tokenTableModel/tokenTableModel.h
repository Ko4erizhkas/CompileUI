#pragma once
#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QString>
#include <QHash>
#include <QVector>

#include "src/lexer/token/token.h"

class TokenTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TokenTableModel(QObject* parent = nullptr);
    int rowCount (const QModelIndex& parent = QModelIndex()) const override;
    int columnCount (const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
public slots:
    void updateTokens(const QVector<Token>& tokens);
private:
    QVector<Token> m_data;
    static QString TokenTypeToString(TokenType tp);
};