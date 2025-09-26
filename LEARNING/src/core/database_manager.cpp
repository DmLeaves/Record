#include "database_manager.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace
{
QString databaseDirectory()
{
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appData.isEmpty() ? QDir::homePath() + QStringLiteral("/.record") : appData);
    if (!dir.exists())
    {
        dir.mkpath(QStringLiteral("."));
    }
    return dir.absolutePath();
}
}

DatabaseManager::DatabaseManager()
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isValid())
    {
        m_database.close();
    }
}

bool DatabaseManager::open(const QString &databasePath)
{
    if (m_database.isValid())
    {
        return true;
    }

    m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    const QString finalPath = databasePath.isEmpty() ? (databaseDirectory() + QStringLiteral("/data.db")) : databasePath;
    m_database.setDatabaseName(finalPath);
    m_database.setUserName(QStringLiteral("RecordApp"));

    if (!m_database.open())
    {
        return false;
    }

    return ensureSchema();
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

bool DatabaseManager::ensureSchema()
{
    if (!m_database.isValid())
    {
        return false;
    }

    QSqlQuery query(m_database);

    const QString createMaster = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS master_password (\n"
        "    id INTEGER PRIMARY KEY CHECK (id = 1),\n"
        "    hash TEXT NOT NULL\n"
        ")");

    if (!query.exec(createMaster))
    {
        return false;
    }

    const QString createRecords = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS form (\n"
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
        "    title TEXT NOT NULL,\n"
        "    user TEXT NOT NULL,\n"
        "    password TEXT NOT NULL,\n"
        "    position TEXT,\n"
        "    plus TEXT\n"
        ")");

    if (!query.exec(createRecords))
    {
        return false;
    }

    return true;
}
