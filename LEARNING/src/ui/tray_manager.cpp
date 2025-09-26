#include "tray_manager.h"

#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QWidget>

TrayManager::TrayManager(QWidget *window, QObject *parent)
    : QObject(parent),
      m_window(window),
      m_trayIcon(new QSystemTrayIcon(window)),
      m_menu(new QMenu(window)),
      m_lockAction(nullptr),
      m_locked(true)
{
    m_trayIcon->setIcon(QIcon(QStringLiteral(":/icon.png")));
    buildMenu();
    m_trayIcon->setContextMenu(m_menu);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger)
        {
            emit requestShow();
        }
    });
    m_trayIcon->show();
}

TrayManager::~TrayManager()
{
    m_trayIcon->hide();
}

void TrayManager::setLocked(bool locked)
{
    m_locked = locked;
    if (m_lockAction)
    {
        m_lockAction->setText(locked ? tr("解锁") : tr("锁定"));
    }
}

void TrayManager::buildMenu()
{
    auto *showAction = new QAction(tr("显示"), this);
    connect(showAction, &QAction::triggered, this, &TrayManager::requestShow);

    m_lockAction = new QAction(tr("锁定"), this);
    connect(m_lockAction, &QAction::triggered, this, [this]() {
        if (m_locked)
        {
            emit requestUnlock();
        }
        else
        {
            emit requestLock();
        }
    });

    auto *quitAction = new QAction(tr("退出"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    m_menu->addAction(showAction);
    m_menu->addAction(m_lockAction);
    m_menu->addSeparator();
    m_menu->addAction(quitAction);

    setLocked(m_locked);
}
