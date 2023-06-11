#ifndef SIGNALANIMALABEL_H
#define SIGNALANIMALABEL_H

#include "animatedlabel.h"
#include <QRect>
#include "data.h"
#include <QVector>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>
#include <QDebug>

class SignalAnimaLabel : public AnimatedLabel
{
public:
    enum class Derection{
        FORWAD,REVERSE
    };

    void setDerection(Derection der);
    void setCurrentIndex(int index);
    void setSpeed(double speed);
    void addKeyframes(int index,const QRect & rect,int fontSize,double opacity);
    void StartNext(double sSpeed = -1);
    void StartToIndex(int index,double sSpeed = -1);
    void clearKeyFrams();


    SignalAnimaLabel();

private:
    const int LABELSIZE = 5;

    Derection derection;
    int currentIndex;
    int speed;
    QVector<leaves::data> nodeList;
    QPropertyAnimation * rectAnim;
    QPropertyAnimation * fontSizeAnim;
    QGraphicsOpacityEffect * opaEffect;
    QPropertyAnimation * opacityAnim;

    void initial();
    void Start();
    bool next();
    void setBegan(const leaves::data & data);
    void setEnd(const leaves::data & data);
    void setDurection(double s);

};

#endif // SIGNALANIMALABEL_H
