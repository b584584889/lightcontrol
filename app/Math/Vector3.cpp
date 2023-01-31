
/*
 * Vector3.h
 *
 *  Created on: 2018/07/30
 *      Author: ikki
 */

#include "Vector3.h"

Vector3::Vector3()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Vector3::Vector3(float ix, float iy, float iz)
{
    x = ix;
    y = iy;
    z = iz;
}

float Vector3::GetSquaredLength()
{
    return ( x*x + y*y + z*z);
}
