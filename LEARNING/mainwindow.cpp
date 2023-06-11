#include "mainwindow.h"

MainWindow::MainWindow(QSqlDatabase * sql, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(sql)
    , m_mousePressed(false)
    , m_dragEnabled(false)
    , fixed(false)
    , dataReady(false)
{
    ui->setupUi(this);
    label = new AnimatedLabel("请输入主密码 ",this);
    myTimer = new QTimer(this);
    group = new QParallelAnimationGroup(this);
    group2 = new QParallelAnimationGroup(this);
    altL = new QShortcut(QKeySequence("Alt+L"),this);
    connect(altL,&QShortcut::activated,this,&MainWindow::lockOper);
    CtrlF = new QShortcut(QKeySequence("Ctrl+F"),this);
    connect(CtrlF,&QShortcut::activated,this,&MainWindow::findShow);
    winIcon = new QSystemTrayIcon(this);
    menu = new QMenu(this);
    roll = new RollWidget(db,this,this);
    input = new newInput(db,roll,roll);
    manmageWidget = new ManageWidget(db,roll,roll);

    initial();
    databaseCheck();


}

void MainWindow::initial(){
    // 设置窗口的背景色为黑色
    roll->close();
    QRect Window_rect = this->rect();
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    // 设置窗口的透明度为半透明
    setWindowOpacity(0.5);

    // 设置窗口的大小
    QSize windowSize = QSize(800, 600);
    setFixedSize(800, 600);
    QSize screenSize = qApp->screens().at(0)->size();
    int x = (screenSize.width() - windowSize.width()) / 2;
    int y = (screenSize.height() - windowSize.height()) / 2;
    this->setGeometry(x, y, windowSize.width(), windowSize.height());

    // 隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    // anime动画效果
    QPropertyAnimation * windowsAnimation = new QPropertyAnimation(this, "geometry");

    QRect startRect = geometry();
    QRect endRect = startRect.adjusted(width() / 4, height() / 4, -width() / 4, -height() / 4);
    windowsAnimation->setStartValue(startRect);
    windowsAnimation->setEndValue(endRect);
    windowsAnimation->setDuration(1000);
    windowsAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(windowsAnimation, &QAbstractAnimation::stateChanged, this, [=](QAbstractAnimation::State newState, QAbstractAnimation::State) {
        if (newState == QAbstractAnimation::Running) {
            // 在动画开始时取消窗口的固定大小
            setMinimumSize(0,0);
            setMaximumSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
        } else if (newState == QAbstractAnimation::Stopped) {
            // 在动画完成时将窗口大小设置为固定值
            setFixedSize(400, 300);
        }
    });
    group2->addAnimation(windowsAnimation);

    // 创建后台图标 和菜单

    menuInitial();

    // 创建一个QLabel控件，显示“主密码”文字
    int fontId = QFontDatabase::addApplicationFont(":/font.ttf");
    if (fontId == -1) {
            qDebug() << "Failed to load font";
    }
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);

    label->setFont(QFont(fontFamily, 32));
    label->setAlignment(Qt::AlignCenter);  // 设置标签文本居中对齐
    label->setStyleSheet("color: white;"); // 设置标签文本颜色为白色
    label->setTextFormat(Qt::RichText);
    QRect label_rect = label->rect();

    position(&Window_rect,&label_rect,Type::Label);
    label->setGeometry(label_rect);



    // 动画效果

    QObject::connect(myTimer, &QTimer::timeout, [=]() {
        static bool isShow = true;
        if (isShow) {
            label->setText(label->text().replace(" ","|"));
        } else {
            label->setText(label->text().replace("|"," "));
        }
        isShow = !isShow;
        label->update();
    });
    myTimer->start(800);

    // anime动画效果

    QPropertyAnimation *fontAnim = new QPropertyAnimation(label, "fontSize");
    fontAnim->setDuration(1000);
    fontAnim->setStartValue(32);
    fontAnim->setEndValue(15);
    fontAnim->setEasingCurve(QEasingCurve::OutQuad);


    QPropertyAnimation *sizeAnim = new QPropertyAnimation(label, "geometry");
    sizeAnim->setDuration(2000);
    sizeAnim->setStartValue(label->geometry());
    sizeAnim->setEndValue(QRect(0, 0, 400, 50));
    sizeAnim->setEasingCurve(QEasingCurve::InOutQuad);


    group->addAnimation(fontAnim);
    group->addAnimation(sizeAnim);




    // 创建一个QLineEdit控件，并将其添加到布局中
    QLineEdit *lineEdit = new QLineEdit(this);
    connect(lineEdit, &QLineEdit::returnPressed, this, &MainWindow::onReturnPressed);
    QRect Edit_rect = lineEdit->rect();
    position(&Window_rect,&Edit_rect,Type::Edit);
    lineEdit->setGeometry(Edit_rect);



    // anime动画效果
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(lineEdit);
    lineEdit->setGraphicsEffect(opacityEffect);

    QPropertyAnimation * editAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    editAnimation->setDuration(1000);
    editAnimation->setStartValue(1.0);
    editAnimation->setEndValue(0.0);

    group->addAnimation(editAnimation);

}



MainWindow::~MainWindow()
{
    qDebug() << "MainWindow";
    db->close();
    delete ui;
}


void MainWindow::position(QRect * mainWindow,QRect * tar,Type type){
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

void MainWindow::SuccessOperation(QLineEdit *lineEdit){

    //为数据库留空间
    QSqlQuery query;
    query.exec("SELECT * FROM user");
    QString username = "";
    if (query.next()) {
        username = query.value("username").toString();
    } else {
        qWarning() << "No records found";
    }


    QTimer::singleShot(1500, this, [=](){
        //启动变形动画
        group->start(QAbstractAnimation::DeleteWhenStopped);
        lineEdit->setEnabled(false);
        //停止光标动画
        myTimer->stop();
        label->setText(label->text().replace("|"," "));
        label->setText(label->text().append(" "));
        QTimer::singleShot(2000, this, [=](){
            appendWorld(username,label,150);
            group2->start(QAbstractAnimation::DeleteWhenStopped);
            //设置flag 为true 进入变形后的窗口设置（允许窗口拖动、滚轮生效、加载数据）
            m_dragEnabled = true;
            QTimer::singleShot(2000, this, [=](){

                roll->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
                roll->show();
                setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
                this->installEventFilter(this);
                show();
                menuStatusChange();
            });
        });
    });
}

void MainWindow::onReturnPressed(){
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender()); // 获取信号的发送者
    if(lineEdit->text().isEmpty()) return;
    if(!dataReady){
        static QString username = "";
        static QString password = "";
        if(username == ""){
            username = lineEdit->text();
            QString world = "请输入您的密码";
            label->setText("|");
            setWorld(world,label,150);
        }else{
            password = lineEdit->text();
            QSqlQuery query;
            query.exec("CREATE DATABASE IF NOT EXISTS main");
            query.exec("CREATE TABLE IF NOT EXISTS user ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT,"
                       "password TEXT"
                       ")");
            query.prepare("INSERT INTO \"main\".\"user\"(\"id\", \"username\", \"password\") VALUES (1, '" + username + "', '" + password + "');");

            QSqlQuery query2;
            query2.exec("CREATE TABLE form ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "user TEXT,"
                       "password TEXT,"
                       "position TEXT,"
                       "plus TEXT"
                       ")");

            QString world = "初始化完成";
            if(!query.exec()){
                world = "初始化失败，请重试";
                username = "";
                password = "";
            }else{
                dataReady = true;
            }
            label->setText("|");
            setWorld(world,label,150);
            //注册后直接登录 设置数据库状态为ready
            QTimer::singleShot(2000, this, [=](){
                QString world = "Welcome";
                label->setText("|");
                setWorld(world,label,150);
                SuccessOperation(lineEdit);
            });
        }
        lineEdit->clear();
        return;
    }


    //为数据库留空间
    QSqlQuery query;
    query.exec("SELECT * FROM main.user");
    if (query.next()) {
        QString password = query.value("password").toString();
    } else {
        qWarning() << "No records found";
    }

    QString password = query.value("password").toString();

    QString text = lineEdit->text(); // 获取用户输入的文本信息
    if(text == password){
        //welcome动画部分
        QString world = "Welcome";
        label->setText("|");
        setWorld(world,label,150);

        // 输入正确的处理
        SuccessOperation(lineEdit);
    }else{
        QString world = "密码错误";
        label->setText("|");
        setWorld(world,label,150);
    }
    lineEdit->clear();
}

// 从只有" "的字符串开始添加
void MainWindow::setWorld(const QString & str,QLabel * tar,int speed){
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        int index = tar->text().length() - 1;
        QString label_text = tar->text();
        if (index >= str.length()) {
            timer->stop();
            return;
        }
        label_text[index] = str[index];
        label_text.append('|');
        tar->setText(label_text);
    });
    timer->start(speed);
}

// 为已经存在字符串的添加
void MainWindow::appendWorld(const QString & str,QLabel * tar,int speed){
    QTimer* timer = new QTimer(this);
    //计算全部的字符长度 通过 全部-当前确定剩余多少个
    int startIndex = tar->text().length();
    int index = 0;
    connect(timer, &QTimer::timeout, [=]() mutable {
        //label存储的是当前的内容
        QString label_text = tar->text();
        if (index >= str.length()) {
            timer->stop();
            //处理末尾的字符
            QString labelText = label->text();
            labelText.chop(1);
            label->setText(labelText);
            return;
        }
        label_text[startIndex + index - 1] = str[index];
        label_text.append('|');
        tar->setText(label_text);
        index ++;
    });
    timer->start(speed);
}


void MainWindow::databaseCheck(){
    QSqlQuery query;
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='user'");
    if (query.exec() && query.next()) {
        //数据库准备就绪
        dataReady = true;
        QString tableName = query.value(0).toString();
        qDebug() << "Table '" << tableName << "' exists in database.";
    } else {
        QString world = "请输入您的用户名";
        label->setText("|");
        setWorld(world,label,150);
    }
}


void MainWindow::lockOper(){
    if(!m_dragEnabled) return;

    if(fixed){
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        fixed = false;
        MessageWindows * ms = new MessageWindows("点击穿透已关闭",1000);
        ms->messageShow();
    }else{

        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
        fixed = true;
        MessageWindows * ms = new MessageWindows("点击穿透已启用",1000);
        ms->messageShow();
    }
    show();
}

void MainWindow::wheelEvent(QWheelEvent *event){
    if(m_dragEnabled){
        QApplication::sendEvent(roll,event);
    }
}

void MainWindow::menuInitial(){
    // 图标设置
    QIcon icon (":/icon.png");
    winIcon->setIcon(icon);
    winIcon->show();
    QIcon wIcon;
    wIcon.addFile(":/icon16.png",QSize(16,16));
    wIcon.addFile(":/icon24.png",QSize(24,24));
    wIcon.addFile(":/icon32.png",QSize(32,32));
    wIcon.addFile(":/icon48.png",QSize(48,48));
    wIcon.addFile(":/icon.png",QSize(64,64));
    setWindowIcon(wIcon);


    // 窗口菜单设置
    actions.append(new QAction(QIcon(":/icons/1.png"),"添加数据",this));
    actions.append(new QAction(QIcon(":/icons/2.png"),"批量管理",this));
    actions.append(new QAction());
    actions.append(new QAction(QIcon(":/icons/3.png"),"点击穿透",this));
    actions.append(new QAction(QIcon(":/icons/4.png"),"隐藏/显现窗口",this));
    actions.append(new QAction());
    actions.append(new QAction(QIcon(":/icons/5.png"),"退出程序",this));
    actions[0]->setEnabled(false);
    actions[1]->setEnabled(false);
    actions[3]->setEnabled(false);
    actions[2]->setSeparator(true);
    actions[5]->setSeparator(true);

    connect(actions[0],&QAction::triggered,input,&newInput::show);
    connect(actions[1],&QAction::triggered,manmageWidget,&ManageWidget::show);
    connect(actions[3],&QAction::triggered,this,&MainWindow::lockOper);
    connect(actions[4],&QAction::triggered,this,&MainWindow::transHideShow);
    connect(actions[6],&QAction::triggered,this,&MainWindow::close);


    auto it = actions.begin();
    while(it != actions.end()){
        menu->addAction((*it));
        ++it;
    }

    winIcon->setContextMenu(menu);
}

void MainWindow::menuStatusChange(){
    if(m_dragEnabled){
        actions[0]->setEnabled(true);
        actions[1]->setEnabled(true);
        actions[3]->setEnabled(true);
    }else{
        actions[0]->setEnabled(false);
        actions[1]->setEnabled(false);
        actions[3]->setEnabled(false);
    }
}

void MainWindow::close(){
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    QTimer::singleShot(0, qApp, &QCoreApplication::quit);
}

void MainWindow::transHideShow(){
    if(this->isHidden()){
        show();
        if(m_dragEnabled)
            roll->showOnly();
    }else{
        hide();
        if(m_dragEnabled)
            roll->hide();
    }
}

void MainWindow::findShow(){
    if(m_dragEnabled)
        roll->showFind();
}
