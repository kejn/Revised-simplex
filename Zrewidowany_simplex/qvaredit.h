#ifndef QVAREDIT_H
#define QVAREDIT_H

#include <QLineEdit>
#include <QDoubleValidator>

const int MAX_WIDTH = 55;

class QVarEdit : public QLineEdit
{
    QDoubleValidator *editValidator;
public:
    QVarEdit(QWidget *parent) : QLineEdit(parent)
    {
        editValidator = new QDoubleValidator(-99999.99,99999.99,2,this);
        editValidator->setNotation(QDoubleValidator::StandardNotation);
        setValidator(editValidator);
        setMaximumWidth(MAX_WIDTH);
    }
    double text2double(){
        QString t(text());
        t.replace(QChar(','),QChar('.'));
        if(t.isEmpty())
            return 0.0;
        return t.toDouble();
    }
};

#endif // QVAREDIT_H
