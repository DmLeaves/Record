#include "record_repository.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

RecordRepository::RecordRepository(const QSqlDatabase &database, QObject *parent)
    : QObject(parent), m_db(database)
{
}

QVector<Record> RecordRepository::fetchAll() const
{
    QVector<Record> records;
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT id, title, user, password, position, plus FROM form ORDER BY id DESC"));

    if (query.exec())
    {
        while (query.next())
        {
            records.append(fromQuery(query));
        }
    }

    return records;
}

QVector<Record> RecordRepository::search(const QString &keyword) const
{
    QVector<Record> records;
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT id, title, user, password, position, plus FROM form\n"
                                  "WHERE title LIKE :keyword OR user LIKE :keyword OR position LIKE :keyword OR plus LIKE :keyword\n"
                                  "ORDER BY id DESC"));
    query.bindValue(QStringLiteral(":keyword"), QStringLiteral("%") + keyword + QStringLiteral("%"));

    if (query.exec())
    {
        while (query.next())
        {
            records.append(fromQuery(query));
        }
    }

    return records;
}

Record RecordRepository::fetchById(int id) const
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT id, title, user, password, position, plus FROM form WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);
    if (query.exec() && query.next())
    {
        return fromQuery(query);
    }
    return {};
}

Record RecordRepository::save(const Record &record)
{
    QSqlQuery query(m_db);
    if (record.id < 0)
    {
        query.prepare(QStringLiteral(
            "INSERT INTO form (title, user, password, position, plus) VALUES (:title, :user, :password, :position, :plus)"));
    }
    else
    {
        query.prepare(QStringLiteral(
            "UPDATE form SET title = :title, user = :user, password = :password, position = :position, plus = :plus WHERE id = :id"));
        query.bindValue(QStringLiteral(":id"), record.id);
    }

    query.bindValue(QStringLiteral(":title"), record.title);
    query.bindValue(QStringLiteral(":user"), record.username);
    query.bindValue(QStringLiteral(":password"), record.password);
    query.bindValue(QStringLiteral(":position"), record.category);
    query.bindValue(QStringLiteral(":plus"), record.notes);

    if (!query.exec())
    {
        qWarning() << "Failed to save record:" << query.lastError();
        return {};
    }

    Record result = record;
    if (record.id < 0)
    {
        result.id = query.lastInsertId().toInt();
    }

    emit recordsChanged();
    return result;
}

bool RecordRepository::remove(int id)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("DELETE FROM form WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec())
    {
        qWarning() << "Failed to delete record:" << query.lastError();
        return false;
    }

    emit recordsChanged();
    return true;
}

Record RecordRepository::fromQuery(const QSqlQuery &query) const
{
    Record record;
    record.id = query.value(0).toInt();
    record.title = query.value(1).toString();
    record.username = query.value(2).toString();
    record.password = query.value(3).toString();
    record.category = query.value(4).toString();
    record.notes = query.value(5).toString();
    return record;
}
