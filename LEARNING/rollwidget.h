#ifndef ROLLWIDGET_H
#define ROLLWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "animatedlabel.h"
#include "signalanimalabel.h"
#include "cyclebox.h"
#include "userdata.h"
#include <QRect>
#include <QVector>
#include <QLinkedList>
#include <QLineEdit>
#include <cmath>
#include <QWheelEvent>
#include <QTimer>
#include "messagewindows.h"
#include <QClipboard>
#include <QKeySequence>
#include <QShortcut>
#include "changlabel.h"
#include <QFontDatabase>
#include <QPainter>
#include <QDialog>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>


class MainWindow;
class FindDialog;

class RollWidget : public QWidget
{
    Q_OBJECT
public:
    RollWidget(QSqlDatabase * Db,MainWindow * mw,QWidget *parent = nullptr);
    ~RollWidget();
    void show();
    void showOnly(){
        QWidget::show();
    }

private:
    friend class FindDialog;
    //留下修改的接口
    const int LabelSize = 5;
    const int FontSize = 16;

    QSqlDatabase * db;
    CycleBox * dataBox;
    FindDialog * findDia;

    //自己需要维护的数据
    QLinkedList<userdata> data;
    SignalAnimaLabel * labels;
    ChangLabel plusLabel;

    int rolls;
    bool rolling;

    int topLabel;
    int bottomLabel;

    // 只需要初始化的数据
    QTimer * mytimer;
    QShortcut * altC;
    QShortcut * altL;
    QShortcut * CtrlF;



    //如果需要调整成其它通用型，需要修改以下代码
    int GodenNum;



    // 初始化函数
    void initial();
    void animeInitial();
    // 计算用函数
    int fontSizeFunction(double derection,double R){
        return (pow((R * R - derection * derection)/ (R * R),0.5) * (FontSize - 1)) + 1;
    };
    double opacityFunction(int index){
        switch(abs(index - 2)){
        case 0: return 1; break;
        case 1: return 0.6; break;
        case 2: return 0 ;break;
        }
        return -1;
    }
    // rolls维护算法
    void rollsPlus(){++rolls;}
    void rollsDecline() {--rolls;}
    double rolls_to_speed(){
        if(abs(rolls) == 1) return 500;
        else if (abs(rolls) == 2) return 300;
        else if (abs(rolls) == 3) return 200;
        else return 100;
    }
    // 动画执行
    void animeRolling(SignalAnimaLabel::Derection,double speed);
    // 数据追踪
    userdata & getCurrentData();





protected:
void wheelEvent(QWheelEvent *event) override;
void paintEvent(QPaintEvent *event) override;
void mousePressEvent(QMouseEvent *event) override{
    if(event->button() == Qt::LeftButton){
        CopyInfo();
    }
}

private slots:
    void timerSolt();
    void CopyInfo();
public slots:
    void freshData();
    void showFind();


};


class FindDialog : public QWidget
{
    Q_OBJECT
public:
    FindDialog(CycleBox * dataBox,RollWidget * rollWidget,QWidget * parent = nullptr);

protected:
    //为其绘制一个方框
    void paintEvent(QPaintEvent *event) override;

    //阻止鼠标滚动事件，放行其余事件
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    QPushButton * Bt;
    QPushButton * Cancel;
    QLineEdit * edit;
    CycleBox * box;
    RollWidget * roll;
    QTimer * timer;
    QHBoxLayout layout;
    QString str;

    bool finding;

    // 设置窗口可以获得焦点，非模态，
    void initial();

private slots:
    void findStr();
    void timerSolt();

};


#endif // ROLLWIDGET_H
