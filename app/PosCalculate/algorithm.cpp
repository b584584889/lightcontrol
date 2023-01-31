/*
 * AnchorRangingDataClass.cpp
 *
 *  Created on: 2018/09/11
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sstream>
#include <iostream>

#include "algorithm.h"

bool resolve_two_anchor(double * AnchorCoordinate0, double * AnchorCoordinate1,
                        double AnchorDistance0, double AnchorDistance1, double * results)
{
    double anDist = TwoPointDistance(AnchorCoordinate0, AnchorCoordinate1);

    double tag_x = 0.0, tag_y = 0.0, tag_z = 0.0;
    if ( AnchorDistance0 < anDist && AnchorDistance1 < anDist )
    {
        double valid_distance = AnchorDistance0 + AnchorDistance1;

        if (valid_distance == 0)
        {
            //printf ("resolve_two_anchor() valid_distance[%f]\n",valid_distance);
            return false;
        }

        double weight = AnchorDistance0 / valid_distance;
        tag_x = (AnchorCoordinate1[0] - AnchorCoordinate0[0]) * weight + AnchorCoordinate0[0];
        tag_y = (AnchorCoordinate1[1] - AnchorCoordinate0[1]) * weight + AnchorCoordinate0[1];
        tag_z = (AnchorCoordinate1[2] - AnchorCoordinate0[2]) * weight + AnchorCoordinate0[2];
    }
    else
    if ( AnchorDistance0 > anDist && AnchorDistance0 > AnchorDistance1)
    {
        // close to AnchorCoordinate1, far away from AnchorDistance0.
        double valid_distance = anDist;
        if (valid_distance == 0)
        {
            //printf ("resolve_two_anchor() valid_distance[%f]\n",valid_distance);
            return false;
        }
        double weight = AnchorDistance0 / valid_distance;
        tag_x = (AnchorCoordinate1[0] - AnchorCoordinate0[0]) * weight + AnchorCoordinate0[0];
        tag_y = (AnchorCoordinate1[1] - AnchorCoordinate0[1]) * weight + AnchorCoordinate0[1];
        tag_z = (AnchorCoordinate1[2] - AnchorCoordinate0[2]) * weight + AnchorCoordinate0[2];
    }
    else
    if ( AnchorDistance1 > anDist && AnchorDistance1 > AnchorDistance0)
    {
        // close to AnchorDistance0, far away from AnchorDistance1.
        double valid_distance = anDist;
        if (valid_distance == 0)
        {
            //printf ("resolve_two_anchor() valid_distance[%f]\n",valid_distance);
            return false;
        }
        double weight = AnchorDistance1 / valid_distance;
        tag_x = (AnchorCoordinate0[0] - AnchorCoordinate1[0]) * weight + AnchorCoordinate1[0];
        tag_y = (AnchorCoordinate0[1] - AnchorCoordinate1[1]) * weight + AnchorCoordinate1[1];
        tag_z = (AnchorCoordinate0[2] - AnchorCoordinate1[2]) * weight + AnchorCoordinate1[2];
    }
    else
    if (AnchorDistance0 == anDist)
    {
        tag_x = AnchorCoordinate1[0];
        tag_y = AnchorCoordinate1[1];
        tag_z = AnchorCoordinate1[2];

        //printf ("resolve_two_anchor() tag_x[%f] tag_y[%f] AnchorCoordinate1(%d,%d)\n",
         //       tag_x, tag_y, (int)AnchorCoordinate1[0], (int)AnchorCoordinate1[1]);
    }
    else
    if (AnchorDistance1 == anDist)
    {
        tag_x = AnchorCoordinate0[0];
        tag_y = AnchorCoordinate0[1];
        tag_z = AnchorCoordinate0[2];

        //printf ("resolve_two_anchor() tag_x[%f] tag_y[%f] AnchorCoordinate0(%d,%d)\n",
        //        tag_x, tag_y, (int)AnchorCoordinate0[0], (int)AnchorCoordinate0[1]);
    }
    //printf ("resolve_two_anchor() tag_x[%f] tag_y[%f]\n",tag_x, tag_y);

    results[0] = tag_x;
    results[1] = tag_y;
    results[2] = tag_z;

    // if (tag_x < -2000000.0f || tag_y < -2000000.0f )
    // {
    //     vilslog::printf ("do_two_anchor() tag_x[%f] tag_y[%f]\n",tag_x, tag_y);
    //     return false;
    // }

    return true;
}

bool resolve_tag_on_one_anchor(double * AnchorCoordinate0, double * AnchorCoordinate1,
                        double AnchorDistance0, double AnchorDistance1, double * tagcoord, double * results)
{
    //double tag_x = 0.0, tag_y = 0.0, tag_z = 0.0;
    double anDist = TwoPointDistance(AnchorCoordinate0, AnchorCoordinate1);
    double an0TagDist = TwoPointDistance(AnchorCoordinate0, tagcoord);
    double an1TagDist = TwoPointDistance(AnchorCoordinate1, tagcoord);


    // vilslog::printf ("resolve_tag_on_one_anchor() raw AnchorDistance0[%d] AnchorDistance1[%d]\n",
    //             (int)AnchorDistance0, (int)AnchorDistance1);


    if ( an0TagDist < anDist && an1TagDist < anDist )
    {
        // between two anchors
        //printf("PosCalculate::resolve_tag_on_one_anchor() between two anchors\n");
        if ( AnchorDistance0 > 0 )
        {
            AnchorDistance1 = anDist - AnchorDistance0;
        }
        else
        if ( AnchorDistance1 > 0 )
        {
            AnchorDistance0 = anDist - AnchorDistance1;
        }
    }
    else
    if ( an0TagDist > anDist && an0TagDist > an1TagDist )
    {
        // close to AnchorCoordinate1, far away from AnchorDistance0.
        //printf("PosCalculate::resolve_tag_on_one_anchor() an0TagDist > anDist\n");
        if ( AnchorDistance0 > 0 )
        {
            AnchorDistance1 = AnchorDistance0 - anDist;
        }
        else
        if ( AnchorDistance1 > 0 )
        {
            AnchorDistance0 = AnchorDistance1 + anDist;
        }
    }
    else
    if ( an1TagDist > anDist && an1TagDist > an0TagDist)
    {
        // close to AnchorDistance0, far away from AnchorDistance1.
        //printf("PosCalculate::resolve_tag_on_one_anchor() an1TagDist > anDist\n");
        if ( AnchorDistance0 > 0 )
        {
            AnchorDistance1 = AnchorDistance0 + anDist;
        }
        else
        if ( AnchorDistance1 > 0 )
        {
            AnchorDistance0 = AnchorDistance1 - anDist;
        }
    }

    // vilslog::printf ("resolve_tag_on_one_anchor() cal AnchorDistance0[%d] AnchorDistance1[%d]\n",
    //             (int)AnchorDistance0, (int)AnchorDistance1);

    return resolve_two_anchor(AnchorCoordinate0, AnchorCoordinate1, AnchorDistance0, AnchorDistance1, results);

}

// bool resolve_two_anchor(double * AnchorCoordinate0, double * AnchorCoordinate1,
//                         double AnchorDistance0, double AnchorDistance1, double * results)
// {
//     double valid_distance = AnchorDistance0 + AnchorDistance1;
//     double weight = AnchorDistance0 / valid_distance;
//     double tag_x = (AnchorCoordinate1[0] - AnchorCoordinate0[0]) * weight + AnchorCoordinate0[0];
//     double tag_y = (AnchorCoordinate1[1] - AnchorCoordinate0[1]) * weight + AnchorCoordinate0[1];
//     double tag_z = (AnchorCoordinate1[2] - AnchorCoordinate0[2]) * weight + AnchorCoordinate0[2];
//
//     //printf ("PosCalculate::do_two_anchor() tag_x[%f] tag_y[%f]\n",tag_x, tag_y);
//
//     results[0] = tag_x;
//     results[1] = tag_y;
//     results[2] = tag_z;
//
//     return true;
// }

bool resolve_three_anchor(double * P1, double * P2, double * P3,
                        double distance1, double distance2, double distance3, double * results, int bGetZUP)
{
    //
    // map 3 ponit to same plane
    //
    double R[16];
    double invR[16];
    findCoordTransformation(P1, P2, P3, R, invR);

    double nP1[4] = {P1[0], P1[1], P1[2], 1};
    double nP2[4] = {P2[0], P2[1], P2[2], 1};
    double nP3[4] = {P3[0], P3[1], P3[2], 1};

    double mP1[4];
    double mP2[4];
    double mP3[4];

    // rotate to new plane
    DotMatrix(4, invR, nP1, mP1);
    DotMatrix(4, invR, nP2, mP2);
    DotMatrix(4, invR, nP3, mP3);
    //printf("mP1:[%f, %f, %f]\n", mP1[0], mP1[1], mP1[2]);
    //printf("mP2:[%f, %f, %f]\n", mP2[0], mP2[1], mP2[2]);
    //printf("mP3:[%f, %f, %f]\n", mP3[0], mP3[1], mP3[2]);

    double mPt[3];
    double distance[3] = {distance1, distance2, distance3};
    bool bRet = Trilateration3D_simple(mP1, mP2, mP3, distance, mPt);
    if ( !bRet )
    {
        //printf("Trilateration3D_simple mPt[%f, %f, %f]\n", mPt[0], mPt[1], mPt[2]);
        return false;
    }
    //printf("Trilateration3D_simple mPt[%f, %f, %f]\n", mPt[0], mPt[1], mPt[2]);

    if (bGetZUP == 0)
    {
        if ( mPt[2] > 0 )
            mPt[2] = mPt[2] * -1;
    }


    // rotate to map user anchor coordination
    // mPta = numpy.array([mPt[0], mPt[1], mPt[2], 1])
    // retP = numpy.dot(R, mPta)
    // triPt = numpy.array([retP[0], retP[1], retP[2]])

    // rotate to map user anchor coordination
    double mPta[4] = {mPt[0], mPt[1], mPt[2], 1};
    double retP[4] = {0, 0, 0, 0};
    DotMatrix(4, R, mPta, retP);

    results[0] = retP[0];
    results[1] = retP[1];
    results[2] = retP[2];

    return true;
}

bool do_trilateration_3D(double * P1, double * P2, double * P3, int distance1, int distance2, int distance3, double * results, bool invZ)
{
    //
    // map 3 ponit to same plane
    //
    double R[16];
    double invR[16];
    findCoordTransformation(P1, P2, P3, R, invR);

    double nP1[4] = {P1[0], P1[1], P1[2], 1};
    double nP2[4] = {P2[0], P2[1], P2[2], 1};
    double nP3[4] = {P3[0], P3[1], P3[2], 1};

    double mP1[4];
    double mP2[4];
    double mP3[4];

    // rotate to new plane
    DotMatrix(4, invR, nP1, mP1);
    DotMatrix(4, invR, nP2, mP2);
    DotMatrix(4, invR, nP3, mP3);
    //printf("mP1:[%f, %f, %f]\n", mP1[0], mP1[1], mP1[2]);
    //printf("mP2:[%f, %f, %f]\n", mP2[0], mP2[1], mP2[2]);
    //printf("mP3:[%f, %f, %f]\n", mP3[0], mP3[1], mP3[2]);

    double mPt[3];
    double distance[3] = {(double)distance1, (double)distance2, (double)distance3};
    bool bRet = Trilateration3D_simple(mP1, mP2, mP3, distance, mPt);
    if ( !bRet )
    {
        //printf("Trilateration3D_simple mPt[%f, %f, %f]\n", mPt[0], mPt[1], mPt[2]);
        return false;
    }

    //
    // if origin is heighter then 150cm set Z pos to negative
    //
    // if (P1[2] >= ZPOS_THRESHOLD)
    // {
    //     mPt[2] = -1 * mPt[2];
    // }
    if (invZ)
    {
        mPt[2] = -1 * mPt[2];
    }

    // rotate to map user anchor coordination
    double mPta[4] = {mPt[0], mPt[1], mPt[2], 1};
    double retP[4] = {0, 0, 0, 0};
    DotMatrix(4, R, mPta, retP);

    results[0] = retP[0];
    results[1] = retP[1];
    results[2] = retP[2];

    return true;
}

bool Trilateration3D_simple(double *P1, double *P2, double *P3, double *distance, double *results)
{
    double d12;
	double t1;
	double t2;
	double t;
    double temp = 0;
	double temp1 = 0;
    double xval;
	double yval;
	double ival;
	double jval;
	double zval_pos;
    //double zval_neg;

    for (int i=0;i<3;i++)
    {
        t1   = P2[i];
        t2   = P1[i];
        t    = t1 - t2;
        temp += (t*t);
    }
    d12 = sqrt(temp);

	ival = P3[0] - P1[0];
	jval = P3[1] - P1[1];

    if (d12 == 0.0 || jval == 0.0)
    {
        return false;
    }

	xval = (pow(distance[0], 2) - pow(distance[1], 2) + pow(d12, 2))/(2*d12);
	yval = ((pow(distance[0], 2) - pow(distance[2], 2) + pow(ival, 2) + pow(jval, 2))/(2*jval)) - ((ival/jval)*xval);

    //xval = (pow(distance[0], 2) - pow(distance[1], 2) - pow(d12, 2))/(2*d12);
	//yval = ((pow(distance[0], 2) - pow(distance[2], 2) - pow(ival, 2) + pow(jval, 2))/(2*jval)) - ((ival/jval)*xval);

	temp1 = pow(distance[0], 2) - pow(xval, 2) - pow(yval, 2);

    if ( temp1 <= 0 )
	{
	    //printf ("Trilateration3D_simple() temp1=%.02f, xval=%.02f, yval=%.02f\n", temp1, xval, yval);
        zval_pos = 0;

        if ( xval > distance[0] && xval > distance[1] && xval > (distance[2] -  P3[0]) )
        {
            //xval = (pow(distance[0], 2) - pow(distance[1], 2) - pow(d12, 2))/(2*d12);
         	//yval = ((pow(distance[0], 2) - pow(distance[2], 2) - pow(ival, 2) + pow(jval, 2))/(2*jval)) - ((ival/jval)*xval);
            //temp1 = pow(distance[0], 2) - pow(xval, 2) - pow(yval, 2);
            //printf ("Trilateration3D_simple() xval not valid value !!\n");
            return false;
        }

        if ( fabs(yval) > distance[0] && fabs(yval) > distance[1] && fabs(yval) > (distance[2] - P3[1]) )
        {
            //xval = (pow(distance[0], 2) - pow(distance[1], 2) - pow(d12, 2))/(2*d12);
         	//yval = ((pow(distance[0], 2) - pow(distance[2], 2) - pow(ival, 2) + pow(jval, 2))/(2*jval)) - ((ival/jval)*xval);
            //temp1 = pow(distance[0], 2) - pow(xval, 2) - pow(yval, 2);
            //printf ("Trilateration3D_simple() yval not valid value !!\n");
            return false;
        }


        //else
        //{
        //    return false;
        //}

/*
        //
        // interpolation
        // z = 0
        double x_diff = P3[0] - xval;
        double y_diff = P3[1] - yval;
        double distancetmp =  sqrt(pow(x_diff, 2) + pow(y_diff, 2));
        double rate = fabs((distance[2] - distancetmp) / distance[2]);

        //printf ("do_trilateration_Orig() rate=%.02f\n", rate);

        double n_xval = xval + x_diff * rate;
        double n_yval = yval + y_diff * rate;

        xval = (xval + n_xval) / 2;
        yval = (yval + n_yval) / 2;

        //printf ("do_trilateration_Orig() P3[0]=%.02f, P3[1]=%.02f, x_diff=%.02f, y_diff=%.02f\n", P3[0], P3[1], x_diff, y_diff);
        vilslog::printf ("do_trilateration_Orig() x_diff=%.02f, y_diff=%.02f\n", x_diff, y_diff);
        vilslog::printf ("do_trilateration_Orig() distance[2]=%.02f, distancetmp=%.02f\n", distance[2], distancetmp);
        vilslog::printf ("do_trilateration_Orig() rate=%.02f, n_xval=%.02f, n_yval=%.02f\n", rate, n_xval, n_yval);
        vilslog::printf ("do_trilateration_Orig() xval=%.02f, yval=%.02f\n", xval, yval);
*/

        //zval_neg = -1 * zval_pos;
        //return false;
	}
	else
	{
	    zval_pos = sqrt(temp1);
	    //zval_neg = -1 * zval_pos;
	    //printf ("do_trilateration_Orig() temp1=%.02f, xval=%.02f, yval=%.02f, zval_pos=%.02f\n", temp1, xval, yval, zval_pos);
	}
	results[0] = xval;
	results[1] = yval;
    results[2] = zval_pos;

    return true;
}
//
// https://en.wikipedia.org/wiki/Trilateration
//
bool do_trilateration_Orig(double * P1, double * P2, double * P3,
                        double distance1, double distance2, double distance3, double * results)
{
	//DECLARATION OF VARIABLE
	double d12;
	double t1;
	double t2;
	double t;
	double temp = 0;
	double temp1 = 0;
	double xval;
	double yval;
	double ival;
	double jval;
	double zval_pos;

    for (int i=0;i<3;i++)
    {
        t1   = P2[i];
        t2   = P1[i];
        t    = t1 - t2;
        temp += (t*t);
    }
    d12 = sqrt(temp);

	ival = P3[0] - P1[0];
	jval = P3[1] - P1[1];

	xval = (pow(distance1, 2) - pow(distance2, 2) + pow(d12, 2))/(2*d12);

	yval = ((pow(distance1, 2) - pow(distance3, 2) + pow(ival, 2) + pow(jval, 2))/(2*jval)) - ((ival/jval)*xval);

	temp1 = pow(distance1, 2) - pow(xval, 2) - pow(yval, 2);
	//printf ("PosCalculate::do_trilateration_Orig() temp1 = %f\n", temp1);

	if ( temp1 <= 0 )
	{
	    //printf ("do_trilateration_Orig() temp1=%.02f, xval=%.02f, yval=%.02f\n", temp1, xval, yval);
		return false;
	}
	else
	{
	    zval_pos = sqrt(temp1);
	    //zval_neg = -1 * zval_pos;
	    //printf ("do_trilateration_Orig() temp1=%.02f, xval=%.02f, yval=%.02f, zval_pos=%.02f\n", temp1, xval, yval, zval_pos);
	}
	results[0] = xval;
	results[1] = yval;
	results[2] = zval_pos;

	return true;
}

int resolveTagMapLayer(RangingResultStruct * validRangingData)
{
    int maxMapLayer = 0;
    int maxMapLayerCount = 0;
    int MapLayerSlot[500];
    for(int i=0;i<500;i++)
        MapLayerSlot[i] = 0;

    //printf ("PosCalculate::resolveTagFloor() validRangingData->validCount[%d]\n", validRangingData->validCount);

    for(int i=0;i<validRangingData->validCount && i<MAX_RANGING_RESULT;i++)
    {
        int MapLayer = validRangingData->MapLayer[i];

        // printf ("resolveTagFloor() i[%d] MapLayer[%d]\n", i, MapLayer);

        if (MapLayer >= 500)
            continue;
        MapLayerSlot[MapLayer]++;
    }

    for(int i=1;i<500;i++)
    {
        if (maxMapLayerCount < MapLayerSlot[i])
        {
            maxMapLayerCount = MapLayerSlot[i];
            maxMapLayer = i;
        }
    }
    // printf ("resolveTagFloor() maxMapLayer[%d] maxMapLayerCount[%d]\n", maxMapLayer, maxMapLayerCount);

    return maxMapLayer;
}

int resolve_one_anchor(double mv_direction, double rangingvlaue, double anTagDistance,
                        int moving_dir_X, int moving_dir_Y,
                        double * AnchorCoordinate, double * TagCoordinate,
                        int fixAxis, int fixCoord, int maxPos, int minPos, double * result)
{
    int found = 0;
    double tolerance = 200; // 200 cm

    if ( mv_direction >= 0)
    {
        // moving same direction with Anchor
        if ( (rangingvlaue < fabs(anTagDistance + tolerance)) || mv_direction == 0)
        {
            // approaching to Anchor, valid value
            //printf("\t mv_direction > 0 approaching to Anchor, valid value\n");
            if (fixAxis == 0)
            {
                // fix X AXIS
                int NewY = TagCoordinate[1];
                //if (moving_dir_Y < 0)
                if (moving_dir_Y >= 0)
                {
                    NewY = (int)AnchorCoordinate[1] - rangingvlaue;
                }
                else
                {
                    NewY = (int)AnchorCoordinate[1] + rangingvlaue;
                }
                if ( NewY > (int)maxPos )
                    NewY = (int)maxPos;
                if ( NewY < (int)minPos )
                    NewY = (int)minPos;
                result[0] = fixCoord;
                result[1] = NewY;
                result[2] = TagCoordinate[2];
            }
            else
            {
                // fix Y AXIS
                int NewX = TagCoordinate[0];

                if (moving_dir_X >= 0)
                //if (moving_dir_X < 0)
                {
                    NewX = (int)AnchorCoordinate[0] - rangingvlaue;
                    //printf("resolve_one_anchor() moving_dir_X >= 0 NewX[%d]\n", NewX);
                }
                else
                {
                    NewX = (int)AnchorCoordinate[0] + rangingvlaue;
                    //printf("resolve_one_anchor() moving_dir_X < 0  NewX[%d]\n", NewX);
                }
                if ( NewX > (int)maxPos )
                    NewX = (int)maxPos;
                if ( NewX < (int)minPos )
                    NewX = (int)minPos;
                result[0] = NewX;
                result[1] = fixCoord;
                result[2] = TagCoordinate[2];
            }
            found = 1;
        }
        else
        {
            //printf("resolve_one_anchor() mv_direction[%d] rangingvlaue[%d] < (%d + %d)\n",
            //            (int)mv_direction, (int)rangingvlaue, (int)anTagDistance, (int)tolerance);


        }
    }
    else
    {
        // moving different direction with P1
        if ( rangingvlaue < fabs(anTagDistance - tolerance) )
        {
            // away from P1, not valid value
            //printf("\t mv_direction > 0  away from P1, not valid value\n");
        }
        else
        {
            //printf("\t mv_dir1 <= 0 away from P1, valid value\n");
            if (fixAxis == 0)
            {
                // fix X AXIS
                int NewY = TagCoordinate[1];
                if (moving_dir_Y >= 0)
                //if (moving_dir_Y < 0)
                {
                    NewY = (int)AnchorCoordinate[1] + rangingvlaue;
                }
                else
                {
                    NewY = (int)AnchorCoordinate[1] - rangingvlaue;
                }
                if ( NewY > (int)maxPos )
                    NewY = (int)maxPos;
                if ( NewY < (int)minPos )
                    NewY = (int)minPos;
                result[0] = fixCoord;
                result[1] = NewY;
                result[2] = TagCoordinate[2];
            }
            else
            {
                // fix Y AXIS
                int NewX = TagCoordinate[0];
                if (moving_dir_X >= 0)
                //if (moving_dir_X < 0)
                {
                    NewX = (int)AnchorCoordinate[0] + rangingvlaue;
                }
                else
                {
                    NewX = (int)AnchorCoordinate[0] - rangingvlaue;
                }
                if ( NewX > (int)maxPos )
                    NewX = (int)maxPos;
                if ( NewX < (int)minPos )
                    NewX = (int)minPos;

                result[0] = NewX;
                result[1] = fixCoord;
                result[2] = TagCoordinate[2];
            }
            found = 1;
            //printf("resolve_one_anchor() mv_direction[%d] rangingvlaue[%d] > (%d - %d)\n",
            //        (int)mv_direction, (int)rangingvlaue, (int)anTagDistance, (int)tolerance);
        }
    }

    // if ( found == 1 )
    // {
    //     double anDist = TwoPointDistance(TagCoordinate, result);
    //     if ( anDist >= 800 )
    //     {
    //         vilslog::printf("\n resolve_one_anchor() anDist[%d]\n\n", (int)anDist);
    //         found = 0;
    //     }
    // }


    return found;
}

bool resolve_one_anchor(double mv_direction, double rangingvlaue, double anTagDistance,
                        double * Momentum, double * AnchorCoordinate, double * TagCoordinate, double * result)
{
    bool found = false;
    double tolerance = 0; // 200 cm

    if ( (int)mv_direction >= 0)
    {
        //接近 Anchor
        // moving same direction with Anchor
        if ( (rangingvlaue < fabs(anTagDistance + tolerance)) || mv_direction == 0)
        {
            // approaching to Anchor, valid value
            //printf("resolve_one_anchor() approaching to Anchor, valid value. mv_direction[%d]\n", (int)mv_direction);
            //
            // vilslog::printf("\t AnchorCoordinate(%d, %d, %d) TagCoordinate(%d, %d, %d) anTagDistance[%d] rangingvlaue[%d]\n",
            //            (int)AnchorCoordinate[0], (int)AnchorCoordinate[1], (int)AnchorCoordinate[2],
            //            (int)TagCoordinate[0], (int)TagCoordinate[1], (int)TagCoordinate[2],
            //            (int)anTagDistance, (int)rangingvlaue);

           found = resolve_two_anchor(AnchorCoordinate, TagCoordinate, rangingvlaue, (anTagDistance - rangingvlaue), result);

           //printf("\t found[%d] result(%d, %d, %d)\n", found, (int)result[0], (int)result[1], (int)result[2]);
        }
        else
        {
            //printf("resolve_one_anchor() approaching to Anchor, mv_direction[%d] rangingvlaue[%d] < (%d + %d), not valid value.\n",
            //           (int)mv_direction, (int)rangingvlaue, (int)anTagDistance, (int)tolerance);
        }
    }
    else
    {
        //遠離 Anchor
        // moving different direction with Anchor
        if ( rangingvlaue < fabs(anTagDistance - tolerance) )
        {
            // away from P1, not valid value
            //printf("resolve_one_anchor() away from Anchor, mv_direction[%d] rangingvlaue[%d] < (%d + %d), not valid value.\n",
             //          (int)mv_direction, (int)rangingvlaue, (int)anTagDistance, (int)tolerance);
        }
        else
        {
            // vilslog::printf("resolve_one_anchor() away from Anchor, valid value. mv_direction[%d]\n", (int)mv_direction);
            //
            // vilslog::printf("\t AnchorCoordinate(%d, %d, %d) TagCoordinate(%d, %d, %d) anTagDistance[%d] rangingvlaue[%d]\n",
            //            (int)AnchorCoordinate[0], (int)AnchorCoordinate[1], (int)AnchorCoordinate[2],
            //            (int)TagCoordinate[0], (int)TagCoordinate[1], (int)TagCoordinate[2],
            //            (int)anTagDistance, (int)rangingvlaue);

            found = resolve_two_anchor(AnchorCoordinate, TagCoordinate, rangingvlaue, anTagDistance, result);

            //printf("\t found[%d] result(%d, %d, %d)\n", found, (int)result[0], (int)result[1], (int)result[2]);
        }
    }

    return found;
}

bool fix_exceed_endpoint(double * endpoint_1, double * endpoint_2, double * point)
{
    bool exceed = false;

    double Dist1 = TwoPointDistance(point, endpoint_1);
    double Dist2 = TwoPointDistance(point, endpoint_2);
    double DistParam = TwoPointDistance(endpoint_1, endpoint_2);
    if ( (Dist1 + Dist2) > DistParam )
    {
        // vilslog::printf("PosCalculate::do_check_Section_ONE_on_two_anchor() 2 (Dist1[%d] + Dist2)[%d] > DistParam[%d] \n",
        //     (int)Dist1, (int)Dist2, (int)DistParam);
        //
        // maybe not in this section
        //
        bool onTheLine = check_node_on_the_line(endpoint_1, endpoint_2, point);

        if (!onTheLine)
        {
            return false;
        }

        if ( Dist1 < Dist2 )
        {
            // map poistion to end point

            // vilslog::printf("fix_exceed_endpoint() point[%d,%d,%d] -> endpoint_1[%d,%d,%d] \n",
            //     (int)point[0], (int)point[1], (int)point[2], (int)endpoint_1[0], (int)endpoint_1[1], (int)endpoint_1[2]);

            point[0] = endpoint_1[0];
            point[1] = endpoint_1[1];
            point[2] = endpoint_1[2];
        }
        else
        {

            // vilslog::printf("fix_exceed_endpoint() point[%d,%d,%d] -> endpoint_2[%d,%d,%d] \n",
            //     (int)point[0], (int)point[1], (int)point[2], (int)endpoint_2[0], (int)endpoint_2[1], (int)endpoint_2[2]);

            point[0] = endpoint_2[0];
            point[1] = endpoint_2[1];
            point[2] = endpoint_2[2];
        }
        exceed = true;
    }

    return exceed;
}

//
// https://math.stackexchange.com/questions/274712/calculate-on-which-side-of-a-straight-line-is-a-given-point-located
//
bool check_node_on_the_line(double * endpoint_1, double * endpoint_2, double * point)
{
    bool onTheLine = true;

    double d = (point[0] - endpoint_1[0])*(endpoint_2[1] - endpoint_1[1]) - (point[1] - endpoint_1[1])*(endpoint_2[0] - endpoint_1[0]);

    if ( d < 0.0 || d > 0.0 )
    {
        return false;
    }

    return onTheLine;
}

//
// https://robotics.stackexchange.com/questions/12782/how-rotate-a-point-around-an-arbitrary-line-in-3d
//
bool rotate_AoA_normal_vector(  int posX, int posY, int posZ,
                                int norX, int norY, int norZ,
                                int roll,
                                double * point, double * result_roll)
{
    bool done = true;
    double normal_vector[3] = {0,0,0};
    double T[16] = {0};
    double Rx[16] = {0};
    double Ry[16] = {0};
    double Rz[16] = {0};
    double invT[16] = {0};
    double invRx[16] = {0};
    double invRy[16] = {0};

    //vilslog::printf("\nrotate_AoA_normal_vector() ======================================\n");

    //float pitch_Radians = ((float)pitch * 3.14) / 180.0f;
    float roll_Radians = ((float)roll * 3.14) / 180.0f;
    //float yaw_Radians = ((float)yaw * 3.14) / 180.0f;

    // normal_vector[0] = (double)cos(yaw_Radians);
    // normal_vector[1] = (double)sin(yaw_Radians);
    // normal_vector[2] = (double)sin(pitch_Radians);

    normal_vector[0] = (double)norX;
    normal_vector[1] = (double)norY;
    normal_vector[2] = (double)norZ;

    double magnitude = sqrt( pow(normal_vector[0], 2) + pow(normal_vector[1], 2) + pow(normal_vector[2], 2) );

    double a = normal_vector[0] / magnitude;
    double b = normal_vector[1] / magnitude;
    double c = normal_vector[2] / magnitude;
    double d = sqrt( pow(b, 2) + pow(c, 2));

    // vilslog::printf("rotate_AoA_normal_vector() normal_vector[%f,%f,%f] unit vector[%f,%f,%f] magnitude[%f] d[%f]\n",
    //     normal_vector[0], normal_vector[1], normal_vector[2], a, b, c, magnitude, d);

    T[0] = 1;
    T[3] = -1 * posX;
    T[5] = 1;
    T[7] = -1 * posY;
    T[10] = 1;
    T[11] = -1 * posZ;
    T[15] = 1;

    Rx[0] = 1;
    if ( d == 0 )
    {
        Rx[5] = 1;
        Rx[6] = -1;
        Rx[9] = 1;
        Rx[10] = 1;
    }
    else
    {
        Rx[5] = c / d;
        Rx[6] = -1 * b / d;
        Rx[9] = b / d;
        Rx[10] = c / d;
    }
    Rx[15] = 1;

    Ry[0] = d;
    Ry[2] = -1 * a;
    Ry[5] = 1;
    Ry[8] = a;
    Ry[10] = d;
    Ry[15] = 1;

    Rz[0] = cos(roll_Radians);
    Rz[1] = -1 * sin(roll_Radians);
    Rz[4] = sin(roll_Radians);
    Rz[5] = cos(roll_Radians);
    Rz[10] = 1;
    Rz[15] = 1;

    //vilslog::printf("\nrotate_AoA_normal_vector() inverse_matrix(4, T, invT) \n");
    inverse_matrix(4, T, invT);
    //vilslog::printf("\nrotate_AoA_normal_vector() inverse_matrix(4, Rx, invRx) \n");
    inverse_matrix(4, Rx, invRx);
    //vilslog::printf("\nrotate_AoA_normal_vector() inverse_matrix(4, Ry, invRy) \n");
    inverse_matrix(4, Ry, invRy);

    double tmp1[16] = {0};
    double tmp2[16] = {0};
    double tmp3[16] = {0};
    double tmp4[16] = {0};
    double tmp5[16] = {0};
    double tmp6[16] = {0};
    dot_matrix(4, 4, invT, invRx, tmp1);
    dot_matrix(4, 4, tmp1, invRy, tmp2);
    dot_matrix(4, 4, tmp2, Rz, tmp3);
    dot_matrix(4, 4, tmp3, Ry, tmp4);
    dot_matrix(4, 4, tmp4, Rx, tmp5);
    dot_matrix(4, 4, tmp5, T, tmp6);

    double pos[16] = {0};
    pos[0] = point[0];
    pos[4] = point[1];
    pos[8] = point[2];
    pos[12] = 1;

    double posOut[16] = {0};
    dot_matrix(4, 4, tmp6, pos, posOut);

    result_roll[0] = posOut[0];
    result_roll[1] = posOut[4];
    result_roll[2] = posOut[8];

    // vilslog::printf("rotate_AoA_normal_vector() point[%f,%f,%f] result_roll[%f,%f,%f]\n",
    //     point[0], point[1], point[2], result_roll[0], result_roll[1], result_roll[2]);
    //
    // vilslog::printf("\nrotate_AoA_normal_vector() ======================================\n");
    return done;
}

bool dot_matrix(int rows, int cols, double * inMatrix1, double * inMatrix2, double * outMatrix)
{
    for(int r=0; r<rows; r++)
    {
        for(int iCol=0; iCol<cols; iCol++)
        {
            outMatrix[ r * rows + iCol ] = 0;
            for(int iRow=0; iRow<rows; iRow++)
            {
                outMatrix[r * rows + iCol] += inMatrix2[iRow * rows + iCol] * inMatrix1[r * rows + iRow];
            }
        }
    }
    return true;
}

//
// https://github.com/md-akhi/Inverse-matrix/blob/master/Inverse-matrix.cpp
//	calculate inverse of matrix
//
bool inverse_matrix(int N, double * inMatrix, double * outMatrix)
{
    double determinte = det(N, N, inMatrix);

    if(determinte == 0)
    {
        cout<<"\nInverse of Entered Matrix is not possible\n";
        return false;
    }
    else
    if(N == 1)
    {
        outMatrix[0*N + 0] = 1;
    }
    else
    {
        cofactor(N, determinte, inMatrix, outMatrix);
    }

    return true;
}

//
//	calculate determinte of matrix
//
double det(int N, int colSize, double * inMatrix)
{
    double b[N*colSize];
	double sum = 0;
	if (N == 1)
	{
		sum = inMatrix[0*colSize + 0];
        return sum;
	}
	else
	if(N == 2)
	{
        sum = (inMatrix[0*colSize + 0] * inMatrix[1*colSize + 1] - inMatrix[0*colSize + 1] * inMatrix[1*colSize + 0]);
        return sum;
	}
	else
	{
		for(int i=0;i<N;i++)
		{
			minorMatrix(i, N, colSize, inMatrix, b);	// read function
			sum = (double)(sum + inMatrix[0*N + i] * pow(-1,i) * det((N-1), colSize, b));	// read function	// sum = determinte matrix
		}
	}
    return sum;
}

//
//	calculate minor of matrix OR build new matrix : k-had = minor
//
void minorMatrix(int index, int N, int colSize, double * inMatrix, double * outMatrix)
{
    int h=0,k=0;
	for(int i=1; i<N; i++)
	{
		for(int j=0; j<N; j++)
		{
			if(j == index)
				continue;
			outMatrix[h*colSize + k] = inMatrix[i*colSize + j];
			k++;
			if(k == (N-1))
			{
				h++;
				k=0;
			}
		}
	}
}

//
//	calculate transpose of matrix
//
void transpose(int N, double determinte, double * inMatrix, double * outMatrix)
{
	double b[N*N];
	for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            b[i*N + j] = inMatrix[j*N + i];
        }
    }
	for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            outMatrix[i*N + j] = b[i*N + j] / determinte;	// array d[][] = inverse matrix
        }
    }
}

//
//	calculate cofactor of matrix
//
void cofactor(int N, double determinte, double * inMatrix, double * outMatrix)
{
	double b[N*N];
    double c[N*N];
	int m,k;
	for (int h=0; h<N; h++)
    {
		for (int l=0; l<N; l++)
        {
			m = 0;
			k = 0;
			for (int i=0; i<N; i++)
            {
                for (int j=0; j<N; j++)
                {
                    if (i != h && j != l)
                    {
						b[m*N + k]= inMatrix[i*N + j];
						if (k<(N-2))
                        {
                            k++;
                        }
						else
                        {
							k = 0;
							m++;
						}
					}
                }
            }
			c[h*N + l] = pow(-1,(h+l)) * det((N-1), N, b);	// c = cofactor Matrix
		}
    }
	transpose(N, determinte, c, outMatrix);	// read function
}

//
// https://stackoverflow.com/questions/38637542/finding-the-shortest-distance-between-two-3d-line-segments
//
double find_two_line_distance(PointStruct * p1, double * vector1, PointStruct * p3, double * vector2, double * crosspt1, double * crosspt2)
{
    // The direction vector of l1 is p2-p1, or d1.
    // The direction vector of l2 is p4-p3, or d2.
    // i1 = p1 + m*d1
    // i2 = p2 + n*d2
    // vx = i2x - i1x = (p3x + n*d2x) - (p1x + m*d1x)
    // double n;
    // double m;
    // double vx = (p3x + n * d2x) - (p1x + m * d1x);
    // double vy = (p3y + n * d2y) - (p1y + m * d1y);
    // double vz = (p3z + n * d2z) - (p1z + m * d1z);

    // v, is perpendicular to both of these direction vectors:
    // d1x*vx + d1y*vy + d1z*vz = 0
    // d2x*vx + d2y*vy + d2z*vz = 0
    // --
    // d1x*((p3x + n * d2x) - (p1x + m * d1x)) +
    // d1y*((p3y + n * d2y) - (p1y + m * d1y)) +
    // d1z*((p3z + n * d2z) - (p1z + m * d1z)) = 0

    // d2x*((p3x + n * d2x) - (p1x + m * d1x)) +
    // d2y*((p3y + n * d2y) - (p1y + m * d1y)) +
    // d2z*((p3z + n * d2z) - (p1z + m * d1z)) = 0
    // --
    // d1x*p3x + n*d2x*d1x - d1x*p1x + m*d1x*d1x +
    // d1y*p3y + n*d2y*d1y - d1y*p1y + m*d1y*d1y +
    // d1z*p3z + n*d2z*d1z - d1z*p1z + m*d1z*d1z = 0

    // d2x*p3x + n*d2x*d2x - d2x*p1x + m*d1x*d2x +
    // d2y*p3y + n*d2y*d2y - d2y*p1y + m*d1y*d2y +
    // d2z*p3z + n*d2z*d2z - d2z*p1z + m*d1z*d2z = 0
    // --
    // n*(d2x*d1x + d2y*d1y + d2z*d1z) - m*(d1x*d1x + d1y*d1y + d1z*d1z) +
    // d1x*p3x + d1y*p3y + d1z*p3z - d1x*p1x - d1y*p1y - d1z*p1z = 0;
    //
    // n*(d2x*d2x + d2y*d2y + d2z*d2z) - m*(d1x*d2x + d1y*d2y + d1z*d2z) +
    // d2x*p3x + d2y*p3y + d2z*p3z - d2x*p1x - d2y*p1y - d2z*p1z = 0;
    // --
    // double A = d2x*d1x + d2y*d1y + d2z*d1z;
    // double B = d1x*d1x + d1y*d1y + d1z*d1z;
    // double C = d1x*p3x + d1y*p3y + d1z*p3z;
    // double D = d1x*p1x + d1y*p1y + d1z*p1z;
    // double E = d2x*d2x + d2y*d2y + d2z*d2z;
    // double F = d1x*d2x + d1y*d2y + d1z*d2z;
    // double G = d2x*p3x + d2y*p3y + d2z*p3z;
    // double H = d2x*p1x + d2y*p1y + d2z*p1z;
    // n*A - m*B + C - D = 0
    // n*E - m*F + G - H = 0
    // --
    // n = (m*B + D - C)/A
    // n = (m*F + H - G)/E
    // --
    // m*B*E + D*E - C*E = m*F*A + H*A - G*A
    // --
    // m*(B*E - F*A) = H*A - G*A - D*E + C*E
    // --
    // m = (H*A - G*A - D*E + C*E)/(B*E - F*A)

    double A = vector2[0]*vector1[0] + vector2[1]*vector1[1] + vector2[2]*vector1[2];
    double B = vector1[0]*vector1[0] + vector1[1]*vector1[1] + vector1[2]*vector1[2];
    double C = vector1[0]*p3->x + vector1[1]*p3->y + vector1[2]*p3->z;
    double D = vector1[0]*p1->x + vector1[1]*p1->y + vector1[2]*p1->z;
    double E = vector2[0]*vector2[0] + vector2[1]*vector2[1] + vector2[2]*vector2[2];
    double F = vector1[0]*vector2[0] + vector1[1]*vector2[1] + vector1[2]*vector2[2];
    double G = vector2[0]*p3->x + vector2[1]*p3->y + vector2[2]*p3->z;
    double H = vector2[0]*p1->x + vector2[1]*p1->y + vector2[2]*p1->z;

    double m = (H*A - G*A - D*E + C*E)/(B*E - F*A);
    double n = (m*F + H - G)/E;

    // i1 = p1 + m*d1
    // i2 = p3 + n*d2
    // double i1[3] = {0};
    // double i2[3] = {0};

    crosspt1[0] = p1->x + m*vector1[0];
    crosspt1[1] = p1->y + m*vector1[1];
    crosspt1[2] = p1->z + m*vector1[2];

    crosspt2[0] = p3->x + n*vector2[0];
    crosspt2[1] = p3->y + n*vector2[1];
    crosspt2[2] = p3->z + n*vector2[2];

    double distance = sqrt( pow(crosspt1[0]-crosspt2[0],2) + pow(crosspt1[1]-crosspt2[1],2) + pow(crosspt1[2]-crosspt2[2],2) );

    // vilslog::printf("PosCalculate::find_two_line_distance() m[%f] n[%f] i1[%d,%d,%d] i2[%d,%d,%d] distance[%f]\n",
    //     m, n, (int)i1[0], (int)i1[1], (int)i1[2], (int)i2[0], (int)i2[1], (int)i2[2], distance);

    return distance;
}

void normalise_vector(int colSize, double * inMatrix, double * outMatrix)
{
    double sum = 0;
    for(int i=0;i<colSize;i++)
    {
        sum += pow(inMatrix[i], 2);
    }

    sum = sqrt(sum) ;

    for(int i=0;i<colSize;i++)
    {
        outMatrix[i] = inMatrix[i] / sum;
    }
}

//
// Calculate Rotation Matrix to align Vector A to Vector B in 3d
// https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
//
bool findRotationMatrix(double * coord1_P1, double * coord1_P2, double * coord1_P3,
                        double * coord2_P1, double * coord2_P2, double * coord2_P3, double *R)
{
    double coord1_X[3];
    coord1_X[0] = coord1_P2[0] - coord1_P1[0];
    coord1_X[1] = coord1_P2[1] - coord1_P1[1];
    coord1_X[2] = coord1_P2[2] - coord1_P1[2];
    double coord1_Y[3];
    coord1_Y[0] = coord1_P3[0] - coord1_P1[0];
    coord1_Y[1] = coord1_P3[1] - coord1_P1[1];
    coord1_Y[2] = coord1_P3[2] - coord1_P1[2];

    double coord1_Z[3] = { 0, 0, 0 };
    crossproduct(coord1_X, coord1_Y, coord1_Z);
    normalise_vector(3, coord1_Z, coord1_Z);

    // vilslog::printRaw("findRotationMatrix coord1_Z[%f,%f,%f]\n", coord1_Z[0], coord1_Z[1], coord1_Z[2]);

    double coord2_X[3];
    coord2_X[0] = coord2_P2[0] - coord2_P1[0];
    coord2_X[1] = coord2_P2[1] - coord2_P1[1];
    coord2_X[2] = coord2_P2[2] - coord2_P1[2];
    double coord2_Y[3];
    coord2_Y[0] = coord2_P3[0] - coord2_P1[0];
    coord2_Y[1] = coord2_P3[1] - coord2_P1[1];
    coord2_Y[2] = coord2_P3[2] - coord2_P1[2];

    double coord2_Z[3] = { 0, 0, 0 };
    crossproduct(coord2_X, coord2_Y, coord2_Z);
    normalise_vector(3, coord2_Z, coord2_Z);

    // vilslog::printRaw("findRotationMatrix coord2_Z[%f,%f,%f]\n", coord2_Z[0], coord2_Z[1], coord2_Z[2]);

    // R = B * inverse(A)

    double A[9] = {0};
    double invA[9] = {0};
    A[0] = coord2_X[0];
    A[3] = coord2_X[1];
    A[6] = coord2_X[2];
    A[1] = coord2_Y[0];
    A[4] = coord2_Y[1];
    A[7] = coord2_Y[2];
    A[2] = coord2_Z[0];
    A[5] = coord2_Z[1];
    A[8] = coord2_Z[2];

    inverse_matrix(3, A, invA);

    double B[9] = {0};
    B[0] = coord1_X[0];
    B[3] = coord1_X[1];
    B[6] = coord1_X[2];
    B[1] = coord1_Y[0];
    B[4] = coord1_Y[1];
    B[7] = coord1_Y[2];
    B[2] = coord1_Z[0];
    B[5] = coord1_Z[1];
    B[8] = coord1_Z[2];

    MultiplyMatrix(3, B, invA, R);

    // double A[16] = {0};
    // double invA[16] = {0};
    // A[0] = coord2_X[0];
    // A[4] = coord2_X[1];
    // A[8] = coord2_X[2];
    // A[12] = 0;
    // A[1] = coord2_Y[0];
    // A[5] = coord2_Y[1];
    // A[9] = coord2_Y[2];
    // A[13] = 0;
    // A[2] = coord2_Z[0];
    // A[6] = coord2_Z[1];
    // A[10] = coord2_Z[2];
    // A[14] = 0;
    // A[3] = coord2_X[0] - coord1_X[0];
    // A[7] = 0;
    // A[11] = 0;
    // A[15] = 1;
    //
    // inverse_matrix(4, A, invA);
    //
    // double B[16] = {0};
    // B[0] = coord1_X[0];
    // B[4] = coord1_X[1];
    // B[8] = coord1_X[2];
    // B[12] = 0;
    // B[1] = coord1_Y[0];
    // B[5] = coord1_Y[1];
    // B[9] = coord1_Y[2];
    // B[13] = 0;
    // B[2] = coord1_Z[0];
    // B[6] = coord1_Z[1];
    // B[10] = coord1_Z[2];
    // B[14] = 0;
    // B[3] = 0;
    // B[7] = 0;
    // B[11] = 0;
    // B[15] = 1;
    //
    // MultiplyMatrix(4, B, invA, R);

    return true;
}

double angle_to_rad(double angle)
{
    // M_PI/180.0 = 0.017453
    return angle * 0.017453;
}

float angle_to_rad(float angle)
{
    return angle * 0.017453f;
}

double rad_to_angle(double rad)
{
    //  180.0/M_PI = 57.295780
    return rad * 57.295780;
}

float rad_to_angle(float rad)
{
    return rad * 57.295780f;
}

bool findCorrectTriangleShape(double *inP1, double *inP2, double *inP3, double *outP1, double *outP2, double *outP3, double *R, int * sequence)
{
    if (tryCorrectTriangleShape(inP1, inP2, inP3, outP1, outP2, outP3, R))
    {
        sequence[0] = 0;
        sequence[1] = 1;
        sequence[2] = 2;
        return true;
    }
    else
    if (tryCorrectTriangleShape(inP1, inP3, inP2, outP1, outP2, outP3, R))
    {
        sequence[0] = 0;
        sequence[1] = 2;
        sequence[2] = 1;
        return true;
    }
    else
    if (tryCorrectTriangleShape(inP2, inP1, inP3, outP1, outP2, outP3, R))
    {
        sequence[0] = 1;
        sequence[1] = 0;
        sequence[2] = 2;
        return true;
    }
    else
    if (tryCorrectTriangleShape(inP2, inP3, inP1, outP1, outP2, outP3, R))
    {
        sequence[0] = 1;
        sequence[1] = 2;
        sequence[2] = 0;
        return true;
    }
    else
    if (tryCorrectTriangleShape(inP3, inP1, inP2, outP1, outP2, outP3, R))
    {
        sequence[0] = 2;
        sequence[1] = 0;
        sequence[2] = 1;
        return true;
    }
    else
    if (tryCorrectTriangleShape(inP3, inP2, inP1, outP1, outP2, outP3, R))
    {
        sequence[0] = 2;
        sequence[1] = 1;
        sequence[2] = 0;
        return true;
    }
    return false;
}

bool tryCorrectTriangleShape(double *P1, double *P2, double *P3, double *outP1, double *outP2, double *outP3, double *R)
{
    double invR[16];
    double mP1[4];
    double mP2[4];
    double mP3[4];

    findCoordTransformation(P1, P2, P3, R, invR);

    double nP1[4] = {P1[0], P1[1], P1[2], 1};
    double nP2[4] = {P2[0], P2[1], P2[2], 1};
    double nP3[4] = {P3[0], P3[1], P3[2], 1};

    // rotate to new plane
    DotMatrix(4, invR, nP1, mP1);
    DotMatrix(4, invR, nP2, mP2);
    DotMatrix(4, invR, nP3, mP3);

    //printf("mP1:[%f, %f, %f]\n", mP1[0], mP1[1], mP1[2]);
    //printf("mP2:[%f, %f, %f]\n", mP2[0], mP2[1], mP2[2]);
    //printf("mP3:[%f, %f, %f]\n", mP3[0], mP3[1], mP3[2]);

    //
    //  P2 X axis and  P3 Y axis
    //
    if ( mP2[0] >= 0 && mP3[0] >= 0 && mP3[1] > 0)
    {
        outP1[0] = mP1[0];
        outP1[1] = mP1[1];
        outP1[2] = mP1[2];
        outP2[0] = mP2[0];
        outP2[1] = mP2[1];
        outP2[2] = mP2[2];
        outP3[0] = mP3[0];
        outP3[1] = mP3[1];
        outP3[2] = mP3[2];
        return true;
    }
    else
        return false;
}


// Changing Coordinate Systems
// tranform 3 anchors coordinate system to origianal system(Anchor 1 is at the origin)
// http://www.math.tau.ac.il/~dcor/Graphics/cg-slides/geom3d.pdf
bool findCoordTransformation(double *P1, double *P2, double *P3, double *R, double *invR)
{
    // ex = (P2 - P1)/(numpy.linalg.norm(P2 - P1))
    double P2P1[3];
    P2P1[0] = P2[0] - P1[0];
    P2P1[1] = P2[1] - P1[1];
    P2P1[2] = P2[2] - P1[2];
    double nP2P1 = norm(P2P1);
    //printf("nP2P1:[%f]\n", nP2P1);

    double ex[3] = { 0, 0, 0 };
    for (int i = 0; i < 3; i++)
    {
        ex[i] = P2P1[i] / nP2P1;
    }
    //printf("ex:[%f, %f, %f]\n", ex[0], ex[1], ex[2]);

    // i = numpy.dot(ex, P3 - P1)
    double P3P1[3];
    P3P1[0] = P3[0] - P1[0];
    P3P1[1] = P3[1] - P1[1];
    P3P1[2] = P3[2] - P1[2];

    double ivar = 0;
    for (int i = 0; i < 3; i++)
    {
        ivar += ex[i] * P3P1[i];
    }
    //printf("ivar:[%f]\n", ivar);

    // ey = (P3 - P1 - i*ex)/(numpy.linalg.norm(P3 - P1 - i*ex))
    double P3P1iex[3];
    P3P1iex[0] = P3[0] - P1[0] - ivar * ex[0];
    P3P1iex[1] = P3[1] - P1[1] - ivar * ex[1];
    P3P1iex[2] = P3[2] - P1[2] - ivar * ex[2];
    double nP3P1iex = norm(P3P1iex);
    //printf("nP3P1iex:[%f]\n", nP3P1iex);

    double ey[3] = { 0, 0, 0 };
    for (int i = 0; i < 3; i++)
    {
        ey[i] = P3P1iex[i] / nP3P1iex;
    }
    //printf("ey:[%f, %f, %f]\n", ey[0], ey[1], ey[2]);

    // ez = numpy.cross(ex,ey)
    double ez[3] = { 0, 0, 0 };
    crossproduct(ex, ey, ez);
    //printf("ez:[%f, %f, %f]\n", ez[0], ez[1], ez[2]);

    // keep Z as positive
    if (ez[2] < -0.01)
    {
        for (int i = 0; i < 3; i++)
        {
            ey[i] = ey[i] * -1;
            ez[i] = ez[i] * -1;
        }
    }

    //R = numpy.array([[ex[0], ey[0], ez[0], P1[0]], [ex[1], ey[1], ez[1], P1[1]], [ex[2], ey[2], ez[2], P1[2]], [0, 0, 0, 1]])
    for (int i = 0; i < 3; i++)
    {
        R[4*i + 0] = ex[i];
        R[4*i + 1] = ey[i];
        R[4*i + 2] = ez[i];
        R[4*i + 3] = P1[i];
    }
    R[12] = 0;
    R[13] = 0;
    R[14] = 0;
    R[15] = 1;

    //invert(R, invR);
    gluInvertMatrix(R, invR);

/*
    for (int i = 0; i < 4; i++)
    {
        printf("R:[%f, %f, %f, %f]\n", R[4*i + 0], R[4*i + 1], R[4*i + 2], R[4*i + 3]);
    }
    for (int i = 0; i < 4; i++)
    {
        printf("invR:[%f, %f, %f, %f]\n", invR[4*i + 0], invR[4*i + 1], invR[4*i + 2], invR[4*i + 3]);
    }
*/
    return true;
}

double norm(double * vector)
{
    double normOfV = 0.0;

    for (int i = 0; i < 3; i++)
    {
        normOfV += pow(vector[i], 2);
    }
    return sqrt(normOfV);
}

bool crossproduct(double * vector1, double * vector2, double * resrult)
{
    resrult[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
    resrult[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
    resrult[2] = vector1[0]*vector2[1] - vector1[1]*vector2[0];
    return true;
}

double dotproduct(double * vector1, double * vector2)
{
    double ivar = 0;
    for (int i = 0; i < 3; i++)
    {
        ivar += vector1[i] * vector2[i];
    }
    return ivar;
}

bool gluInvertMatrix(double * m, double * invOut)
{
    double inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

bool DotMatrix(int d, double * m, double * v, double * vOut)
{
    for(int iCol=0; iCol<d; iCol++)
    {
        vOut[iCol] = 0;
        for(int iRow=0; iRow<d; iRow++)
        {
            vOut[iCol] += v[iRow] * m[iCol*d + iRow];
        }
    }
    return true;
}

bool MultiplyMatrix(int d, double * m, double * v, double * vOut)
{
    for(int iRow=0; iRow<d; iRow++)
    {
        int row_index = iRow * d;
        for(int iCol=0; iCol<d; iCol++)
        {
            double sum = 0;
            for(int iR=0; iR<d; iR++)
            {
                sum += v[iR*d + iCol] * m[row_index + iR];
            }
            vOut[row_index + iCol] = sum;

            // vilslog::printRaw("MultiplyMatrix iRow[%d] iCol[%d] sum[%f]\n", iRow, iCol, sum);
        }
    }
    return true;
}

double TwoPointDistance(double * pt1, double * pt2)
{
	return sqrt(pow(pt1[0] - pt2[0], 2) + pow(pt1[1] - pt2[1], 2) + pow(pt1[2] - pt2[2], 2));
}

//
// http://mathworld.wolfram.com/Circle-CircleIntersection.html
//
bool findTwoCircleIntersecPoint(double * P1, double * P2, double Radius1, double Radius2, double * results)
{
    // set P1 as origin (0,0)
    double nP2[2] = {P2[0] - P1[0], 0.0};
    double x = 0.0;
    double d = nP2[0];
    double R = Radius1;
    double r = Radius2;

    if (d < r)
    {
        // interset point on the left
        //printf("findTwoCircleIntersecPoint() interset point on the left \n");
        if ((r - d) > R)
        {
            printf("findTwoCircleIntersecPoint() interset point on the left, (2) big circle contain (1) small circle \n");
            x = - R + (R - (r - d)) / 2;
        }
        else
        {
            printf("findTwoCircleIntersecPoint() interset point on the left, has one or two interset points \n");
            x = (pow(d, 2) - pow(r, 2) + pow(R, 2)) / (2*d);
        }
    }
    else
    if ( (d + r) <= R )
    {
        // big circle contain small circle
        printf("findTwoCircleIntersecPoint() (1) big circle contain (2) small circle\n");
        x = (d + r) + (R - (d + r)) / 2;
    }
    else
    if ( (d + r) > R)
    {
        // big circle contain small circle
        printf("findTwoCircleIntersecPoint() (2) big circle contain (1) small circle\n");
        x = (pow(d, 2) - pow(r, 2) + pow(R, 2)) / (2*d);
    }
    else
    if ( (R + r) >= d)
    {
        // has one or two interset points
        printf("findTwoCircleIntersecPoint() has one or two interset points\n");
        x = (pow(d, 2) - pow(r, 2) + pow(R, 2)) / (2*d);
    }
    else
    {
        // find non-intersec point
        printf("findTwoCircleIntersecPoint() find non-intersec point\n");
        x = (pow(d, 2) - pow(r, 2) + pow(R, 2)) / (2*d);
    }

    results[0] = x;
    results[1] = 0;
    return true;
}

bool findLinePlaneIntersectPoint(double * plane_pt, double * plane_norm, double * line_pt, double * line_dir, double * results)
{
    // plane equation
    // ( P - plane_pt ) dot plane_norm = 0 ---- (1)

    // vector equation
    // P = line_pt + line_dir * d ------------ (2)

    // (1) -> (1)
    // ( (ine_pt + line_dir * d) - plane_pt ) dot plane_norm = 0
    // (line_dir dot plane_norm) * d + ( line_pt - plane_pt ) dot plane_norm = 0
    // d = (( plane_pt - line_pt) dot plane_norm ) / (line_dir dot plane_norm)

    // if (line_dir dot plane_norm) == 0
    //    --> line and plane are parallel
    //        if (( plane_pt - line_pt) dot plane_norm ) == 0
    //              line is contain in the plane.
    //        else
    //              no interaction
    // else
    // if (line_dir dot plane_norm) != 0
    //    --> there is a single point of intersction.
    //    point is line_pt + line_dir * d

    double v1[3] = {0};
    v1[0] = plane_pt[0] - line_pt[0];
    v1[1] = plane_pt[1] - line_pt[1];
    v1[2] = plane_pt[2] - line_pt[2];

    double top_value = dotproduct(v1, plane_norm);
    double bottom_value = dotproduct(line_dir, plane_norm);

    if ( bottom_value == 0 )
    {
        // line and plane are parallel
        if ( top_value == 0 )
        {
            // line is contain in the plane.
            results[0] = plane_pt[0];
            results[1] = plane_pt[1];
            results[2] = plane_pt[2];
        }
        else
        {
            return false;
        }
    }
    else
    {
        double d = top_value / bottom_value;
        results[0] = line_pt[0] + (line_dir[0] * d);
        results[1] = line_pt[1] + (line_dir[1] * d);
        results[2] = line_pt[2] + (line_dir[2] * d);
    }

    return true;
}

bool rotate_point(float tx, float ty, float cx, float cy, float angle, float * p)
{
    //float Radians = (angle * 3.14) / 180.0;
    float Radians = angle_to_rad(angle);
    float s = sin(Radians);
    float c = cos(Radians);

    // translate point back to origin:
    tx -= cx;
    ty -= cy;

    // rotate point
    float xnew = tx * c - ty * s;
    float ynew = tx * s + ty * c;

    // translate point back:
    p[0] = xnew + cx;
    p[1] = ynew + cy;
    return true;
}

int convert_to_1D_ranging(double * TagCoordinate, double * AnchorCoordinate, int rangingvlaue, int fixAxis, int fixCoord, int maxPos, int minPos, int Mapping_Height, double * result)
{
    int success = 0;
    // printf("convert_to_1D_ranging() rangingvlaue[%d] fixAxis[%d] fixCoord[%d] maxPos[%d] minPos[%d] Mapping_Height[%d]\n", rangingvlaue, fixAxis, fixCoord, maxPos, minPos, Mapping_Height);
    // printf("\t TagCoordinate[%d,%d,%d] AnchorCoordinate[%d, %d, %d]\n",
    //         (int)TagCoordinate[0], (int)TagCoordinate[1], (int)TagCoordinate[2],
    //         (int)AnchorCoordinate[0], (int)AnchorCoordinate[1], (int)AnchorCoordinate[2]);

    if (fixAxis == 0)
    {
        // fix X AXIS
        double diffX = AnchorCoordinate[0] - fixCoord;
        double diffZ = AnchorCoordinate[2] - Mapping_Height;
        double AnToCoordDistance = sqrt(pow(diffX, 2) + pow(diffZ, 2));

        if ( AnToCoordDistance > rangingvlaue )
        {
            //printf("\t Failed! AnToCoordDistance(%d) > rangingvlaue(%d)\n", (int)AnToCoordDistance, (int)rangingvlaue);
            return 0;
        }

        double TagToAnDistance = sqrt(pow(rangingvlaue, 2) - pow(AnToCoordDistance, 2));
        result[0] = fixCoord;
        result[1] = AnchorCoordinate[1];
        result[2] = Mapping_Height;
        result[3] = TagToAnDistance;
    }
    else
    {
        // fix Y AXIS
        double diffY = AnchorCoordinate[1] - fixCoord;
        double diffZ = AnchorCoordinate[2] - Mapping_Height;
        double AnToCoordDistance = sqrt(pow(diffY, 2) + pow(diffZ, 2));

        if ( AnToCoordDistance > rangingvlaue )
        {
            //printf("\t Failed! AnToCoordDistance(%d) > rangingvlaue(%d)\n", (int)AnToCoordDistance, (int)rangingvlaue);
            return 0;
        }

        double TagToAnDistance = sqrt(pow(rangingvlaue, 2) - pow(AnToCoordDistance, 2));
        result[0] = AnchorCoordinate[0];
        result[1] = fixCoord;
        result[2] = Mapping_Height;
        result[3] = TagToAnDistance;
    }
    success = 1;
    // printf("\t result[%d,%d,%d] TagToAnDistance[%d]\n",
    //         (int)result[0], (int)result[1], (int)result[2], (int)result[3]);


    return success;
}

bool map_to_1D(double * AnchorCoordinate, int startX, int startY, int startZ, int endX, int endY, int endZ, double * result)
{
    //
    // 直線方程式
    //
    //double t = 1;// 未知數
    // 直線方向向量
    double line_vector_x = endX - startX;
    double line_vector_y = endY - startY;
    double line_vector_z = endZ - startZ;

    double t = (line_vector_x * AnchorCoordinate[0] + line_vector_y * AnchorCoordinate[1] + line_vector_z * AnchorCoordinate[2] - line_vector_x * startX - line_vector_y * startY - line_vector_z * startZ) / (pow(line_vector_x, 2) + pow(line_vector_y, 2) + pow(line_vector_z, 2));
    double mapX = startX + line_vector_x * t;
    double mapY = startY + line_vector_y * t;
    double mapZ = startZ + line_vector_z * t;

    // printf("\t t[%.02f] map[%.02f, %.02f, %.02f] TagToMappingAnDistance[%.02f]\n", t, mapX, mapY, mapZ, TagToMappingAnDistance);
    // printf("\t AnToLineDistance[%.02f]\n", AnToLineDistance);

    result[0] = mapX;
    result[1] = mapY;
    result[2] = mapZ;

    return true;
}

bool map_to_1D_ranging(double * AnchorCoordinate, int rangingvlaue, int startX, int startY, int startZ, int endX, int endY, int endZ, double * result)
{
    // printf("map_to_1D_ranging() rangingvlaue[%d] startX[%d] startY[%d] endX[%d] endY[%d] Mapping_Height[%d]\n",
    //             rangingvlaue, startX, startY, endX, endY, Mapping_Height);
    // printf("\t AnchorCoordinate[%d, %d, %d]\n",(int)AnchorCoordinate[0], (int)AnchorCoordinate[1], (int)AnchorCoordinate[2]);

    //
    // 直線方程式
    //
    //double t = 1;// 未知數
    // 直線方向向量
    double line_vector_x = endX - startX;
    double line_vector_y = endY - startY;
    double line_vector_z = endZ - startZ;
    // 投影點 mapX, mapY, mapZ
    //double mapX = startX + line_vector_x * t;
    //double mapY = startY + line_vector_y * t;
    //double mapZ = Mapping_Height;
    // 投影點 與 anchor 直線方向向量
    //double line_mapping_x = mapX - AnchorCoordinate[0];
    //double line_mapping_y = mapY - AnchorCoordinate[1];
    //double line_mapping_z = mapZ - AnchorCoordinate[2];

    //Anchor->投影點 與 直線方向向量 內積應為0
    //line_vector_x * line_mapping_x + line_vector_y * line_mapping_y + line_vector_z * line_mapping_z = 0;
    //
    //line_vector_x * (startX + line_vector_x * t - AnchorCoordinate[0]) +
    //line_vector_y * (startY + line_vector_y * t - AnchorCoordinate[1]) +
    //line_vector_z * (startZ + line_vector_z * t - AnchorCoordinate[2]) = 0
    //
    // line_vector_x * startX + line_vector_x^2 * t - line_vector_x * AnchorCoordinate[0] +
    // line_vector_y * startY + line_vector_y^2 * t - line_vector_y * AnchorCoordinate[1] +
    // line_vector_z * startZ + line_vector_z^2 * t - line_vector_z * AnchorCoordinate[2] = 0
    //
    // (line_vector_x^2 + line_vector_y^2 + line_vector_z^2) * t =
    //  line_vector_x * AnchorCoordinate[0] + line_vector_y * AnchorCoordinate[1] + line_vector_z * AnchorCoordinate[2] -
    //  line_vector_x * startX - line_vector_y * startY - line_vector_z * startZ
    //
    double t = (line_vector_x * AnchorCoordinate[0] + line_vector_y * AnchorCoordinate[1] + line_vector_z * AnchorCoordinate[2] - line_vector_x * startX - line_vector_y * startY - line_vector_z * startZ) / (pow(line_vector_x, 2) + pow(line_vector_y, 2) + pow(line_vector_z, 2));
    double mapX = startX + line_vector_x * t;
    double mapY = startY + line_vector_y * t;
    double mapZ = startZ + line_vector_z * t;

    double diffX = AnchorCoordinate[0] - mapX;
    double diffY = AnchorCoordinate[1] - mapY;
    double diffZ = AnchorCoordinate[2] - mapZ;
    double AnToLineDistance = sqrt(pow(diffX, 2) + pow(diffY, 2) + pow(diffZ, 2));

    double value1 = pow(rangingvlaue, 2);
    double value2 = pow(AnToLineDistance, 2);

    if (value1 <= value2)
    {
        // printf("map_to_1D_ranging() startX[%d] startY[%d] endX[%d] endY[%d] Mapping_Height[%d]\n",
        //             startX, startY, endX, endY, Mapping_Height);
        // printf("\t AnchorCoordinate[%d, %d, %d] rangingvlaue[%d] AnToLineDistance[%d]\n",
        //             (int)AnchorCoordinate[0], (int)AnchorCoordinate[1], (int)AnchorCoordinate[2], rangingvlaue, (int)AnToLineDistance);
        // printf("\t Failed!!!!  value1[%.02f] <= value2[%.02f]\n", value1, value2);
        return false;
    }

    double TagToMappingAnDistance = sqrt(value1 - value2);

    // printf("\t t[%.02f] map[%.02f, %.02f, %.02f] TagToMappingAnDistance[%.02f]\n", t, mapX, mapY, mapZ, TagToMappingAnDistance);
    // printf("\t AnToLineDistance[%.02f]\n", AnToLineDistance);

    result[0] = mapX;
    result[1] = mapY;
    result[2] = mapZ;
    result[3] = TagToMappingAnDistance;

    return true;
}
