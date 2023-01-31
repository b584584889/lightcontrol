/*
 * RayCasting.h
 *
 *  Created on: 2017/05/25
 *      Author: ikki
 */

#ifndef RayCasting_H
#define RayCasting_H

#include "../common.h"

using namespace std;

class RayCasting
{
public:
    RayCasting();
    ~RayCasting();

    void start();
    void stop();
    string findNodeArea(NodeStruct * node);
    string findNodeArea(int maplayer, int posX, int posY);
    MapLayerAreaStruct* findNodeAreaStruct(int maplayer, int posX, int posY);
    MapLayerAreaStruct* findNodeAreaStructWithtype(int maplayer, int posX, int posY, int areatype);
    MapLayerAreaStruct* findNodeWallAreaStruct(int maplayer, int posX1, int posY1, int posX2, int posY2, int & edgePosX, int & edgePosY);
    //void setAreaList(list<AreaStruct*> * areaList);
    void setAreaList(list<MapLayerAreaStruct*> * areaList);
    bool checkInsideArea(const char * AreaId, int posX, int posY);
    int getAreaType(const char * AreaId);
    // void setOrigin(double WGS48_Origin_X, double WGS48_Origin_Y);

private:
    int init();
    bool mRun;
    list<MapLayerAreaStruct*> * mAreaList;
    //list<AreaStruct*> * mAreaList;
    // double mWGS48_Origin_X;
    // double mWGS48_Origin_Y;


    bool checkInsidePolygon(int posX, int posY, PolygonStruct * poly, double WGS48_Origin_X, double WGS48_Origin_Y);
    int checkHitPolygon(int posX1, int posY1, int posX2, int posY2, PolygonStruct * poly, int & edgePosX, int & edgePosY, double WGS48_Origin_X, double WGS48_Origin_Y);
    int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
        float p2_x, float p2_y, float p3_x, float p3_y, float & i_x, float & i_y);
};


#endif
