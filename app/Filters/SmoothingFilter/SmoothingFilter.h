
/*
 * SmoothingFilter.h
 *
 *  Created on: 2018/06/09
 *      Author: ikki
 *  Reference:
 *  https://github.com/slek120/Unity-Touch-Smoothing-One-Euro-Filter/blob/master/OneEuroSmoothing.cs
 */


#ifndef SMOOTHING_FILTER_H
#define SMOOTHING_FILTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>
#include "../Filter.h"
#include "../../Math/Vector3.h"

class SmoothingFilter : public Filter
{
public:
	SmoothingFilter();
	~SmoothingFilter();

    Vector3 mFilteredPosition;
    Vector3 mFilteredVelocity;

    int initial(double * values);
    int update(double * values, double * results);

private:

    void start(double * values);

    void start(Vector3 point);
    void update(Vector3 point, Vector3 & result);

    //Lower value reduces jitter (fcmin or minimum cutoff frequency)
    //Keep above 0. Start at 1. Adjust until jitter is reasonable.
    //Recommended 1
    float jitterReduction;
    //Higher values reduce lag (beta or slope of velocity for cutoff frequency)
    //Keep above 0. Start at 0. Increase until no lag.
    //Recommended 1
    float lagReduction;

    Vector3 mCurrentPosition;
    Vector3 mCurrentVelocity;

    void doFilter(float dt);
    void Alpha(Vector3 cutoff, float dt, Vector3 & alpha);
    float clamp(float v, float lo, float hi);
    void kernel(Vector3 current, Vector3 previous, Vector3 alpha, Vector3 & output);

    bool approximatelyEqual(float a, float b, float epsilon);
    bool essentiallyEqual(float a, float b, float epsilon);
    bool definitelyGreaterThan(float a, float b, float epsilon);
    bool definitelyLessThan(float a, float b, float epsilon);
    float GetSquaredLength(const Vector3 inVect);
};


#endif
