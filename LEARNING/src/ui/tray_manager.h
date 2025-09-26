#pragma once

#include <QObject>

class QAction;
class QMenu;
class QSystemTrayIcon;
class QWidget;

class TrayManager : public QObject
{
    Q_OBJECT
public:
    explicit TrayManager(QWidget *window, QObject *parent = nullptr);
    ~TrayManager();

    void setLocked(bool locked);

signals:
    void requestShow();
    void requestLock();
    void requestUnlock();

private:
    void buildMenu();

    QWidget *m_window;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_menu;
    QAction *m_lockAction;
    bool m_locked;
};
