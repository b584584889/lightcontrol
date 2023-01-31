

#ifndef MATRIX_H
#define MATRIX_H


#include "LUDecomposition.h"

/**
 * Created by ikki on 2016/06/06.
 */

typedef struct
{
    double A[6][6];
} AArray;

class Matrix
{
private:

	/** Array for internal storage of elements.
	@serial internal array storage.
	*/
    //double**A;
	AArray A;// 2019.08.26 ikki change

	/** Row and column dimensions.
	@serial row dimension.
	@serial column dimension.
	*/
	int m, n;
    void release_data();
    void copy(const Matrix & mat);

    int initial;
    
public:
	Matrix(int m, int n);
    Matrix(int m, int n, double s);
    Matrix(double* A, int m);
    Matrix(double** A, int m, int n);
    ~Matrix();

    Matrix & operator = (const Matrix & mat);

    AArray * getArray();
    
//    double** getArray() const;
    double** getArrayCopy() const;

    int getRowDimension() const;
    int getColumnDimension() const;

    Matrix * identity();
    static Matrix * identity(int m, int n);
    static Matrix * identity(int m, int n, double value);

    Matrix * times(double s);
    Matrix * times(Matrix * B);

    Matrix * gemm(Matrix * B, Matrix * C, double alpha, double beta);
    Matrix * transpose();
    Matrix * solve(Matrix * B);

    double get(int i, int j) const;
    Matrix * getMatrix(int* r, int length, int j0, int j1) const;
    void set(int i, int j, double s);

	LUDecomposition * lu();

    static double inv(Matrix * A, Matrix * Ainv);
};

#endif // MATRIX_H
