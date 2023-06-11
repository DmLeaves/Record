#include "getfile.h"

GetFile::GetFile(QSqlDatabase* db, QWidget* parent)
    : QWidget(parent), Oper(db),m_mousePressed(false)
{
    initial();
}

void GetFile::initial()
{
    //设置字体属性
    int fontId = QFontDatabase::addApplicationFont(":/font.ttf");
    if (fontId == -1) {
            qDebug() << "Failed to load font";
    }
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    confirm.setFont(QFont(fontFamily,12));
    cancel.setFont(QFont(fontFamily,12));
    getFilePath.setFont(QFont(fontFamily,12));

    //控件属性
    userlist.setReadOnly(true);

    // 设置窗口属性
    this->setWindowTitle("批量导入用户");
    this->setFixedSize(450, 400);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(82,78,77));
    setPalette(palette);

    palette.setColor(QPalette::Base, QColor(117,135,138));
    filePath.setPalette(palette);
    userlist.setPalette(palette);
    confirm.setFont(QFont(fontFamily,12));


    // 添加控件
    getFilePath.setText("获取文件路径");
    confirm.setText("确定");
    cancel.setText("取消");

    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->addWidget(&filePath);
    pathLayout->addWidget(&getFilePath);
    layout->addLayout(pathLayout);
    layout->addWidget(&userlist);
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(&confirm);
    btnLayout->addWidget(&cancel);
    layout->addLayout(btnLayout);


    // 连接信号和槽
    connect(&getFilePath, &QPushButton::clicked,this, [this]() {
        // 打开文件对话框，获取文件路径
        QString path = fileDialog.getOpenFileName(this, "选择文件", ".", "Text Files (*.txt)");
        if (!path.isEmpty()) {
            if(hasChineseCharacters(path)){
                MessageWindows * ms = new MessageWindows("路径不能含有中文",2000);
                ms->messageShow();
            }else{
                filePath.setText(path);
            }
        }
    });

    connect(&cancel, &QPushButton::clicked, this,[this]() {
        // 点击取消按钮，关闭窗口
        this->close();
    });

    connect(&confirm, &QPushButton::clicked, this,[this]() {
        // 点击确定按钮，启动批量导入功能
        batchImport();
    });
    this->close();
}

void GetFile::batchImport()
{
    // 清空上一次的提示信息
    Message.clear();
    list.clear();


    // 从QLineEdit中获取文件路径
    QString path = filePath.text();

    // 检查文件是否存在
    if (!QFile::exists(path)) {
        Message = "文件不存在";
        showMessage();
        return;
    }

    // 打开文件进行解析

    QFile file(path.toLocal8Bit());


    // 解析文件，并将其转换为userdata对象
    bool result = parser.GetResult(&file, list);
    if (!result) {
        Message = "文件格式错误";
        showMessage();
        return;
    }

    // 将userdata向量添加到数据库中
    for (const auto& user : qAsConst(list)) {
        if (!Oper.addUser(user)) {
            Message = "导入失败，请重试";
            showMessage();
            return;
        }
    }

    // 显示成功提示信息和userdata内容
    Message = "文件读取完成";
    showMessage();
    userlist.clear();
    showData();
    filePath.clear();

    // 发出导入成功的信号
    emit ImportSuccess();
}

void GetFile::showData()
{
    // 清空上一次的userdata内容
    userlist.clear();

    // 将userdata添加到QPlainTextEdit中
    int index = 0;
    for (const auto& user : qAsConst(list)) {
        userlist.insertPlainText("数据" + QString::number(index) + "\n");
        QString data = QString("用户名：%1\n密码：%2\n职位：%3\n备注：%4\n\n").arg(user.username, user.password, user.position, user.plus);
        userlist.insertPlainText(data);
        index++;
    }
}

void GetFile::showMessage()
{
    MessageWindows * msg = new MessageWindows(Message, 2000);
    msg->messageShow();
}

bool GetFile::hasChineseCharacters(const QString& path) {
    QRegExp chineseCharacters("[\\x4e00-\\x9fa5]+"); // 中文字符范围
    return chineseCharacters.indexIn(path) != -1;
}
