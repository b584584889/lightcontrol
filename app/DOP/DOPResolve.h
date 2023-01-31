/*
 *  DOPResolve.h
 *
 *  Created on: 2017/10/25
 *      Author: ikki
 */

#ifndef DOPResolve_H
#define DOPResolve_H

#include "../config.h"


class DOPResolve
{
public:
	DOPResolve();
	~DOPResolve();
    void setData(double * tagcoordinate, int count, double ** coordinates, double * distances);
    double resolve();

private:
    double * mTagCoordinate;
    int mAnchorLength;
    double ** mAnchorCoordinate;
    double *  mDistance;
};

#endif
