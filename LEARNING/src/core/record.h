#pragma once

#include <QString>

struct Record
{
    int id = -1;
    QString title;
    QString username;
    QString password;
    QString category;
    QString notes;

    bool isValid() const noexcept { return !title.trimmed().isEmpty(); }
};
