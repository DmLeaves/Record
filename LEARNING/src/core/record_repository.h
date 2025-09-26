#pragma once

#include "record.h"

#include <QObject>
#include <QSqlDatabase>
#include <QVector>

class QSqlQuery;

class RecordRepository : public QObject
{
    Q_OBJECT
public:
    explicit RecordRepository(const QSqlDatabase &database, QObject *parent = nullptr);

    QVector<Record> fetchAll() const;
    QVector<Record> search(const QString &keyword) const;
    Record fetchById(int id) const;
    Record save(const Record &record);
    bool remove(int id);

signals:
    void recordsChanged();

private:
    Record fromQuery(const QSqlQuery &query) const;

    QSqlDatabase m_db;
};
