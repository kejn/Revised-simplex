#include "stepsdialog.h"
#include "rootwidget.h"
#include "qseparator.h"
#include <iostream>
#include <cmath>
#include <string>
#include <QPushButton>

StepsDialog::StepsDialog(QComboBox *comboMinMaxFunction, QVarEdit **edit_c,
                         QVarEdit ***editA,
                         QComboBox **comboInequal,
                         QVarEdit **edit_b,
                         int nVars,
                         int nConstr,
                         QWidget *parent) :
    QDialog(parent),
    nVars(nVars),
    nConstr(nConstr),
    nVars2(nVars+nConstr),
    nYVars(0),
    AI(nConstr,nVars,editA),
    b(nConstr,1,&edit_b,true),
    c(nVars,1,&edit_c,true)
{
    setParent(parent);
    setWindowTitle(tr("Etapy rozwiązania"));

    layoutRootWithGraphics = new QHBoxLayout();
    layoutRoot = new QVBoxLayout();

    initEquation(comboInequal);
    minimizeFunction=false;
    if(comboMinMaxFunction->currentIndex())
        minimizeFunction=true;

    initLabelPhase();
    initLabelEquation();
    initTable();
    initButtons();
    buttonStart->setDisabled(true);

    if(nVars==2)
        initGraphics();

    layoutRootWithGraphics->addItem(layoutRoot);
    setLayout(layoutRootWithGraphics);
}

void StepsDialog::hide() {
    QDialog::hide();
    emit hidden();
}

void StepsDialog::startFromBeginning()
{
    hide();
    RootWidget *p = dynamic_cast<RootWidget*>(parentWidget());
    p->getButtonRSM().click();
}


int StepsDialog::simplexNext()
{
    if(L < 1)
        buttonStart->setEnabled(true);

    int whatToDo = iterateRevisedSimplex();
    if(nVars==2){
        if(liczbaRozwiazan < 1)
            viewConstraints->removeItem(0);
        if(viewConstraints->graphCount()-1 == funcGraphIndex)
          viewConstraints->removeGraph(funcGraphIndex);
        drawCurrentResult();
    }
    ++L;

    labelPhase->setText(generateFunctionLabel()+tr("\n...Iteracja ")+QString::number(L));

    if(whatToDo >= 0){
        buttonNext->setDisabled(true);
        buttonResult->setDisabled(true);
        QString currTxt = labelPhase->text();
        labelPhase->setText(currTxt+tr("\nKoniec fazy ")+tr((faza1 ? "1" : "2")));
    }
    updateLabelsTable(whatToDo);
    printResultInfo(whatToDo);

    return whatToDo;
}

void StepsDialog::simplexGoToResult()
{
    while(maxL - L > 0){
        if(simplexNext() >= 0)
            break;
    }

    buttonResult->setDisabled(true);
    buttonStart->setEnabled(true);
}

void StepsDialog::initLabelPhase()
{
    labelPhase = new QLabel(generateFunctionLabel());
    layoutRoot->addWidget(labelPhase);
    layoutRoot->addSpacerItem(new QSpacerItem(1,1));
    layoutRoot->setStretch(1,10);
}

void StepsDialog::initLabelEquation()
{
    layoutTransformedEx = new QGridLayout();
    labelXSA = new QVarLabel**[nConstr];
    labelAI = new QLabel**[nConstr];
    unsigned lengthAI = nVars2;
    if(faza1)
        lengthAI+=nYVars;

    for(unsigned i=0; i < nConstr; ++i){
        labelAI[i] = new QLabel*[lengthAI];
        labelXSA[i] = new QVarLabel*[lengthAI];
        for(unsigned j=0; j < lengthAI; ++j){
            labelAI[i][j] = new QLabel(QString::number(AI[i][j],'f',2),this);
            if(j < nVars)
                labelXSA[i][j] = new QVarLabel(tr(" x"),j+1," + ",this);
            else if(j < nVars2){
                labelXSA[i][j] = new QVarLabel(tr(" s"),j-nVars+1," + ",this);

                if(j+1 == nVars2 && lengthAI==nVars2){
                    labelXSA[i][j]->setExtra(tr(" = "));
                    labelXSA[i][j]->setTextExtra();
                }
            } else {
                labelXSA[i][j] = new QVarLabel(tr(" y"),j-nVars2+1," + ",this);

                if(j+1 == lengthAI){
                    labelXSA[i][j]->setExtra(tr(" = "));
                    labelXSA[i][j]->setTextExtra();
                }
            }

            layoutTransformedEx->addWidget(labelAI[i][j],i,2*j);
            layoutTransformedEx->addWidget(labelXSA[i][j],i,2*j+1);
        }
    }

    label_b = new QLabel*[nConstr];
    for(unsigned i=0; i < nConstr; ++i){
        label_b[i] = new QLabel(QString::number(b[i][0],'f',2));
        layoutTransformedEx->addWidget(label_b[i],i,2*lengthAI+1);
    }

    layoutRoot->addItem(layoutTransformedEx);
    layoutRoot->addSpacerItem(new QSpacerItem(1,1));
    layoutRoot->setStretch(3,10);
}

void StepsDialog::initTable()
{
    unsigned tableCols = nVars+2;
    unsigned tableRows = nConstr+2;
    if(faza1)
        tableCols += nYVars;

    labelTableTitle = new QLabel(tr("Tablica simpleksowa:"),this);

    layoutTable = new QGridLayout();
    labelTable = new QLabel**[tableRows]; // etykiety, funkcja celu i xB
    for(unsigned i=0; i < tableRows; ++i){
        labelTable[i] = new QLabel*[tableCols]; // etykiety, xB, [xN y]
        for(unsigned j=0; j < tableCols; ++j){
            if(i==0){ // pierwszy wiersz: etykiety niebazowe
                if(j > 1){
                    if(j-2 < nVars)
                        labelTable[i][j] = new QLabel(tr("x")+QString::number(labelsBN[j+nConstr-2]),this);
                    else
                        labelTable[i][j] = new QLabel(tr("s")+QString::number(-labelsBN[j+nConstr-2]),this);
                } else
                    labelTable[i][j] = new QLabel(this);
            } else {
                if(j > 0){
                    if(j > 1)
                        if(i==1)
                            labelTable[i][j] = new QLabel(QString::number(p[j-2][0],'f',2),this);
                        else
                            labelTable[i][j] = new QLabel(QString::number(AI[i-2][j-2],'f',2),this);
                    else if(i > 1){
                        labelTable[i][j] = new QLabel(
                                    QString::number(b[i-2][0]/B[i-2][i-2],'f',2),
                                    this);
                    }
                    else {
                        try {
                            Matrix x(xN), c;
                            x.joinBottom(xB);

                            if(faza1) {
                                c = cPhase1;
                            }
                            else {
                                c = cN;
                                c.joinBottom(cB);
                            }
                            x0 = (c.transpose()*x)[0][0];
                        } catch(std::string s) {
                            std::cout << s << std::endl;
                        }

                        labelTable[i][j] = new QLabel(QString::number(x0,'f',2),this);
                    }
                }
                else if(faza1){
                    if(i == 1)
                        labelTable[i][j] = new QLabel(this);
                    else if(labelsBN[i-2] > int(nVars2))
                        labelTable[i][j] = new QLabel(tr("y")+QString::number(labelsBN[i-2]-int(nVars2)),this);
                    else
                        labelTable[i][j] = new QLabel(tr("s")+QString::number(-labelsBN[i-2]),this);
                } else
                    labelTable[i][j] = new QLabel(tr("s")+QString::number(i-1),this);
            }
            layoutTable->addWidget(labelTable[i][j],i,j);
        }
    }
    labelTable[1][0]->setText(tr("x0"));

    layoutRoot->addWidget(new QSeparator(this));
    layoutRoot->addWidget(labelTableTitle);
    layoutRoot->addItem(layoutTable);
    layoutRoot->addSpacerItem(new QSpacerItem(1,1));
    layoutRoot->setStretch(7,10);
}

void StepsDialog::resizeTableFaza2()
{
    std::vector<int> labTmp(labelsBN.begin()+nConstr,labelsBN.end());
    unsigned found=0;
    for(unsigned j=0; j+found < nVars+nYVars; ++j){
        if(labTmp[j] > int(nVars2)){
            for(unsigned i=0; i < nConstr+2; ++i){
                for(unsigned k=j+2; k < nVars+nYVars+1-found; ++k){
                    labelTable[i][k]->setText(labelTable[i][k+1]->text());
                }
            }
            labTmp.erase(labTmp.begin()+j);
            ++found;
            --j;
        }
    }

    for(unsigned i=0; i < nConstr+2; ++i){
        for(unsigned j=0; j < nYVars; ++j){
            labelTable[i][j+nVars+2]->hide();
            if(i < nConstr){
                labelXSA[i][j+nVars2]->hide();
                labelAI[i][j+nVars2]->hide();
            }
        }
        if(i < nConstr){
            labelXSA[i][nVars2-1]->setExtra(tr(" = "));
            labelXSA[i][nVars2-1]->setTextExtra();
        }
    }
}


void StepsDialog::initButtons()
{
    buttonBox = new QHBoxLayout();
    buttonStart = new QPushButton(tr(" << Od nowa "),this);
    buttonNext = new QPushButton(tr(" Dalej > "),this);
    buttonResult = new QPushButton(tr(" Rozwiązanie >> "),this);

    buttonBox->addWidget(buttonStart);
    connect(buttonStart,SIGNAL(clicked()),this,SLOT(startFromBeginning()));
    buttonBox->addSpacerItem(new QSpacerItem(1,1));
    buttonBox->addWidget(buttonNext);
    connect(buttonNext,SIGNAL(clicked()),this,SLOT(simplexNext()));
    buttonBox->addSpacerItem(new QSpacerItem(1,1));
    buttonBox->addWidget(buttonResult);
    connect(buttonResult,SIGNAL(clicked()),this,SLOT(simplexGoToResult()));
    buttonBox->setStretch(1,10);
    buttonBox->setStretch(3,10);

    layoutRoot->addWidget(new QSeparator(this));
    layoutRoot->addItem(buttonBox);
    layoutRoot->addSpacerItem(new QSpacerItem(1,1));
    layoutRoot->setStretch(10,10);
}

void StepsDialog::initGraphics()
{
    RootWidget *p = dynamic_cast<RootWidget*>(parentWidget());

    viewConstraints = new QCustomPlot();
    viewConstraints->setFixedSize(400,300);
    viewConstraints->xAxis->setLabel(tr("X1"));
    viewConstraints->yAxis->setLabel(tr("X2"));

//    int alfa = 60;
//    colorBlue[0] = QColor(0,0,255,alfa);    // blue
//    colorBlue[1] = QColor(255,0,0,alfa);    // red
//    colorBlue[2] = QColor(0,255,0,alfa);    // green
//    colorBlue[3] = QColor(153,51,255,alfa); // purple
//    colorBlue[4] = QColor(153,0,0,alfa);    // dark red
//    colorBlue[5] = QColor(0,204,204,alfa);  // cyan
//    colorBlue[6] = QColor(255,0,127,alfa);  // pink
//    colorBlue[7] = QColor(51,255,51,alfa);  // light green
//    colorBlue[8] = QColor(255,128,0,alfa);  // orange
//    colorBlue[9] = QColor(102,0,51,alfa);   // king

    colorBlue = QColor(0,0,255,60);       // blue
    colorFunction = QColor(255,0,0,150);    // function

    double maxX1 = calcMaxX1Range();
    QVector<double> x1(2);
    x1[0]=0.0;
    x1[1]=maxX1;

    viewConstraints->xAxis->setRange(0,x1[1]);
    viewConstraints->yAxis->setRange(0,1);

    for(unsigned i=0; i < nConstr; ++i){
        Matrix x11(x1);
        QVector<double> x2(2);

        viewConstraints->addGraph();

        if(std::abs(AI[i][0]) < 1e-10)
            x2 = (ones(x11.nRows(),1)*b[i][0]).toQVector();
        else {
            x2 = ((-x11)*AI[i][0] + b[i][0]).toQVector();
        }
        if(std::abs(AI[i][1]) > 1e-10)
            x2 = (Matrix(x2)/AI[i][1]).toQVector();
        else {
            x11[0][0] = b[i][0] / AI[i][0];
            x11[1][0] = x11[0][0];

            double x20 = viewConstraints->yAxis->range().lower;
            double x21 = viewConstraints->yAxis->range().upper;
            x2[0]=x20;
            x2[1]=x21;

        }

        viewConstraints->graph(i)->setData(x11.toQVector(),x2);

        if(x2[1] > viewConstraints->yAxis->range().upper ||
                x2[0] > viewConstraints->yAxis->range().upper)
            viewConstraints->yAxis->setRange(viewConstraints->yAxis->range().lower,std::max(x2[0],x2[1])*1.15);

        if(x2[0] < viewConstraints->yAxis->range().lower)
            viewConstraints->yAxis->setRange(x2[0]*1.15,viewConstraints->yAxis->range().upper);

        viewConstraints->graph(i)->setPen(QColor(colorBlue.rgb()));
        viewConstraints->replot();
    }
    unsigned constraint=0;
    for(unsigned i=0; i < nConstr; ++i){
        if(p->getComboInequal(i)->currentIndex()){
            QVector<double> x1(2), x2(2);
            x1[0] = 0.0;
            x1[1] = viewConstraints->xAxis->range().upper;
            x2[0] = viewConstraints->yAxis->range().lower;
            x2[1] = viewConstraints->yAxis->range().upper;
            viewConstraints->addGraph();
            if(AI[i][0] < 0){
                if(AI[i][1] > 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                else if(AI[i][1] < 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[0]));
                else {
                    x1[0] = viewConstraints->graph(i)->data()->last().key;
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                    viewConstraints->graph(nConstr+constraint)->setPen(Qt::NoPen);
                    viewConstraints->graph(nConstr+constraint)->setBrush(QBrush(colorBlue));
                }
            } else {
                if(AI[i][1] < 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[0]));
                else if(AI[i][1] > 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                else {
                    x1[0] = viewConstraints->graph(i)->data()->last().key;
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                    viewConstraints->graph(nConstr+constraint)->setPen(Qt::NoPen);
                    viewConstraints->graph(nConstr+constraint)->setBrush(QBrush(colorBlue));
                }
            }
            viewConstraints->graph(i)->setChannelFillGraph(viewConstraints->graph(nConstr+constraint));
            ++constraint;
        } else {
            QVector<double> x1(2), x2(2);
            x1[0] = 0.0;
            x1[1] = viewConstraints->xAxis->range().upper;
            x2[0] = viewConstraints->yAxis->range().lower;
            x2[1] = viewConstraints->yAxis->range().upper;
            viewConstraints->addGraph();
            if(AI[i][0] < 0){
                if(AI[i][1] > 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[0]));
                else if(AI[i][1] < 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                else {
                    x1[1] = viewConstraints->graph(i)->data()->last().key;
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                    viewConstraints->graph(nConstr+constraint)->setPen(Qt::NoPen);
                    viewConstraints->graph(nConstr+constraint)->setBrush(QBrush(colorBlue));
                }
            } else {
                if(AI[i][1] > 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[0]));
                else if(AI[i][1] < 0)
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                else {
                    x1[1] = viewConstraints->graph(i)->data()->last().key;
                    viewConstraints->graph(nConstr+constraint)->setData(x1,QVector<double>(2,x2[1]));
                    viewConstraints->graph(nConstr+constraint)->setPen(Qt::NoPen);
                    viewConstraints->graph(nConstr+constraint)->setBrush(QBrush(colorBlue));
                }
            }
            viewConstraints->graph(i)->setChannelFillGraph(viewConstraints->graph(nConstr+constraint));
            ++constraint;
        }
        viewConstraints->graph(i)->setBrush(QBrush(colorBlue));
        viewConstraints->repaint();
    }

    funcGraphIndex = viewConstraints->graphCount();
    drawCurrentResult();
    layoutRootWithGraphics->addWidget(viewConstraints);
}

void StepsDialog::initEquation(QComboBox **comboInequal)
{
    liczbaRozwiazan = 0;
    try {
        AI.joinRight(eye(nConstr,comboInequal));
        c.joinBottom(zeros(nConstr,1));

        if(checkIfPhase1(comboInequal)){
            cB = ones(nConstr,1);
            cN = zeros(nVars2,1);
            origCB = zeros(nConstr,1);
            origCN = c;
            xB = b;
            xN = zeros(nVars2,1);
            B = eye(nConstr);
            N = Matrix(AI);
            AI.joinRight(eye(nConstr));

            // etykiety
            for(int i=1, j=1; i <= int(nConstr); ++i){
                if(comboInequal[i-1]->currentIndex()){
                    labelsBN.push_back(nVars2+j);
                    ++j;
                } else
                    labelsBN.push_back(-i);
            }
            for(int i=int(nConstr)+1, j=1; i <= int(nVars2+nConstr); ++i, ++j){
                if(j > int(nVars))
                    labelsBN.push_back(-(j-int(nVars)));
                else
                    labelsBN.push_back(j);
            }

            auto removed = 0;
            for(const auto &colIn : notYVarsIndexes){
                Matrix tmp(N.column(nVars+colIn-removed));
                swapCols(B,colIn,tmp,0);
                cB[colIn][0] = 0;

                auto j = nVars+colIn-removed;
                N.removeColumn(j);
                cN.removeRow(j);
                xN.removeRow(j);

                AI.removeColumn(nVars2+colIn-removed);
                labelsBN.erase(labelsBN.begin()+nVars2+colIn-removed);
                ++removed;
            }
            cPhase1 = cN;
            cPhase1.joinBottom(cB);

            for(const auto &i : labelsBN)
                std::cout << i << ' ';
            std::cout << std::endl;

            AI.print("AI");
            cB.print("cB");
            cN.print("cN");
            xB.print("xB");
            xN.print("xN");
            B.print("B");
            N.print("N");

            Matrix lambda = gauss(B.transpose(),cB);
            p = (cN.transpose() - lambda.transpose()*N).transpose();
        } else {
            cN = Matrix(c,0,nVars,0,1);
            cB = Matrix(c,nVars,nVars2,0,1);
            xB = b;
            xN = zeros(nVars,1);
            B = Matrix(AI,0,nConstr,nVars,nVars2);
            N = Matrix(AI,0,nConstr,0,nVars);

            // etykiety
            for(int i=-1; -i <= int(nConstr); --i) // -1 : -1 : -m
                labelsBN.push_back(i);
            for(int i=1; i <= int(nVars); ++i) // 1 : n
                labelsBN.push_back(i);

            Matrix lambda = gauss(B.transpose(),cB);
            p = (cN.transpose() - lambda.transpose()*N).transpose();
        }
    } catch(std::string str) {
        std::cout << str << std::endl;
    }

    comingIndex = 0;
    leavingIndex = 0;
    L = 0; // aktualna liczba iteracji

    // max. liczba iteracji
    auto silnia = [](unsigned n)->unsigned {
        unsigned result = 1;
        for(; n > 1; --n)
            result *= n;
        return result;
    };
    if(faza1)
        maxL = silnia(nVars2+nConstr)/(silnia(nConstr)*silnia(nVars2));
    else
        maxL = silnia(nVars2)/(silnia(nConstr)*silnia(nVars));
}

bool StepsDialog::checkIfPhase1(QComboBox **comboInequal)
{
    // sprawdzenie czy uruchamiamy fazę 1.
    faza1=false;

    for(unsigned i=0; i < nConstr; ++i){
        if(b[i][0] < 0){
            RootWidget *p = dynamic_cast<RootWidget*>(parentWidget());
            for(unsigned k=0; k < AI.nCols(); ++k){
                AI[i][k] = -AI[i][k];
                p->setEditA(i,k,QString::number(AI[i][k],'f',2));
            }
            b[i][0] = -b[i][0];
            p->setEdit_b(i,QString::number(b[i][0],'f',2));
            comboInequal[i]->setCurrentIndex((comboInequal[i]->currentIndex()+1) % 2);
        }
    }

    for(unsigned i=0; i < nConstr; ++i){
        if(comboInequal[i]->currentIndex()){
            faza1=true;
            ++nYVars;
        } else
          notYVarsIndexes.push_back(i);
    }
    std::cout << "nYVars=" << nYVars << std::endl;
    return faza1;
}

/* B, N, cB, cN, xB, labelsX, L
 return:
 0 - Optimum (faza1: pierwsza baza albo brak rozwiazan; faza2: optimum)
 1 - Uklad ograniczen jest nieograniczony
-1 - Kontynuowac obliczenia
*/
int StepsDialog::iterateRevisedSimplex()
{
//    // KROK 1: B'*lambda = cB
//    Matrix lambda = gauss(B.transpose(),cB);

//    // KROK 2: kolumna wchodząca do bazy (k)
//    p = (cN.transpose() - lambda.transpose()*N).transpose();
    std::pair<double,unsigned> pk;
    if(faza1 || minimizeFunction)
        pk = p.min();
    else
        pk = (-p).min();

    if(pk.first >= -1e-10){ // ...STOP! Pierwsza dopuszczalna baza albo brak rozwiązań (faza1), noweRozw. (faza2)
        x0 = (Matrix(xB).joinBottom(xN).transpose()*Matrix(cB).joinBottom(cN))[0][0];
        if(faza1)
            return 0;
        if(std::abs(pk.first) < 1e-10){
            Matrix noweRozw(std::vector<int>(labelsBN.begin(),labelsBN.begin()+nConstr));
            if(liczbaRozwiazan > 1){
                for(unsigned i=0; i < liczbaRozwiazan; ++i){
                    checkIfFinished(i);
                    if(koniec)
                        return 0;
                    rozwiazania.push_back(std::make_pair(x0,getXInOrder()));
                    rozwiazaniaLabels.joinRight(noweRozw);
                }
            } else if(liczbaRozwiazan == 0){
                rozwiazania.push_back(std::make_pair(x0,getXInOrder()));
                rozwiazaniaLabels = Matrix(noweRozw);
            } else {
                try {
                    rozwiazania.push_back(std::make_pair(x0,getXInOrder()));
                    rozwiazaniaLabels.joinRight(noweRozw);
                } catch(std::string s){
                    std::cout << s << std::endl;
                }
            }
            ++liczbaRozwiazan;
        } else {
            try {
                rozwiazania.push_back(std::make_pair(x0,getXInOrder()));
            } catch(std::string s){
                std::cout << s << std::endl;
            }
            return 0;
        }
    }

    comingIndex = pk.second;
    Matrix ak(N,0,nConstr,comingIndex,comingIndex+1);
    // KROK 3: B*y=ak
    y = gauss(B,ak);
    // KROK 4: sprawdzenie czy układ ograniczeń jest ograniczony
    std::vector<unsigned> positiveIndexesY;
    for(unsigned i=0; i < y.nRows(); ++i){
        if(y[i][0] > 0)
            positiveIndexesY.push_back(i);
    }
    if(positiveIndexesY.empty()){
        if(liczbaRozwiazan==0)
//            std::cout << "Uklad ograniczen jest nieograniczony\n"
//                      << "Rozwiazaniem jest zbior pusty" << std::endl;
            return 1;
        else
            return 0;
    }
    Matrix theta = xB / y;
    theta.print("theta");
    leavingIndex = positiveIndexesY[0];
    double tmin = theta[leavingIndex][0]; // wybieramy pierwszy dodatni do porównania
    std::vector<unsigned>::iterator it=positiveIndexesY.begin();
    for(; it != positiveIndexesY.end(); ++it){
        if((theta[*it][0] < tmin) || ((theta[*it][0] == tmin) && (labelsBN[*it] > int(nVars2)))){
            tmin = theta[*it][0]; // kolumna opuszczająca bazę
            leavingIndex = *it;
        }
    }
    // KROK 5: aktualizacja rozwiazań bazowych.
    for(unsigned i=0; i < xB.nRows(); ++i)
        xB[i][0] -= tmin*y[i][0];
    xB[leavingIndex][0] = tmin;
    // KROK 6: przestawienie kolumn/wierszy w A, c, x oraz labels
    swapCols(B,leavingIndex,N,comingIndex);
    swapRows(cB,leavingIndex,cN,comingIndex);
    if(faza1){
        swapRows(origCB,leavingIndex,origCN,comingIndex);
    }
    std::swap(labelsBN[leavingIndex],labelsBN[xB.nRows()+comingIndex]);
    x0 = (Matrix(xB).joinBottom(xN).transpose()*Matrix(cB).joinBottom(cN))[0][0];

    // aktualizacja zredukowanych kosztów dla kolejnej iteracji
    Matrix lambda = gauss(B.transpose(),cB);
    p = (cN.transpose() - lambda.transpose()*N).transpose();

    return -1;
}

void StepsDialog::updateLabelsTable(int whatToDo)
{
    labelTable[1][1]->setText(QString::number(x0,'f',2));
    for(unsigned i=0; i < p.nRows(); ++i)
        labelTable[1][i+2]->setText(QString::number(p[i][0],'f',2));

    for(unsigned i=0; i < xB.nRows(); ++i)
        labelTable[i+2][1]->setText(QString::number(xB[i][0],'f',2));

    for(unsigned i=0; i < y.nRows(); ++i)
        labelTable[i+2][comingIndex+2]->setText(QString::number(y[i][0],'f',2));

    if(whatToDo < 0 && buttonNext->isEnabled()){
        QString tmpIndex = labelTable[leavingIndex+2][0]->text();
        labelTable[leavingIndex+2][0]->setText(labelTable[0][comingIndex+2]->text());
        labelTable[0][comingIndex+2]->setText(tmpIndex);
    }
}

void StepsDialog::printResultInfo(int whatToDo)
{
    if(faza1 && whatToDo>=0){ // koniec fazy pierwszej
        if(x0 > 0){
            QString currTxt = labelPhase->text();
            labelPhase->setText(currTxt+
                                tr(": Układ ograniczeń jest sprzeczny."));
        } else if(whatToDo > 0){
            QString currTxt = labelPhase->text();
            labelPhase->setText(currTxt+
                                tr(": Układ ograniczeń jest nieograniczony."));
        } else {
            QString currTxt = labelPhase->text();
            labelPhase->setText(currTxt+
                                tr(": Wyznaczono początkowe bazowe rozwiązanie dopuszczalne."));

            disconnect(buttonNext,SIGNAL(clicked()),this,SLOT(simplexNext()));
            connect(buttonNext,SIGNAL(clicked()),this,SLOT(moveTable2Phase2()));
            buttonNext->setEnabled(true);
        }
    } else if(whatToDo==0){
        QString currTxt = labelPhase->text();
        Matrix x(xB);
        x.joinBottom(xN);
        bool optimum=true;
//        if(nConstr < nVars){
//            optimum=false;
//            for(unsigned i=nConstr; i < nVars2; ++i){
//                if(labelsBN[i] > 0 && std::abs(x[i][0]) > 1e-10){
//                    optimum=true;
//                    break;
//                }
//            }
//        }
        if(optimum){
            labelPhase->setText(currTxt+tr(": Optimum! x0=")+QString::number(x0,'f',2)+tr("\n"));
            if(liczbaRozwiazan==2){
                currTxt = labelPhase->text();
                labelPhase->setText(currTxt+tr("... na odcinku:\n"));

                currTxt = labelPhase->text();
                QString iksy = "{[x1";
                for(unsigned i=1; i < nVars; ++i){
                    iksy += tr(", x")+QString::number(i+1);
                }
                iksy+=tr("]} = {");
                labelPhase->setText(currTxt+iksy);
                for(unsigned k=0; k < liczbaRozwiazan; ++k){
                    printResultX(k);
                    currTxt = labelPhase->text();
                    if(k+1 < liczbaRozwiazan){
                        labelPhase->setText(currTxt+tr(", "));
                    } else {
                        labelPhase->setText(currTxt+tr("}"));
                    }

                }
                if(nVars==2){
                    if(viewConstraints->graphCount()-1 == funcGraphIndex)
                      viewConstraints->removeGraph(funcGraphIndex);
                    viewConstraints->addGraph();

                    QVector<double> x1(2);
                    QVector<double> x2(2);

                    x1[0] = rozwiazania[0].second[0][0];
                    x1[1] = rozwiazania[1].second[0][0];
                    x2[0] = rozwiazania[0].second[1][0];
                    x2[1] = rozwiazania[1].second[1][0];

                    viewConstraints->graph(funcGraphIndex)->setData(x1,x2);
                    QPen resPen;
                    resPen.setColor(colorFunction);
                    resPen.setWidth(3);
                    resPen.setStyle(Qt::SolidLine);
                    viewConstraints->graph(funcGraphIndex)->setPen(resPen);
                    viewConstraints->replot();
                }
            } else if(liczbaRozwiazan==1){
                currTxt = labelPhase->text();
                labelPhase->setText(currTxt+tr("... na półprostej:"));
                for(unsigned i=0; i < nVars; ++i){
                    currTxt = labelPhase->text();
                    labelPhase->setText(currTxt+tr("\nx")+QString::number(i+1)+
                                        tr(" = ") + QString::number(getXInOrder()[i][0],'f',2)+
                                        tr(" + ") + QString::number(-getYInOrder()[i][0],'f',2)+
                                        tr("t")/*+ QString::number(i+1)*/);
                }
                if(nVars==2){
                    if(viewConstraints->graphCount()-1 == funcGraphIndex)
                      viewConstraints->removeGraph(funcGraphIndex);
                    viewConstraints->addGraph();

                    QVector<double> x1(2,getXInOrder()[0][0]);
                    QVector<double> x2(2,getXInOrder()[1][0]);
                    int parallel = whichConstraintParallel();

                    x1[1] = viewConstraints->graph(parallel)->data()->last().key;
                    x2[1] = viewConstraints->graph(parallel)->data()->last().value;

                    viewConstraints->graph(funcGraphIndex)->setData(x1,x2);
                    QPen resPen;
                    resPen.setColor(colorFunction);
                    resPen.setWidth(3);
                    resPen.setStyle(Qt::SolidLine);
                    viewConstraints->graph(funcGraphIndex)->setPen(resPen);
                    viewConstraints->replot();
                }
            } else {
                // dla jednego jedynego OPTIMUM (liczbaRozwiazan == 0)
                currTxt = labelPhase->text();
                QString iksy = "[x1";
                for(unsigned i=1; i < nVars; ++i){
                    iksy += tr(", x")+QString::number(i+1);
                }
                iksy+=tr("] = ");
                labelPhase->setText(currTxt+iksy);
                currTxt = labelPhase->text();

                rozwiazania.push_back(std::make_pair(x0,getXInOrder()));
                printResultX(0);

            }
        } else
            labelPhase->setText(currTxt+tr(": Istnieje nieskończenie wiele rozwiązań. x0=")+QString::number(x0,'f',2));
    } else if(whatToDo > 0){
        QString currTxt = labelPhase->text();
        labelPhase->setText(currTxt+tr(": Układ ograniczeń jest nieograniczony."));
    }
}

void StepsDialog::dropAllY()
{
    Matrix c(origCB.transpose());
    c.joinRight(origCN.transpose());

    Matrix x(xB.transpose());
    x.joinRight(xN.transpose());

    unsigned j=0;
    for(unsigned i=0; i < nVars2+nConstr; ++i){
        if(labelsBN[j] > int(nVars2)){
            x.removeColumn(j);
            labelsBN.erase(labelsBN.begin()+j);
            N.removeColumn(j-nConstr);
            c.removeColumn(j);
            --j;
        }
        ++j;
    }
    c = c.transpose();
    x = x.transpose();
    xB = Matrix(x,0,nConstr,0,1);
    xN = Matrix(x,nConstr,nVars2,0,1);
    cB = Matrix(c,0,nConstr,0,1);
    cN = Matrix(c,nConstr,nVars2,0,1);
    p = cN;
    x0 = (cB.transpose()*xB)[0][0];
}

Matrix StepsDialog::getXInOrder()
{
    Matrix x(xB);
    x.joinBottom(xN);
    Matrix X(x);
    for(unsigned i=0; i < labelsBN.size(); ++i){
        if(labelsBN[i] < 0){
            X[nVars-labelsBN[i]-1][0] = x[i][0];
        } else {
            X[labelsBN[i]-1][0] = x[i][0];
        }
    }
    return X;
}

Matrix StepsDialog::getYInOrder()
{
    Matrix x(xB);
    x.joinBottom(xN);

    double proportion=1;
    Matrix Y(zeros(nVars,1));
    for(unsigned i=0; i < nConstr; ++i){
        if((labelsBN[i] > 0) && (labelsBN[i] <= int(nVars))){
            Y[labelsBN[i]-1][0] = y[i][0];
            proportion = c[labelsBN[i]-1][0] / y[i][0];
        }
    }
    for(unsigned i=0; i < nConstr; ++i){
        if(Y[i][0] == 0){
            Y[i][0] = c[i][0]/proportion;
        }
    }
    return Y;
}

bool StepsDialog::checkIfFinished(unsigned i)
{
    Matrix noweRozw(std::vector<int>(labelsBN.begin(),labelsBN.begin()+nConstr));
    Matrix result(rozwiazaniaLabels.column(i));
    result = result - noweRozw;
    koniec=true;
    for(unsigned j=0; j < result.nRows(); ++j){
        if(std::abs(result[j][0]) > 1e-10){
            koniec=false;
            break;
        }
    }
    return koniec;
}

void StepsDialog::printResultX(unsigned i)
{
    QString currTxt = labelPhase->text();
    labelPhase->setText(currTxt+tr("["));
    currTxt = labelPhase->text();
    for(unsigned j=0; j < nVars; ++j){
        labelPhase->setText(currTxt+QString::number(rozwiazania[i].second[j][0],'f',2));
        currTxt = labelPhase->text();
        if(j+1 < nVars){
            labelPhase->setText(currTxt+tr("; "));
            currTxt = labelPhase->text();
        }
    }
    labelPhase->setText(currTxt+tr("]"));

}

double StepsDialog::calcMaxX1Range()
{
    double result = 0.1;

    for(unsigned i=0; i < nConstr; ++i){
        if(std::abs(AI[i][0]) > 1e-10){
            if(std::abs(b[i][0]/AI[i][0]) > result){
                result = std::abs(b[i][0]/AI[i][0]);
            }
        }
    }

    return result*1.15;
}

void StepsDialog::drawCurrentResult()
{
    if(liczbaRozwiazan < 2){
        double _x1, _x2;
        double yLower = viewConstraints->yAxis->range().lower;
        double yUpper = viewConstraints->yAxis->range().upper;
        double xUpper = viewConstraints->xAxis->range().upper;
        _x1 = getXInOrder()[0][0];
        _x2 = getXInOrder()[1][0];

        QCPItemEllipse *ell = new QCPItemEllipse(viewConstraints);
        ell->setPen(colorFunction);
        double ry = (yUpper - yLower) / 15;
        double rx = xUpper / 20;
        ell->topLeft->setCoords(_x1-rx,_x2+ry);
        ell->bottomRight->setCoords(_x1+rx,_x2-ry);
        viewConstraints->addItem(ell);


        QVector<double> x1(2), x2(2);
        x1[0] = 0.0;
        x1[1] = xUpper;

        Matrix x11(x1);
        double x0 = this->x0;
        if(faza1){
            x0 = 0.0;
            for(unsigned i=0; i < nConstr; ++i){
                if(labelsBN[i] > 0 && labelsBN[i] <= int(nVars)){
                    x0 += xB[i][0]*origCB[i][0];
                }
            }
        }

        if(std::abs(c[0][0]) < 1e-10)
            x2 = (ones(x11.nRows(),1)*x0).toQVector();
        else {
            x2 = ((-x11)*c[0][0] + x0).toQVector();
        }
        if(std::abs(c[1][0]) > 1e-10)
            x2 = (Matrix(x2)/c[1][0]).toQVector();
        else {
            x11[0][0] = x0 / c[0][0];
            x11[1][0] = x11[0][0];

            x2[0]=yLower;
            x2[1]=yUpper;

        }

        viewConstraints->addGraph();
        viewConstraints->graph(funcGraphIndex)->setData(x11.toQVector(),x2);
        QPen fPen;
        fPen.setColor(colorFunction);
        fPen.setWidth(2);
        fPen.setStyle(Qt::SolidLine);
        viewConstraints->graph(funcGraphIndex)->setPen(fPen);

        viewConstraints->replot();
    }
}

int StepsDialog::whichConstraintParallel()
{
    int i=0;
    for( ; i < int(nConstr); ++i){
        Matrix Ac(AI,i,i+1,0,nVars);
        Ac = (Ac.transpose()).joinRight(Matrix(c,0,nVars,0,1));

        if(Ac.det() == 0)
            break;
    }
    return i;
}

QString StepsDialog::generateFunctionLabel()
{
    QString minMax("max  ");
    QString fazaString("FAZA 2:   ");
    QString func("y1");
    RootWidget *p = dynamic_cast<RootWidget*>(parentWidget());

    if(faza1){
        minMax = tr("min  ");
        for(unsigned i=2; i <= nYVars; ++i)
            func += tr(" + y") + QString::number(i);
        fazaString = tr("FAZA 1:   ");
    }
    else {
        if(p->getComboMinMaxFunc()->currentIndex())
            minMax = tr("min  ");
        func = QString::number(c[0][0],'f',2) + tr("x1");
        for(unsigned i=2; i <= nVars2-nConstr; ++i){
            if(c[i-1][0] >= 0)
                func += tr(" +");
            else
                func += tr(" ");
            func += QString::number(double(c[i-1][0]),'f',2);
            if(i <= nVars)
                func += tr("x") + QString::number(i);
            else
                func += tr("s") + QString::number(i-nVars);
        }
    }

    return fazaString + minMax + func;
}

void StepsDialog::moveTable2Phase2()
{
    faza1=false;

    labelPhase->setText(generateFunctionLabel());

    resizeTableFaza2();
    dropAllY();

    // KROK 1: B'*lambda = cB
    Matrix lambda = gauss(B.transpose(),cB);

    // KROK 2: kolumna wchodząca do bazy (k)
    p = (cN.transpose() - lambda.transpose()*N).transpose();

    updateLabelsTable(0);
    disconnect(buttonNext,SIGNAL(clicked()),this,SLOT(moveTable2Phase2()));
    connect(buttonNext,SIGNAL(clicked()),this,SLOT(simplexNext()));
    buttonResult->setEnabled(true);
}
