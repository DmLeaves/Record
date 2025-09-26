#include "../core/database_manager.h"
#include "../ui/main_window.h"

#include <QApplication>
#include <QMessageBox>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Record"));
    QApplication::setOrganizationName(QStringLiteral("Record"));

    DatabaseManager database;
    if (!database.open(QString()))
    {
        QMessageBox::critical(nullptr, QObject::tr("错误"), QObject::tr("无法打开数据库"));
        return 1;
    }

    MainWindow window(&database);
    window.show();

    return app.exec();
}
