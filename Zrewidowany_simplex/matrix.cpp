#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <cassert>

Matrix::Matrix(const Matrix &other) :
    rows(other.nRows()),
    cols(other.nCols())
{
    data.resize(rows);
    for(unsigned i=0; i < rows; ++i){
        data[i].resize(cols);
        for(unsigned j=0; j < cols; ++j){
            data[i][j] = other[i][j];
        }
    }
}

// zeros(nRows,nCols)
Matrix::Matrix(unsigned nRows, unsigned nCols, double defaultValue) :
    rows(nRows),
    cols(nCols)
{
    data.resize(rows);
    for(unsigned i=0; i < rows; ++i){
        data[i].resize(cols);
        for(unsigned j=0; j < cols; ++j)
            data[i][j]=defaultValue;
    }
}

Matrix::Matrix(unsigned nRows, unsigned nCols, QVarEdit ***editMatrix, bool isVerticalVector) :
    rows(nRows),
    cols(nCols)
{
    data.resize(rows); // alokacja pamięci
    for(unsigned i=0; i < rows; ++i){
        data[i].resize(cols);
        for(unsigned j=0; j < cols; ++j){
            if(isVerticalVector)
                data[i][j] = editMatrix[j][i]->text2double();
            else
                data[i][j] = editMatrix[i][j]->text2double();
        }
    }
}


// creates matrix from submatrix of other: [fromRow,toRow), [fromCol,toCol)
Matrix::Matrix(const Matrix &other, unsigned fromRow, unsigned toRow, unsigned fromCol, unsigned toCol) :
    rows(toRow-fromRow),
    cols(toCol-fromCol)
{
    if(rows < 0 || cols < 0)
        throw std::string("Podano ujemny zakres");
    if((rows > other.nRows()) || cols > other.nCols())
        throw std::string("To nie jest podmacierz!");

    data.resize(rows); // alokacja pamięci
    for(unsigned i=0; i < rows; ++i){
        data[i].resize(cols);
        for(unsigned j=0; j < cols; ++j){
            data[i][j] = other[fromRow+i][fromCol+j];
        }
    }
}


Matrix::Matrix(const std::vector<int> &v) :
    rows(static_cast<unsigned>(v.size())),
    cols(1)
{
    data.resize(rows);
    for(unsigned i=0; i < rows; ++i){
        data[i].push_back(double(v[i]));
    }
}


Matrix::Matrix(const QVector<double> &v) :
    rows(static_cast<unsigned>(v.size())),
    cols(1)
{
    data.resize(rows);
    for(unsigned i=0; i < rows; ++i){
        data[i].push_back(v[i]);
    }
}


// prints all elements of matrix to std::cout
void Matrix::print(std::string label)
{
    if(label!="")
        std::cout << label << std::endl;
    for(unsigned i=0; i < rows; ++i){
        for(unsigned j=0; j < cols; ++j)
            std::cout << std::setw(6) << data[i][j] << ' ';
        std::cout << std::endl;
    }
}

// return row-th row of matrix - able to modify
doubleVector &Matrix::operator[](unsigned row)
{
    return data[row];
}

// return row-th row of matrix - not able to modify
const doubleVector &Matrix::operator[](unsigned row) const
{
    return data[row];
}


// join two matrices AB = [A B]
Matrix Matrix::joinRight(Matrix &other)
{
    if(other.nRows() != rows)
        throw std::string("Nie zgadza się liczba wierszy");

    unsigned k = other.nCols();
    for(unsigned i=0; i < rows; ++i){
        data[i].resize(cols+k);
        for(unsigned j=0; j < k; ++j)
            data[i][j+cols] = other[i][j];
    }
    cols += k;

    return *this;
}

// join two matrices AB = [A;B]
Matrix Matrix::joinBottom(Matrix &other)
{
    if(other.nCols() != cols)
        throw std::string("Nie zgadza się liczba kolumn");

    unsigned w = other.nRows();
    for(unsigned i=0; i < w; ++i){
        data.push_back(other[i]);
    }
    rows += w;
    return *this;
}

Matrix Matrix::column(unsigned colIndex)
{
    Matrix result(rows,1);
    for(unsigned i=0; i < rows; ++i)
        result[i][0] = data[i][colIndex];
    return result;
}

void Matrix::removeColumn(unsigned colIndex)
{
    if(colIndex >= cols)
        throw (QString("Indeks przekracza zakres! (kolumna: ")+QString::number(colIndex)+QString(" >= ")+QString::number(cols)).toStdString();
    doubleVector::iterator it;
    for(unsigned i=0; i < rows; ++i){
        it = data[i].begin();
        data[i].erase(it+colIndex);
    }
    --cols;
}

void Matrix::removeRow(unsigned rowIndex)
{
    if(rowIndex >= rows)
        throw (QString("Indeks przekracza zakres! (wiersz: ")+QString::number(rowIndex)+QString(" >= ")+QString::number(rows)).toStdString();
    std::vector<doubleVector>::iterator it = data.begin();
    data.erase(it+rowIndex);
    --rows;
}

// Assignment Operator
Matrix& Matrix::operator=(const Matrix& rhs) {
    if (&rhs == this)
        return *this;

    unsigned new_rows = rhs.nRows();
    unsigned new_cols = rhs.nCols();

    data.resize(new_rows);
    for (unsigned i = 0; i < new_rows; i++) {
        data[i].resize(new_cols);
        for (unsigned j = 0; j < new_cols; j++) {
            data[i][j] = rhs[i][j];
        }
    }
    rows = new_rows;
    cols = new_cols;

    return *this;
}

// Addition of two matrices
Matrix Matrix::operator+(const Matrix& rhs) {
    if((rows != rhs.nRows()) && cols != rhs.nCols())
        throw std::string("Niezgodnosc wymiarow macierzy (this i RHS");
    Matrix result(rows, cols);

    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            result[i][j] = this->data[i][j] + rhs[i][j];
        }
    }

    return result;
}

Matrix Matrix::operator+(const double &rhs)
{
    Matrix result(rows, cols);

    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            result[i][j] = this->data[i][j] + rhs;
        }
    }

    return result;
}

// Subtraction of this matrix and another
Matrix Matrix::operator-(const Matrix& rhs) {
    if((rows != rhs.nRows()) && cols != rhs.nCols())
        throw std::string("Niezgodnosc wymiarow macierzy (this i RHS");

    Matrix result(rows, cols);

    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            result[i][j] = this->data[i][j] - rhs[i][j];
        }
    }

    return result;
}

Matrix Matrix::operator-(const double &rhs)
{
    Matrix result(rows, cols);

    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            result[i][j] = this->data[i][j] - rhs;
        }
    }

    return result;
}

// negates elements in matrix
Matrix Matrix::operator-() const {
    Matrix result(*this);
    for (unsigned i = 0; i < nRows(); i++) {
        for (unsigned j = 0; j < nCols(); j++) {
            if(result[i][j]!=0)
                result[i][j] *= -1;
            else
                result[i][j] = 0;
        }
    }

    return result;
}

// Left multiplication of this matrix and another
Matrix Matrix::operator*(const Matrix& rhs) {
    if(nCols() != rhs.nRows())
        throw std::string("Liczba kolumn (this) rozna od liczby wierszy w RHS");

    unsigned RHSRows = rhs.nRows();
    unsigned RHSCols = rhs.nCols();
    Matrix result(nRows(), rhs.nCols());

    for (unsigned i = 0; i < nRows(); i++) {
        for (unsigned j = 0; j < RHSCols; j++) {
            for (unsigned k = 0; k < RHSRows; k++) {
                result[i][j] += data[i][k] * rhs[k][j];
            }
        }
    }
    return result;
}

// Matrix/scalar multiplication
Matrix Matrix::operator*(const double& rhs) {
    Matrix result(rows, cols);

    for (unsigned i = 0; i < rows; i++) {
        for (unsigned j = 0; j < cols; j++) {
            result[i][j] = data[i][j] * rhs;
        }
    }
    return result;
}

// A ./ B
Matrix Matrix::operator/(const Matrix &rhs)
{
    if((cols!=rhs.nCols()) || (rows!=rhs.nRows()))
        throw std::string("Wymiary macierzy musza byc identyczne!");

    Matrix result(*this);

    for(unsigned i=0; i < rows; ++i){
        for(unsigned j=0; j < cols; ++j)
            result[i][j] /= rhs[i][j];
    }

    return result;
}

Matrix Matrix::operator/(const double &rhs)
{
    Matrix result(*this);

    for(unsigned i=0; i < rows; ++i){
        for(unsigned j=0; j < cols; ++j)
            result[i][j] /= rhs;
    }

    return result;
}

QVector<double> Matrix::toQVector()
{
    QVector<double> result;
    for(unsigned i=0; i < rows; ++i)
        result.push_back(data[i][0]);
    return result;
}


Matrix Matrix::abs() const
{
    Matrix result(*this);
    for(unsigned i=0; i < nRows(); ++i){
        for(unsigned j=0; j < nCols(); ++j){
            if(result[i][j] < 0){
                result[i][j] = -result[i][j];
            }
        }
    }
    return result;
}

// Calculate a transpose of this matrix
Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (unsigned i = 0; i < cols; i++) {
        for (unsigned j = 0; j < rows; j++) {
            result[i][j] = data[j][i];
        }
    }
    return result;
}

// Compute determinant - main method
double Matrix::det() const {
    assert(rows == cols);
    int size = rows;
    Matrix m_minor(size, size);
    double s = 1, det = 0;
    int i, j, m, n, c;
    if (size == 1)
        return data[0][0];
    else {
        det = 0;
        for (c = 0; c < size; c++) {
            m = 0;
            n = 0;
            for (i = 1; i < size; i++) {
                for (j = 0; j < size; j++) {
                    m_minor[i][j] = 0;
                    if (i != 0 && j != c) {
                        m_minor[m][n] = data[i][j];
                        if (n < (size - 2))
                            n++;
                        else {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            det = det + s * (data[0][c] * m_minor.detR(size - 1));
            s = -1 * s;
        }
    }
    return (det);
}

unsigned Matrix::rank() const
{
    unsigned rank=0;
    unsigned size = std::min(rows,cols);
    for(unsigned i=0; i < size; ++i){

    }
    return rank;
}

// Compute determinant - recursive method
double Matrix::detR(int size) const {
    Matrix m_minor(size, size);
    double s = 1, det = 0;
    int i, j, m, n, c;
    if (size == 1)
        return data[0][0];
    else {
        det = 0;
        for (c = 0; c < size; c++) {
            m = 0;
            n = 0;
            for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                    m_minor[i][j] = 0;
                    if (i != 0 && j != c) {
                        m_minor[m][n] = data[i][j];
                        if (n < (size - 2))
                            n++;
                        else {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            det = det + s * (data[0][c] * m_minor.detR(size - 1));
            s = -1 * s;
        }
    }
    return (det);
}

// Return index of the minimal value in the vector
std::pair<double,unsigned> Matrix::min(unsigned inColumn) const {
    double min_val = data[0][inColumn];
    unsigned min_ind = 0;
    for (unsigned  i = 0; i < rows; i++) {
        if (data[i][inColumn] < min_val) {
            min_val = data[i][inColumn];
            min_ind = i;
        }
    }
    return std::make_pair(min_val,min_ind);
}

// Swap two rows in a matrix
void Matrix::swapRows(unsigned row1, unsigned row2) {
    for (unsigned int i = 0; i < this->cols; i++)
        std::swap(this->data[row1][i], this->data[row2][i]);
}

// Swap two columns in a matrix
void Matrix::swapCols(unsigned col1, unsigned col2) {
    for (unsigned int i = 0; i < this->rows; i++)
        std::swap(this->data[i][col1], this->data[i][col2]);
}

// Swap two rows in two matrices
void swapRows(Matrix& ob1, unsigned row1, Matrix& ob2, unsigned row2) {
    assert(ob1.nCols() == ob2.nCols());
    for (unsigned int i = 0; i < ob1.nCols(); i++)
        std::swap(ob1[row1][i], ob2[row2][i]);
}

// Swap two columns in two matrices
void swapCols(Matrix& ob1, unsigned col1, Matrix& ob2, unsigned col2) {
    assert(ob1.nRows() == ob2.nRows());
    for (unsigned int i = 0; i < ob1.nRows(); i++)
        std::swap(ob1[i][col1], ob2[i][col2]);
}

// Return the row number of the largest sub-diagonal value of a given column
unsigned findPivot(Matrix A, unsigned col) {
    double maxval = fabs(A[col][col]);
    unsigned rowval = col;

    for (unsigned i = col + 1; i < A.nRows(); i++) {
        if (fabs(A[i][col]) > maxval) {
            maxval = fabs(A[i][col]);
            rowval = i;
        }
    }
    return rowval;
}

// Solve Ax=b using gaussian elimination with partial pivoting
Matrix gauss(const Matrix& A, const Matrix& b) {
    unsigned n = A.nRows();
    Matrix L(n, n);
    Matrix Utemp = eye(n);
    Matrix Atemp = A;

    for (unsigned j = 0; j < n; j++) {
        Utemp.swapRows(findPivot(Atemp, j), j);
        Atemp = Utemp * A;
        L = eye(n);
        for (unsigned i = j; i <= n - 1; i++) {
            assert(fabs(Atemp[j][j]) > (10 ^ (-15)));
            L[i][j] = -Atemp[i][j] / Atemp[j][j];
        }
        Utemp = L * Utemp;
        Atemp = Utemp * A;
    }

    for (unsigned j = 0; j < n - 1; j++) {
        for (unsigned i = j; i <= n - 1; i++) {
            if (fabs(Atemp[i][j]) < (5 * (10 ^ (-16)))) {
                Atemp[i][j] = 0;
            }
        }
    }

    Matrix U = Utemp * A;
    Matrix y = Utemp * b;
    Matrix x(n,1);
    x[n-1][0] = y[n-1][0] / U[n-1][n-1];

    double temp = 0;
    for (int i = n - 1; i >= 1; i--) {
        temp = y[i-1][0];
        for (int j = n; j > i; j--) {
            temp = temp - U[i-1][j-1] * x[j-1][0];
        }
        x[i-1][0] = temp / U[i-1][i-1];
    }
    return x;
}

// Return identity matrix
Matrix eye(unsigned size, QComboBox **comboInequal) {
    Matrix temp_eye(size, size);
    for (unsigned i = 0; i < size; i++) {
        temp_eye[i][i] = double(1);
    }
    if(comboInequal!=0){
        for(unsigned i=0; i < size; ++i){
            if(comboInequal[i]->currentIndex())
                temp_eye[i][i] = -1;
        }
    }
    return temp_eye;
}


Matrix ones(unsigned rows, unsigned cols)
{
    Matrix temp_ones(rows, cols);
    for(unsigned i=0; i < rows; ++i){
        for(unsigned j=0; j < cols; ++j)
            temp_ones[i][j] = double(1);
    }
    return temp_ones;
}


Matrix zeros(unsigned rows, unsigned cols)
{
    return Matrix(rows,cols);
}


void swapLabels(std::vector<int> labels, unsigned one, unsigned another)
{
    int tmp(labels[one]);
    labels[one] = labels[another];
    labels[another] = tmp;
}
