#ifndef ROOTWIDGET_H
#define ROOTWIDGET_H

#include <QWidget>
#include "stepsdialog.h"
#include "examplesdialog.h"

#include <QSpinBox>
#include <QDoubleValidator>
#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>

#include "qvarlabel.h"
#include "qvaredit.h"


class RootWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RootWidget(QWidget *parent = 0);
    void setEdit_c(int index, QString value);
    void setComboMinMaxFunc(int maxMin);
    void setEditA(int row, int col, QString value);
    void setComboInequal(int row, int ltGt);
    QComboBox *getComboInequal(int row) const;
    void setEdit_b(int row, QString value);
    void setN(int value);
    void setM(int value);
    void setCurrentEx(int value);

    QPushButton &getButtonRSM();
    QComboBox *getComboMinMaxFunc();

    ~RootWidget();

private slots:
    void onButtonExamplesClicked();
    void onButtonRSMClicked();
    void onSDialExit();
    void onExDialExit();
    void onSpinNValueChanged(int n);
    void onSpinMValueChanged(int m);

private:
    StepsDialog    *sDial;
    ExamplesDialog *exDial;

    const int   MIN_MN; // N - liczba zmiennych
    const int   MAX_MN; // M - liczba ograniczen

    // interfejs
    QVBoxLayout      *layoutRoot;

    void        initN();
    QHBoxLayout *layoutN;
    QLabel      *labelN;
    QSpinBox    *spinN;
    QPushButton *buttonLoadEx;
    int n() const
    { return spinN->text().toInt(); }

    void        initFunc();
    QHBoxLayout *layoutFunc;
    QComboBox   *comboMinMaxFunc;
    QVarLabel  **labelX;
    QVarEdit   **edit_c;

    void        initM();
    QHBoxLayout *layoutM;
    QLabel      *labelM;
    QSpinBox    *spinM;
    int m() const
    { return spinM->text().toInt(); }

    void        initA_b();
    QHBoxLayout *layoutOuterA_b;
    QGridLayout *layoutA_b;
    QLabel     **labelARowsNo;
    QVarEdit  ***editA;
    QVarLabel ***labelXA;
    QComboBox  **comboInequal;
    QVarEdit  **edit_b;

    void        initButton();
    QHBoxLayout *layoutButton;
    QPushButton *buttonRSM;

    int currentEx;
};

#endif // ROOTWIDGET_H
