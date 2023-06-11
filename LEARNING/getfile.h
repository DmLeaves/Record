#ifndef GETFILE_H
#define GETFILE_H

#include <QDialog>
#include <QPushButton>
#include <QFileDialog>
#include "dataoper.h"
#include <QSqlDatabase>
#include <QLineEdit>
#include <QPlainTextEdit>
#include "userdataparser.h"
#include "messagewindows.h"
#include <QVBoxLayout>


class GetFile : public QWidget
{
    Q_OBJECT
public:
    //初始化变量
    GetFile(QSqlDatabase * db,QWidget * parent = nullptr);
    //通过用户在QFileDialog输入的数据获得文件路径
    //检测文件是否存在，若文件不存在 Message中设置“文件不存在”
    //获取文件路径，调用UserDataParser转换文件中的对象，若GetResult返回false则Message中设置“文件格式错误”
    //GetResult返回true，则完成转换，通过dataoper对象将QVector<userdata>中的成员添加到数据库中，并将Meaasge设置为"文件读取完成"
    //不论函数怎么结束，都需要调用函数showmessage
    //完成转换后，调用showdata函数
    void batchImport();

private:
    const int interval = 10;
    //确认按钮，点击后启动getfile函数
    QPushButton confirm;
    //取消按钮，点击后关闭当前窗口
    QPushButton cancel;
    //获取路径按钮，点击后通过QFileDialog获取用户选择的路径并将之保存在QLineEdit中
    QPushButton getFilePath;
    //文件路径 保存从filedialog中获得的文件路径
    QLineEdit filePath;
    //文件路径获取dialog，用于获得文件路径
    QFileDialog fileDialog;
    //用于转换文件路径中的文件中的内容
    UserDataParser parser;
    //用于存放和展示读取出来的string，用户不可以修改内容
    QPlainTextEdit userlist;
    //用于操作
    dataOper Oper;
    //存放信息
    QString Message;
    QVector<userdata>  list;
    //初始化窗口属性和变量属性，设置其为模态的
    void initial();
    //将成功读取的userdata内容放在QPlainTextEdit中，每一个userdata结构之间间隔一个空行
    void showData();
    //不需要实现，留着
    void showMessage();
    bool hasChineseCharacters(const QString& path);
signals:
    void ImportSuccess();

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

#endif // GETFILE_H
