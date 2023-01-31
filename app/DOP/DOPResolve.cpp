/*
 *  DOPResolve.cpp
 *  https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)
 *  Created on: 2017/10/25
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "DOPResolve.h"
#include "../Math/Matrix.h"


DOPResolve::DOPResolve()
{
    mAnchorLength = 0;
}

DOPResolve::~DOPResolve()
{

}

void DOPResolve::setData(double * tagcoordinate, int count, double ** coordinates, double * distances)
{
    mTagCoordinate = tagcoordinate;
    mAnchorLength = count;
	mAnchorCoordinate = coordinates;
	mDistance = distances;
}

//
// http://www.dtic.mil/dtic/tr/fulltext/u2/a497140.pdf
// return GDOP value
//
double DOPResolve::resolve()
{
    //printf ("resolve() mAnchorLength[%d]\n", mAnchorLength);

    double SV[MAX_ANCHOR_NUMBER][3];
    for(int i=0; i<mAnchorLength; i++)
    {
        SV[i][0] = mAnchorCoordinate[i][0] - mTagCoordinate[0];
        SV[i][1] = mAnchorCoordinate[i][1] - mTagCoordinate[1];
        SV[i][2] = mAnchorCoordinate[i][2] - mTagCoordinate[2];

        //printf ("resolve() SV[%d] (%d,%d,%d)\n", i, (int)SV[i][0], (int)SV[i][1], (int)SV[i][2]);
    }

    //----------------------------------------------------------------------
    //Calculate DOP for Specified Time
    //----------------------------------------------------------------------
    //Pseudo-Range and Directional Derivative Loop
    double Dx[MAX_ANCHOR_NUMBER], Dy[MAX_ANCHOR_NUMBER], Dz[MAX_ANCHOR_NUMBER], Dt[MAX_ANCHOR_NUMBER];

    for(int i=0; i<mAnchorLength; i++)
    {
        //Calculate directional derivatives for East, North, Up and Time
        Dx[i] = SV[i][0] / mDistance[i];
        Dy[i] = SV[i][1] / mDistance[i];
        Dz[i] = SV[i][2] / mDistance[i];
        Dt[i] = -1;
        //printf ("resolve() [%d] Dx[%f] Dy[%f] Dz[%f]\n", i, Dx[i], Dy[i], Dz[i]);
    }

    //printf ("resolve() 1\n");

    //
    // Produce the Covariance Matrix from the Directional Derivatives
    //
    double Alp[MAX_ANCHOR_NUMBER][4];
    for (int i=0; i<mAnchorLength; i++)
    {
        Alp[i][0] = Dx[i];
        Alp[i][1] = Dy[i];
        Alp[i][2] = Dz[i];
        Alp[i][3] = Dt[i];
    }

    //printf ("resolve() 2\n");

    //
    // Transpose Alp to get Brv
    //
    double Brv[4][MAX_ANCHOR_NUMBER];
    for (int j=0; j<4; j++)
    {
        for (int i=0; i<mAnchorLength; i++)
        {
            Brv[j][i] = Alp[i][j];
        }
    }

    //printf ("resolve() 3\n");

    //
    // Matrix multiplication of Brv and Alp
    //
    double Chl[4][4];
    for(int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            Chl[i][j] = 0;
            for (int k=0; k<mAnchorLength; k++)
            {
                Chl[i][j] = Chl[i][j] + (Brv[i][k] * Alp[k][j]);
            }
            //printf ("resolve() Chl[%d][%d] [%f]\n", i, j, Chl[i][j]);
        }
    }

    //printf ("resolve() 4\n");

    //
    // Inverse Chl
    //
    Matrix * m = new Matrix(4, 4, 0.0);
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            m->set(i, j, Chl[i][j]); //Assign Chl to matrix, m
        }
    }

    //printf ("resolve() 5\n");

    Matrix * minv = new Matrix(4, 4, 0.0);
    //double det =
    Matrix::inv(m, minv);

    // vilslog::printf ("resolve() det[%f]\n", det);

    double Dlt[4][4];
    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            Dlt[i][j] = minv->get(i, j); //Assign inversed matrix, minv, to Dlt
            // vilslog::printf ("resolve() Dlt[%d][%d] [%f]\n", i, j, Dlt[i][j]);
        }
    }

    //printf ("resolve() 6\n");

    //
    // Calculate DOP
    //
    // double xdop = sqrt(Dlt[0][0]);
    // double ydop = sqrt(Dlt[1][1]);
    // double vdop = sqrt(Dlt[2][2]);
    // double tdop = sqrt(Dlt[3][3]);
    // double hdop = sqrt(Dlt[0][0] + Dlt[1][1]);
    // double pdop = sqrt(Dlt[0][0] + Dlt[1][1] + Dlt[2][2]);
    double gdop = sqrt(Dlt[0][0] + Dlt[1][1] + Dlt[2][2] + Dlt[3][3]);

    // vilslog::printf ("resolve() gdop[%d] pdop[%d] hdop[%d] tdop[%d] vdop[%d] xdop[%d] ydop[%d]\n",
    //      (int)gdop, (int)pdop, (int)hdop, (int)tdop, (int)vdop, (int)xdop, (int)ydop);

    //
    // https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)
    //
    // DOP value
    // < 1	Ideal
    // 1-2  Excellent
    // 2-5	Good
    // 5-10	Moderate
    // 10-20	Fair
    // >20	Poor


    // Release matrix
    delete m;
    delete minv;

    return gdop;
}
