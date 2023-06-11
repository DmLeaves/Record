#include "rollwidget.h"
#include "mainwindow.h"

RollWidget::RollWidget(QSqlDatabase * Db,MainWindow * mw,QWidget * parent)
    :QWidget(parent)
    ,db(Db)
    ,rolls(0)
    ,rolling(false)

{
    if(!db->isOpen()){
        qDebug() << "Error in rollWidget database.";
    }
    dataBox = new CycleBox(db);
    labels = new SignalAnimaLabel[LabelSize];
    findDia = new FindDialog(dataBox,this,this);

    mytimer = new QTimer(this);
    connect(mytimer,&QTimer::timeout,this,&RollWidget::timerSolt);
    altC = new QShortcut(QKeySequence("Alt+C"),this);
    connect(altC,&QShortcut::activated,this,&RollWidget::CopyInfo);
    altL = new QShortcut(QKeySequence("Alt+L"),this);
    connect(altL,&QShortcut::activated,mw,&MainWindow::lockOper);
    CtrlF = new QShortcut(QKeySequence("Ctrl+F"),this);
    connect(CtrlF,&QShortcut::activated,findDia,&FindDialog::show);

    topLabel = 0;
    bottomLabel = LabelSize - 1;
    initial();
    animeInitial();

}

RollWidget::~RollWidget(){
    qDebug() << "RollWidget";
    delete dataBox;
    delete[] labels;

}

void RollWidget::initial(){
    // 窗口大小在父窗口进行调整  下面是调试代码需要删除
    setFixedSize(400, 300);
    // 窗口风格统一
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    this->setPalette(palette);

    // 隐藏标题栏 透明背景
    setWindowFlags(windowFlags() |Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    int fontId = QFontDatabase::addApplicationFont(":/font.ttf");
    if (fontId == -1) {
            qDebug() << "Failed to load font";
    }
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);

    for(int i = 0;i<LabelSize;i++){
        labels[i].setParent(this);
        labels[i].setFont(QFont(fontFamily, FontSize));
        labels[i].setStyleSheet("color: white; font-weight: bold;");
        labels[i].setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    }

    //初始化备注label
    plusLabel.setParent(this);
    plusLabel.setGeometry(0,0,200,50);
    plusLabel.setStyleSheet("color: rgba(215, 221, 249, 1);");
    plusLabel.setFont(QFont(fontFamily, FontSize * 0.8));
    plusLabel.setAlignment(Qt::AlignCenter | Qt::AlignVCenter);


    //移动到窗口右下角
    plusLabel.move(200, 250);
}

void RollWidget::animeInitial(){
    //这部分程序是“死的” 不能被拓展 暂时没想到如何拓展这段程序
    //分配各个关键帧节点
    QRect rect = this->geometry();
    GodenNum = rect.height() * 0.248;//粗略计算黄金比例
    int basePoint = rect.height() / 2 - rect.height()/8;
    //初始化文字内容

    dataBox->getFocalList(data);
    auto it = data.begin();
    auto end = data.end();
    int index = 0;

    //各个label的内容分发 考虑内容为空的情况
    while((it != end) && (index < LabelSize)){
        //初始化各个label 的文字属性
        labels[index].setText((*it).position);
        ++it;
        ++index;
    }


    it = data.begin();
    //初始化各个label 的动画属性
    for(int i = 0;(i<LabelSize) && (it != end) ;++i){
        labels[i].setGeometry(0,basePoint,rect.width(),rect.height()/5);
        labels[i].setSpeed(500);
        labels[i].setText((*it).position);
        ++it;

        int * nums = new int[LabelSize];
        nums[0] = - rect.height() / 10 ; nums[1] = basePoint - GodenNum;
        nums[2] = basePoint;
        nums[3] = basePoint + GodenNum ; nums[4] = rect.height() - rect.height() / 10;

        for(int r = 0;r < 5; ++r){
            if(r == 0 || r == 4){
                labels[i].addKeyframes(
                            r,
                            QRect(rect.left(),nums[r],rect.width(),rect.height()/5),
                            fontSizeFunction(rect.height()/2,rect.height()/2),
                            opacityFunction(r)
                            );
            }else{
                //算法不够 硬改来凑
                labels[i].addKeyframes(
                            r,
                            QRect(rect.left(),nums[r],rect.width(),rect.height()/5),
                            fontSizeFunction(abs(nums[r] - basePoint),rect.height()/2),
                            opacityFunction(r)
                            );
            }

        }
        labels[i].setCurrentIndex(2);
        delete[] nums;
    }
    plusLabel.Change(getCurrentData().plus);
}

void RollWidget::wheelEvent(QWheelEvent *event){

    // 向上滚动
    if (event->angleDelta().y() > 0) {
        rollsDecline();
        if(!rolling){
            rolling = true;
            mytimer->setInterval(500);
            mytimer->start();
        }
    }
    // 向下滚动
    else if (event->angleDelta().y() < 0) {
        rollsPlus();
        if(!rolling){
            rolling = true;
            mytimer->setInterval(500);
            mytimer->start();
        }
    }
}
void RollWidget::paintEvent(QPaintEvent *event){
     QPainter painter(this);
     QColor fillColor(117,135,138, 212);
     painter.setPen(QPen(fillColor, 2));

     // 设置画刷颜色
     painter.setBrush(fillColor);

     // 绘制矩形 美术修正再搞
     painter.drawRect(0, 122, 400, 50);
     QWidget::paintEvent(event);
}

void RollWidget::animeRolling(SignalAnimaLabel::Derection derection,double speed){
    for(int i = 0;i<LabelSize;i++){
        labels[i].setDerection(derection);
        labels[i].StartNext(speed);
    }


    if(derection == SignalAnimaLabel::Derection::FORWAD){
        //更新cycle模型获取新的数据 并更新data 最终更新toplabel和bottomlabel
        topLabel = (topLabel - 1 + LabelSize) % LabelSize;
        bottomLabel = (bottomLabel - 1 + LabelSize) %LabelSize;
        userdata tempdata;
        dataBox->moveFocal(CycleBox::Derection::ROLLBACK,&tempdata);
        data.pop_back();
        data.push_front(tempdata);
        labels[topLabel].setText(tempdata.position);
    }else{
        topLabel = (topLabel + 1 + LabelSize) % LabelSize;
        bottomLabel = (bottomLabel + 1 + LabelSize) %LabelSize;
        userdata tempdata;
        dataBox->moveFocal(CycleBox::Derection::FORWARD,&tempdata);
        data.pop_front();
        data.push_back(tempdata);
        labels[bottomLabel].setText(tempdata.position);

    }
    plusLabel.Change(getCurrentData().plus);
}

void RollWidget::timerSolt(){
    if(rolls == 0){
        rolling  = false;
        mytimer->stop();
        return;
    }else if(rolls > 0){
        animeRolling(SignalAnimaLabel::Derection::REVERSE,rolls_to_speed());
        mytimer->stop();
        mytimer->setInterval(rolls_to_speed()+50);
        rollsDecline();
        if(rolls > 5) rolls = 5;
    }else{
        animeRolling(SignalAnimaLabel::Derection::FORWAD,rolls_to_speed());
        mytimer->stop();
        mytimer->setInterval(rolls_to_speed()+50);
        rollsPlus();
        if(rolls < -5) rolls = -5;
    }

    mytimer->start();
}

userdata & RollWidget::getCurrentData(){
    auto it = data.begin();
    it += 2;
    return (*it);
}
void RollWidget::CopyInfo(){
    if(dataBox->isEmpty()) return;
    QClipboard *clipboard = QApplication::clipboard();
    userdata tempdata = getCurrentData();
    if(tempdata.username == tempdata.password){
        clipboard->setText(tempdata.username);
        MessageWindows * ms = new MessageWindows("已复制账户&密码(两者一致)",2000);
        ms->messageShow();
        return;
    }

    if(tempdata.username == clipboard->text()){
        clipboard->setText(tempdata.password);
        MessageWindows * ms = new MessageWindows("已复制密码",2000);
        ms->messageShow();
    }else{
        clipboard->setText(tempdata.username);
        MessageWindows * ms = new MessageWindows("已复制账号",2000);
        ms->messageShow();
    }
}

void RollWidget::show(){
    QWidget::show();
    for(int i = 0;i<LabelSize;++i){
        labels[i].StartToIndex(i,1500);
    }
}
void RollWidget::freshData(){
     for(int i = 0;i<LabelSize;++i){
         labels[i].clear();
     }
     topLabel = 0;
     bottomLabel = LabelSize - 1;
     dataBox->fresh();
     animeInitial();
     for(int i = 0;i<LabelSize;++i){
         labels[i].StartToIndex(i,1500);
     }
}
void RollWidget::showFind(){
    findDia->show();
}

/*----------------------------------友元类部分------------------------------------------------*/


FindDialog::FindDialog(CycleBox *dataBox, RollWidget *rollWidget, QWidget *parent)
    : QWidget(parent),
      box(dataBox),
      roll(rollWidget),
      finding(false)
{
    Bt = new QPushButton("Button", this);
    Cancel = new QPushButton("Button", this);
    edit = new QLineEdit(this);
    timer = new QTimer(this);
    initial();
}

void FindDialog::paintEvent(QPaintEvent *event) {
//    QPainter painter(this);
//    painter.drawRect(rect());

    QWidget::paintEvent(event);
}


bool FindDialog::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Wheel) {
        // 阻止鼠标滚轮事件
        return true;
    }
    return QWidget::eventFilter(obj, event);
}

void FindDialog::initial() {
    edit->setGeometry(0,0,150,25);
    Bt->setGeometry(0,0,60,25);
    Cancel->setGeometry(0,0,60,25);
    setLayout(&layout);
    layout.addWidget(edit,2);
    layout.addWidget(Bt,1);
    layout.addWidget(Cancel,1);

    int fontId = QFontDatabase::addApplicationFont(":/font.ttf");
    if (fontId == -1) {
            qDebug() << "Failed to load font";
    }
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    Bt->setFont(QFont(fontFamily,12));
    Bt->setText("下一条");
    Cancel->setFont(QFont(fontFamily,12));
    Cancel->setText("取消");
    this->setWindowTitle("查找");
    QSize windowsSize(300,50);
    this->setFixedSize(windowsSize.width(), windowsSize.height());
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - windowsSize.width()) / 2;
    int y = (screenGeometry.height() - windowsSize.height()) / 2;
    move(x, y);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(82,78,77));
    setPalette(palette);

    connect(timer,&QTimer::timeout,this,&FindDialog::timerSolt);
    connect(Bt,&QPushButton::clicked,this,&FindDialog::findStr);
    connect(edit,&QLineEdit::returnPressed,this,&FindDialog::findStr);
    connect(Cancel,&QPushButton::clicked,this,&QWidget::close);
}

void FindDialog::findStr(){
    if(finding) return;
    if(edit->text().isEmpty()){
        MessageWindows * ms = new MessageWindows("输入为空",1000);
        ms->messageShow();
        return;
    }
    str = edit->text();
    if(box->find(str)){
        roll->installEventFilter(this);
        roll->animeRolling(SignalAnimaLabel::Derection::REVERSE,50);
        timer->setInterval(70);
        timer->start();
        finding = true;
    }else{
        MessageWindows * ms = new MessageWindows("搜索结果为空",1000);
        ms->messageShow();
    }

}

void FindDialog::timerSolt(){
    if(!box->Compare(str))
        roll->animeRolling(SignalAnimaLabel::Derection::REVERSE,50);
    else{
        timer->stop();
        roll->removeEventFilter(this);
        finding = false;
    }

}

