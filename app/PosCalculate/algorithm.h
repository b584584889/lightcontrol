/*
 * AnchorRangingDataClass.h
 *
 *  Created on: 2018/09/11
 *      Author: ikki
 */


#ifndef algorithm_H
#define algorithm_H

#include "../config.h"
#include "../common.h"

#define PI 3.14159265

bool resolve_two_anchor(double * AnchorCoordinate0, double * AnchorCoordinate1,
                        double AnchorDistance0, double AnchorDistance1, double * results);
bool resolve_tag_on_one_anchor(double * AnchorCoordinate0, double * AnchorCoordinate1,
                        double AnchorDistance0, double AnchorDistance1, double * tagcoord, double * results);
bool resolve_three_anchor(double * P1, double * P2, double * P3,
                        double distance1, double distance2, double distance3, double * results, int bGetZUP);
bool do_trilateration_3D(double * P1, double * P2, double * P3, int distance1, int distance2, int distance3, double * results, bool invZ);
bool Trilateration3D_simple(double *P1, double *P2, double *P3, double *distance, double *results);
bool do_trilateration_Orig(double * P1, double * P2, double * P3,
                        double distance1, double distance2, double distance3, double * results);

int resolveTagMapLayer(RangingResultStruct * validRangingData);

int resolve_one_anchor(double mv_direction, double rangingvlaue, double anTagDistance,
                        int moving_dir_X, int moving_dir_Y,
                        double * AnchorCoordinate, double * TagCoordinate,
                        int fixAxis, int fixCoord, int maxPos, int minPos, double * result);

bool resolve_one_anchor(double mv_direction, double rangingvlaue, double anTagDistance,
                        double * Momentum, double * AnchorCoordinate, double * TagCoordinate, double * result);
bool fix_exceed_endpoint(double * endpoint_1, double * endpoint_2, double * point);
bool check_node_on_the_line(double * endpoint_1, double * endpoint_2, double * point);

bool rotate_AoA_normal_vector(int posX, int posY, int posZ,
                                int norX, int norY, int norZ,
                                int roll,
                                double * point, double * result_roll);
bool inverse_matrix(int N, double * inMatrix, double * outMatrix);
bool dot_matrix(int rows, int cols, double * inMatrix1, double * inMatrix2, double * outMatrix);
double det(int N, int colSize, double * inMatrix);
void minorMatrix(int index, int N, int colSize, double * inMatrix, double * outMatrix);
void transpose(int N, double determinte, double * inMatrix, double * outMatrix);
void cofactor(int N, double determinte, double * inMatrix, double * outMatrix);
double find_two_line_distance(PointStruct * p1, double * vector1, PointStruct * p3, double * vector2, double * crosspt1, double * crosspt2);
void normalise_vector(int colSize, double * inMatrix, double * outMatrix);
bool findRotationMatrix(double * coord1_P1, double * coord1_P2, double * coord1_P3,
                        double * coord2_P1, double * coord2_P2, double * coord2_P3, double *R);
double angle_to_rad(double angle);
float angle_to_rad(float angle);
double rad_to_angle(double rad);
float rad_to_angle(float rad);
bool findCorrectTriangleShape(double *inP1, double *inP2, double *inP3, double *outP1, double *outP2, double *outP3, double *R, int * sequence);
bool tryCorrectTriangleShape(double *inP1, double *inP2, double *inP3, double *outP1, double *outP2, double *outP3, double *R);
bool findCoordTransformation(double *P1, double *P2, double *P3, double *R, double *invR);
double norm(double * vector);
bool crossproduct(double * vector1, double * vector2, double * resrult);
double dotproduct(double * vector1, double * vector2);
bool gluInvertMatrix(double * m, double * invOut);
bool DotMatrix(int d, double * m, double * v, double * vOut);
bool MultiplyMatrix(int d, double * m, double * v, double * vOut);
double TwoPointDistance(double * pt1, double * pt2);
bool findTwoCircleIntersecPoint(double * P1, double * P2, double Radius1, double Radius2, double * results);
bool findLinePlaneIntersectPoint(double * plane_pt, double * plane_norm, double * line_pt, double * line_dir, double * results);
bool rotate_point(float tx, float ty, float cx, float cy, float angle, float * p);
int convert_to_1D_ranging(double * TagCoordinate, double * AnchorCoordinate, int rangingvlaue, int fixAxis, int fixCoord, int maxPos, int minPos, int Mapping_Height, double * result);
bool map_to_1D(double * AnchorCoordinate, int startX, int startY, int startZ, int endX, int endY, int endZ, double * result);
bool map_to_1D_ranging(double * AnchorCoordinate, int rangingvlaue, int startX, int startY, int startZ, int endX, int endY, int endZ, double * result);

#endif //algorithm_H
