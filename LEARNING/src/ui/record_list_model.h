#pragma once

#include "../core/record.h"

#include <QAbstractListModel>
#include <QVector>

class RecordListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        UsernameRole,
        CategoryRole
    };

    explicit RecordListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setRecords(const QVector<Record> &records);
    const Record &recordAt(int row) const;

private:
    QVector<Record> m_records;
};
