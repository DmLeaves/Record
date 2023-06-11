#ifndef CYCLEBOX_H
#define CYCLEBOX_H
#include <QSqlDatabase>
#include <QVector>
#include <QRect>
#include "userdata.h"
#include <QVector>
#include <QDebug>
#include <QSqlQuery>
#include <QLinkedList>


class CycleBox
{
public:
    enum class Derection{
        FORWARD,ROLLBACK
    };

    CycleBox(QSqlDatabase * db);
    ~ CycleBox(){ delete[] container; }
    void getFocalList(QLinkedList<userdata> & list);
    void moveFocal(const Derection & derection, userdata * result);
    void fresh();
    bool isEmpty(){return empty;}

    //提供一个快速判断的函数
    bool Compare(const QString & tar);

    //查找数据的接口 判断数据中有没有目标
    bool find(const QString & tar);

    //简单接口 过于容易出现BUG
    //void SetFocalsSize(int size){focalsSize = size;}


private:
    //变量
    QSqlDatabase * db;
    userdata *  container;
    int focals;
    int boxSize;
    int focalsSize;
    bool empty;

    //函数
    void initial();
    int startFocals(const Derection & derection = Derection::FORWARD);
    int endFocals(const Derection & derection = Derection::FORWARD);
    int next(int num ,const Derection & derection = Derection::FORWARD);




};

#endif // CYCLEBOX_H
