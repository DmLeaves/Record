#pragma once

#include "../core/record.h"

#include <QWidget>

class RecordRepository;
class RecordListModel;
class QListView;
class QLineEdit;
class QTextEdit;
class QPushButton;
class NotificationLabel;
class QLabel;
class QModelIndex;

class RecordPage : public QWidget
{
    Q_OBJECT
public:
    RecordPage(RecordRepository *repository, QWidget *parent = nullptr);

signals:
    void requestLock();

public slots:
    void refresh();

private slots:
    void onSearchTextChanged(const QString &text);
    void onSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
    void onAddRecord();
    void onEditRecord();
    void onDeleteRecord();
    void onImportRecords();
    void copyUsername();
    void copyPassword();

private:
    void buildUi();
    void displayRecord(const Record &record);
    void showNotification(const QString &message);

    RecordRepository *m_repository;
    RecordListModel *m_model;
    QListView *m_listView;
    QLineEdit *m_searchEdit;
    QLabel *m_titleLabel;
    QLabel *m_categoryLabel;
    QLabel *m_usernameLabel;
    QLabel *m_passwordLabel;
    QTextEdit *m_notesEdit;
    NotificationLabel *m_notification;
};
