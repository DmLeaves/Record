#include "managewidget.h"
#include "ui_managewidget.h"
#include "rollwidget.h"



ManageWidget::ManageWidget(QSqlDatabase *db,RollWidget * rw, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageWidget),
    oper(db),
    db(db),
    par(rw),
    m_mousePressed(false)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    file = new GetFile(db,this);
    initial();
    loadData();
}

ManageWidget::~ManageWidget()
{
    qDebug() << "ManageWindow";
    delete ui;
}

void ManageWidget::deleteSelected()
{
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) return;
    ui->undoDeleteButton->setEnabled(true);

    std::sort(selectedRows.begin(), selectedRows.end(), &ManageWidget::compareRowsDescending);

    for (const QModelIndex &index : qAsConst(selectedRows)) {
        int row = index.row();
        int userId = model->item(index.row(), 0)->text().toInt();
        if (oper.deleteUser(userId)) {
            userdata deletedUser;
            deletedUser.id = model->item(row, 0)->text().toInt();
            deletedUser.username = model->item(row, 1)->text();
            deletedUser.password = model->item(row, 2)->text();
            deletedUser.position = model->item(row, 3)->text();
            deletedUser.plus = model->item(row, 4)->text();
            deletedUsers.append(deletedUser);
            model->removeRow(index.row());
        }
    }
}

void ManageWidget::addNew()
{
    int newRow = 0;
    userdata newUser;
    newUser.username = model->item(newRow, 1)->text();
    newUser.password = model->item(newRow, 2)->text();
    newUser.position = model->item(newRow, 3)->text();
    newUser.plus = model->item(newRow, 4)->text();

    if (oper.addUser(newUser)) {
        loadData();
    }
}

void ManageWidget::loadData()
{
    QList<userdata> users = oper.queryUsers();
    model->setRowCount(users.size());
    model->setColumnCount(5);
    model->setHorizontalHeaderLabels({"ID", "账户", "密码", "应用站点", "备注"});

    int row = 0;
    for (const userdata &user : qAsConst(users)) {
        model->setItem(row, 0, new QStandardItem(QString::number(user.id)));
        model->setItem(row, 1, new QStandardItem(user.username));
        model->setItem(row, 2, new QStandardItem(user.password));
        model->setItem(row, 3, new QStandardItem(user.position));
        model->setItem(row, 4, new QStandardItem(user.plus));
        row++;
    }


    ui->tableView->setModel(model);
    ui->tableView->setColumnHidden(0, true);
}

void ManageWidget::initial()
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(82,78,77));
    setPalette(palette);

    // 设置窗口的透明度为半透明
    setWindowOpacity(0.9);
    setWindowFlags(windowFlags() |Qt::FramelessWindowHint | Qt::Window);

    ui->tableView->setStyleSheet("QTableView {"
                                 "color: white;  "                                     /*表格内文字颜色*/
                                 "gridline-color: black;    "                          /*表格内框颜色*/
                                 "background-color: rgb(117,135,138);"               /*表格内背景色*/
                                 "alternate-background-color: rgb(99, 99, 99);"
                                 "selection-color: white; "                            /*选中区域的文字颜色*/
                                 "selection-background-color: rgb(107,104,130);"        /*选中区域的背景色*/
                                 "border: 2px groove gray;"
                                 "border-radius: 0px;"
                                  "padding: 2px 4px;"
                                  "}"
                                  "QHeaderView::section {"
                                  "    background-color: #595455;"
                                  "    color: #ffffff;"
                                  "    padding: 4px;"
                                  "    border: 1px solid #7d7d7d;"
                                  "}"
                                  "QTableCornerButton::section {"
                                  "    background-color: #2d2d2d;"
                                  "    border: 1px solid #767676;"
                                  "}");




    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setSortingEnabled(true);

    connect(model, &QStandardItemModel::itemChanged, this, &ManageWidget::onItemChanged);
    connect(ui->DleleteAll,&QPushButton::clicked,this,&ManageWidget::deleteSelected);
    connect(ui->submit,&QPushButton::clicked,this,&ManageWidget::onSaveChangesClicked);
    connect(ui->close,&QPushButton::clicked,this,&QWidget::close);
    connect(ui->undoDeleteButton, &QPushButton::clicked, this, &ManageWidget::undoDelete);
    connect(ui->importByFile,&QPushButton::clicked,file,&GetFile::show);
    connect(this,&ManageWidget::WindowsClose,par,&RollWidget::freshData);
    connect(file,&GetFile::ImportSuccess,par,&RollWidget::freshData);
    connect(file,&GetFile::ImportSuccess,this,&ManageWidget::loadData);
    ui->undoDeleteButton->setEnabled(false);

    QSize windowsSize = size();
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - windowsSize.width()) / 2;
    int y = (screenGeometry.height() - windowsSize.height()) / 2;
    move(x, y);
}

void ManageWidget::onItemChanged(QStandardItem *item){
    modifiedRows.insert(item->row());
}
void ManageWidget::onSaveChangesClicked(){

    for (auto row = modifiedRows.begin();row != modifiedRows.end();row ++) {
        int userId = model->item(*row, 0)->text().toInt();
        userdata updatedUser;


        updatedUser.id = userId;
        updatedUser.username = model->item(*row, 1)->text();
        updatedUser.password = model->item(*row, 2)->text();
        updatedUser.position = model->item(*row, 3)->text();
        updatedUser.plus = model->item(*row, 4)->text();

        oper.updateUser(updatedUser);
    }

    modifiedRows.clear();
}

void ManageWidget::undoDelete(){
    if (deletedUsers.isEmpty()) return;

        // Restore the last deleted user
        userdata userToRestore = deletedUsers.takeLast();
        oper.addUser(userToRestore);

        // Add restored user to the model
        loadData();

        // Disable undo button when no deleted users available
        if (deletedUsers.isEmpty()) {
            ui->undoDeleteButton->setEnabled(false);
        }
}

void ManageWidget::show(){
    loadData();
    QWidget::show();
}

void ManageWidget::closeEvent(QCloseEvent *event){
    emit WindowsClose();
    QWidget::closeEvent(event);
}


