#ifndef MESSAGEWINDOWS_H
#define MESSAGEWINDOWS_H

#include <QWidget>
#include <QScreen>
#include <QPropertyAnimation>
#include <QLabel>
#include <QtDebug>
#include <QScreen>
#include <QApplication>
#include <QTimer>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>



class MessageWindows :public QWidget
{
public:
    MessageWindows(const QString & str,int ms,QWidget * parent = nullptr);
    void messageShow();
private:

    QLabel * label;
    QPropertyAnimation * opacityAnim;
    int durection;
};

#endif // MESSAGEWINDOWS_H
