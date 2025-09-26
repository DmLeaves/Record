#include "notifications.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

NotificationLabel::NotificationLabel(QWidget *parent)
    : QLabel(parent),
      m_effect(new QGraphicsOpacityEffect(this)),
      m_animation(new QPropertyAnimation(m_effect, "opacity", this))
{
    setObjectName(QStringLiteral("notificationLabel"));
    setAlignment(Qt::AlignCenter);
    setVisible(false);
    m_timer.setSingleShot(true);
    setGraphicsEffect(m_effect);
    m_animation->setDuration(400);
    m_animation->setStartValue(1.0);
    m_animation->setEndValue(0.0);
    connect(m_animation, &QPropertyAnimation::finished, this, [this]() {
        setVisible(false);
    });

    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_animation->stop();
        m_animation->setStartValue(1.0);
        m_animation->setEndValue(0.0);
        m_animation->start();
    });
}

void NotificationLabel::showMessage(const QString &message, int timeoutMs)
{
    m_timer.stop();
    setText(message);
    setVisible(true);
    raise();
    m_effect->setOpacity(1.0);
    m_timer.start(timeoutMs);
}
