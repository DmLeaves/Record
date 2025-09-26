#include "main_window.h"

#include "../core/database_manager.h"
#include "../core/master_password_service.h"
#include "../core/record_repository.h"
#include "record_page.h"
#include "tray_manager.h"
#include "unlock_widget.h"

#include <QCloseEvent>
#include <QIcon>
#include <QStackedWidget>

MainWindow::MainWindow(DatabaseManager *database, QWidget *parent)
    : QMainWindow(parent),
      m_passwordService(new MasterPasswordService(database->database(), this)),
      m_repository(new RecordRepository(database->database(), this)),
      m_unlockWidget(nullptr),
      m_recordPage(nullptr),
      m_tray(new TrayManager(this, this))
{
    buildUi();
    connect(m_tray, &TrayManager::requestShow, this, &MainWindow::showWindow);
    connect(m_tray, &TrayManager::requestLock, this, &MainWindow::lock);
    connect(m_tray, &TrayManager::requestUnlock, this, &MainWindow::unlockFromTray);
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    hide();
}

void MainWindow::onUnlocked()
{
    auto *stack = qobject_cast<QStackedWidget *>(centralWidget());
    if (!stack)
    {
        return;
    }

    if (!m_recordPage)
    {
        m_recordPage = new RecordPage(m_repository, this);
        connect(m_recordPage, &RecordPage::requestLock, this, &MainWindow::lock);
        stack->addWidget(m_recordPage);
    }

    m_recordPage->refresh();
    stack->setCurrentWidget(m_recordPage);
    m_tray->setLocked(false);
    if (m_unlockWidget)
    {
        m_unlockWidget->resetForm();
    }
    showWindow();
}

void MainWindow::lock()
{
    auto *stack = qobject_cast<QStackedWidget *>(centralWidget());
    if (!stack)
    {
        return;
    }

    if (m_unlockWidget)
    {
        m_unlockWidget->resetForm();
    }
    stack->setCurrentWidget(m_unlockWidget);
    m_tray->setLocked(true);
    if (!isVisible())
    {
        showWindow();
    }
}

void MainWindow::showWindow()
{
    showNormal();
    raise();
    activateWindow();
}

void MainWindow::unlockFromTray()
{
    auto *stack = qobject_cast<QStackedWidget *>(centralWidget());
    if (stack && m_unlockWidget)
    {
        if (stack->currentWidget() != m_unlockWidget)
        {
            stack->setCurrentWidget(m_unlockWidget);
        }
        m_unlockWidget->resetForm();
    }

    if (m_tray)
    {
        m_tray->setLocked(true);
    }

    showWindow();
}

void MainWindow::buildUi()
{
    auto *stack = new QStackedWidget(this);
    m_unlockWidget = new UnlockWidget(m_passwordService, this);
    connect(m_unlockWidget, &UnlockWidget::unlocked, this, &MainWindow::onUnlocked);
    stack->addWidget(m_unlockWidget);

    setCentralWidget(stack);
    resize(900, 560);
    setWindowTitle(tr("Record"));
    setWindowIcon(QIcon(QStringLiteral(":/icon.png")));

    if (m_tray)
    {
        m_tray->setLocked(m_passwordService->hasPassword());
    }

    if (!m_passwordService->hasPassword())
    {
        onUnlocked();
    }
}
