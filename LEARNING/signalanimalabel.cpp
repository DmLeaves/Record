#include "signalanimalabel.h"
#include "data.h"

SignalAnimaLabel::SignalAnimaLabel()
    :currentIndex(0)
    ,speed(700)
{
    rectAnim = new QPropertyAnimation(this,"geometry",this);
    fontSizeAnim = new QPropertyAnimation(this,"fontSize",this);
    opaEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(opaEffect);
    opacityAnim = new QPropertyAnimation(opaEffect,"opacity",this);
    initial();
}

void SignalAnimaLabel::setDerection(Derection der){
    derection = der;
}
void SignalAnimaLabel::setCurrentIndex(int index){
    currentIndex = index;
}
void SignalAnimaLabel::setSpeed(double speed){
    this->speed = speed;
}
void SignalAnimaLabel::addKeyframes(int index,const QRect & rect,int fontSize,double opacity){
    nodeList.append(leaves::data(index,rect,fontSize,opacity));
}
void SignalAnimaLabel::StartNext(double sSpeed){
    double durection;
    if(sSpeed == -1){
        durection  = speed;
    }else{
        durection = sSpeed;
    }
    setBegan(nodeList[currentIndex]);
    if(next()){
        setEnd(nodeList[currentIndex]);
        setDurection(durection);
        Start();
    }
}
void SignalAnimaLabel::StartToIndex(int index,double sSpeed){
    double durection;
    if(sSpeed == -1){
        durection  = speed;
    }else{
        durection = sSpeed;
    }
    setBegan(nodeList[currentIndex]);
    setEnd(nodeList[index]);
    setDurection(durection);
    Start();
    currentIndex = index;
}

void SignalAnimaLabel::initial(){
    rectAnim->setEasingCurve(QEasingCurve::InSine);
    fontSizeAnim->setEasingCurve(QEasingCurve::InSine);
    opacityAnim->setEasingCurve(QEasingCurve::InCirc);
}

void SignalAnimaLabel::Start(){
    rectAnim->start();
    fontSizeAnim->start();
    opacityAnim->start();
}

bool SignalAnimaLabel::next(){
    if(derection == Derection::FORWAD){
        currentIndex = (currentIndex + 1 + LABELSIZE) % LABELSIZE;
        return currentIndex != 0;
    }else{
        currentIndex = (currentIndex - 1 + LABELSIZE) % LABELSIZE;
        return currentIndex != LABELSIZE - 1;
    }
}
void SignalAnimaLabel::setBegan(const leaves::data & data){
    rectAnim->setStartValue(data.rect);
    fontSizeAnim->setStartValue(data.fontSize);
    opacityAnim->setStartValue(data.opacity);
}
void SignalAnimaLabel::setEnd(const leaves::data & data){
    rectAnim->setEndValue(data.rect);
    fontSizeAnim->setEndValue(data.fontSize);
    opacityAnim->setEndValue(data.opacity);
}
void SignalAnimaLabel::setDurection(double s){
    rectAnim->setDuration(s);
    fontSizeAnim->setDuration(s);
    opacityAnim->setDuration(s/2);
}
void SignalAnimaLabel::clearKeyFrams(){
    nodeList.clear();
}
