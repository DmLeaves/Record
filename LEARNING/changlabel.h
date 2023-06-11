#ifndef CHANGLABEL_H
#define CHANGLABEL_H

#include<QLabel>
#include<QGraphicsEffect>
#include<QPropertyAnimation>
#include<QTimer>
#include<QDebug>

class ChangLabel : public QLabel
{
public:
    ChangLabel(QWidget * parent = nullptr);
    ~ChangLabel();
    void Change(QString & str);

private:
    QGraphicsOpacityEffect * goe;
    QPropertyAnimation * opAnim;

};

#endif // CHANGLABEL_H
