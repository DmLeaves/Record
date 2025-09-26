#pragma once

#include <QLabel>
#include <QTimer>

class QGraphicsOpacityEffect;
class QPropertyAnimation;

class NotificationLabel : public QLabel
{
    Q_OBJECT
public:
    explicit NotificationLabel(QWidget *parent = nullptr);

    void showMessage(const QString &message, int timeoutMs = 2000);

private:
    QTimer m_timer;
    QGraphicsOpacityEffect *m_effect;
    QPropertyAnimation *m_animation;
};
