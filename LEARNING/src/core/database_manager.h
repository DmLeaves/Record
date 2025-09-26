#pragma once

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    bool open(const QString &databasePath);
    QSqlDatabase database() const;
    bool ensureSchema();

private:
    QSqlDatabase m_database;
};
