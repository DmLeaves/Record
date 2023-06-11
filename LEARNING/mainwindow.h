#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include <QLabel>
#include <QParallelAnimationGroup>
#include "animatedlabel.h"
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QLineEdit>
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QEasingCurve>
#include <QAbstractAnimation>
#include <QSize>
#include <QScreen>
#include <Windows.h>
#include <WinUser.h>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include "rollwidget.h"
#include <QShortcut>
#include <QKeySequence>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QEvent>
#include "newinput.h"
#include "managewidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class Type{
        Label, Edit
    };

public:
    MainWindow(QSqlDatabase * sql = nullptr ,QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent * event) override{
        event->ignore();
        transHideShow();
    }
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override
    {
        if(fixed)
            return;
        if (event->button() == Qt::LeftButton) {
            m_mousePressed = true;
            m_lastMousePos = event->globalPos();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {

        if(fixed)
            return;
        if (event->button() == Qt::LeftButton) {
            m_mousePressed = false;
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if(fixed)
            return;
        if (m_mousePressed && m_dragEnabled) {
            QPoint delta = event->globalPos() - m_lastMousePos;
            move(pos() + delta);
            roll->move(roll->pos() + delta);
            m_lastMousePos = event->globalPos();
        }
    }

//    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;


private:
    Ui::MainWindow *ui;
    QSqlDatabase * db;
    AnimatedLabel * label;
    QSystemTrayIcon * winIcon;
    QTimer *myTimer;
    QParallelAnimationGroup *group ;
    QParallelAnimationGroup *group2;
    bool m_mousePressed;
    bool m_dragEnabled;
    bool fixed;
    bool dataReady;
    QShortcut * altL;
    QShortcut * CtrlF;
    QPoint m_lastMousePos;
    RollWidget * roll;
    QMenu * menu;
    QVector<QAction *> actions;
    newInput * input;
    ManageWidget * manmageWidget;



    void SuccessOperation(QLineEdit *lineEdit);
    void databaseCheck();
    void initial();
    void setWorld(const QString & str,QLabel * tar,int speed);
    void appendWorld(const QString & str,QLabel * tar,int speed);
    void appear();
    void disappear();
    void menuInitial();
    void menuStatusChange();

public slots:

    void lockOper();
private slots:
    void position(QRect * mainWindow,QRect * tar,Type type);
    void onReturnPressed();
    void close();
    void transHideShow();
    void findShow();



};
#endif // MAINWINDOW_H
