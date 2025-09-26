#include "master_password_service.h"

#include <QCryptographicHash>
#include <QSqlQuery>
#include <QVariant>

MasterPasswordService::MasterPasswordService(const QSqlDatabase &database, QObject *parent)
    : QObject(parent), m_db(database)
{
}

bool MasterPasswordService::hasPassword() const
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT COUNT(*) FROM master_password"));
    return query.exec() && query.next() && query.value(0).toInt() > 0;
}

bool MasterPasswordService::setPassword(const QString &password)
{
    QSqlQuery query(m_db);
    if (hasPassword())
    {
        query.prepare(QStringLiteral("UPDATE master_password SET hash = :hash WHERE id = 1"));
    }
    else
    {
        query.prepare(QStringLiteral("INSERT INTO master_password (id, hash) VALUES (1, :hash)"));
    }

    query.bindValue(QStringLiteral(":hash"), hashPassword(password));
    const bool success = query.exec();
    if (success)
    {
        emit passwordCreated();
    }
    return success;
}

bool MasterPasswordService::verifyPassword(const QString &password) const
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT hash FROM master_password WHERE id = 1"));
    if (!query.exec() || !query.next())
    {
        return false;
    }

    const QByteArray stored = query.value(0).toByteArray();
    return stored == hashPassword(password);
}

QByteArray MasterPasswordService::hashPassword(const QString &password) const
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512).toHex();
}
