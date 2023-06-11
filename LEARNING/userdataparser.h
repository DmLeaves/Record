#ifndef USERDATAPARSER_H
#define USERDATAPARSER_H
#include "userdata.h"
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QVector>
#include <QDebug>

class UserDataParser
{
public:
    UserDataParser();
    //通过file文件按行读取，检查是否为有效行
    //有效行的定义为： 非“//”开头的非空行，以“//”开头的行不会影响程序运行，直接忽略
    //有效行读取后要进行处理，删除两端的空格和回车等不可见字符
    //找到第一个有效行读出并开始计数，直到遇到第一个空行，如果计数为3或者4，则是一个有效数据
    //将有效数据以 username，password，position，plus的顺序写入datauser的结构中，plus在计数为3的时候为空
    //然后将datauser结构的数据存储到dataList中
    //如果直到文件结束都是有效数据，则返回true，如果文件中途碰到了计数小于3或者大于4就碰到了空行的情况，则返回false，并停止继续读取文件
    bool GetResult(QFile * file,QVector<userdata> & dataList);

private:
    //用于计数
    int currentIndex;

    //检查是否为注释行
    bool isCommentLine(const QString &line) const;
    //检查是否为空行
    bool isEmptyLine(const QString &line) const;
};

#endif // USERDATAPARSER_H
