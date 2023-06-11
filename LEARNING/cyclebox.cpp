#include "cyclebox.h"

CycleBox::CycleBox(QSqlDatabase * Db):
    db(Db),
    empty(false)
{
    if(!db->isOpen())
        qDebug() << "Error in Cyclebox with database_open";
    // 焦点大小确认
    focalsSize = 5;
    focals = 0;
    initial();
}


void CycleBox::getFocalList(QLinkedList<userdata> & list){
    list.clear();
    int num = startFocals();
    for(int i = 0;i<focalsSize;i++){
        list.append(container[num]);
        num = next(num);
    }
}

void CycleBox::moveFocal(const Derection & derection, userdata * result){
    focals = next(focals,derection);
    int num = endFocals(derection);
    userdata::copy_ass_to_head(result,container + num);
}

void CycleBox::fresh(){
    focals = 0;
    delete[] container;
    initial();
}

bool CycleBox::Compare(const QString & tar){
    //对tar进行格式清除
    QString str = tar.trimmed();
    if(container[focals].position.contains(str,Qt::CaseInsensitive) || container[focals].plus.contains(str,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}

bool CycleBox::find(const QString & tar){
    //需要进行优化 复杂度过高 但是直接查数据库有着数据不同步的风险
    QString str = tar.trimmed();
    for(int i = 0;i<boxSize;i++){
        if(container[i].position.contains(str,Qt::CaseInsensitive) || container[i].plus.contains(str,Qt::CaseInsensitive)){
            return true;
        }
    }
    return false;
}

void CycleBox::initial(){
    QSqlQuery query;
    query.exec("SELECT COUNT(*) FROM form;");
    query.next();
    boxSize = query.value(0).toInt();
    if(boxSize == 0){
        boxSize = 1;
        container = new userdata[boxSize];
        userdata temp(-1,"","","","右键后台图标添加数据");
        userdata::copy_ass_to_head(&container[0],&temp);
        empty = true;
        return;
    }

    query.exec("SELECT * FROM form;");

    // 创建容器
    container = new userdata[boxSize];

    // 获取数据
    // 0 id
    // 1 username
    // 2 password
    // 3 position
    // 4 plus
    int i = 0;
    while(query.next()){
        container[i].id = query.value(0).toInt();
        container[i].username = query.value(1).toString();
        container[i].password = query.value(2).toString();
        container[i].position = query.value(3).toString();
        container[i].plus = query.value(4).toString();
        ++i;
    }
    empty = false;
}


int CycleBox::endFocals(const Derection & derection){
    if(derection == Derection::FORWARD)
        return (focals + (focalsSize - 1)/2) % boxSize;
    else
        return (focals + boxSize - (focalsSize - 1)/2) % boxSize;
}

int CycleBox::startFocals(const Derection & derection){
    if(derection == Derection::FORWARD)
        return (focals + boxSize - (focalsSize - 1)/2) % boxSize;
    else
        return (focals + (focalsSize - 1)/2) % boxSize;
}

int CycleBox::next(int num ,const Derection & derection){
    if(derection == Derection::FORWARD){
        return (num + 1 + boxSize) % boxSize;

    }
    else{
        return (num - 1 + boxSize) % boxSize;

    }

}

