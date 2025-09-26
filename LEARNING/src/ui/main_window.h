#pragma once

#include <QMainWindow>

class DatabaseManager;
class MasterPasswordService;
class RecordRepository;
class UnlockWidget;
class RecordPage;
class TrayManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(DatabaseManager *database, QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onUnlocked();
    void lock();
    void showWindow();
    void unlockFromTray();

private:
    void buildUi();

    MasterPasswordService *m_passwordService;
    RecordRepository *m_repository;
    UnlockWidget *m_unlockWidget;
    RecordPage *m_recordPage;
    TrayManager *m_tray;
};
