#include "examplesdialog.h"
#include "exercises.h"
#include "qseparator.h"
#include "rootwidget.h"
#include <iostream>

ExamplesDialog::ExamplesDialog(int exNumber, QWidget *parent) :
    QDialog(parent),
    MAX_M(10),
    MAX_N(31),
    exNumber(exNumber)
{
    setWindowTitle(tr("Wybierz przykład"));
    setWhatsThis(tr("Okno pokazuje kolejne etapy rozwiązania metodą zrewidowanego Simpleksu"));
    layoutRoot = new QVBoxLayout();

    initExTitle();
    initFunc();
    initSpinner();
    initButtons();

    setLayout(layoutRoot);
}


void ExamplesDialog::loadEx()
{
    RootWidget *p = dynamic_cast<RootWidget*>(parentWidget());
    int m = static_cast<int>(zadanieDouble::M(exNumber));
    int n = static_cast<int>(zadanieDouble::N(exNumber));
    int i, j;

    p->setN(n);
    p->setM(m);
    p->setComboMinMaxFunc(zadanieDouble::zad(exNumber,0,0));
    try {
        p->setCurrentEx(exNumber);
    } catch(std::string s) {
        std::cout << s << std::endl;
    }


    for(i=0; i < n; ++i){
        p->setEdit_c(i,QString::number(zadanieDouble::zad(exNumber,0,i+1)));
    }
    for(i=0; i < m; ++i){
        for(j=0; j < n; ++j){
            p->setEditA(i,j,QString::number(zadanieDouble::zad(exNumber,i+1,j)));
        }
        p->setComboInequal(i,zadanieDouble::zad(exNumber,i+1,n));
        p->setEdit_b(i,QString::number(zadanieDouble::zad(exNumber,i+1,n+1)));
    }
    hide();
}

void ExamplesDialog::loadNextEx()
{
    exNumber = (exNumber+1)%zadanieDouble::exNum();

    labelTitleEx->setText(tr("Zestaw nr ")+QString::number(exNumber+1)+tr(":"));
    labelFunc->setText(tr(zadanieString::zad(exNumber,0,0)));
    for(int i=0; i < MAX_M; ++i){
        for(int j=0; j < MAX_N; ++j){
            if(i < zadanieString::M(exNumber) && j < zadanieString::N(exNumber)){
                labelAx_b[i][j]->setText(tr(zadanieString::zad(exNumber,i+1,j)));
                labelAx_b[i][j]->show();
            } else
                labelAx_b[i][j]->hide();
        }
    }
}

void ExamplesDialog::loadPrevEx()
{
    if(exNumber==0)
        exNumber = static_cast<int>(zadanieDouble::exNum());
    exNumber = (exNumber-1)%zadanieDouble::exNum();

    labelTitleEx->setText(tr("Zestaw nr ")+QString::number(exNumber+1)+tr(":"));
    labelFunc->setText(tr(zadanieString::zad(exNumber,0,0)));
    for(int i=0; i < MAX_M; ++i){
        for(int j=0; j < MAX_N; ++j){
            if(i < zadanieString::M(exNumber) && j < zadanieString::N(exNumber)){
                labelAx_b[i][j]->setText(tr(zadanieString::zad(exNumber,i+1,j)));
                labelAx_b[i][j]->show();
            } else
                labelAx_b[i][j]->hide();
        }
    }
}

void ExamplesDialog::hide()
{
    QDialog::hide();
    emit hidden();
}

void ExamplesDialog::initExTitle()
{
    labelTitleEx = new QLabel(tr("Zestaw nr ")+QString::number(exNumber+1)+tr(":"),this);
    layoutRoot->addWidget(labelTitleEx);
    layoutRoot->setAlignment(labelTitleEx,Qt::AlignCenter);
    layoutRoot->addWidget(new QSeparator(this));
}

void ExamplesDialog::initFunc()
{
    labelFunc = new QLabel(tr(zadanieString::zad(exNumber,0,0)),this);
    layoutRoot->addWidget(labelFunc);
    layoutRoot->setAlignment(labelFunc,Qt::AlignCenter);
}

void ExamplesDialog::initSpinner()
{
    layoutSpinner = new QHBoxLayout();
    buttonPrev = new QPushButton(tr("<<"),this);
    layoutEx = new QGridLayout();
    buttonNext = new QPushButton(tr(">>"),this);
    layoutSpinner->addWidget(buttonPrev);
    connect(buttonPrev,SIGNAL(clicked()),this,SLOT(loadPrevEx()));
    layoutSpinner->addWidget(new QSeparator(QFrame::VLine,this));

    // load constraints
    labelAx_b = new QLabel**[MAX_M];
    for(int i=0; i < MAX_M; ++i){
        labelAx_b[i] = new QLabel*[MAX_N];
        for(int j=0; j < MAX_N; ++j){
            if(i < zadanieString::M(exNumber) && j < zadanieString::N(exNumber))
                labelAx_b[i][j] = new QLabel(tr(zadanieString::zad(exNumber,i+1,j)),this);
            else {
                labelAx_b[i][j] = new QLabel(tr(""),this);
                labelAx_b[i][j]->hide();
            }
            layoutEx->addWidget(labelAx_b[i][j],i,j);
        }
    }
    layoutEx->setMargin(10);

    layoutSpinner->addItem(layoutEx);
    layoutSpinner->addWidget(new QSeparator(QFrame::VLine,this));
    layoutSpinner->addWidget(buttonNext);
    connect(buttonNext,SIGNAL(clicked()),this,SLOT(loadNextEx()));
    layoutRoot->addItem(layoutSpinner);

}

void ExamplesDialog::initButtons()
{
    buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Ok|QDialogButtonBox::Cancel ,this);
    connect(buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(loadEx()));
    connect(buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(hide()));
    layoutRoot->addWidget(buttonBox);
    layoutRoot->addSpacerItem(new QSpacerItem(1,1));
    layoutRoot->setStretch(5,10);
}
