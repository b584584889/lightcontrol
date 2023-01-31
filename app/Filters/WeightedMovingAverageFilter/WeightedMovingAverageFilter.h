
/*
 * WeightedMovingFilter.h
 *
 *  Created on: 2018/07/30
 *      Author: ikki
  */


#ifndef WEIGHTED_MOVING_FILTER_H
#define WEIGHTED_MOVING_FILTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>
#include "../Filter.h"
#include "../../Math/Vector3.h"

class WeightedMovingAverageFilter : public Filter
{
public:
	WeightedMovingAverageFilter(float w1, float w2, float w3);
	~WeightedMovingAverageFilter();

    int initial(double * values);
    int update(double * values, double * results);
    int setArguments(float WeightMovingAverageValue1,
        float WeightMovingAverageValue2,
        float WeightMovingAverageValue3);
private:

    float mWeight1;
    float mWeight2;
    float mWeight3;

    Vector3 mPrePosition1;
    Vector3 mPrePosition2;
    Vector3 mCurrentPosition;
    Vector3 mFilteredPosition;

    void doFilter();
    void kernel(Vector3 pre1, Vector3 pre2, Vector3 pre3, float w1, float w2, float w3, Vector3 & output);
};


#endif
