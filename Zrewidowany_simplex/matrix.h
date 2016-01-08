#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <utility>
#include "qvaredit.h"
#include <QComboBox>
#include <QLabel>

typedef std::vector<double> doubleVector;

class Matrix
{
    unsigned rows;
    unsigned cols;
    std::vector<doubleVector> data;
public:
    Matrix() : rows(0), cols(0) {}
    Matrix(const Matrix &other);
    Matrix(unsigned nRows, unsigned nCols, double defaultValue=0.0);
    Matrix(unsigned nRows, unsigned nCols, QVarEdit ***editMatrix, bool isVerticalVector=false);
    Matrix(const Matrix &other, unsigned fromRow, unsigned toRow, unsigned fromCol, unsigned toCol);
    Matrix(const std::vector<int> &v);
    Matrix(const QVector<double> &v);


    void print(std::string label="");
    doubleVector &operator[](unsigned row);
    const doubleVector &operator[](unsigned row) const;

    Matrix joinRight(Matrix &other);
    Matrix joinBottom(Matrix &other);
    Matrix column(unsigned colIndex);
    void removeColumn(unsigned colIndex);
    void removeRow(unsigned rowIndex);

    Matrix& operator=(const Matrix& rhs);
    Matrix operator+(const Matrix& rhs);
    Matrix operator+(const double& rhs);
    Matrix operator-(const Matrix& rhs);
    Matrix operator-(const double& rhs);
    Matrix operator-() const;
    Matrix operator*(const Matrix& rhs);
    Matrix operator*(const double& rhs);
    Matrix operator/(const Matrix& rhs);
    Matrix operator/(const double& rhs);

    QVector<double> toQVector();

    Matrix transpose() const;
    double det() const;
    unsigned rank() const;
    double detR(int size) const;
    std::pair<double,unsigned> min(unsigned inColumn=0) const;
    Matrix abs() const;

    void swapRows(unsigned row1, unsigned row2);
    void swapCols(unsigned col1, unsigned col2);

    unsigned nRows() const { return this->rows; }
    unsigned nCols() const { return this->cols; }
    ~Matrix() {}


    friend unsigned findPivot(Matrix A, unsigned column);
    friend Matrix gauss(const Matrix& A, const Matrix& b);
};



void swapRows(Matrix& ob1, unsigned row1, Matrix& ob2, unsigned row2);
void swapCols(Matrix& ob1, unsigned col1, Matrix& ob2, unsigned col2);

inline int factorial(int n) { return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n; }
unsigned findPivot(Matrix A, unsigned column);

Matrix gauss(const Matrix& A, const Matrix& b);
Matrix eye(unsigned size, QComboBox **comboInequal=0);
Matrix ones(unsigned rows, unsigned cols);
Matrix zeros(unsigned rows, unsigned cols);

void swapLabels(std::vector<int> labels, unsigned one, unsigned another);
//unsigned silnia(unsigned n);
#endif // MATRIX_H
