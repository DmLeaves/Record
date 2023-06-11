#ifndef NEWINPUT_H
#define NEWINPUT_H

#include<QWidget>
#include<QLineEdit>
#include<QLabel>
#include<QPushButton>
#include<QFontDatabase>
#include<QPaintEvent>
#include <QPainter>
#include<QFocusEvent>
#include "dataoper.h"
#include "userdata.h"
#include "messagewindows.h"
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>

class RollWidget;

class newInput :public QWidget
{
    Q_OBJECT

public:
    newInput(QSqlDatabase * db,RollWidget * rw,QWidget * parent =nullptr);
    ~newInput(){
         qDebug() << "RollWidget";
         delete [] edits;
         delete [] labels;
    }
    void insertNew();


protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    const int InputNumbers = 4;
    const QSize labelSize;
    const QSize editSize;
    const int interval = 10;


    dataOper oper;
    QLineEdit * edits;
    QLabel * labels;
    QPushButton submit;
    QPushButton cancel;
    RollWidget * par;
    QVBoxLayout *verticalLayout;
    void initial();
private slots:
    void submitUserData();
public slots:
    void show();

signals:
    void WindowsClose();

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_mousePressed = true;
            m_lastMousePos = event->globalPos();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            m_mousePressed = false;
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (m_mousePressed ) {
            QPoint delta = event->globalPos() - m_lastMousePos;
            move(pos() + delta);
            m_lastMousePos = event->globalPos();
        }
    }
private:
    bool m_mousePressed;
    QPoint m_lastMousePos;

};

#endif // NEWINPUT_H
