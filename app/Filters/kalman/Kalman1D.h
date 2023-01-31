


#ifndef KALMAN1D_H
#define KALMAN1D_H

#include "Kalman.h"

/**
 * Created by ikki on 2017/03/16.
 */

#define	DIMENSION1D	1  //X

class Kalman1D
{
private:

	Kalman * kalman;
	Matrix * kalman_s;
	Matrix * kalman_c;
	Matrix * kalman_m;
	double last_values[DIMENSION1D];

	void init_kalmam();

    //double * results;


public:
    Kalman1D();
    ~Kalman1D();

    int update(double * values, double * results);
};

#endif // KALMAN3D_H
