/*
 * SimulationPowerTag.cpp
 *
 *  Created on: 2020/02/10
 *      Author: ikki
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "SimulationPowerTag.h"
#include "../PosCalculate/algorithm.h"
#include "../common.h"

SimulationPowerTag::SimulationPowerTag(DeviceManager * pDevManager)
    : mDevManager(pDevManager)
{
}

SimulationPowerTag::~SimulationPowerTag()
{
}

void SimulationPowerTag::start()
{
    sleep(15);

    AoANodeStruct * aoaNode = new AoANodeStruct();
    aoaNode->posX = 0;
    aoaNode->posY = 0;
    aoaNode->posZ = 0;
    aoaNode->pitch = 90;
    aoaNode->roll = 0;
    aoaNode->yaw = 32;

    AoARangingInfoStruct * aoainfo = new AoARangingInfoStruct();
    aoainfo->projectid = 1;
    aoainfo->aoaNode = aoaNode;
    aoainfo->TagAngle = -45;
    aoainfo->Ranging = 1000;
    aoainfo->MapLayer = 1;

    float pitch_Radians = angle_to_rad((float)aoaNode->pitch);
    float yaw_Radians = angle_to_rad((float)aoaNode->yaw);

    aoainfo->aoaNorm[0] = (double)cos(yaw_Radians)*1000;
    aoainfo->aoaNorm[1] = (double)sin(yaw_Radians)*1000;
    aoainfo->aoaNorm[2] = (double)sin(pitch_Radians)*1000;

    vilslog::printRaw("SimulationPowerTag::start() aoaNorm[%f, %f, %f]\n",
        aoainfo->aoaNorm[0], aoainfo->aoaNorm[1], aoainfo->aoaNorm[2]);

    double AoAAnchor1[3] = {357,466,300};
    double AoAAnchor2[3] = {365,-489,300};
    double AoAAnchor3[3] = {-360,-453,300};

    vilslog::printRaw("AoAAnchor AoAAnchor1[%d,%d,%d] AoAAnchor2[%d,%d,%d] AoAAnchor3[%d,%d,%d]\n",
            (int)AoAAnchor1[0], (int)AoAAnchor1[1], (int)AoAAnchor1[2],
            (int)AoAAnchor2[0], (int)AoAAnchor2[1], (int)AoAAnchor2[2],
            (int)AoAAnchor3[0], (int)AoAAnchor3[1], (int)AoAAnchor3[2]);

    // double P2P1[3];
    // P2P1[0] = AoAAnchor2[0] - AoAAnchor1[0];
    // P2P1[1] = AoAAnchor2[1] - AoAAnchor1[1];
    // P2P1[2] = AoAAnchor2[2] - AoAAnchor1[2];
    // double P3P1[3];
    // P3P1[0] = AoAAnchor3[0] - AoAAnchor1[0];
    // P3P1[1] = AoAAnchor3[1] - AoAAnchor1[1];
    // P3P1[2] = AoAAnchor3[2] - AoAAnchor1[2];
    //
    // double norm[3] = { 0, 0, 0 };
    // crossproduct(P2P1, P3P1, norm);
    //
    // vilslog::printRaw("AoAAnchor norm[%f,%f,%f]\n",  norm[0], norm[1], norm[2]);

    //
    // map 3 ponit to same plane
    //
    // double R[16];
    // double invR[16];
    // findCoordTransformation(AoAAnchor1, AoAAnchor2, AoAAnchor3, R, invR);
    //
    // double P1[4] = {AoAAnchor1[0], AoAAnchor1[1], AoAAnchor1[2], 1};
    // double P2[4] = {AoAAnchor2[0], AoAAnchor2[1], AoAAnchor2[2], 1};
    // double P3[4] = {AoAAnchor3[0], AoAAnchor3[1], AoAAnchor3[2], 1};
    //
    // double mP1[4];
    // double mP2[4];
    // double mP3[4];
    // // rotate to new plane
    // DotMatrix(4, invR, P1, mP1);
    // DotMatrix(4, invR, P2, mP2);
    // DotMatrix(4, invR, P3, mP3);
    //
    // vilslog::printRaw("AoAAnchor mP1[%d,%d,%d] mP2[%d,%d,%d] mP3[%d,%d,%d]\n",
    //         (int)mP1[0], (int)mP1[1], (int)mP1[2],
    //         (int)mP2[0], (int)mP2[1], (int)mP2[2],
    //         (int)mP3[0], (int)mP3[1], (int)mP3[2]);
    while(1)
    {
        // vilslog::printRaw("\n\n");
        //long long start1 = getSystemTime();

        aoainfo->TagAngle = -45;
        std::list<PointStruct *> * ptList1 = list_cicle_point(aoainfo);

        // long long start2 = getSystemTime();
        // long long list1Using = start2 - start1;

        vilslog::printRaw("\n\n");
        // vilslog::printRaw("find_cross_circle_point 1 list1Using[%d]\n", list1Using);
        std::list<PointStruct *> * resultPtList1 = find_cross_circle_point(aoaNode, ptList1, AoAAnchor1[2], false);

        aoainfo->TagAngle = 46;
        std::list<PointStruct *> * ptList2 = list_cicle_point(aoainfo);

        // long long start3 = getSystemTime();
        // long long list2Using = start3 - start2;

        vilslog::printRaw("\n\n");
        // vilslog::printRaw("find_cross_circle_point 2 list2Using[%d]\n", list2Using);
        std::list<PointStruct *> * resultPtList2 = find_cross_circle_point(aoaNode, ptList2, AoAAnchor2[2], false);

        aoainfo->TagAngle = 44;
        std::list<PointStruct *> * ptList3 = list_cicle_point(aoainfo);

        long long start4 = getSystemTime();
        // long long list3Using = start4 - start3;

        vilslog::printRaw("\n\n");
        // vilslog::printRaw("find_cross_circle_point 3 list3Using[%d]\n", list3Using);
        std::list<PointStruct *> * resultPtList3 = find_cross_circle_point(aoaNode, ptList3, AoAAnchor3[2], false);

        double results[4];
        bool bFound = find_AoA_position(AoAAnchor1, AoAAnchor2, AoAAnchor3, resultPtList1, resultPtList2, resultPtList3, results);

        if (bFound)
        {
            vilslog::printRaw("results[%f,%f,%f] yaw[%f]\n", results[0], results[1], results[2], results[3]);
        }
        else
        {
            vilslog::printRaw("Not Foudn!!!!\n");
        }

        long long start5 = getSystemTime();
        long long find_AoA_position_Using = start5 - start4;
        vilslog::printRaw("find_AoA_position_Using[%d]\n", find_AoA_position_Using);

        clear_PointStruct_list(ptList1);
        delete ptList1;
        clear_PointStruct_list(resultPtList1);
        delete resultPtList1;

        clear_PointStruct_list(ptList2);
        delete ptList2;
        clear_PointStruct_list(resultPtList2);
        delete resultPtList2;

        clear_PointStruct_list(ptList3);
        delete ptList3;
        clear_PointStruct_list(resultPtList3);
        delete resultPtList3;

        //usleep(10000000); // 10000 ms

        sleep(600);
    }
}


std::list<PointStruct *> * SimulationPowerTag::list_cicle_point(AoARangingInfoStruct * aoainfo)
{
    std::list<PointStruct *> * ptList = new std::list<PointStruct *>();
    float Radians = angle_to_rad(aoainfo->TagAngle);
    int radius = abs(aoainfo->Ranging * cos(Radians));
    float Xoffset = aoainfo->Ranging * sin(Radians);

    // move 5 cm a step
    //float minAngle = rad_to_angle(asin(5.0f/(float)aoainfo->Ranging));

    // move 10 cm a step
    float minAngle = rad_to_angle(asin(10.0f/(float)aoainfo->Ranging));

    // vilslog::printRaw("SimulationPowerTag::list_cicle_point() Ranging[%d] TagAngle[%f] radius[%d] Xoffset[%f] minAngle[%f]\n",
    //     aoainfo->Ranging, aoainfo->TagAngle, radius, Xoffset, minAngle);

    int count = 0;
    PointStruct * ptPre = NULL;
    //for(float angle=20.0f;angle<160.0f;angle+=minAngle)
    for(float angle=30.0f;angle<150.0f;angle+=minAngle)
    {
        double results[3] = {0,0,0};

        calc_rotate_point(aoainfo, Xoffset, radius, angle - (float)aoainfo->aoaNode->pitch, results);

        if ( ptPre != NULL )
        {
            if ((int)ptPre->x == (int)results[0] &&
                (int)ptPre->y == (int)results[1] &&
                (int)ptPre->z == (int)results[2] )
            {
                // same point
                continue;
            }
            // vilslog::printRaw(",");
        }

        // vilslog::printRaw("[%d,%d,%d]", (int)results[0], (int)results[1], (int)results[2]);

        PointStruct * newPt = new PointStruct();
        newPt->x = results[0];
        newPt->y = results[1];
        newPt->z = results[2];
        ptList->push_back(newPt);

        ptPre = newPt;
        count++;
    }

    return ptList;
}

int SimulationPowerTag::calc_rotate_point(AoARangingInfoStruct * aoainfo, float Xoffset, int radius, float angle, double * results)
{
    float angleRad = angle_to_rad(angle);
    float y = radius * sin(angleRad);
    float z = radius * cos(angleRad);

    double origAoA[3] = {0,0,0};
    origAoA[0] = Xoffset;
    origAoA[1] = (double)y;
    origAoA[2] = (double)z;

    // vilslog::printRaw("SimulationPowerTag::calc_rotate_point() origAoA[%f, %f, %f]\n",
    //     origAoA[0], origAoA[1], origAoA[2]);

    calc_AoA_result(aoainfo, aoainfo->aoaNorm, origAoA, results);

    return 0;
}

int SimulationPowerTag::calc_AoA_result(AoARangingInfoStruct * aoainfo, double * normalVector, double * origAoA, double * results)
{
    int iRet = 0;

    // AoA Coordinator to VILS Coordinator
    float results_t[2] = {0,0};
    double results_totate[3] = {0,0,0};
    rotate_point(origAoA[0], origAoA[1], 0, 0, -90, results_t);
    results_totate[0] = results_t[0];
    results_totate[1] = results_t[1];
    results_totate[2] = origAoA[2];

    // vilslog::printRaw("SimulationPowerTag::calc_AoA_result() results_totate[%f, %f, %f]\n",
    //     results_totate[0], results_totate[1], results_totate[2]);

    double results_map[3] = {0,0,0};
    rotate_point(results_totate[0], results_totate[1], 0, 0, aoainfo->aoaNode->yaw, results_t);
    results_map[0] = aoainfo->aoaNode->posX + results_t[0];
    results_map[1] = aoainfo->aoaNode->posY + results_t[1];
    results_map[2] = aoainfo->aoaNode->posZ + origAoA[2];


    // vilslog::printRaw("SimulationPowerTag::calc_AoA_result() results_map[%f, %f, %f]\n",
    //     results_map[0], results_map[1], results_map[2]);


    //
    // roll angle
    //
    double result_roll[3] = {0,0,0};
    results[2] = 0;
    rotate_AoA_normal_vector(aoainfo->aoaNode->posX, aoainfo->aoaNode->posY, aoainfo->aoaNode->posZ,
            normalVector[0], normalVector[1], normalVector[2],
            aoainfo->aoaNode->roll,
            results_map, result_roll);

    results[0] = result_roll[0];
    results[1] = result_roll[1];
    results[2] = result_roll[2];

    iRet = 1;

    return iRet;
}

std::list<PointStruct *> * SimulationPowerTag::find_cross_circle_point(AoANodeStruct * aoaNode, std::list<PointStruct *> * ptList, int z_height, bool dumplog)
{
    std::list<PointStruct *> * retList = new std::list<PointStruct *>();

    for (std::list<PointStruct*>::iterator pt1 = ptList->begin(); pt1 != ptList->end(); pt1++)
    {
        double plane_pt[3] = { 0 };
        double plane_norm[3] = { 0 };
        double line_pt[3] = { 0 };
        double line_dir[3] = { 0 };
        double results[3] = { 0 };

        plane_pt[0] = 0;
        plane_pt[1] = 0;
        plane_pt[2] = z_height;

        plane_norm[0] = 0;
        plane_norm[1] = 0;
        plane_norm[2] = z_height;

        line_pt[0] = aoaNode->posX;
        line_pt[1] = aoaNode->posY;
        line_pt[2] = aoaNode->posZ;

        line_dir[0] = (*pt1)->x - aoaNode->posX;
        line_dir[1] = (*pt1)->y - aoaNode->posY;
        line_dir[2] = (*pt1)->z - aoaNode->posZ;

        bool bSuccess = findLinePlaneIntersectPoint(plane_pt, plane_norm, line_pt, line_dir, results);
        if (!bSuccess)
        {
            vilslog::printf("findLinePlaneIntersectPoint() failed!!! \n");
        }
        else
        {
            PointStruct * newPt = new PointStruct();
            newPt->x = results[0];
            newPt->y = results[1];
            newPt->z = results[2];

            if (dumplog)
            {
                if ( (int)retList->size() > 0)
                   vilslog::printRaw(",");
               vilslog::printRaw("[%d,%d,%d]", (int)results[0], (int)results[1], (int)results[2]);
            }
            retList->push_back(newPt);
        }
    }
    if (dumplog)
        vilslog::printRaw("\n");
    return retList;
}

bool SimulationPowerTag::find_AoA_position(double * AoAAnchor1, double * AoAAnchor2, double * AoAAnchor3,
        std::list<PointStruct *> * ptList1, std::list<PointStruct *> * ptList2, std::list<PointStruct *> * ptList3, double * results)
{
    double dist1n2 = TwoPointDistance(AoAAnchor1, AoAAnchor2);
    double dist1n3 = TwoPointDistance(AoAAnchor1, AoAAnchor3);
    double dist2n3 = TwoPointDistance(AoAAnchor2, AoAAnchor3);

    double Threshold = 15;
    double min_diff = 10000000;

    bool bFound = false;
    double P1[4] = {0,0,0,1};
    double P2[4] = {0,0,0,1};
    double P3[4] = {0,0,0,1};

    double P2P1Anchor[3];
    P2P1Anchor[0] = AoAAnchor2[0] - AoAAnchor1[0];
    P2P1Anchor[1] = AoAAnchor2[1] - AoAAnchor1[1];
    P2P1Anchor[2] = AoAAnchor2[2] - AoAAnchor1[2];
    double P3P1Anchor[3];
    P3P1Anchor[0] = AoAAnchor3[0] - AoAAnchor1[0];
    P3P1Anchor[1] = AoAAnchor3[1] - AoAAnchor1[1];
    P3P1Anchor[2] = AoAAnchor3[2] - AoAAnchor1[2];

    double normAnchor[3] = { 0, 0, 0 };
    crossproduct(P2P1Anchor, P3P1Anchor, normAnchor);

    for (std::list<PointStruct*>::iterator pt1 = ptList1->begin(); pt1 != ptList1->end(); pt1++)
    {
        double point1[3] = {0};
        point1[0] = (*pt1)->x;
        point1[1] = (*pt1)->y;
        point1[2] = (*pt1)->z;

        for (std::list<PointStruct*>::iterator pt2 = ptList2->begin(); pt2 != ptList2->end(); pt2++)
        {
            double point2[3] = {0};
            point2[0] = (*pt2)->x;
            point2[1] = (*pt2)->y;
            point2[2] = (*pt2)->z;

            double dist12 = TwoPointDistance(point1, point2);

            double diff12 = fabs(dist1n2 - dist12);
            if ( diff12 <= Threshold )
            {
                for (std::list<PointStruct*>::iterator pt3 = ptList3->begin(); pt3 != ptList3->end(); pt3++)
                {
                    double point3[3] = {0};
                    point3[0] = (*pt3)->x;
                    point3[1] = (*pt3)->y;
                    point3[2] = (*pt3)->z;

                    double dist13 = TwoPointDistance(point1, point3);
                    double dist23 = TwoPointDistance(point2, point3);

                    double diff13 = fabs(dist1n3 - dist13);
                    double diff23 = fabs(dist2n3 - dist23);

                    if ( diff13 <= Threshold && diff23 <= Threshold)
                    {
                        double new_diff = diff12 + diff13 + diff23;

                        if ( min_diff > new_diff )
                        {
                            //
                            // check normal vector
                            //
                            double P2P1[3] = {0};
                            P2P1[0] = point2[0] - point1[0];
                            P2P1[1] = point2[1] - point1[1];
                            P2P1[2] = point2[2] - point1[2];
                            double P3P1[3] = {0};
                            P3P1[0] = point3[0] - point1[0];
                            P3P1[1] = point3[1] - point1[1];
                            P3P1[2] = point3[2] - point1[2];

                            double norm[3] = {0};
                            crossproduct(P2P1, P3P1, norm);

                            double direction = dotproduct(normAnchor, norm);
                            if ( direction > 0 )
                            {
                                P1[0] = point1[0];
                                P1[1] = point1[1];
                                P1[2] = point1[2];

                                P2[0] = point2[0];
                                P2[1] = point2[1];
                                P2[2] = point2[2];

                                P3[0] = point3[0];
                                P3[1] = point3[1];
                                P3[2] = point3[2];

                                bFound = true;
                                min_diff = new_diff;

                                // vilslog::printRaw("\nmin_diff[%d] diff12[%d] diff13[%d] diff23[%d] direction[%f]\npoint1[%d,%d,%d] point2[%d,%d,%d] point3[%d,%d,%d]\n",
                                //     (int)min_diff, (int)diff12, (int)diff13, (int)diff23, direction, (int)point1[0], (int)point1[1], (int)point1[2],
                                //     (int)point2[0], (int)point2[1], (int)point2[2], (int)point3[0], (int)point3[1], (int)point3[2]);

                            }
                        }
                    }
                }
            }
        }
    }

    if (bFound)
    {
        //==================================
/*
        double P2P1World[3] = {0};
        P2P1World[0] = AoAAnchor2[0] - AoAAnchor1[0];
        P2P1World[1] = AoAAnchor2[1] - AoAAnchor1[1];
        P2P1World[2] = AoAAnchor2[2] - AoAAnchor1[2];
        double P3P1World[3] = {0};
        P3P1World[0] = AoAAnchor3[0] - AoAAnchor1[0];
        P3P1World[1] = AoAAnchor3[1] - AoAAnchor1[1];
        P3P1World[2] = AoAAnchor3[2] - AoAAnchor1[2];

        double normWorld[3] = {0};
        crossproduct(P2P1World, P3P1World, normWorld);

        vilslog::printRaw("P2P1World[%d,%d,%d] P3P1World[%d,%d,%d] normWorld[%f,%f,%f]\n",
                (int)P2P1World[0], (int)P2P1World[1], (int)P2P1World[2],
                (int)P3P1World[0], (int)P3P1World[1], (int)P3P1World[2],
                normWorld[0], normWorld[1], normWorld[2]);

        double XAxisWorld[3] = {0};
        double YAxisWorld[3] = {0};
        double ZAxisWorld[3] = {0};

        normalise_vector(3, P2P1World, XAxisWorld);
        normalise_vector(3, P3P1World, YAxisWorld);
        normalise_vector(3, normWorld, ZAxisWorld);

        vilslog::printRaw("XAxisWorld[%f,%f,%f] YAxisWorld[%f,%f,%f] ZAxisWorld[%f,%f,%f]\n",
                XAxisWorld[0], XAxisWorld[1], XAxisWorld[2],
                YAxisWorld[0], YAxisWorld[1], YAxisWorld[2],
                ZAxisWorld[0], ZAxisWorld[1], ZAxisWorld[2]);

        double P2P1[3] = {0};
        P2P1[0] = P2[0] - P1[0];
        P2P1[1] = P2[1] - P1[1];
        P2P1[2] = P2[2] - P1[2];
        double P3P1[3] = {0};
        P3P1[0] = P3[0] - P1[0];
        P3P1[1] = P3[1] - P1[1];
        P3P1[2] = P3[2] - P1[2];

        double norm[3] = {0};
        crossproduct(P2P1, P3P1, norm);

        double XAxisLocal[3] = {0};
        double YAxisLocal[3] = {0};
        double ZAxisLocal[3] = {0};

        normalise_vector(3, P2P1, XAxisLocal);
        normalise_vector(3, P3P1, YAxisLocal);
        normalise_vector(3, norm, ZAxisLocal);

        vilslog::printRaw("XAxisLocal[%f,%f,%f] YAxisLocal[%f,%f,%f] ZAxisLocal[%f,%f,%f]\n",
                XAxisLocal[0], XAxisLocal[1], XAxisLocal[2],
                YAxisLocal[0], YAxisLocal[1], YAxisLocal[2],
                ZAxisLocal[0], ZAxisLocal[1], ZAxisLocal[2]);

        // http://www.meshola.com/Articles/converting-between-coordinate-systems

        double R[9] = {0};
        double invR[9] = {0};
        R[0] = dotproduct(XAxisWorld, XAxisLocal);
        R[1] = dotproduct(XAxisWorld, YAxisLocal);
        R[2] = dotproduct(XAxisWorld, ZAxisLocal);
        R[3] = dotproduct(YAxisWorld, XAxisLocal);
        R[4] = dotproduct(YAxisWorld, YAxisLocal);
        R[5] = dotproduct(YAxisWorld, ZAxisLocal);
        R[6] = dotproduct(ZAxisWorld, XAxisLocal);
        R[7] = dotproduct(ZAxisWorld, YAxisLocal);
        R[8] = dotproduct(ZAxisWorld, ZAxisLocal);
        invR[0] = dotproduct(XAxisLocal, XAxisWorld);
        invR[1] = dotproduct(XAxisLocal, YAxisWorld);
        invR[2] = dotproduct(XAxisLocal, ZAxisWorld);
        invR[3] = dotproduct(YAxisLocal, XAxisWorld);
        invR[4] = dotproduct(YAxisLocal, YAxisWorld);
        invR[5] = dotproduct(YAxisLocal, ZAxisWorld);
        invR[6] = dotproduct(ZAxisLocal, XAxisWorld);
        invR[7] = dotproduct(ZAxisLocal, YAxisWorld);
        invR[8] = dotproduct(ZAxisLocal, ZAxisWorld);

        double mP1[3];
        double mP2[3];
        double mP3[3];

        double nP1[3] = {P1[0] - AoAAnchor1[0], P1[1] - AoAAnchor1[1], P1[2] - AoAAnchor1[2]};
        double nP2[3] = {P2[0] - AoAAnchor1[0], P2[1] - AoAAnchor1[1], P2[2] - AoAAnchor1[2]};
        double nP3[3] = {P3[0] - AoAAnchor1[0], P3[1] - AoAAnchor1[1], P3[2] - AoAAnchor1[2]};

        DotMatrix(3, R, nP1, mP1);
        DotMatrix(3, R, nP2, mP2);
        DotMatrix(3, R, nP3, mP3);
        vilslog::printRaw("   R mP1[%d,%d,%d] mP2[%d,%d,%d] mP3[%d,%d,%d]\n",
                (int)mP1[0] + (int)AoAAnchor1[0], (int)mP1[1] + (int)AoAAnchor1[1], (int)mP1[2] + (int)AoAAnchor1[2],
                (int)mP2[0] + (int)AoAAnchor1[0], (int)mP2[1] + (int)AoAAnchor1[1], (int)mP2[2] + (int)AoAAnchor1[2],
                (int)mP3[0] + (int)AoAAnchor1[0], (int)mP3[1] + (int)AoAAnchor1[1], (int)mP3[2] + (int)AoAAnchor1[2]);
        DotMatrix(3, invR, nP1, mP1);
        DotMatrix(3, invR, nP2, mP2);
        DotMatrix(3, invR, nP3, mP3);
        vilslog::printRaw("invR mP1[%d,%d,%d] mP2[%d,%d,%d] mP3[%d,%d,%d]\n",
                (int)mP1[0], (int)mP1[1], (int)mP1[2],
                (int)mP2[0], (int)mP2[1], (int)mP2[2],
                (int)mP3[0], (int)mP3[1], (int)mP3[2]);
*/
        //==================================
/*
        double P2P1Anchor[3];
        P2P1Anchor[0] = AoAAnchor2[0] - AoAAnchor1[0];
        P2P1Anchor[1] = AoAAnchor2[1] - AoAAnchor1[1];
        P2P1Anchor[2] = AoAAnchor2[2] - AoAAnchor1[2];
        double P3P1Anchor[3];
        P3P1Anchor[0] = AoAAnchor3[0] - AoAAnchor1[0];
        P3P1Anchor[1] = AoAAnchor3[1] - AoAAnchor1[1];
        P3P1Anchor[2] = AoAAnchor3[2] - AoAAnchor1[2];

        double normAnchor[3] = { 0, 0, 0 };
        crossproduct(P2P1Anchor, P3P1Anchor, normAnchor);
        normalise_vector(3, normAnchor, normAnchor);

        vilslog::printRaw("AoAAnchor normAnchor[%f,%f,%f]\n",  normAnchor[0], normAnchor[1], normAnchor[2]);

        double P2P1[3];
        P2P1[0] = P2[0] - P1[0];
        P2P1[1] = P2[1] - P1[1];
        P2P1[2] = P2[2] - P1[2];
        double P3P1[3];
        P3P1[0] = P3[0] - P1[0];
        P3P1[1] = P3[1] - P1[1];
        P3P1[2] = P3[2] - P1[2];

        double norm[3] = { 0, 0, 0 };
        crossproduct(P2P1, P3P1, norm);

        normalise_vector(3, norm, norm);

        vilslog::printRaw("result norm[%f,%f,%f]\n",  norm[0], norm[1], norm[2]);

        // R = B * inverse(A)

        double A[9] = {0};
        double invA[9] = {0};
        A[0] = P2P1[0];
        A[3] = P2P1[1];
        A[6] = P2P1[2];
        A[1] = P3P1[0];
        A[4] = P3P1[1];
        A[7] = P3P1[2];
        A[2] = norm[0];
        A[5] = norm[1];
        A[8] = norm[2];

        inverse_matrix(3, A, invA);

        double B[9] = {0};
        B[0] = P2P1Anchor[0];
        B[3] = P2P1Anchor[1];
        B[6] = P2P1Anchor[2];
        B[1] = P3P1Anchor[0];
        B[4] = P3P1Anchor[1];
        B[7] = P3P1Anchor[2];
        B[2] = normAnchor[0];
        B[5] = normAnchor[1];
        B[8] = normAnchor[2];


        MultiplyMatrix(3, B, invA, R);
*/

        double R[9] = {0};
        findRotationMatrix(AoAAnchor1, AoAAnchor2, AoAAnchor3, P1, P2, P3, R);

        //vilslog::printRaw("result R[3]=[%f] R[7]=[%f] R[11]=[%f]\n",  R[3], R[7], R[11]);


        //double invR[9] = {0};
        //inverse_matrix(3, R, invR);

        double mP1[3];
        double mP2[3];
        double mP3[3];
        DotMatrix(3, R, P1, mP1);
        DotMatrix(3, R, P2, mP2);
        DotMatrix(3, R, P3, mP3);

        vilslog::printRaw("mP1[%d,%d,%d] mP2[%d,%d,%d] mP3[%d,%d,%d]\n",
                (int)mP1[0], (int)mP1[1], (int)mP1[2],
                (int)mP2[0], (int)mP2[1], (int)mP2[2],
                (int)mP3[0], (int)mP3[1], (int)mP3[2]);

        double diff_1_x = AoAAnchor1[0] - mP1[0];
        double diff_1_y = AoAAnchor1[1] - mP1[1];
        double diff_1_z = AoAAnchor1[2] - mP1[2];

        // vilslog::printRaw("diff_1[%d,%d,%d]\n", (int)diff_1_x, (int)diff_1_y, (int)diff_1_z);

        double diff_2_x = AoAAnchor2[0] - mP2[0];
        double diff_2_y = AoAAnchor2[1] - mP2[1];
        double diff_2_z = AoAAnchor2[2] - mP2[2];

        // vilslog::printRaw("diff_2[%d,%d,%d]\n", (int)diff_2_x, (int)diff_2_y, (int)diff_2_z);

        double diff_3_x = AoAAnchor3[0] - mP3[0];
        double diff_3_y = AoAAnchor3[1] - mP3[1];
        double diff_3_z = AoAAnchor3[2] - mP3[2];

        // vilslog::printRaw("diff_3[%d,%d,%d]\n", (int)diff_3_x, (int)diff_3_y, (int)diff_3_z);

        results[0] = (diff_1_x + diff_2_x + diff_3_x) / 3;
        results[1] = (diff_1_y + diff_2_y + diff_3_y) / 3;
        results[2] = (diff_1_z + diff_2_z + diff_3_z) / 3;

        // double P4[3];
        // double mP4[3];
        // P4[0] = results[0];
        // P4[1] = results[1];
        // P4[2] = results[2];
        // DotMatrix(3, R, P4, mP4);
        // vilslog::printRaw("P4[%f,%f,%f] mP4[%f,%f,%f]\n", P4[0], P4[1], P4[2], mP4[0], mP4[1], mP4[2]);

        double P5[3];
        double mP5[3];
        P5[0] = 100;
        P5[1] = 0;
        P5[2] = 0;
        DotMatrix(3, R, P5, mP5);

        double angle = atan2(mP5[1], mP5[0]) * 180.0 / PI;
        // vilslog::printRaw("P5[%f,%f,%f] mP5[%f,%f,%f] angle[%f]\n", P5[0], P5[1], P5[2], mP5[0], mP5[1], mP5[2], angle);

        results[3] = angle;

        // DotMatrix(3, R, AoAAnchor1, mP1);
        // DotMatrix(3, R, AoAAnchor2, mP2);
        // DotMatrix(3, R, AoAAnchor3, mP3);
        //
        // vilslog::printRaw("mP1[%d,%d,%d] mP2[%d,%d,%d] mP3[%d,%d,%d]\n",
        //         (int)mP1[0], (int)mP1[1], (int)mP1[2],
        //         (int)mP2[0], (int)mP2[1], (int)mP2[2],
        //         (int)mP3[0], (int)mP3[1], (int)mP3[2]);

        //==============================
        // double _R[16];
        // double _invR[16];
        // findCoordTransformation(P1, P2, P3, _R, _invR);
        //
        // double mP1[4];
        // double mP2[4];
        // double mP3[4];
        // // rotate to new plane
        // DotMatrix(4, invR, P1, mP1);
        // DotMatrix(4, invR, P2, mP2);
        // DotMatrix(4, invR, P3, mP3);
        //
        // vilslog::printRaw("mP1[%d,%d,%d] mP2[%d,%d,%d] mP3[%d,%d,%d]\n",
        //         (int)mP1[0], (int)mP1[1], (int)mP1[2],
        //         (int)mP2[0], (int)mP2[1], (int)mP2[2],
        //         (int)mP3[0], (int)mP3[1], (int)mP3[2]);
        //==============================

    }

    return bFound;
}
