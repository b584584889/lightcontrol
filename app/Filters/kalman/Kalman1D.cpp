

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "Kalman1D.h"

Kalman1D::Kalman1D()
{
	init_kalmam();
}

Kalman1D::~Kalman1D()
{
	delete kalman;
	//delete kalman_s;
	//delete kalman_c;
	delete kalman_m;

	//delete [] results;
}

void Kalman1D::init_kalmam()
{

	kalman = new Kalman(2, DIMENSION1D, 0);
	// init
	//kalman_s = new Matrix(2, 1); // state [x, dx]
	//kalman_c = new Matrix(2, 1); // corrected state [x, dx]
	kalman_m = new Matrix(DIMENSION1D, 1); // measurement x

	// transitions for x, y, z, dx, dy, dz
	double tr [2][2] = {
	        {1, 1},
	        {0, 1}
	};

	double **trarray = new double*[2];
	for(int i=0;i<2;i++)
	{
		trarray[i] = new double[2];
		for(int j=0;j<2;j++)
			trarray[i][j] = tr[i][j];
	}
	kalman->setTransition_matrix(new Matrix(trarray, 2, 2));
    
    // release
    for(int i=0;i<2;i++)
    {
        delete [] trarray[i];
    }
    delete [] trarray;
    
	// 1s somewhere?
	kalman->setError_cov_post(kalman->getError_cov_post()->identity());

//    for(int i=0;i<2;i++)
//    {
//        kalman_s->set(i, 0, 0.0);
//        kalman_c->set(i, 0, 0.0);
//    }

	// first position
	for (int i = 0; i < DIMENSION1D; i++)
	{
	    kalman_m->set(i, 0, 0.0);
	}

	//results = new double[DIMENSION];
}


int Kalman1D::update(double* values, double * results)
{
	//double results[DIMENSION];
	//printf ("Kalman3D::update() values[%.02f, %.02f, %.02f]\n", values[0], values[1], values[2]);

    //
    // apply Kalman Filter
    //
    // check state before
    kalman_s = kalman->Predict();

	//printf ("Kalman3D::update() update measurement\n");

    // update measurement
    for(int i=0;i<DIMENSION1D;i++)
    {
        kalman_m->set(i, 0, values[i]);
        last_values[i] = values[i];
    }

	//printf ("Kalman3D::update() Correct\n");
    
    kalman_c = kalman->Correct(kalman_m);
    for(int i=0;i<DIMENSION1D;i++)
    {
        results[i] = kalman_c->get(i, 0);
    }

 	//printf ("Kalman3D::update() results[%.02f, %.02f, %.02f]\n", results[0], results[1], results[2]);
	return DIMENSION1D;
}
