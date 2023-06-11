#ifndef DATAOPER_H
#define DATAOPER_H

#include "userdata.h"
#include <QSqlDatabase>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>

class dataOper
{
public:
    dataOper(QSqlDatabase * db);

    //增删改查并返回结果
    bool addUser(const userdata &user);
    bool deleteUser(int userId);
    bool updateUser(const userdata &user);
    QList<userdata> queryUsers();

private:
    QSqlDatabase * db;
};

#endif // DATAOPER_H
