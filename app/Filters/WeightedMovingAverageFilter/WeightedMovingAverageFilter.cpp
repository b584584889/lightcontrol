
/*
 * WeightedMovingFilter.cpp
 *
 *  Created on: 2018/07/30
 *      Author: ikki
 */


#include "WeightedMovingAverageFilter.h"

#include <math.h>

WeightedMovingAverageFilter::WeightedMovingAverageFilter(float w1, float w2, float w3)
{
    mWeight1 = w1;
    mWeight2 = w2;
    mWeight3 = w3;
}

WeightedMovingAverageFilter::~WeightedMovingAverageFilter()
{
}

int WeightedMovingAverageFilter::initial(double * values)
{
    mCurrentPosition.x = (float)values[0];
    mCurrentPosition.y = (float)values[1];
    mCurrentPosition.z = (float)values[2];
    mFilteredPosition.x = (float)values[0];
    mFilteredPosition.y = (float)values[1];
    mFilteredPosition.z = (float)values[2];

    mPrePosition1.x = mCurrentPosition.x;
    mPrePosition1.y = mCurrentPosition.y;
    mPrePosition1.z = mCurrentPosition.z;

    mPrePosition2.x = mCurrentPosition.x;
    mPrePosition2.y = mCurrentPosition.y;
    mPrePosition2.z = mCurrentPosition.z;


    return 0;
}

int WeightedMovingAverageFilter::update(double * values, double * results)
{
    mPrePosition1.x = mPrePosition2.x;
    mPrePosition1.y = mPrePosition2.y;
    mPrePosition1.z = mPrePosition2.z;

    mPrePosition2.x = mCurrentPosition.x;
    mPrePosition2.y = mCurrentPosition.y;
    mPrePosition2.z = mCurrentPosition.z;

    mCurrentPosition.x = results[0];
    mCurrentPosition.y = results[1];
    mCurrentPosition.z = results[2];

    doFilter();

    results[0] = mFilteredPosition.x;
    results[1] = mFilteredPosition.y;
    results[2] = mFilteredPosition.z;

    return 0;
}

int WeightedMovingAverageFilter::setArguments(float WeightMovingAverageValue1,
    float WeightMovingAverageValue2,
    float WeightMovingAverageValue3)
{
    mWeight1 = WeightMovingAverageValue1;
    mWeight2 = WeightMovingAverageValue2;
    mWeight3 = WeightMovingAverageValue3;

    return 0;
}

void WeightedMovingAverageFilter::doFilter()
{
    kernel(mPrePosition1, mPrePosition2, mCurrentPosition, mWeight1, mWeight2, mWeight3, mFilteredPosition);
}


void WeightedMovingAverageFilter::kernel(Vector3 pre1, Vector3 pre2, Vector3 pre3,
        float w1, float w2, float w3, Vector3 & output)
{
	output.x = w1 * pre1.x + w2 * pre2.x + w3 * pre3.x;
	output.y = w1 * pre1.y + w2 * pre2.y + w3 * pre3.y;
    output.z = w1 * pre1.z + w2 * pre2.z + w3 * pre3.z;
}
