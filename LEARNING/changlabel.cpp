#include "changlabel.h"

ChangLabel::ChangLabel(QWidget * parent)
    :QLabel(parent)
{
    goe = new QGraphicsOpacityEffect();
    this->setGraphicsEffect(goe);
    opAnim = new QPropertyAnimation(goe,"opacity");
    opAnim->setDuration(200);
    opAnim->setEasingCurve(QEasingCurve::OutCirc);
}



void ChangLabel::Change(QString & str){
    opAnim->setStartValue(1);
    opAnim->setEndValue(0);
    opAnim->start();
    QTimer::singleShot(200,this,[=](){
        setText(str);
        opAnim->setStartValue(0);
        opAnim->setEndValue(1);
        opAnim->start();
    });
}

ChangLabel::~ChangLabel(){
    qDebug() << "ChangLabel";
    delete goe;
    delete opAnim;
}
