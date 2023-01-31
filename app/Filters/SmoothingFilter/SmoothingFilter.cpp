
/*
 * SmoothingFilter.cpp
 *
 *  Created on: 2018/06/09
 *      Author: ikki
 */


#include "SmoothingFilter.h"

#include <math.h>

SmoothingFilter::SmoothingFilter()
{
    jitterReduction = 0.8f;
    lagReduction = 0.8f;
}

SmoothingFilter::~SmoothingFilter()
{
}

int SmoothingFilter::initial(double * values)
{
    Vector3 point;
    point.x = (float)values[0];
    point.y = (float)values[1];
    point.z = (float)values[2];

    start(point);

    return 0;
}

int SmoothingFilter::update(double * values, double * results)
{
    Vector3 point;
    Vector3 result;
    point.x = (float)values[0];
    point.y = (float)values[1];
    point.z = (float)values[2];

    update(point, result);

    results[0] = result.x;
    results[1] = result.y;
    results[2] = result.z;

    return 0;
}

void SmoothingFilter::start(Vector3 point)
{
    mCurrentPosition.x = point.x;
    mCurrentPosition.y = point.y;
    mCurrentPosition.z = point.z;
    mFilteredPosition.x = point.x;
    mFilteredPosition.y = point.y;
    mFilteredPosition.z = point.z;
}

void SmoothingFilter::update(Vector3 point, Vector3 & result)
{
    float deltaTime = 0.03;// 30ms

    mCurrentPosition.x = point.x;
    mCurrentPosition.y = point.y;
    mCurrentPosition.z = point.z;

    mCurrentVelocity.x = (mCurrentPosition.x - mFilteredPosition.x) / deltaTime;
    mCurrentVelocity.y = (mCurrentPosition.y - mFilteredPosition.y) / deltaTime;
    mCurrentVelocity.z = (mCurrentPosition.z - mFilteredPosition.z) / deltaTime;

    //printf("update() mCurrentVelocity(%f, %f)\n", mCurrentVelocity.x, mCurrentVelocity.y);

    doFilter(deltaTime);

    result.x = mFilteredPosition.x;
    result.y = mFilteredPosition.y;
    result.z = mFilteredPosition.z;
}

void SmoothingFilter::doFilter(float dt)
{
    Vector3 vdiff;
    vdiff.x = mCurrentVelocity.x - mFilteredVelocity.x;
    vdiff.y = mCurrentVelocity.y - mFilteredVelocity.y;
    vdiff.z = mCurrentVelocity.z - mFilteredVelocity.z;
    float slv = vdiff.GetSquaredLength();
    if (approximatelyEqual(slv, 0.0f, 0.00000001f))
    {
        //Skip if filtering is unnecessary
        mFilteredVelocity = mCurrentVelocity;
    }
    else
    {
        Vector3 one(1.0f, 1.0f, 1.0f);
        Vector3 alpha(1.0f, 1.0f, 1.0f);
        Alpha(one, dt, alpha);
        //Get a smooth velocity using exponential smoothing
        kernel(mCurrentVelocity, mFilteredVelocity, alpha, mFilteredVelocity);
    }

    Vector3 pdiff;
    pdiff.x = mCurrentPosition.x - mFilteredPosition.x;
    pdiff.y = mCurrentPosition.y - mFilteredPosition.y;
    pdiff.z = mCurrentPosition.z - mFilteredPosition.z;
    float slp = pdiff.GetSquaredLength();
    if (approximatelyEqual(slp, 0.0f, 0.00000001f))
    {
        //Skip if filtering is unnecessary
        mFilteredPosition = mCurrentPosition;
    }
    else
    {
        //Use velocity to get smoothing factor for position
        Vector3 cutoffFrequency;
        cutoffFrequency.x = jitterReduction + 0.01f * lagReduction * fabs(mFilteredVelocity.x);
        cutoffFrequency.y = jitterReduction + 0.01f * lagReduction * fabs(mFilteredVelocity.y);
        cutoffFrequency.z = jitterReduction + 0.01f * lagReduction * fabs(mFilteredVelocity.z);
        Vector3 alpha(1.0f, 1.0f, 1.0f);
        Alpha(cutoffFrequency, dt, alpha);
        //Get a smooth position using exponential smoothing with smoothing factor from velocity
        kernel(mCurrentPosition, mFilteredPosition, alpha, mFilteredPosition);
    }
}

void SmoothingFilter::Alpha(Vector3 cutoff, float dt, Vector3 & alpha)
{
	float tauX = 1 / (2 * M_PI * cutoff.x);
	float tauY = 1 / (2 * M_PI * cutoff.y);
    float tauZ = 1 / (2 * M_PI * cutoff.z);
	float alphaX = 1 / (1 + tauX / dt);
	float alphaY = 1 / (1 + tauY / dt);
    float alphaZ = 1 / (1 + tauZ / dt);
	alpha.x = clamp(alphaX, 0, 1);
	alpha.y = clamp(alphaY, 0, 1);
    alpha.z = clamp(alphaZ, 0, 1);
}

float SmoothingFilter::clamp(float v, float lo, float hi)
{
    float value = (v < lo) ? lo:v;
    value = (v > hi) ? hi:v;
    return value;
}

void SmoothingFilter::kernel(Vector3 current, Vector3 previous, Vector3 alpha, Vector3 & output)
{
	output.x = alpha.x * current.x + (1 - alpha.x) * previous.x;
	output.y = alpha.y * current.y + (1 - alpha.y) * previous.y;
    output.z = alpha.z * current.z + (1 - alpha.z) * previous.z;
}

bool SmoothingFilter::approximatelyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool SmoothingFilter::essentiallyEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool SmoothingFilter::definitelyGreaterThan(float a, float b, float epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool SmoothingFilter::definitelyLessThan(float a, float b, float epsilon)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}
