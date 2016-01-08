#ifndef QVARLABEL_H
#define QVARLABEL_H

#include <QLabel>
#include <QString>

class QVarLabel : public QLabel
{
    QString varName;
    const int varIndex;
    QString extraText;

public:
    QVarLabel(const QString &varName, int varIndex, const QString &extraText="", QWidget *parent=0)
        : QLabel(parent),
          varName(varName),
          varIndex(varIndex),
          extraText(extraText)
    {
        setTextExtra();
    }

    QVarLabel(QVarLabel *otherVarLabel, QWidget *parent=0)
        : QLabel(parent),
          varIndex(otherVarLabel->getVarIndex())
    {
        varName = otherVarLabel->getVarName();
        extraText = otherVarLabel->getExtraText();
        setTextExtra();
    }

    void setTextExtra(bool atBegin=false)
    {
        if(atBegin)
            setText(extraText + varName + QString::number(varIndex));
        else
            setText(varName + QString::number(varIndex) + extraText);
    }

    void setExtra(const QString &extraText)
    {
        this->extraText=extraText;
    }

    void setTextNoExtra()
    {
        setText(varName + QString::number(varIndex));
    }

    void show(int index=-2, int limit=0)
    {
        if(index+1 < limit)
            setTextExtra();
        else
            setTextNoExtra();
        QLabel::show();
    }

    void hide()
    {
        clear();
        QLabel::hide();
    }

    QString getVarName() { return varName; }
    const int getVarIndex() { return varIndex; }
    QString getExtraText() { return extraText; }
};

#endif // QVARLABEL_H
