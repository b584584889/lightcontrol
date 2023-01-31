

#ifndef QRDecomposition_H
#define QRDecomposition_H

#include "Matrix.h"

/**
 * Created by ikki on 2016/06/06.
 */

class QRDecomposition
{
private:
	/** Array for internal storage of decomposition.
	@serial internal array storage.
	*/
	double** QR;
	
	/** Row and column dimensions.
	@serial column dimension.
	@serial row dimension.
	*/
	int m, n;
	
	/** Array for internal storage of diagonal of R.
	@serial diagonal of R.
	*/
	double* Rdiag;
   
public:    
	QRDecomposition(Matrix * A);
    ~QRDecomposition();
    
    Matrix * solve(Matrix * B);
    bool isFullRank();
};

#endif // QRDecomposition_H