#ifndef ANIMATEDLABEL_H
#define ANIMATEDLABEL_H

#include <QLabel>


class AnimatedLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize)

public:
    AnimatedLabel(const QString & text = "" ,QWidget *parent = nullptr) : QLabel(text,parent) {}

    int fontSize() const {
        return font().pointSize();
    }

    void setFontSize(int size) {
        QFont f = font();
        f.setPointSize(size);
        setFont(f);
    }
};

#endif // ANIMATEDLABEL_H
