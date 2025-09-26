#pragma once

#include <QObject>
#include <QSqlDatabase>

class MasterPasswordService : public QObject
{
    Q_OBJECT
public:
    explicit MasterPasswordService(const QSqlDatabase &database, QObject *parent = nullptr);

    bool hasPassword() const;
    bool setPassword(const QString &password);
    bool verifyPassword(const QString &password) const;

signals:
    void passwordCreated();

private:
    QByteArray hashPassword(const QString &password) const;

    QSqlDatabase m_db;
};
