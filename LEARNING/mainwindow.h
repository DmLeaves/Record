#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractAnimation>
#include <QCloseEvent>
#include <QEvent>
#include <QLabel>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QParallelAnimationGroup>
#include <QPoint>
#include <QRect>
#include <QShortcut>
#include <QSize>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWheelEvent>
#include <QtSql/QSqlDatabase>
#include <QVector>
#include <functional>

#include "animatedlabel.h"
#include "managewidget.h"
#include "newinput.h"
#include "rollwidget.h"
#include "ui_mainwindow.h"

QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QSqlQuery)


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
    void closeEvent(QCloseEvent * event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

//    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;


private:
    Ui::MainWindow *ui;
    QSqlDatabase * db;
    AnimatedLabel * label;
    QLineEdit * passwordInput;
    QSystemTrayIcon * winIcon;
    QTimer *myTimer;
    QParallelAnimationGroup *introAnimationGroup;
    QParallelAnimationGroup *windowAnimationGroup;
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



    void SuccessOperation();
    void databaseCheck();
    void setWorld(const QString & str,QLabel * tar,int speed);
    void appendWorld(const QString & str,QLabel * tar,int speed);
    void createCoreObjects();
    void setupShortcuts();
    void initializeUi();
    void configureWindowAppearance();
    void configureAnimatedLabel();
    void configurePasswordInput();
    void setupCursorBlinking();
    void setupIntroAnimations();
    void setupWindowAnimation();
    void configureSystemTray();
    void setupTrayActions();
    void connectTrayActions();
    void updateTrayAvailability();
    void setupRollWidgets();
    void showTypedMessage(const QString &text, int speed = 150, bool append = false);
    void stopCursorBlinking();
    void runLater(int delayMs, const std::function<void()> &task);
    void setWindowInteractivity(bool clickThroughEnabled);
    void toggleRollVisibility(bool visible);

public slots:

    void lockOper();
private slots:
    void position(QRect * mainWindow,QRect * tar,Type type);
    void onReturnPressed();
    void requestQuit();
    void transHideShow();
    void findShow();



};
#endif // MAINWINDOW_H
