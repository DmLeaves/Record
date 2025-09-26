#pragma once

#include <QWidget>

class MasterPasswordService;
class QLineEdit;
class QPushButton;
class QLabel;

class UnlockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UnlockWidget(MasterPasswordService *service, QWidget *parent = nullptr);

signals:
    void unlocked();

public slots:
    void resetForm();

private slots:
    void submit();

private:
    void buildUi();
    void updateUi();

    MasterPasswordService *m_service;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_confirmEdit;
    QLabel *m_hintLabel;
    QPushButton *m_submitButton;
};
