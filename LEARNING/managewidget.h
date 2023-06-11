#ifndef MANAGEWIDGET_H
#define MANAGEWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include "dataoper.h"
#include <QSqlDatabase>
#include <QItemSelectionModel>
#include <QCloseEvent>
#include "getfile.h"

namespace Ui {
class ManageWidget;
}
class RollWidget ;

class ManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManageWidget(QSqlDatabase * db,RollWidget * rw,QWidget *parent = nullptr);

    ~ManageWidget();

    //删除在tableview选中的所有行，并在数据库同步删除
    void deleteSelected();

    //将用户填写在原本的空行的数据生成为一个userdata对象，并更新到数据库，再在原有的表格的第一行插入一行空行
    void addNew();

protected:
    void closeEvent(QCloseEvent *event) override;

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


private:
    Ui::ManageWidget *ui;
    dataOper oper;
    QSqlDatabase * db;
    QStandardItemModel *model;
    QSet<int> modifiedRows;
    QList<userdata> deletedUsers;
    RollWidget * par;
    GetFile * file;


    //载入database数据



    //初始化，创建table view 的表头id username password position plus
    //其中id所在的列能被隐藏
    //用户能够按住shift或者ctrl进行多选
    //表头以外的第一行总是空行，用于用户添加新的用户
    //表的列数不为定值，根据数据库中的数量确定，用户能够通过滚轮向下翻阅数据，有侧边滑块
    void initial();
    static bool compareRowsDescending(const QModelIndex &a, const QModelIndex &b) {
        return a.row() > b.row();
    }

private slots:
    void onItemChanged(QStandardItem *item);
    void onSaveChangesClicked();
    void undoDelete();
    void loadData();

public slots:
    void show();

signals:
    void WindowsClose();

};

#endif // MANAGEWIDGET_H
