#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFont>
#include <QFontDatabase>
#include <QGraphicsOpacityEffect>
#include <QIcon>
#include <QLineEdit>
#include <QMenu>
#include <QPalette>
#include <QPropertyAnimation>
#include <QScreen>
#include <QSqlQuery>
#include <QTimer>
#include <QWheelEvent>
#include <QtDebug>
#include <QEasingCurve>

#include "messagewindows.h"

namespace {
constexpr int kTypingSpeed = 150;
constexpr int kMessageDuration = 1000;

QIcon buildWindowIcon()
{
    QIcon icon;
    icon.addFile(":/icon16.png", QSize(16, 16));
    icon.addFile(":/icon24.png", QSize(24, 24));
    icon.addFile(":/icon32.png", QSize(32, 32));
    icon.addFile(":/icon48.png", QSize(48, 48));
    icon.addFile(":/icon.png", QSize(64, 64));
    return icon;
}

} // namespace

MainWindow::MainWindow(QSqlDatabase * sql, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(sql)
    , label(nullptr)
    , passwordInput(nullptr)
    , winIcon(nullptr)
    , myTimer(nullptr)
    , introAnimationGroup(nullptr)
    , windowAnimationGroup(nullptr)
    , m_mousePressed(false)
    , m_dragEnabled(false)
    , fixed(false)
    , dataReady(false)
    , altL(nullptr)
    , CtrlF(nullptr)
    , roll(nullptr)
    , menu(nullptr)
    , input(nullptr)
    , manmageWidget(nullptr)
{
    ui->setupUi(this);

    createCoreObjects();
    setupShortcuts();
    setupRollWidgets();
    configureSystemTray();
    initializeUi();
    setupIntroAnimations();
    setupWindowAnimation();
    databaseCheck();
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow";
    db->close();
    delete ui;
}

void MainWindow::createCoreObjects()
{
    label = new AnimatedLabel(tr("请输入主密码 "), this);
    passwordInput = new QLineEdit(this);
    winIcon = new QSystemTrayIcon(this);
    menu = new QMenu(this);
    myTimer = new QTimer(this);
    introAnimationGroup = new QParallelAnimationGroup(this);
    windowAnimationGroup = new QParallelAnimationGroup(this);

    roll = new RollWidget(db, this, this);
    input = new newInput(db, roll, roll);
    manmageWidget = new ManageWidget(db, roll, roll);
}

void MainWindow::setupShortcuts()
{
    altL = new QShortcut(QKeySequence(QStringLiteral("Alt+L")), this);
    connect(altL, &QShortcut::activated, this, &MainWindow::lockOper);

    CtrlF = new QShortcut(QKeySequence(QStringLiteral("Ctrl+F")), this);
    connect(CtrlF, &QShortcut::activated, this, &MainWindow::findShow);
}

void MainWindow::setupRollWidgets()
{
    if (roll) {
        roll->close();
    }
}

void MainWindow::configureSystemTray()
{
    winIcon->setIcon(QIcon(":/icon.png"));
    winIcon->show();
    setWindowIcon(buildWindowIcon());

    setupTrayActions();
    connectTrayActions();
    winIcon->setContextMenu(menu);
}

void MainWindow::setupTrayActions()
{
    actions.clear();
    actions.reserve(7);
    menu->clear();

    actions.append(new QAction(QIcon(":/icons/1.png"), tr("添加数据"), this));
    actions.append(new QAction(QIcon(":/icons/2.png"), tr("批量管理"), this));
    actions.append(new QAction(this));
    actions.append(new QAction(QIcon(":/icons/3.png"), tr("点击穿透"), this));
    actions.append(new QAction(QIcon(":/icons/4.png"), tr("隐藏/显现窗口"), this));
    actions.append(new QAction(this));
    actions.append(new QAction(QIcon(":/icons/5.png"), tr("退出程序"), this));

    actions[2]->setSeparator(true);
    actions[5]->setSeparator(true);

    for (QAction *action : actions) {
        menu->addAction(action);
    }

    updateTrayAvailability();
}

void MainWindow::connectTrayActions()
{
    connect(actions[0], &QAction::triggered, input, &newInput::show);
    connect(actions[1], &QAction::triggered, manmageWidget, &ManageWidget::show);
    connect(actions[3], &QAction::triggered, this, &MainWindow::lockOper);
    connect(actions[4], &QAction::triggered, this, &MainWindow::transHideShow);
    connect(actions[6], &QAction::triggered, this, &MainWindow::requestQuit);
}

void MainWindow::initializeUi()
{
    configureWindowAppearance();
    configureAnimatedLabel();
    configurePasswordInput();
    setupCursorBlinking();
    updateTrayAvailability();
}

void MainWindow::configureWindowAppearance()
{
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    setWindowOpacity(0.5);

    const QSize windowSize(800, 600);
    setFixedSize(windowSize);

    const QSize screenSize = qApp->screens().at(0)->size();
    const int x = (screenSize.width() - windowSize.width()) / 2;
    const int y = (screenSize.height() - windowSize.height()) / 2;
    setGeometry(x, y, windowSize.width(), windowSize.height());

    setWindowFlags(Qt::FramelessWindowHint);
}

void MainWindow::configureAnimatedLabel()
{
    const int fontId = QFontDatabase::addApplicationFont(":/font.ttf");
    if (fontId == -1) {
        qWarning() << "Failed to load font";
    }

    const QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).value(0, font().family());
    label->setFont(QFont(fontFamily, 32));
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(QStringLiteral("color: white;"));
    label->setTextFormat(Qt::RichText);

    QRect windowRect = rect();
    QRect labelRect = label->rect();
    position(&windowRect, &labelRect, Type::Label);
    label->setGeometry(labelRect);
}

void MainWindow::configurePasswordInput()
{
    connect(passwordInput, &QLineEdit::returnPressed, this, &MainWindow::onReturnPressed);

    QRect windowRect = rect();
    QRect inputRect = passwordInput->rect();
    position(&windowRect, &inputRect, Type::Edit);
    passwordInput->setGeometry(inputRect);
}

void MainWindow::setupCursorBlinking()
{
    connect(myTimer, &QTimer::timeout, this, [this]() {
        static bool isShow = true;
        const QString from = isShow ? QStringLiteral(" ") : QStringLiteral("|");
        const QString to = isShow ? QStringLiteral("|") : QStringLiteral(" ");
        label->setText(label->text().replace(from, to));
        isShow = !isShow;
        label->update();
    });

    myTimer->start(800);
}

void MainWindow::setupIntroAnimations()
{
    auto *fontAnim = new QPropertyAnimation(label, QByteArrayLiteral("fontSize"));
    fontAnim->setDuration(1000);
    fontAnim->setStartValue(32);
    fontAnim->setEndValue(15);
    fontAnim->setEasingCurve(QEasingCurve::OutQuad);

    auto *sizeAnim = new QPropertyAnimation(label, QByteArrayLiteral("geometry"));
    sizeAnim->setDuration(2000);
    sizeAnim->setStartValue(label->geometry());
    sizeAnim->setEndValue(QRect(0, 0, 400, 50));
    sizeAnim->setEasingCurve(QEasingCurve::InOutQuad);

    auto *opacityEffect = new QGraphicsOpacityEffect(passwordInput);
    passwordInput->setGraphicsEffect(opacityEffect);

    auto *editAnimation = new QPropertyAnimation(opacityEffect, QByteArrayLiteral("opacity"));
    editAnimation->setDuration(1000);
    editAnimation->setStartValue(1.0);
    editAnimation->setEndValue(0.0);

    introAnimationGroup->addAnimation(fontAnim);
    introAnimationGroup->addAnimation(sizeAnim);
    introAnimationGroup->addAnimation(editAnimation);
}

void MainWindow::setupWindowAnimation()
{
    auto *windowsAnimation = new QPropertyAnimation(this, QByteArrayLiteral("geometry"));
    const QRect startRect = geometry();
    const QRect endRect = startRect.adjusted(width() / 4, height() / 4, -width() / 4, -height() / 4);
    windowsAnimation->setStartValue(startRect);
    windowsAnimation->setEndValue(endRect);
    windowsAnimation->setDuration(1000);
    windowsAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(windowsAnimation, &QAbstractAnimation::stateChanged, this,
            [this](QAbstractAnimation::State newState, QAbstractAnimation::State) {
                if (newState == QAbstractAnimation::Running) {
                    setMinimumSize(0, 0);
                    setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
                } else if (newState == QAbstractAnimation::Stopped) {
                    setFixedSize(400, 300);
                }
            });

    windowAnimationGroup->addAnimation(windowsAnimation);
}

void MainWindow::updateTrayAvailability()
{
    const bool enabled = m_dragEnabled;
    if (actions.size() >= 4) {
        actions[0]->setEnabled(enabled);
        actions[1]->setEnabled(enabled);
        actions[3]->setEnabled(enabled);
    }
}

void MainWindow::showTypedMessage(const QString &text, int speed, bool append)
{
    if (append) {
        appendWorld(text, label, speed);
    } else {
        label->setText(QStringLiteral("|"));
        setWorld(text, label, speed);
    }
}

void MainWindow::stopCursorBlinking()
{
    if (myTimer->isActive()) {
        myTimer->stop();
    }
    label->setText(label->text().replace(QStringLiteral("|"), QStringLiteral(" ")));
}

void MainWindow::runLater(int delayMs, const std::function<void()> &task)
{
    QTimer::singleShot(delayMs, this, [task]() {
        if (task) {
            task();
        }
    });
}

void MainWindow::setWindowInteractivity(bool clickThroughEnabled)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, clickThroughEnabled);

    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
    if (clickThroughEnabled) {
        flags |= Qt::Tool;
    }
    setWindowFlags(flags);
    show();
}

void MainWindow::toggleRollVisibility(bool visible)
{
    if (!roll) {
        return;
    }

    if (visible) {
        roll->showOnly();
    } else {
        roll->hide();
    }
}

void MainWindow::SuccessOperation()
{
    QSqlQuery query;
    query.exec(QStringLiteral("SELECT * FROM user"));

    QString username;
    if (query.next()) {
        username = query.value(QStringLiteral("username")).toString();
    } else {
        qWarning() << "No records found";
    }

    runLater(1500, [this, username]() {
        introAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped);
        passwordInput->setEnabled(false);
        stopCursorBlinking();

        label->setText(label->text().append(QStringLiteral(" ")));
        runLater(2000, [this, username]() {
            appendWorld(username, label, kTypingSpeed);
            windowAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped);
            m_dragEnabled = true;
            runLater(2000, [this]() {
                roll->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
                roll->show();
                setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
                installEventFilter(this);
                show();
                updateTrayAvailability();
            });
        });
    });
}

void MainWindow::databaseCheck()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT name FROM sqlite_master WHERE type='table' AND name='user'"));
    if (query.exec() && query.next()) {
        dataReady = true;
        const QString tableName = query.value(0).toString();
        qDebug() << "Table '" << tableName << "' exists in database.";
    } else {
        showTypedMessage(tr("请输入您的用户名"), kTypingSpeed, false);
    }
}

void MainWindow::setWorld(const QString & str,QLabel * tar,int speed)
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        const int index = tar->text().length() - 1;
        QString label_text = tar->text();
        if (index >= str.length()) {
            timer->stop();
            timer->deleteLater();
            return;
        }
        label_text[index] = str[index];
        label_text.append('|');
        tar->setText(label_text);
    });
    timer->start(speed);
}

void MainWindow::appendWorld(const QString & str,QLabel * tar,int speed)
{
    QTimer* timer = new QTimer(this);
    const int startIndex = tar->text().length();
    int index = 0;
    connect(timer, &QTimer::timeout, [=]() mutable {
        QString label_text = tar->text();
        if (index >= str.length()) {
            timer->stop();
            timer->deleteLater();
            QString labelText = label->text();
            labelText.chop(1);
            label->setText(labelText);
            return;
        }
        label_text[startIndex + index - 1] = str[index];
        label_text.append('|');
        tar->setText(label_text);
        ++index;
    });
    timer->start(speed);
}

void MainWindow::position(QRect * mainWindow,QRect * tar,Type type)
{
    if(type == Type::Label){
        tar->setWidth(mainWindow->width());
        tar->setTop(mainWindow->height()*0.2);
        tar->setBottom(mainWindow->height()*0.7);
    }else if(type == Type::Edit){
        tar->setLeft(mainWindow->width()*0.02);
        tar->setRight(mainWindow->width()*0.98);
        tar->setTop(mainWindow->height() * 0.94);
        tar->setBottom(mainWindow->height()*0.98);
    }
}

void MainWindow::onReturnPressed()
{
    if (passwordInput->text().isEmpty()) {
        return;
    }

    if(!dataReady){
        static QString username = "";
        static QString password = "";
        if(username.isEmpty()){
            username = passwordInput->text();
            showTypedMessage(tr("请输入您的密码"), kTypingSpeed, false);
        }else{
            password = passwordInput->text();
            QSqlQuery query;
            query.exec(QStringLiteral("CREATE DATABASE IF NOT EXISTS main"));
            query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS user ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT,"
                       "password TEXT"
                       ")"));
            query.prepare(QStringLiteral("INSERT INTO \"main\".\"user\"(\"id\", \"username\", \"password\") VALUES (1, :username, :password);"));
            query.bindValue(":username", username);
            query.bindValue(":password", password);

            QSqlQuery query2;
            query2.exec(QStringLiteral("CREATE TABLE form ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "user TEXT,"
                       "password TEXT,"
                       "position TEXT,"
                       "plus TEXT"
                       ")"));

            QString world = tr("初始化完成");
            if(!query.exec()){
                world = tr("初始化失败，请重试");
                username.clear();
                password.clear();
            }else{
                dataReady = true;
            }
            showTypedMessage(world, kTypingSpeed, false);
            runLater(2000, [this]() {
                showTypedMessage(QStringLiteral("Welcome"), kTypingSpeed, false);
                SuccessOperation();
            });
        }
        passwordInput->clear();
        return;
    }

    QSqlQuery query;
    query.exec(QStringLiteral("SELECT * FROM main.user"));
    QString password = query.next() ? query.value(QStringLiteral("password")).toString() : QString();
    if(password == passwordInput->text()){
        showTypedMessage(QStringLiteral("Welcome"), kTypingSpeed, false);
        SuccessOperation();
    }else{
        showTypedMessage(tr("密码错误"), kTypingSpeed, false);
    }
    passwordInput->clear();
}

void MainWindow::lockOper()
{
    if(!m_dragEnabled) {
        return;
    }

    fixed = !fixed;
    setWindowInteractivity(fixed);

    const QString message = fixed ? tr("点击穿透已启用") : tr("点击穿透已关闭");
    auto *ms = new MessageWindows(message, kMessageDuration);
    ms->messageShow();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(m_dragEnabled){
        QApplication::sendEvent(roll,event);
    }
}

void MainWindow::requestQuit()
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    QTimer::singleShot(0, qApp, &QCoreApplication::quit);
}

void MainWindow::transHideShow()
{
    const bool currentlyHidden = isHidden();
    setVisible(currentlyHidden);
    if(m_dragEnabled){
        toggleRollVisibility(currentlyHidden);
    }
}

void MainWindow::findShow()
{
    if(m_dragEnabled){
        roll->showFind();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(fixed){
        return;
    }
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = true;
        m_lastMousePos = event->globalPos();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(fixed){
        return;
    }
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(fixed){
        return;
    }
    if (m_mousePressed && m_dragEnabled) {
        QPoint delta = event->globalPos() - m_lastMousePos;
        move(pos() + delta);
        roll->move(roll->pos() + delta);
        m_lastMousePos = event->globalPos();
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    event->ignore();
    transHideShow();
}
