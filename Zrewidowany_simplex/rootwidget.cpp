#include "rootwidget.h"
#include "qseparator.h"


RootWidget::RootWidget(QWidget *parent) :
    QWidget(parent),
    MIN_MN(1),
    MAX_MN(10),
    currentEx(0)
{
    // layout opakowujący wszystko inne
    layoutRoot = new QVBoxLayout(this);

    initN();
    initFunc();
    layoutRoot->addWidget(new QSeparator(this));
    initM();
    initA_b();
    layoutRoot->addWidget(new QSeparator(this));
    initButton();

    setLayout(layoutRoot);
    setMaximumWidth(this->width());
    setMaximumHeight(this->height());
}

void RootWidget::setEdit_c(int index, QString value)
{
    edit_c[index]->setText(value);
}

void RootWidget::setComboMinMaxFunc(int maxMin){
    comboMinMaxFunc->setCurrentIndex(maxMin);
}

void RootWidget::setEditA(int row, int col, QString value)
{
    editA[row][col]->setText(value);
}

void RootWidget::setComboInequal(int row, int ltGt)
{
    comboInequal[row]->setCurrentIndex(ltGt);
}

QComboBox* RootWidget::getComboInequal(int row) const
{
    return comboInequal[row];
}

void RootWidget::setEdit_b(int row, QString value)
{
    edit_b[row]->setText(value);
}

void RootWidget::setN(int value){
    if(value < 1 || value > 10)
        throw tr("zły zakres N");
    spinN->setValue(value);
}

void RootWidget::setM(int value){
    if(value < 1 || value > 10)
        throw tr("zły zakres M");
    spinM->setValue(value);
}

void RootWidget::setCurrentEx(int value)
{
    if(value < 0)
        throw std::string("Zestaw nie moze miec ujemnego indeksu!");
    currentEx=value;
}

QPushButton &RootWidget::getButtonRSM()
{
    return *buttonRSM;
}

QComboBox *RootWidget::getComboMinMaxFunc()
{
    return comboMinMaxFunc;
}

RootWidget::~RootWidget()
{

}

void RootWidget::onButtonExamplesClicked()
{
    exDial = new ExamplesDialog(currentEx,this);
    exDial->exec();
    connect(exDial,SIGNAL(hidden()),this,SLOT(onExDialExit()));
}

void RootWidget::onButtonRSMClicked()
{
    sDial = new StepsDialog(comboMinMaxFunc,edit_c,editA,comboInequal,edit_b,n(),m(),this);
    sDial->exec();
    connect(sDial,SIGNAL(hidden()),this,SLOT(onSDialExit()));
}

void RootWidget::onSDialExit()
{
    delete sDial;
}

void RootWidget::onExDialExit()
{
    delete exDial;
}

void RootWidget::onSpinNValueChanged(int n)
{
    for(int col=0; col < MAX_MN; ++col){
        if(col < n){
            labelX[col]->show(col,n);
            edit_c[col]->show();
            for(int row=0; row < m(); ++row){
                labelXA[row][col]->show(col,n);
                editA[row][col]->show();
            }
        }
        else {
            edit_c[col]->hide();
            labelX[col]->hide();
            for(int row=0; row < m(); ++row){
                editA[row][col]->hide();
                labelXA[row][col]->hide();
            }
        }
    }
    adjustSize();
}

void RootWidget::onSpinMValueChanged(int m)
{
    for(int row=0; row < MAX_MN; ++row){
        if(row < m){
            labelARowsNo[row]->show();
            comboInequal[row]->show();
            edit_b[row]->show();
            for(int col=0; col < n(); ++col){
                editA[row][col]->show();
                labelXA[row][col]->show(col,n());
            }
        }
        else {
            labelARowsNo[row]->hide();
            comboInequal[row]->hide();
            edit_b[row]->hide();
            for(int col=0; col < n(); ++col){
                editA[row][col]->hide();
                labelXA[row][col]->hide();
            }
        }
    }
    adjustSize();
}

void RootWidget::initN()
{
    layoutN = new QHBoxLayout();
    layoutN->setSpacing(3);

    labelN = new QLabel(tr("Liczba zmiennych (max. 10): "),this);
    spinN = new QSpinBox(this);
    spinN->setRange(MIN_MN,MAX_MN);
    spinN->setValue(MAX_MN);
    spinN->setKeyboardTracking(false);
    connect(spinN,SIGNAL(valueChanged(int)),this,SLOT(onSpinNValueChanged(int)));

    buttonLoadEx = new QPushButton(tr(" Wczytaj z zestawu zadań "),this);
    connect(buttonLoadEx,SIGNAL(clicked()),this,SLOT(onButtonExamplesClicked()));

    layoutN->addWidget(labelN);
    layoutN->addWidget(spinN);
    layoutN->addWidget(new QPushButton(tr("OK"),this));
    layoutN->addSpacerItem(new QSpacerItem(1,1));
    layoutN->setStretch(3,10);
    layoutN->addWidget(buttonLoadEx);

    layoutRoot->addLayout(layoutN);
}

void RootWidget::initFunc()
{
    layoutFunc = new QHBoxLayout();
    comboMinMaxFunc = new QComboBox(this);
    comboMinMaxFunc->addItem(tr("max"),tr("max"));
    comboMinMaxFunc->addItem(tr("min"),tr("min"));
    layoutFunc->addWidget(comboMinMaxFunc);

    labelX = new QVarLabel*[MAX_MN];
    edit_c = new QVarEdit*[MAX_MN];
    for(int i=0; i < MAX_MN; ++i){
        edit_c[i] = new QVarEdit(this);
        labelX[i] = new QVarLabel(tr("x"),i+1,tr(" +"),this);
        if(i == MAX_MN-1)
            labelX[i]->setTextNoExtra();

        layoutFunc->addWidget(edit_c[i]);
        layoutFunc->addWidget(labelX[i]);
    }
    layoutFunc->addSpacerItem(new QSpacerItem(1,1));
    layoutFunc->setStretch(2*MAX_MN+1,10);

    layoutRoot->addLayout(layoutFunc);
}

void RootWidget::initM()
{
    layoutM = new QHBoxLayout();
    layoutM->setSpacing(3);

    labelM = new QLabel(tr("Liczba ograniczeń (max. 10):"),this);
    spinM = new QSpinBox(this);
    spinM->setRange(MIN_MN,MAX_MN);
    spinM->setValue(MAX_MN);
    spinM->setKeyboardTracking(false);
    connect(spinM,SIGNAL(valueChanged(int)),this,SLOT(onSpinMValueChanged(int)));

    layoutM->addWidget(labelM);
    layoutM->addWidget(spinM);
    layoutM->addWidget(new QPushButton(tr("OK"),this));
    layoutM->addSpacerItem(new QSpacerItem(1,1));
    layoutM->setStretch(3,10);

    layoutRoot->addLayout(layoutM);
}

void RootWidget::initA_b()
{
    layoutOuterA_b = new QHBoxLayout();
    layoutA_b = new QGridLayout();
    layoutA_b->setSpacing(3);

    labelARowsNo = new QLabel*[MAX_MN];
    editA = new QVarEdit**[MAX_MN];
    labelXA = new QVarLabel**[MAX_MN];
    comboInequal = new QComboBox*[MAX_MN];
    edit_b = new QVarEdit*[MAX_MN];
    for(int i=0; i < MAX_MN; ++i){
        labelARowsNo[i] = new QLabel(QString::number(i+1) + tr(" :"));
        editA[i] = new QVarEdit*[MAX_MN];
        labelXA[i] = new QVarLabel*[MAX_MN];
        comboInequal[i] = new QComboBox(this);
        comboInequal[i]->addItem("<=");
        comboInequal[i]->addItem(">=");
        edit_b[i] = new QVarEdit(this);
        for(int j=0; j< MAX_MN; ++j){
            editA[i][j] = new QVarEdit(this);
            labelXA[i][j] = new QVarLabel(labelX[j],this);
            if(j == MAX_MN-1)
                labelXA[i][j]->setTextNoExtra();
            layoutA_b->addWidget(editA[i][j],i,2*j+1);
            layoutA_b->addWidget(labelXA[i][j],i,2*j+2);
        }
        layoutA_b->addWidget(labelARowsNo[i],i,0,Qt::AlignRight);
        layoutA_b->addWidget(comboInequal[i],i,2*MAX_MN+2);
        layoutA_b->addWidget(edit_b[i],i,2*MAX_MN+3);
    }

    layoutOuterA_b->addLayout(layoutA_b);
    layoutOuterA_b->addSpacerItem(new QSpacerItem(1,1));
    layoutOuterA_b->setStretch(1,10);
    layoutRoot->addLayout(layoutOuterA_b);
}

void RootWidget::initButton()
{
    layoutButton = new QHBoxLayout();
    buttonRSM = new QPushButton(" Rozwiąż metodą Zrewidowanego Simpleksu ",this);
    connect(buttonRSM,SIGNAL(clicked()),this,SLOT(onButtonRSMClicked()));

    layoutButton->addWidget(buttonRSM);
    layoutButton->addSpacerItem(new QSpacerItem(1,1));
    layoutButton->setStretch(1,10);

    layoutRoot->addLayout(layoutButton);
    layoutRoot->addSpacerItem(new QSpacerItem(1,1));
    layoutRoot->setStretch(7,10);
}
