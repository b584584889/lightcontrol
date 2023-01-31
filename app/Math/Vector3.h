
/*
 * Vector3.h
 *
 *  Created on: 2018/07/30
 *      Author: ikki
 */



#ifndef VECTOE3_H
#define VECTOE3_H

class Vector3
{
public:
    float x;
    float y;
    float z;
    Vector3();
    Vector3(float ix, float iy, float iz);
    float GetSquaredLength();
};
#endif
