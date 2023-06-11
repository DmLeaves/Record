#include "messagewindows.h"

MessageWindows::MessageWindows(const QString & str,int ms,QWidget * parent)
    :QWidget(parent)
    ,durection(ms)
{
    QSize windowSize = QSize(600, 100);
    opacityAnim = new QPropertyAnimation(this,"windowOpacity");
    label = new QLabel(parent);

    opacityAnim->setStartValue(1);
    opacityAnim->setEndValue(0);
    opacityAnim->setDuration(ms);
    opacityAnim->setEasingCurve(QEasingCurve::InSine);
    setWindowOpacity(0.7);


    label->setText(str);
    label->setParent(this);
    label->setStyleSheet("color: rgb(248,251,248);");
    label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter );
    label->setFont(QFont("黑体", 20));

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(label);
    shadow->setBlurRadius(10); // 阴影模糊半径
    shadow->setColor(QColor(57,47,65)); // 阴影颜色
    shadow->setOffset(0, 0); // 阴影偏移量
    label->setGraphicsEffect(shadow); // 将阴影效果应用到label上

    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    // 设置窗口的透明度为半透明
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);


    // 设置label的大小

    QRect parentRect;
    if(parent == nullptr){
        QScreen * screen = QApplication::primaryScreen();
       parentRect = screen->geometry();
    }else{
       parentRect = parentWidget()->geometry();
    }

    int x = (parentRect.width() - windowSize.width()) / 2;
    int y = (parentRect.height() - windowSize.height()) / 2;
    label->setGeometry(0, 0, windowSize.width(), windowSize.height());
    this->setGeometry(x, y, windowSize.width(), windowSize.height());

}

void MessageWindows::messageShow(){
    show();
    opacityAnim->start(QAbstractAnimation::DeleteWhenStopped);
    QTimer::singleShot(durection,this,[=](){
        close();
        delete this;
    });
}
