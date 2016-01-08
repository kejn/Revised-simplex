#ifndef STEPSDIALOG_H
#define STEPSDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QColor>
#include <QPen>
#include <QBrush>

#include "qvarlabel.h"
#include "qvaredit.h"
#include "matrix.h"

#include "qcustomplot.h"

class StepsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StepsDialog(QComboBox *comboMinMaxFunction,
                         QVarEdit **edit_c,
                         QVarEdit ***editA,
                         QComboBox **comboInequal,
                         QVarEdit **edit_b,
                         int nVars,
                         int nConstr,
                         QWidget *parent = 0);
    ~StepsDialog() {}

signals:
    void hidden();

private:
    void hide();

private slots:
    void startFromBeginning();
    int  simplexNext();
    void simplexGoToResult();

    void moveTable2Phase2();

private:
    QHBoxLayout *layoutRootWithGraphics;
    QVBoxLayout *layoutRoot;

    void    initLabelPhase();
    QString generateFunctionLabel();
    QLabel *labelPhase;

    void initLabelEquation();
    QGridLayout *layoutTransformedEx;
    QLabel    ***labelAI;
    QVarLabel ***labelXSA;
    QLabel     **label_b;

    void initTable();
    void resizeTableFaza2();
    QLabel      *labelTableTitle;
    QGridLayout *layoutTable;
    QLabel    ***labelTable;

    void initButtons();
    QHBoxLayout *buttonBox;
    QPushButton *buttonStart;
    QPushButton *buttonNext;
    QPushButton *buttonResult;

    void initGraphics();
    QCustomPlot *viewConstraints;
    QColor colorBlue;
    QColor colorFunction;
    int funcGraphIndex;


    void initEquation(QComboBox **comboInequal);
    bool checkIfPhase1(QComboBox **comboInequal);
    int iterateRevisedSimplex();
    void updateLabelsTable(int whatToDo=-1);
    void printResultInfo(int whatToDo);
    void dropAllY();
    Matrix getXInOrder();
    Matrix getYInOrder();
    bool checkIfFinished(unsigned i);
    void printResultX(unsigned i);
    double calcMaxX1Range();
    void drawCurrentResult();
    int whichConstraintParallel();

    unsigned nVars;
    unsigned nVars2;
    unsigned nConstr;
    unsigned nYVars;
    std::vector<unsigned> notYVarsIndexes;
    double x0;
    Matrix xB;
    Matrix xN;
    Matrix cB;
    Matrix cN;
    Matrix B;
    Matrix N;
    Matrix AI;
    Matrix b;
    Matrix c;
    Matrix p;
    Matrix y;
    unsigned comingIndex;
    unsigned leavingIndex;
    Matrix cPhase1;
    Matrix origCB;
    Matrix origCN;
    std::vector<int> labelsBN;
    unsigned L;
    unsigned maxL;
    bool faza1;

    bool minimizeFunction;
    bool koniec;
    unsigned liczbaRozwiazan;
    unsigned dontShow;
    std::vector<std::pair<double,Matrix>> rozwiazania;
    Matrix rozwiazaniaLabels;
};

#endif // STEPSWIDGET_H
