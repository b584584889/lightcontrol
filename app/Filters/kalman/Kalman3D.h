


#ifndef KALMAN3D_H
#define KALMAN3D_H

#include "Kalman.h"
#include "../Filter.h"

/**
 * Created by ikki on 2016/06/06.
 */

#define	DIMENSION	3  //X, Y, Z

class Kalman3D : public Filter
{
private:

	Kalman * kalman;
	Matrix * kalman_s;
	Matrix * kalman_c;
	Matrix * kalman_m;
	double last_values[DIMENSION];

	void init_kalmam(double * values);

    //double * results;


public:
    Kalman3D();
    ~Kalman3D();

    int initial(double * values);
    int update(double * values, double * results);
};

#endif // KALMAN3D_H
