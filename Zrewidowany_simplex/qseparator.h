#ifndef QSEPARATOR_H
#define QSEPARATOR_H

#include <QFrame>

class QSeparator : public QFrame
{
public:
    QSeparator(QWidget *parent=0) : QFrame(parent)
    {
        setFrameShape(QFrame::HLine);
        setFrameShadow(QFrame::Sunken);
    }
    QSeparator(Shape shape, QWidget *parent=0) : QFrame(parent)
    {
        setFrameShape(shape);
        setFrameShadow(QFrame::Sunken);
    }
};

#endif // QSEPARATOR_H
