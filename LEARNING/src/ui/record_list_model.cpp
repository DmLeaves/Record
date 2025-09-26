#include "record_list_model.h"

RecordListModel::RecordListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int RecordListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_records.size();
}

QVariant RecordListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_records.size())
    {
        return {};
    }

    const Record &record = m_records.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
    case TitleRole:
        return record.title;
    case UsernameRole:
        return record.username;
    case CategoryRole:
        return record.category;
    case IdRole:
        return record.id;
    default:
        return {};
    }
}

QHash<int, QByteArray> RecordListModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[IdRole] = "id";
    names[TitleRole] = "title";
    names[UsernameRole] = "username";
    names[CategoryRole] = "category";
    return names;
}

void RecordListModel::setRecords(const QVector<Record> &records)
{
    beginResetModel();
    m_records = records;
    endResetModel();
}

const Record &RecordListModel::recordAt(int row) const
{
    static Record empty;
    if (row < 0 || row >= m_records.size())
    {
        return empty;
    }
    return m_records.at(row);
}
