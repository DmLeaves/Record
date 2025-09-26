#include "unlock_widget.h"

#include "../core/master_password_service.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

UnlockWidget::UnlockWidget(MasterPasswordService *service, QWidget *parent)
    : QWidget(parent),
      m_service(service),
      m_passwordEdit(nullptr),
      m_confirmEdit(nullptr),
      m_hintLabel(nullptr),
      m_submitButton(nullptr)
{
    buildUi();
    updateUi();
    connect(m_service, &MasterPasswordService::passwordCreated, this, &UnlockWidget::updateUi);
}

void UnlockWidget::submit()
{
    const QString password = m_passwordEdit->text();

    if (!m_service->hasPassword())
    {
        if (password.length() < 6)
        {
            m_hintLabel->setText(tr("主密码长度至少为 6"));
            m_hintLabel->setStyleSheet(QStringLiteral("color: #b3261e;"));
            return;
        }
        if (password != m_confirmEdit->text())
        {
            m_hintLabel->setText(tr("两次输入的密码不一致"));
            m_hintLabel->setStyleSheet(QStringLiteral("color: #b3261e;"));
            return;
        }
        if (m_service->setPassword(password))
        {
            m_hintLabel->setText(tr("主密码设置成功"));
            m_hintLabel->setStyleSheet(QStringLiteral("color: #1b5e20;"));
            emit unlocked();
        }
        else
        {
            m_hintLabel->setText(tr("保存主密码失败"));
            m_hintLabel->setStyleSheet(QStringLiteral("color: #b3261e;"));
        }
        return;
    }

    if (m_service->verifyPassword(password))
    {
        m_hintLabel->clear();
        emit unlocked();
    }
    else
    {
        m_hintLabel->setText(tr("密码错误"));
        m_hintLabel->setStyleSheet(QStringLiteral("color: #b3261e;"));
    }
}

void UnlockWidget::resetForm()
{
    updateUi();
}

void UnlockWidget::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(16);

    auto *title = new QLabel(tr("Record"), this);
    title->setObjectName(QStringLiteral("unlockTitle"));
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto *form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignRight);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    form->addRow(tr("主密码"), m_passwordEdit);

    m_confirmEdit = new QLineEdit(this);
    m_confirmEdit->setEchoMode(QLineEdit::Password);
    form->addRow(tr("确认密码"), m_confirmEdit);

    layout->addLayout(form);

    m_submitButton = new QPushButton(this);
    layout->addWidget(m_submitButton);

    m_hintLabel = new QLabel(this);
    m_hintLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_hintLabel);

    connect(m_submitButton, &QPushButton::clicked, this, &UnlockWidget::submit);
}

void UnlockWidget::updateUi()
{
    const bool hasPassword = m_service->hasPassword();
    m_confirmEdit->setVisible(!hasPassword);
    m_submitButton->setText(hasPassword ? tr("解锁") : tr("创建主密码"));
    m_hintLabel->setText(hasPassword ? tr("请输入主密码解锁") : tr("首次使用，请设置主密码"));
    m_hintLabel->setStyleSheet(QStringLiteral("color: #5f6368;"));
    m_passwordEdit->clear();
    m_confirmEdit->clear();
}
