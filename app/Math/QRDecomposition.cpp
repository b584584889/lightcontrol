
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "QRDecomposition.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* ------------------------
   Constructor
 * ------------------------ */

/** QR Decomposition, computed by Householder reflections.
@param A    Rectangular matrix
@return     Structure to access R and the Householder vectors and compute Q.
*/
QRDecomposition::QRDecomposition(Matrix * A)
{
	// Initialize.
	QR = A->getArrayCopy();
	m = A->getRowDimension();
	n = A->getColumnDimension();
	Rdiag = new double[n];

	// Main loop.
	for (int k = 0; k < n; k++)
	{
	   // Compute 2-norm of k-th column without under/overflow.
	   double nrm = 0;
	   for (int i = k; i < m; i++)
	   {
	      nrm = hypot(nrm, QR[i][k]);
	   }

	   if (nrm != 0.0)
	   {
	      // Form k-th Householder vector.
	      if (QR[k][k] < 0)
	      {
	         nrm = -nrm;
	      }
	      for (int i = k; i < m; i++)
	      {
	         QR[i][k] /= nrm;
	      }
	      QR[k][k] += 1.0;

	      // Apply transformation to remaining columns.
	      for (int j = k+1; j < n; j++)
	      {
	         double s = 0.0;
	         for (int i = k; i < m; i++)
	         {
	            s += QR[i][k]*QR[i][j];
	         }
	         s = -s/QR[k][k];
	         for (int i = k; i < m; i++)
	         {
	            QR[i][j] += s*QR[i][k];
	         }
	      }
	   }
	   Rdiag[k] = -nrm;
	}
}


QRDecomposition::~QRDecomposition()
{
    for (int i = 0; i < m; i++)
        delete [] QR[i];
    delete [] QR;
    delete [] Rdiag;
}

/** Least squares solution of A*X = B
@param B    A Matrix with as many rows as A and any number of columns.
@return     X that minimizes the two norm of Q*R*X-B.
@exception  IllegalArgumentException  Matrix row dimensions must agree.
@exception  RuntimeException  Matrix is rank deficient.
*/
Matrix * QRDecomposition::solve(Matrix * B)
{
	if (B->getRowDimension() != m)
	{
      //throw new IllegalArgumentException("Matrix row dimensions must agree.");
      return NULL;
	}
	if (!this->isFullRank())
	{
   		//throw new RuntimeException("Matrix is rank deficient.");
		return NULL;
	}

	// Copy right hand side
	int nx = B->getColumnDimension();
	double** X = B->getArrayCopy();

	// Compute Y = transpose(Q)*B
	for (int k = 0; k < n; k++)
	{
		for (int j = 0; j < nx; j++)
		{
			double s = 0.0;
			for (int i = k; i < m; i++)
			{
            	s += QR[i][k]*X[i][j];
			}
			s = -s/QR[k][k];
			for (int i = k; i < m; i++)
			{
            	X[i][j] += s*QR[i][k];
			}
		}
	}

	// Solve R*X = Y;
	for (int k = n-1; k >= 0; k--)
	{
		for (int j = 0; j < nx; j++)
		{
			X[k][j] /= Rdiag[k];
		}
		for (int i = 0; i < k; i++)
		{
			for (int j = 0; j < nx; j++)
			{
	        	X[i][j] -= X[k][j]*QR[i][k];
			}
		}
	}
    Matrix * nMat = new Matrix(X,n,nx);
    Matrix * retMat = nMat->getMatrix(0,n-1,0,nx-1);

    for (int i = 0; i < m; i++)
        delete [] X[i];
    delete [] X;
    delete nMat;

    return retMat;
	//return (new Matrix(X,n,nx))->getMatrix(0,n-1,0,nx-1);
}

/** Is the matrix full rank?
@return     true if R, and hence A, has full rank.
*/

bool QRDecomposition::isFullRank()
{
   for (int j = 0; j < n; j++)
   {
      if (Rdiag[j] == 0)
         return false;
   }
   return true;
}
