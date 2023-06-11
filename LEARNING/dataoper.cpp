#include "dataoper.h"

dataOper::dataOper(QSqlDatabase * db)
    :db(db)
{

}

bool dataOper::addUser(const userdata &user) {
    QSqlQuery query(*db);
    query.prepare("INSERT INTO form (user, password, position, plus) VALUES (:user, :password, :position, :plus)");
    query.bindValue(":user", user.username);
    query.bindValue(":password", user.password);
    query.bindValue(":position", user.position);
    query.bindValue(":plus", user.plus);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "addUser error:" << query.lastError().text();
        return false;
    }
}

bool dataOper::deleteUser(int userId) {
    QSqlQuery query(*db);
    query.prepare("DELETE FROM form WHERE id=:id");
    query.bindValue(":id", userId);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "deleteUser error:" << query.lastError().text();
        return false;
    }
}

bool dataOper::updateUser(const userdata &user) {
    QSqlQuery query(*db);
    query.prepare("UPDATE form SET user=:user, password=:password, position=:position, plus=:plus WHERE id=:id");
    query.bindValue(":user", user.username);
    query.bindValue(":password", user.password);
    query.bindValue(":position", user.position);
    query.bindValue(":plus", user.plus);
    query.bindValue(":id", user.id);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "updateUser error:" << query.lastError().text();
        return false;
    }
}

QList<userdata> dataOper::queryUsers() {
    QList<userdata> users;
    QSqlQuery query(*db);
    query.prepare("SELECT * FROM form");

    if (query.exec()) {
        while (query.next()) {
            userdata user;
            user.id = query.value("id").toInt();
            user.username = query.value("user").toString();
            user.password = query.value("password").toString();
            user.position = query.value("position").toString();
            user.plus = query.value("plus").toString();
            users.append(user);
        }
    } else {
        qDebug() << "queryUsers error:" << query.lastError().text();
    }

    return users;
}
