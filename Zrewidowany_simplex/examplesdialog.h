#ifndef EXAMPLESDIALOG_H
#define EXAMPLESDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>

class ExamplesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExamplesDialog(int exNumber=0, QWidget *parent = 0);

signals:
    void hidden();

private slots:
    void loadEx();
    void loadNextEx();
    void loadPrevEx();
    void hide();

private:
    QVBoxLayout *layoutRoot;

    const int MAX_M; // N - liczba zmiennych
    const int MAX_N; // M - liczba ograniczen
    int       exNumber;

    void initExTitle();
    QLabel   *labelTitleEx;

    void initFunc();
    QLabel   *labelFunc;

    void initSpinner();
    QHBoxLayout *layoutSpinner;
    QPushButton *buttonPrev;
    QGridLayout *layoutEx;
    QLabel    ***labelAx_b;
    QPushButton *buttonNext;

    void initButtons();
    QDialogButtonBox * buttonBox;
};

#endif // EXAMPLESDIALOG_H
