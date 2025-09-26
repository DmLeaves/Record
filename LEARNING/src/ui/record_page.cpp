#include "record_page.h"

#include "../core/import_service.h"
#include "../core/record_repository.h"
#include "import_dialog.h"
#include "notifications.h"
#include "record_editor_dialog.h"
#include "record_list_model.h"

#include <QAbstractItemView>
#include <algorithm>
#include <QDialog>
#include <QClipboard>
#include <QModelIndex>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMessageBox>
#include <QChar>
#include <QPushButton>
#include <QShortcut>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVector>

RecordPage::RecordPage(RecordRepository *repository, QWidget *parent)
    : QWidget(parent),
      m_repository(repository),
      m_model(new RecordListModel(this)),
      m_listView(nullptr),
      m_searchEdit(nullptr),
      m_titleLabel(nullptr),
      m_categoryLabel(nullptr),
      m_usernameLabel(nullptr),
      m_passwordLabel(nullptr),
      m_notesEdit(nullptr),
      m_notification(new NotificationLabel(this))
{
    buildUi();
    connect(m_repository, &RecordRepository::recordsChanged, this, &RecordPage::refresh);
    refresh();
}

void RecordPage::refresh()
{
    const QString keyword = m_searchEdit->text().trimmed();
    QVector<Record> records = keyword.isEmpty() ? m_repository->fetchAll() : m_repository->search(keyword);
    m_model->setRecords(records);
    if (m_model->rowCount() > 0)
    {
        m_listView->setCurrentIndex(m_model->index(0, 0));
    }
    else
    {
        displayRecord({});
    }
}

void RecordPage::onSearchTextChanged(const QString &text)
{
    if (text.trimmed().isEmpty())
    {
        refresh();
    }
    else
    {
        m_model->setRecords(m_repository->search(text.trimmed()));
        if (m_model->rowCount() > 0)
        {
            m_listView->setCurrentIndex(m_model->index(0, 0));
        }
        else
        {
            displayRecord({});
        }
    }
}

void RecordPage::onSelectionChanged(const QModelIndex &current, const QModelIndex &)
{
    const Record &record = m_model->recordAt(current.row());
    displayRecord(record);
}

void RecordPage::onAddRecord()
{
    RecordEditorDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    Record record = dialog.record();
    if (!record.isValid())
    {
        showNotification(tr("标题不能为空"));
        return;
    }

    record = m_repository->save(record);
    refresh();
    showNotification(tr("已添加 %1").arg(record.title));
}

void RecordPage::onEditRecord()
{
    const QModelIndex current = m_listView->currentIndex();
    if (!current.isValid())
    {
        return;
    }

    const Record &existing = m_model->recordAt(current.row());
    RecordEditorDialog dialog(this);
    dialog.setRecord(existing);
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    Record record = dialog.record();
    record.id = existing.id;
    if (!record.isValid())
    {
        showNotification(tr("标题不能为空"));
        return;
    }

    m_repository->save(record);
    refresh();
    showNotification(tr("已更新 %1").arg(record.title));
}

void RecordPage::onDeleteRecord()
{
    const QModelIndex current = m_listView->currentIndex();
    if (!current.isValid())
    {
        return;
    }

    const Record &record = m_model->recordAt(current.row());
    if (QMessageBox::question(this, tr("删除"), tr("确定要删除 %1 吗？").arg(record.title)) == QMessageBox::Yes)
    {
        m_repository->remove(record.id);
        refresh();
        showNotification(tr("已删除 %1").arg(record.title));
    }
}

void RecordPage::onImportRecords()
{
    ImportService importService(this);
    ImportDialog dialog(&importService, this);
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const QVector<Record> records = dialog.importedRecords();
    int successCount = 0;
    for (Record record : records)
    {
        if (!record.isValid())
        {
            continue;
        }
        m_repository->save(record);
        ++successCount;
    }

    if (successCount > 0)
    {
        refresh();
        showNotification(tr("成功导入 %1 条记录").arg(successCount));
    }
}

void RecordPage::copyUsername()
{
    const Record &record = m_model->recordAt(m_listView->currentIndex().row());
    if (record.username.isEmpty())
    {
        return;
    }
    QGuiApplication::clipboard()->setText(record.username);
    showNotification(tr("账号已复制"));
}

void RecordPage::copyPassword()
{
    const Record &record = m_model->recordAt(m_listView->currentIndex().row());
    if (record.password.isEmpty())
    {
        return;
    }
    QGuiApplication::clipboard()->setText(record.password);
    showNotification(tr("密码已复制"));
}

void RecordPage::buildUi()
{
    auto *layout = new QHBoxLayout(this);
    layout->setSpacing(24);

    auto *listPanel = new QWidget(this);
    auto *listLayout = new QVBoxLayout(listPanel);
    listLayout->setSpacing(12);

    m_searchEdit = new QLineEdit(listPanel);
    m_searchEdit->setPlaceholderText(tr("搜索"));
    connect(m_searchEdit, &QLineEdit::textChanged, this, &RecordPage::onSearchTextChanged);
    listLayout->addWidget(m_searchEdit);

    m_listView = new QListView(listPanel);
    m_listView->setModel(m_model);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged, this, &RecordPage::onSelectionChanged);
    listLayout->addWidget(m_listView);

    auto *listButtons = new QHBoxLayout;
    auto *addButton = new QPushButton(tr("新增"), listPanel);
    connect(addButton, &QPushButton::clicked, this, &RecordPage::onAddRecord);
    auto *editButton = new QPushButton(tr("编辑"), listPanel);
    connect(editButton, &QPushButton::clicked, this, &RecordPage::onEditRecord);
    auto *deleteButton = new QPushButton(tr("删除"), listPanel);
    connect(deleteButton, &QPushButton::clicked, this, &RecordPage::onDeleteRecord);
    listButtons->addWidget(addButton);
    listButtons->addWidget(editButton);
    listButtons->addWidget(deleteButton);
    listLayout->addLayout(listButtons);

    auto *importButton = new QPushButton(tr("导入"), listPanel);
    connect(importButton, &QPushButton::clicked, this, &RecordPage::onImportRecords);
    listLayout->addWidget(importButton);

    layout->addWidget(listPanel, 1);

    auto *detailPanel = new QWidget(this);
    auto *detailLayout = new QVBoxLayout(detailPanel);
    detailLayout->setSpacing(8);

    m_titleLabel = new QLabel(detailPanel);
    m_titleLabel->setObjectName(QStringLiteral("recordTitle"));
    detailLayout->addWidget(m_titleLabel);

    m_categoryLabel = new QLabel(detailPanel);
    detailLayout->addWidget(m_categoryLabel);

    m_usernameLabel = new QLabel(detailPanel);
    detailLayout->addWidget(m_usernameLabel);

    m_passwordLabel = new QLabel(detailPanel);
    detailLayout->addWidget(m_passwordLabel);

    auto *copyButtons = new QHBoxLayout;
    auto *copyUserButton = new QPushButton(tr("复制账号"), detailPanel);
    connect(copyUserButton, &QPushButton::clicked, this, &RecordPage::copyUsername);
    auto *copyPasswordButton = new QPushButton(tr("复制密码"), detailPanel);
    connect(copyPasswordButton, &QPushButton::clicked, this, &RecordPage::copyPassword);
    copyButtons->addWidget(copyUserButton);
    copyButtons->addWidget(copyPasswordButton);
    detailLayout->addLayout(copyButtons);

    m_notesEdit = new QTextEdit(detailPanel);
    m_notesEdit->setReadOnly(true);
    detailLayout->addWidget(m_notesEdit, 1);

    auto *lockButton = new QPushButton(tr("锁定"), detailPanel);
    connect(lockButton, &QPushButton::clicked, this, &RecordPage::requestLock);
    detailLayout->addWidget(lockButton);

    detailPanel->setLayout(detailLayout);
    layout->addWidget(detailPanel, 2);

    m_notification->setParent(this);
    m_notification->setStyleSheet(QStringLiteral("background-color: rgba(33, 150, 243, 0.85); color: white; padding: 8px 16px; border-radius: 6px;"));
    m_notification->setFixedHeight(36);
    m_notification->raise();

    auto *copyShortcut = new QShortcut(QKeySequence::Copy, this);
    connect(copyShortcut, &QShortcut::activated, this, &RecordPage::copyPassword);
}

void RecordPage::displayRecord(const Record &record)
{
    if (record.id < 0 && record.title.isEmpty())
    {
        m_titleLabel->setText(tr("请选择一条记录"));
        m_categoryLabel->setText(QString());
        m_usernameLabel->setText(QString());
        m_passwordLabel->setText(QString());
        m_notesEdit->clear();
        return;
    }

    m_titleLabel->setText(record.title);
    m_categoryLabel->setText(record.category.isEmpty() ? tr("无分类") : record.category);
    m_usernameLabel->setText(tr("账号：%1").arg(record.username));
    m_passwordLabel->setText(tr("密码：%1").arg(record.password.isEmpty() ? tr("未设置") : QString(record.password.length(), QChar(0x2022))));
    m_notesEdit->setPlainText(record.notes);
}

void RecordPage::showNotification(const QString &message)
{
    m_notification->showMessage(message);
    const int x = (width() - m_notification->sizeHint().width()) / 2;
    m_notification->move(std::max(16, x), 16);
}
