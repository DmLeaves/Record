#include "newinput.h"
#include "rollwidget.h"

newInput::newInput(QSqlDatabase * db,RollWidget * rw,QWidget * parent):
    QWidget(parent),
    par(rw),
    labelSize(150,50),
    editSize(300,30),
    oper(db),
    m_mousePressed(false)
{
    edits = new QLineEdit[InputNumbers];
    labels = new QLabel[InputNumbers];
    verticalLayout = new QVBoxLayout(this);
    initial();
}

void newInput::initial(){
    setWindowFlags( windowFlags() |Qt::Tool |Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground, true);

    // 初始化参数
    int fontId = QFontDatabase::addApplicationFont(":/font.ttf");
    if (fontId == -1) {
            qDebug() << "Failed to load font";
    }
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    QString title[] = {"用户名","密码","应用站点/软件","备注"};

    for(int i = 0;i<InputNumbers;i++){
        QHBoxLayout *horizontalLayout = new QHBoxLayout;

        labels[i].setGeometry(0,0,labelSize.width(),labelSize.height());
        edits[i].setGeometry(0,0,editSize.width(),editSize.height());

        labels[i].setText(title[i]);
        labels[i].setAlignment(Qt::AlignCenter | Qt::AlignVCenter );
        labels[i].setStyleSheet("color: white; font-weight: bold;");
        labels[i].setFont(QFont(fontFamily,13));
        if(i % 2 == 0){
            horizontalLayout->addWidget(labels + i);
            horizontalLayout->addWidget(edits + i);
        }else{
            horizontalLayout->addWidget(edits + i);
            horizontalLayout->addWidget(labels + i);
        }
        verticalLayout->addLayout(horizontalLayout);
    }

    submit.setParent(this);
    submit.setGeometry(0,0,(labelSize.width() + editSize.width())/4,40);
    submit.setText("保存并提交");
    submit.setFont(QFont(fontFamily,14));

    cancel.setParent(this);
    cancel.setGeometry(0,0,(labelSize.width() + editSize.width())/4,40);
    cancel.setText("关闭");
    cancel.setFont(QFont(fontFamily,14));
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(&submit);
    horizontalLayout->addWidget(&cancel);
    verticalLayout->addLayout(horizontalLayout);

    verticalLayout->setSpacing(20);

    QSize windowsSize(labelSize.width() + editSize.width() + 2*interval,(labelSize.height() + interval) * (InputNumbers + 1) + interval);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - windowsSize.width()) / 2;
    int y = (screenGeometry.height() - windowsSize.height()) / 2;
    move(x, y);

    connect(&submit,&QPushButton::clicked,this,&newInput::submitUserData);
    connect(&cancel,&QPushButton::clicked,this,&newInput::close);
    connect(this,&newInput::WindowsClose,par,&RollWidget::freshData);
}

void newInput::paintEvent(QPaintEvent *event){
     QPainter painter(this);
     painter.setPen(QPen(Qt::gray, 1));

     // 设置画刷颜色
     painter.setBrush(Qt::gray);

     QColor fillColor(82,78,77, 212);
     painter.setBrush(fillColor);

     // 绘制矩形 美术修正再搞
     painter.drawRect(0, 0,
                      labelSize.width() + editSize.width() + 2*interval ,
                      (labelSize.height() + interval) * (InputNumbers + 1) + interval);
     QWidget::paintEvent(event);
}


void newInput::submitUserData(){
    userdata user;
    user.username = edits[0].text();
    user.password = edits[1].text();
    user.position = edits[2].text();
    user.plus = edits[3].text();
    oper.addUser(user);
    for(int i = 0;i<InputNumbers;i++){
        edits[i].clear();
    }
    MessageWindows * ms = new MessageWindows("已添加数据",1500);
    ms->messageShow();
}

void newInput::show(){
    for(int i = 0;i<InputNumbers;i++){
        edits[i].clear();
    }
    QWidget::show();
}

void newInput::closeEvent(QCloseEvent *event){
    emit WindowsClose();
    QWidget::closeEvent(event);
}





