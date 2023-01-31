
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "QRDecomposition.h"
#include "Matrix.h"

/** Construct an m-by-n matrix of zeros.
@param m    Number of rows.
@param n    Number of colums.
*/
Matrix::Matrix(int m, int n)
{
    initial = 0;
	this->m = m;
	this->n = n;

//    A = new double*[m];
//    for(int i = 0; i < m; ++i)
//    {
//        A[i] = new double[n];
//    }

    initial = 1;
}

/** Construct an m-by-n constant matrix.
@param m    Number of rows.
@param n    Number of colums.
@param s    Fill the matrix with this scalar value.
*/
Matrix::Matrix(int m, int n, double s)
{
    initial = 0;
	this->m = m;
	this->n = n;

	//A = new double*[m];
	for(int i = 0; i < m; ++i)
	{
    	//A[i] = new double[n];
    	for(int j = 0; j < n; ++j)
    		A.A[i][j] = s;
	}

    initial = 2;
}

/** Construct a matrix from a one-dimensional packed array
@param vals One-dimensional array of doubles, packed by columns (ala Fortran).
@param m    Number of rows.
@exception  IllegalArgumentException Array length must be a multiple of m.
 */
Matrix::Matrix(double* A, int m)
{
    initial = 0;
	this->m = m;
    this->n = 1;

	//this->A = new double*[m];
	for(int i = 0; i < m; ++i)
	{
    	//this->A[i] = new double[n];
    	for(int j = 0; j < n; ++j)
    		this->A.A[i][j] = A[i + j * m];
	}
    initial = 3;
}

/** Construct a matrix quickly without checking arguments.
@param A    Two-dimensional array of doubles.
@param m    Number of rows.
@param n    Number of colums.
*/
Matrix::Matrix(double** A, int m, int n)
{
    initial = 0;
	this->m = m;
	this->n = n;

	//this->A = new double*[m];
	for(int i = 0; i < m; ++i)
	{
    	//this->A[i] = new double[n];
    	for(int j = 0; j < n; ++j)
    		this->A.A[i][j] = A[i][j];
	}
    initial = 4;
}

Matrix::~Matrix()
{
    release_data();
}

void Matrix::release_data()
{
    if (initial == 0)
    {
        return;
    }

//    for(int i = 0; i < this->m; ++i)
//    {
//        delete [] this->A[i];
//    }
//    delete [] A;
}

Matrix & Matrix::operator = (const Matrix & mat)
{
    if( this == &mat )
        return *this; // If two sides equal, do nothing.
    release_data(); // Delete data on left hand side
    this->copy(mat); // Copy right hand side to l.h.s.
    return *this;
}

/** Access the internal two-dimensional array.
@return     Pointer to the two-dimensional array of matrix elements.
*/
AArray * Matrix::getArray()
{
    return &this->A;
}

//double** Matrix::getArray() const
//{
//    return this->A;
//}

/** Copy the internal two-dimensional array.
@return     Two-dimensional array copy of matrix elements.
 */
double** Matrix::getArrayCopy() const
{
    double** C = new double*[this->m];
    for (int i = 0; i < this->m; i++)
    {
    	C[i] = new double[n];
        for (int j = 0; j < this->n; j++)
        {
            C[i][j] = this->A.A[i][j];
        }
    }
    return C;
}

/** Get row dimension.
@return     m, the number of rows.
 */
int Matrix::getRowDimension() const
{
    return this->m;
}

/** Get column dimension.
@return     n, the number of columns.
 */
int Matrix::getColumnDimension() const
{
    return this->n;
}

/** Generate identity matrix
@return     An m-by-n matrix with ones on the diagonal and zeros elsewhere.
*/
Matrix * Matrix::identity()
{
	//printf ("Matrix::identity() this->m = %d, this->n = %d\n", this->m, this->n);

	Matrix * locA = new Matrix(this->m, this->n);
	//double** X = locA->getArray();
    AArray * X = locA->getArray();
	for (int i = 0; i < this->m; i++)
	{
	    for (int j = 0; j < this->n; j++)
	    {
	        X->A[i][j] = (i == j ? 1.0 : 0.0);
	    }
	}
	return locA;
}

/** Generate identity matrix
@param m    Number of rows.
@param n    Number of colums.
@return     An m-by-n matrix with ones on the diagonal and zeros elsewhere.
*/
Matrix * Matrix::identity(int m, int n)
{
	Matrix * iA = new Matrix(m, n);
	//double** X = iA->getArray();
    AArray * X = iA->getArray();
	for (int i = 0; i < m; i++)
	{
	    for (int j = 0; j < n; j++)
	    {
	        X->A[i][j] = (i == j ? 1.0 : 0.0);
	    }
	}
	return iA;
}

/** Generate identity matrix
@param m    Number of rows.
@param n    Number of colums.
@return     An m-by-n matrix with value on the diagonal and zeros elsewhere.
*/
Matrix * Matrix::identity(int m, int n, double value)
{
	Matrix * iA = new Matrix(m, n);
	//double** X = iA->getArray();
    AArray * X = iA->getArray();
	for (int i = 0; i < m; i++)
	{
	    for (int j = 0; j < n; j++)
	    {
	        X->A[i][j] = (i == j ? value : 0.0);
	    }
	}
	return iA;
}

/** Multiply a matrix by a scalar, C = s*A
@param s    scalar
@return     s*A
*/
Matrix * Matrix::times(double s)
{
   Matrix * X = new Matrix(this->m, this->n);
   //double** C = X->getArray();
   AArray * C = X->getArray();
   for (int i = 0; i < this->m; i++)
   {
       for (int j = 0; j < this->n; j++)
       {
           C->A[i][j] = s * this->A.A[i][j];
       }
   }
   return X;
}

/** Linear algebraic matrix multiplication, A * B
@param B    another matrix
@return     Matrix product, A * B
@exception  IllegalArgumentException Matrix inner dimensions must agree.
 */
Matrix * Matrix::times(Matrix * B)
{
    Matrix * X = new Matrix(m, B->n);

    if ( B->m != this->n )
    	return X;

    //double** C = X->getArray();
    AArray * C = X->getArray();
    double* Bcolj = new double[this->n];
    for (int j = 0; j < B->n; j++)
    {
        for (int k = 0; k < this->n; k++)
        {
            Bcolj[k] = B->A.A[k][j];
        }
        for (int i = 0; i < this->m; i++)
        {
            double* Arowi = this->A.A[i];
            double s = 0;
            for (int k = 0; k < n; k++)
            {
                s += Arowi[k] * Bcolj[k];
            }
            C->A[i][j] = s;
        }
    }
    delete [] Bcolj;
    return X;
}


/***************************************************************************
 * Generalized linear algebraic matrix-matrix multiplication (of A);
 * <tt>C = alpha*A x B + beta*C</tt>.
 * <p>
 * Matrix shapes: <tt>A(m x n), B(n x p), C(m x p)</tt>.
 * <p>
 * After potential transpositions and shape conformance check;
 * <tt>C[i,j] = alpha*Sum(A[i,k] * B[k,j]) + beta*C[i,j], k=0..n-1</tt>.
 * <p>
 * Use:<br>
 * <code>C = A.gemm(B, null, 1, 1); // similar to A.times(B);</code><br>
 * <code>C = A.transpose().gemm(B.transpose(), C, 1, -1);</code>
 *
 * @param B the second source matrix.
 * @param C the matrix where results are to be stored. Set this parameter to
 *  <tt>null</tt> to indicate that a new result matrix shall be constructed.
 * @param alpha a scale factor.
 * @param beta a result scale factor.
 * @return C product matrix (for convenience only).
 *
 * @throws IllegalArgumentException if <tt>B.rows() != A.columns()</tt>.
 * @throws IllegalArgumentException if <tt>C.rows() != A.rows() || C.columns() != B.columns()</tt>.
 * @throws IllegalArgumentException if <tt>A == C || B == C</tt>.
 */
Matrix * Matrix::gemm(Matrix * B, Matrix * C, double alpha, double beta)
{

    // m = rows;
    // n = columns;
    int p = B->n; // columns of B

    // cannot change C :(
    Matrix* X = new Matrix(this->m, p); // X ~ C

    if (C == NULL)
    { // multiplication - rare, but necessary :(

        if (B->m != this->n)
        {
            return X;
        }

        for (int j = p; --j >= 0; )
        {
            for (int i = this->m; --i >= 0; )
            {
                double s = 0;
                for (int k = this->n; --k >= 0; )
                {
                    s += this->A.A[i][k] * B->A.A[k][j];
                }
                X->A.A[i][j] = alpha * s;
            }
        }
        return X;

    }
    else
    {

        if (B->m != this->n)
        {
            return X;
        }

        if (C->m != this->m || C->n != p)
        {
            return X;
        }

        for (int j = p; --j >= 0; )
        {
            for (int i = this->m; --i >= 0; )
            {
                double s = 0;
                for (int k = this->n; --k >= 0; )
                {
                    s += this->A.A[i][k] * B->A.A[k][j];
                }
                X->A.A[i][j] = alpha * s + beta * C->A.A[i][j];
            }
        }

    }
    return X;
}

/** Matrix transpose.
@return    A'
 */
Matrix * Matrix::transpose()
{
    Matrix* X = new Matrix(this->n, this->m);
    //double** C = X->getArray();
    AArray * C = X->getArray();
    for (int i = 0; i < this->m; i++)
    {
        for (int j = 0; j < this->n; j++)
        {
            C->A[j][i] = this->A.A[i][j];
        }
    }
    return X;
}

/** Solve A*X = B
@param B    right hand side
@return     solution if A is square, least squares solution otherwise
*/
Matrix * Matrix::solve(Matrix * B)
{
    Matrix * rSolve;
	if ( this->m == this->n )
	{
		LUDecomposition * lud = new LUDecomposition(this);
        rSolve = lud->solve(B);
        delete lud;
	}
	else
	{
		QRDecomposition * qrd = new QRDecomposition(this);
        rSolve = qrd->solve(B);
        delete qrd;
	}
    return rSolve;
}

/** Get a single element.
@param i    Row index.
@param j    Column index.
@return     A(i,j)
@exception  ArrayIndexOutOfBoundsException
*/
double Matrix::get(int i, int j) const
{
    return this->A.A[i][j];
}


/** Get a submatrix.
@param r    Array of row indices.
@param r    Length of row indices Array.
@param i0   Initial column index
@param i1   Final column index
@return     A(r(:),j0:j1)
@exception  ArrayIndexOutOfBoundsException Submatrix indices
 */
Matrix * Matrix::getMatrix(int* r, int length, int j0, int j1) const
{
    Matrix * X = new Matrix(length, j1 - j0 + 1);
    //double** B = X->getArray();
    AArray * B = X->getArray();
	for (int i = 0; i < length; i++)
	{
	    for (int j = j0; j <= j1; j++)
	    {
	        B->A[i][j - j0] = this->A.A[r[i]][j];
	    }
	}
    return X;
}

/** Set a single element.
@param i    Row index.
@param j    Column index.
@param s    A(i,j).
@exception  ArrayIndexOutOfBoundsException
 */
void Matrix::set(int i, int j, double s)
{
    this->A.A[i][j] = s;
}

/**LU Decomposition
* @return LUDecomposition
* @see jama.LUDecomposition
*/
LUDecomposition * Matrix::lu()
{
   return new LUDecomposition(this);
}

void Matrix::copy(const Matrix & mat)
{
    initial = 0;
    m = mat.getRowDimension();
    n = mat.getColumnDimension();

//    A = new double*[m];
//    for(int i = 0; i < m; ++i)
//    {
//        A[i] = new double[n];
//    }

    for (int i=0; i<m; i++)
    {
        for (int j=0; j<n; j++)
        {
            double value = mat.get(i, j);
            set(i, j, value);
            //printf ("Matrix::copy() value[%d][%d] [%f]\n", i, j, value);
        }
    }
    initial = 5;
}

double Matrix::inv(Matrix * A, Matrix * Ainv)
{
    // Compute inverse of matrix
    // Input
    // A - Matrix A (N by N)
    // Outputs
    // Ainv - Inverse of matrix A (N by N)
    int N = A->getRowDimension();
    if ( N != A->getColumnDimension() )
    {
        return -1;
    }
    // vilslog::printf ("Matrix::inv() N[%d]\n", N);

    *Ainv = *A; // Copy matrix to ensure Ainv is same size

    Matrix scale(N, 1); // Scale factor
    int index[N];

    //printf ("Matrix::inv() 2\n");

    // work array
    // Matrix b is initialized to the identity matrix
    Matrix * b = Matrix::identity(N,N);

    //printf ("Matrix::inv() 3\n");

    // Set scale factor, scale(i) = max( |a(i,j)| ), for each row
    for(int i=0; i<N; i++)
    {
        index[i] = i; // Initialize row index list
        double scalemax = 0.0;
        for(int j=0; j<N; j++)
            scalemax = (scalemax > fabs(A->get(i, j))) ? scalemax : fabs(A->get(i, j));
        scale.set(i, 0, scalemax);
        // vilslog::printf ("Matrix::inv() i[%d] scalemax[%f]\n", i, scalemax);
    }

    //printf ("Matrix::inv() 4\n");

    // Loop over rows k = 0, ..., (N-2)
    int signDet = 1;
    for(int k=0; k<N-1; k++)
    {
        // Select pivot row from max( |a(j,k)/s(j)| )
        double ratiomax = 0.0;
        int jPivot = k;
        for(int i=k; i<N; i++)
        {
            double ratio = fabs(A->get(index[i], k))/scale.get(index[i], 0);
            if( ratio > ratiomax )
            {
                jPivot = i;
                ratiomax = ratio;
            }
        }
        // vilslog::printf ("Matrix::inv() k[%d] jPivot[%d] ratiomax[%f]\n", k, jPivot, ratiomax);

        // Perform pivoting using row index list
        int indexJ = index[k];
        if( jPivot != k )
        { // Pivot
            indexJ = index[jPivot];
            index[jPivot] = index[k]; // Swap index jPivot and k
            index[k] = indexJ;
            signDet *= -1; // Flip sign of determinant
        }

        // vilslog::printf ("Matrix::inv() indexJ[%d] jPivot[%d] signDet[%d]\n", indexJ, jPivot, signDet);

        // Perform forward elimination
        for(int i=k+1; i<N; i++)
        {
            double coeff = A->get(index[i], k)/A->get(indexJ, k);

            // vilslog::printf ("\t k[%d] i[%d] coeff[%f]\n", k, i, coeff);

            for(int j=k+1; j<N; j++)
            {
                //A(index[i],j) -= coeff*A(indexJ,j);
                double avalue = A->get(indexJ, j);
                double value = A->get(index[i], j);
                double cvalue = value - coeff * avalue;
                A->set(index[i], j, cvalue);

                // vilslog::printf ("\t\t A->set(%d, %d, cvalue[%f]) avalue[%f] value[%f] \n",
                //         index[i], j, cvalue, avalue, value);
            }

            A->set(index[i], k, coeff);
            // vilslog::printf ("\t A->set(%d, %d, coeff[%f])\n", index[i], k, coeff);

            for(int j=0; j<N; j++ )
            {
                //b(index[i],j) -= A(index[i],k)*b(indexJ,j);
                double avalue = A->get(index[i], k);
                double bvalue = b->get(indexJ, j);
                double value = b->get(index[i], j);
                double abvalue = value - avalue * bvalue;
                b->set(index[i], j, abvalue);
                // vilslog::printf ("\t\t b->set(%d, %d, abvalue[%f]) avalue[%f] bvalue[%f] value[%f]\n", index[i], j, abvalue, avalue, bvalue, value);
            }
        }
    }

    //* Compute determinant as product of diagonal elements
    double determ = signDet; // Sign of determinant
    for(int i=0; i<N; i++)
        determ *= A->get(index[i], i);

    // vilslog::printf ("Matrix::inv() determ[%f] signDet[%f]\n", determ, signDet);

    //* Perform backsubstitution
    for(int k=0; k<N; k++)
    {
        //Ainv(N,k) = b(index[N],k)/A(index[N],N);
        double value = b->get(index[N-1], k)/A->get(index[N-1], N-1);
        Ainv->set(N-1, k, value);

        // vilslog::printf ("Matrix::inv() k[%d] value[%f]\n", k, value);

        //for(int i=N-1; i>=1; i--)
        for(int i=N-2; i>=0; i--)
        {
            double sum = b->get(index[i], k);

            // vilslog::printf ("Matrix::inv() k[%d] i[%d] sum[%f]\n", k, i, sum);

            //for(int j=i+1; j<=N; j++ )
            for(int j=i+1; j<N; j++ )
            {
                sum -= A->get(index[i], j) * Ainv->get(j, k);
            }
            // vilslog::printf ("Matrix::inv() k[%d] i[%d] sum[%f]\n", k, i, sum);

            double value = sum/A->get(index[i],i);
            Ainv->set(i,k, value);

            // vilslog::printf ("Matrix::inv() Ainv->set( i[%d] k[%d] value[%f])\n", i, k, value);
        }
    }

    // vilslog::printf ("Matrix::inv() end\n");

    delete b;
    return( determ );
}
