#pragma once

#include "record.h"

#include <QObject>
#include <QString>
#include <QVector>

class ImportService : public QObject
{
    Q_OBJECT
public:
    explicit ImportService(QObject *parent = nullptr);

    QVector<Record> parseTextFile(const QString &filePath, QString *error) const;
};
