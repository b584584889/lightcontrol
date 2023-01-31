
#ifndef LUDecomposition_H
#define LUDecomposition_H

class Matrix;

/**
 * Created by ikki on 2016/06/06.
 */

class LUDecomposition
{
private:

  	/** Array for internal storage of decomposition.
  	@serial internal array storage.
  	*/
  	double** LU;
  	
  	/** Row and column dimensions, and pivot sign.
  	@serial column dimension.
  	@serial row dimension.
  	@serial pivot sign.
  	*/
  	int m, n, pivsign; 
  	
  	/** Internal storage of pivot vector.
  	@serial pivot vector.
  	*/
  	int* piv;


public:    
	LUDecomposition(Matrix * A);
    ~LUDecomposition(); 
    
    Matrix * solve(Matrix * B);
    bool isNonsingular();
};

#endif // LUDecomposition_H