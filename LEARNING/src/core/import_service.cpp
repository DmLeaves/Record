#include "import_service.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

ImportService::ImportService(QObject *parent)
    : QObject(parent)
{
}

QVector<Record> ImportService::parseTextFile(const QString &filePath, QString *error) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (error)
        {
            *error = tr("无法打开文件：%1").arg(filePath);
        }
        return {};
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    QVector<Record> records;
    QStringList buffer;
    auto flushBuffer = [&]() {
        if (buffer.size() < 3 || buffer.size() > 4)
        {
            if (error)
            {
                *error = tr("格式错误：每条记录必须包含 3 或 4 行");
            }
            records.clear();
            return false;
        }
        Record record;
        record.title = buffer[0].trimmed();
        record.username = buffer[1].trimmed();
        record.password = buffer[2].trimmed();
        record.category = buffer.size() == 4 ? buffer[3].trimmed() : QString();
        records.append(record);
        buffer.clear();
        return true;
    };

    while (!stream.atEnd())
    {
        const QString rawLine = stream.readLine();
        const QString line = rawLine.trimmed();
        if (line.startsWith(QStringLiteral("//")))
        {
            continue;
        }
        if (line.isEmpty())
        {
            if (!buffer.isEmpty())
            {
                if (!flushBuffer())
                {
                    return {};
                }
            }
            continue;
        }
        buffer.append(line);
    }

    if (!buffer.isEmpty())
    {
        if (!flushBuffer())
        {
            return {};
        }
    }

    return records;
}
