#include <QApplication>
#include <QMainWindow>
#include <QPalette>
#include <QVBoxLayout>
#include <QWidget>
#include "ui/mainwindow.h"
#include "ui/rollwidget.h"
#include "ui/messagewindows.h"
#include "ui/managewidget.h"
#include "ui/newinput.h"
#include "ui/getfile.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //数据库初始化
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    db.setUserName("Program");
    db.setPassword("Ql123[]");
    db.open();

    MainWindow mainWindow(&db);
    mainWindow.show();

    //测试代码][

//    RollWidget roll(&db,&mainWindow);
//    roll.show();

//    ManageWidget win(&db);
//    win.show();
//    newInput input(&db);
//    input.show();


//    GetFile file(&db);
//    file.show();


    return app.exec();
}


