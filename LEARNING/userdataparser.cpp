#include "userdataparser.h"

UserDataParser::UserDataParser() : currentIndex(0)
{
}

bool UserDataParser::GetResult(QFile *file, QVector<userdata> &dataList)
{
    currentIndex = 0;
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Unable to open file";
        return false;
    }

    QTextStream in(file);
    QString line;
    QStringList validLines;
    userdata user;
    bool isValidData = false;

    in.setCodec("UTF-8");

    while (!in.atEnd()){
        line = in.readLine();
        line = line.trimmed();
        qDebug() << line << currentIndex;
        qDebug() << "Value: " << isCommentLine(line) << " " << isEmptyLine(line);
        if (isCommentLine(line)){
            continue;
        }

        if (isEmptyLine(line)){
            if (currentIndex >= 3 && currentIndex <= 4){
                isValidData = true;
            }else if(currentIndex == 0){
                continue;
            }else{
                isValidData = false;
                break;
            }

            if (isValidData){
                user.username = validLines[0];
                user.password = validLines[1];
                user.position = validLines[2];
                user.plus = (currentIndex == 4) ? validLines[3] : QString();
                dataList.append(user);
                validLines.clear();
                currentIndex = 0;
            }
        }else{
            validLines.append(line);
            currentIndex++;
        }
    }

    line = in.readLine();
    line = line.trimmed();
    qDebug() << line << currentIndex;
    qDebug() << "Value: " << isCommentLine(line) << " " << isEmptyLine(line);
    if (currentIndex >= 3 && currentIndex <= 4){
        user.username = validLines[0];
        user.password = validLines[1];
        user.position = validLines[2];
        user.plus = (currentIndex == 4) ? validLines[3] : QString();
        if(dataList.size() == 0) isValidData = true;
        dataList.append(user);

    }else{
        if(currentIndex != 0)isValidData = false;
    }
    file->close();
    return isValidData;
}

bool UserDataParser::isCommentLine(const QString &line) const
{
    return line.startsWith("//");
}

bool UserDataParser::isEmptyLine(const QString &line) const
{
    return line.isEmpty();
}
