

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"
#include "Kalman3D.h"
#include "../../common.h"

Kalman3D::Kalman3D()
{
}

Kalman3D::~Kalman3D()
{
	delete kalman;
	//delete kalman_s;
	//delete kalman_c;
	delete kalman_m;

	//delete [] results;
}

int Kalman3D::initial(double * values)
{
	init_kalmam(values);
	return 0;
}

void Kalman3D::init_kalmam(double * values)
{

	kalman = new Kalman(6, DIMENSION, 0);
	// init
	//kalman_s = new Matrix(6, 1); // state [x, y, z, dx, dy, dz]
	//kalman_c = new Matrix(6, 1); // corrected state [x, y, z, dx, dy, dz]
	kalman_m = new Matrix(DIMENSION, 1); // measurement x, y, z

	// transitions for x, y, z, dx, dy, dz
	double tr [6][6] = {
	        {1, 0, 0, 1, 0, 0},
	        {0, 1, 0, 0, 1, 0},
	        {0, 0, 1, 0, 0, 1},
	        {0, 0, 0, 1, 0, 0},
	        {0, 0, 0, 0, 1, 0},
	        {0, 0, 0, 0, 0, 1}
	};

	double **trarray = new double*[6];
	for(int i=0;i<6;i++)
	{
		trarray[i] = new double[6];
		for(int j=0;j<6;j++)
			trarray[i][j] = tr[i][j];
	}
	kalman->setTransition_matrix(new Matrix(trarray, 6, 6));

    // release
    for(int i=0;i<6;i++)
    {
        delete [] trarray[i];
    }
    delete [] trarray;


	// 1s somewhere?
	kalman->setError_cov_post(kalman->getError_cov_post()->identity());

//    for(int i=0;i<6;i++)
//    {
//        kalman_s->set(i, 0, 0.0);
//        kalman_c->set(i, 0, 0.0);
//    }

	// first position
	for (int i = 0; i < DIMENSION; i++)
	{
	    //kalman_m->set(i, 0, 0.0);
        kalman_m->set(i, 0, values[i]);
	}
    // run 100 times
    double results[3];
    for(int i=0;i<100;i++)
    {
        update(values, results);
    }

	//results = new double[DIMENSION];
}

int Kalman3D::update(double* values, double * results)
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
    for(int i=0;i<DIMENSION;i++)
    {
        kalman_m->set(i, 0, values[i]);
        last_values[i] = values[i];
    }

	//printf ("Kalman3D::update() Correct\n");

    kalman_c = kalman->Correct(kalman_m);
    if (kalman_c == NULL)
    {
        vilslog::printf ("Kalman3D::update() kalman_c == NULL \n");
        return DIMENSION;
    }

    for(int i=0;i<DIMENSION;i++)
    {
        results[i] = kalman_c->get(i, 0);
    }

 	//printf ("Kalman3D::update() results[%.02f, %.02f, %.02f]\n", results[0], results[1], results[2]);
	return 0;
}
