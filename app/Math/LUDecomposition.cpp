
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "Matrix.h"

#include "LUDecomposition.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/* ------------------------
   Constructor
 * ------------------------ */

/** LU Decomposition
@param  A   Rectangular matrix
@return     Structure to access L, U and piv.
*/
LUDecomposition::LUDecomposition(Matrix * A)
{
	// Use a "left-looking", dot-product, Crout/Doolittle algorithm.
	//printf ("LUDecomposition::LUDecomposition() \n");

	LU = A->getArrayCopy();
	//printf ("LUDecomposition::LUDecomposition() LU[0][0] = %f\n", LU[0][0]);

	m = A->getRowDimension();
	n = A->getColumnDimension();

	//printf ("LUDecomposition::LUDecomposition() m = %d, n = %d\n", m, n);

	piv = new int[m];
	for (int i = 0; i < m; i++)
	{
	   piv[i] = i;
	}
	pivsign = 1;
	double* LUrowi;
	double* LUcolj = new double[m];

	//printf ("LUDecomposition::LUDecomposition() start...\n");

	// Outer loop.
	for (int j = 0; j < n; j++)
	{
		// Make a copy of the j-th column to localize references.
		for (int i = 0; i < m; i++)
		{
			LUcolj[i] = LU[i][j];
		}

		// Apply previous transformations.
	   for (int i = 0; i < m; i++)
	   {
			LUrowi = LU[i];

			// Most of the time is spent in the following dot product.
			int kmax = MIN(i,j);
			double s = 0.0;
			for (int k = 0; k < kmax; k++)
			{
				s += LUrowi[k]*LUcolj[k];
			}

			LUrowi[j] = LUcolj[i] -= s;
		}

		// Find pivot and exchange if necessary.

		int p = j;
		for (int i = j+1; i < m; i++)
		{
			if (fabs(LUcolj[i]) > fabs(LUcolj[p]))
			{
				p = i;
			}
		}
		if (p != j)
		{
			for (int k = 0; k < n; k++)
			{
				double t = LU[p][k]; LU[p][k] = LU[j][k]; LU[j][k] = t;
			}
			int k = piv[p]; piv[p] = piv[j]; piv[j] = k;
			pivsign = -pivsign;
		}

		// Compute multipliers.

		if (j < m && LU[j][j] != 0.0)
		{
			for (int i = j+1; i < m; i++)
			{
				LU[i][j] /= LU[j][j];
			}
		}
	}

	delete [] LUcolj;
}


LUDecomposition::~LUDecomposition()
{
    for (int i = 0; i < m; i++)
        delete [] LU[i];
    delete [] LU;
    delete [] piv;
}

/** Solve A*X = B
@param  B   A Matrix with as many rows as A and any number of columns.
@return     X so that L*U*X = B(piv,:)
@exception  IllegalArgumentException Matrix row dimensions must agree.
@exception  RuntimeException  Matrix is singular.
*/
Matrix * LUDecomposition::solve(Matrix * B)
{
	if (B->getRowDimension() != m)
	{
		printf("Matrix row dimensions must agree.\n");
		return NULL;
	}
	if (!this->isNonsingular())
	{
		//printf("Matrix is singular.\n");
		return NULL;
	}

	//printf ("LUDecomposition::solve() m = %d, B->getRowDimension() = %d\n", m, B->getRowDimension());


   // Copy right hand side with pivoting
   int nx = B->getColumnDimension();
   Matrix * Xmat = B->getMatrix(piv, m, 0, nx-1);
   //double** X = Xmat->getArray();
   AArray * X = Xmat->getArray();

   // Solve L*Y = B(piv,:)
   for (int k = 0; k < n; k++)
   {
      for (int i = k+1; i < n; i++)
      {
         for (int j = 0; j < nx; j++)
         {
            X->A[i][j] -= X->A[k][j]*LU[i][k];
			//printf ("LUDecomposition::solve() [1] k=%d, X[%d][%d] = %f\n", k, i, j, X[i][j]);
         }
      }
   }
   // Solve U*X = Y;
   for (int k = n-1; k >= 0; k--)
   {
      for (int j = 0; j < nx; j++)
      {
         X->A[k][j] /= LU[k][k];
      }
      for (int i = 0; i < k; i++)
      {
         for (int j = 0; j < nx; j++)
         {
            X->A[i][j] -= X->A[k][j]*LU[i][k];
			//printf ("LUDecomposition::solve() [2]  k=%d, X[%d][%d] = %f\n", k, i, j, X[i][j]);
         }
      }
   }

//    test only
//   double** testX = Xmat->getArray();
//   for (int k = 0; k < n; k++)
//   {
//      for (int i = k+1; i < n; i++)
//      {
//         for (int j = 0; j < nx; j++)
//         {
//            printf ("LUDecomposition::solve() [3] k=%d, testX[%d][%d] = %f\n", k, i, j, testX[i][j]);
//         }
//      }
//   }



   return Xmat;
}

/** Is the matrix nonsingular?
@return     true if U, and hence A, is nonsingular.
*/
bool LUDecomposition::isNonsingular()
{
   for (int j = 0; j < n; j++)
   {
      if (LU[j][j] == 0)
         return false;
   }
   return true;
}
