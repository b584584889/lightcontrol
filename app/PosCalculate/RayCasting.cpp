/*
 * RayCasting.cpp
 *
 *  Created on: 2017/05/25
 *      Author: ikki
 */

#include <stdlib.h>
#include <iostream>

#include "RayCasting.h"


using namespace std;

//
//
//
RayCasting::RayCasting()
{
    mAreaList = NULL;
    mRun = false;
    init();
}

RayCasting::~RayCasting()
{
}

int RayCasting::init()
{
    // mWGS48_Origin_X = 0;
    // mWGS48_Origin_Y = 0;
    return 0;
}

void RayCasting::start()
{
    mRun = true;
}

void RayCasting::stop()
{
    mRun = false;
}

//void RayCasting::setAreaList(list<AreaStruct*> * areaList)
void RayCasting::setAreaList(list<MapLayerAreaStruct*> * areaList)
{
    if (mAreaList != NULL)
    {
        clear_maplayerarea_list(mAreaList);
        delete mAreaList;
    }
    mAreaList = new list<MapLayerAreaStruct*>();

    for (std::list <MapLayerAreaStruct *>::iterator i = areaList->begin(); i != areaList->end(); i++)
    {
        MapLayerAreaStruct * area = new MapLayerAreaStruct();
        //area->projectid = (*i)->projectid;
        area->layerid = (*i)->layerid;
        area->areaid = (*i)->areaid;
        area->areaname = (*i)->areaname;
        area->datetime = (*i)->datetime;
        area->type = (*i)->type;
        area->WGS48_Origin_X = (*i)->WGS48_Origin_X;
        area->WGS48_Origin_Y = (*i)->WGS48_Origin_Y;
        //area->Polygons = (*i)->Polygons;

        area->Polygons = new list<PolygonStruct*>();

        for (list <PolygonStruct *>::iterator poly = (*i)->Polygons->begin(); poly != (*i)->Polygons->end(); poly++)
        {
            PolygonStruct * newPoly = new PolygonStruct();
            newPoly->coordinates = new std::list<PointStruct*>();

            for (list <PointStruct *>::iterator pt = (*poly)->coordinates->begin(); pt != (*poly)->coordinates->end(); pt++)
            {
                PointStruct * point = new PointStruct();
                point->x = (*pt)->x;
                point->y = (*pt)->y;

                newPoly->coordinates->push_back(point);
            }
            area->Polygons->push_back(newPoly);
        }


        mAreaList->push_back(area);
    }
    //printf("RayCasting::setAreaList() mAreaList size=%lu\n", mAreaList->size());
    //mAreaList = areaList;
}

bool RayCasting::checkInsideArea(const char * AreaId, int posX, int posY)
{
    bool bInside = false;

    if (mAreaList == NULL)
    {
        vilslog::printf("RayCasting::checkInsideArea mAreaList == NULL\n");

        return bInside;
    }

    //for (list <AreaStruct *>::iterator area = mAreaList->begin(); area != mAreaList->end(); area++)
    for (list <MapLayerAreaStruct *>::iterator area = mAreaList->begin(); area != mAreaList->end(); area++)
    {
        // if ( project_id != (*area)->projectid )
        // {
        //     continue;
        // }

        if ( strcmp((*area)->areaid.c_str(), AreaId) == 0)
        {
            std::list<PolygonStruct*> * Polygons = (*area)->Polygons;

            bInside = false;
            for (list <PolygonStruct *>::iterator poly = Polygons->begin(); poly != Polygons->end(); poly++)
            {
                bInside = checkInsidePolygon(posX, posY, (*poly), (*area)->WGS48_Origin_X, (*area)->WGS48_Origin_Y);
                if (bInside)
                    break;
            }
            break;
        }
    }

    return bInside;
}

int RayCasting::getAreaType(const char * AreaId)
{
    int area_type = 0;

    if (mAreaList == NULL)
    {
        vilslog::printf("RayCasting::getAreaType mAreaList == NULL\n");

        return area_type;
    }

    //for (list <AreaStruct *>::iterator area = mAreaList->begin(); area != mAreaList->end(); area++)
    for (list <MapLayerAreaStruct *>::iterator area = mAreaList->begin(); area != mAreaList->end(); area++)
    {
        // if ( project_id != (*area)->projectid )
        // {
        //     continue;
        // }

        if ( strcmp((*area)->areaid.c_str(), AreaId) == 0)
        {
            area_type = (*area)->type;
            break;
        }
    }

    return area_type;
}

//
// void RayCasting::setOrigin(double WGS48_Origin_X, double WGS48_Origin_Y)
// {
//     mWGS48_Origin_X = WGS48_Origin_X;
//     mWGS48_Origin_Y = WGS48_Origin_Y;
// }

string RayCasting::findNodeArea(NodeStruct * node)
{
    return findNodeArea(node->maplayer, node->posX, node->posY);
}

string RayCasting::findNodeArea(int maplayer, int posX, int posY)
{
    if (!mRun)
        return "";

    //AreaStruct * area = findNodeAreaStruct(project_id, posX, posY);
    MapLayerAreaStruct * area = findNodeAreaStruct(maplayer, posX, posY);

    if (area == NULL)
    {
        vilslog::printf("RayCasting::findNodeArea area == NULL\n");

        return "";
    }

    return area->areaid;
}

//AreaStruct * RayCasting::findNodeAreaStruct(int project_id, int posX, int posY)
MapLayerAreaStruct * RayCasting::findNodeAreaStruct(int maplayer, int posX, int posY)
{
    if (!mRun)
    {
        vilslog::printf("RayCasting::findNodeAreaStruct mRun = false\n");
        return NULL;
    }

    //
    // 先檢查 病床
    //
    //AreaStruct * area = findNodeAreaStructWithtype(project_id, posX, posY, 6);
    MapLayerAreaStruct * area = findNodeAreaStructWithtype(maplayer, posX, posY, 6);

    if ( area == NULL )
    {
        area = findNodeAreaStructWithtype(maplayer, posX, posY, -1); // find all

        return area;
    }
    else
    {
        return area;
    }
}

//AreaStruct * RayCasting::findNodeAreaStructWithtype(int project_id, int posX, int posY, int areatype)
MapLayerAreaStruct * RayCasting::findNodeAreaStructWithtype(int maplayer, int posX, int posY, int areatype)
{
    if (mAreaList == NULL)
    {
        vilslog::printf("RayCasting::findNodeAreaStructWithtype mAreaList == NULL\n");

        return NULL;
    }

    for (list <MapLayerAreaStruct *>::iterator area = mAreaList->begin(); area != mAreaList->end(); area++)
    {
        // if ( project_id != (*area)->projectid )
        // {
        //     // vilslog::printf("RayCasting::findNodeAreaStruct areaid[%s] areaname[%s] project_id[%d] != (*area)->projectid[%d]\n",
        //     //     (*area)->areaid.c_str(), (*area)->areaname.c_str(), project_id, (*area)->projectid);
        //     continue;
        // }

        if ( (areatype != -1) && (areatype != (*area)->type))
        {
            // vilslog::printf("RayCasting::findNodeAreaStruct areaid[%s] areaname[%s] project_id[%d] != (*area)->projectid[%d]\n",
            //     (*area)->areaid.c_str(), (*area)->areaname.c_str(), project_id, (*area)->projectid);
            continue;
        }

        if (maplayer != (*area)->layerid )
        {
            // vilslog::printf("RayCasting::findNodeAreaStruct areaid[%s] areaname[%s] project_id[%d] != (*area)->projectid[%d]\n",
            //     (*area)->areaid.c_str(), (*area)->areaname.c_str(), project_id, (*area)->projectid);
            continue;
        }


        // vilslog::printf("RayCasting::findNodeAreaStruct project_id[%d]  areaid[%s] areaname[%s] WGS48_Origin_X[%f] WGS48_Origin_Y[%f]\n",
        //     project_id, (*area)->areaid.c_str(), (*area)->areaname.c_str(), (*area)->WGS48_Origin_X, (*area)->WGS48_Origin_Y);

        std::list<PolygonStruct*> * Polygons = (*area)->Polygons;

        bool inside = false;
        for (list <PolygonStruct *>::iterator poly = Polygons->begin(); poly != Polygons->end(); poly++)
        {
            inside = checkInsidePolygon(posX, posY, (*poly), (*area)->WGS48_Origin_X, (*area)->WGS48_Origin_Y);
            if (inside)
                break;
        }
        if (inside)
        {
            // vilslog::printf("RayCasting::findNodeAreaStruct inside[%d]\n", inside);
            return (*area);
        }
    }
    return NULL;
}

//AreaStruct * RayCasting::findNodeWallAreaStruct(int project_id, int posX1, int posY1, int posX2, int posY2, int & edgePosX, int & edgePosY)
MapLayerAreaStruct * RayCasting::findNodeWallAreaStruct(int maplayer, int posX1, int posY1, int posX2, int posY2, int & edgePosX, int & edgePosY)
{
    if (!mRun)
        return NULL;

    if (mAreaList == NULL)
    {
        vilslog::printf("RayCasting::findNodeWallAreaStruct mAreaList == NULL\n");

        return NULL;
    }

    for (list <MapLayerAreaStruct *>::iterator area = mAreaList->begin(); area != mAreaList->end(); area++)
    {
        // if ( project_id != (*area)->projectid )
        // {
        //     //printf("RayCasting::findNodeArea project_id[%d] != (*area)->projectid[%d]\n", project_id, (*area)->projectid);
        //     continue;
        // }
        if (maplayer != (*area)->layerid)
        {
            // vilslog::printf("RayCasting::findNodeAreaStruct areaid[%s] areaname[%s] project_id[%d] != (*area)->projectid[%d]\n",
            //     (*area)->areaid.c_str(), (*area)->areaname.c_str(), project_id, (*area)->projectid);
            continue;
        }

        std::list<PolygonStruct*> * Polygons = (*area)->Polygons;

        int cn = 0;
        bool inside = false;
        for (list <PolygonStruct *>::iterator poly = Polygons->begin(); poly != Polygons->end(); poly++)
        {
            inside = checkInsidePolygon(posX2, posY2, (*poly), (*area)->WGS48_Origin_X, (*area)->WGS48_Origin_Y);
            if (inside)
            {
                printf("RayCasting::findNodeWallAreaStruct inside[%d]\n", inside);
            }

            cn = checkHitPolygon(posX1, posY1, posX2, posY2, (*poly), edgePosX, edgePosY, (*area)->WGS48_Origin_X, (*area)->WGS48_Origin_Y);
            if ((cn > 0) || inside)
            {
                printf("RayCasting::findNodeWallAreaStruct cn[%d]\n", cn);
                break;
            }
        }
        if ((cn > 0) || inside)
        {
            printf("RayCasting::findNodeWallAreaStruct cn[%d] inside[%d]\n", cn, inside);
            return (*area);
        }
    }
    return NULL;
}

bool RayCasting::checkInsidePolygon(int posX, int posY, PolygonStruct * poly, double WGS48_Origin_X, double WGS48_Origin_Y)
{
    bool inside = false;

    // convert to earth Latitude and Longitude
    // double x = centimeterTolatLng(posX);
    // double y = centimeterTolatLng(posY);
    double y = centimeterToLatitude(posY);
    double x = centimeterToLongitude(WGS48_Origin_Y, posX);

    // vilslog::printf("RayCasting::checkInsidePolygon node(%d,%d) -> (%.30f,%.30f)\n", posX, posY, x, y);

    PointStruct * ptEnd = poly->coordinates->back();
    // double xj = ptEnd->x;
    // double yj = ptEnd->y;
    double xj = (double)(ptEnd->x - WGS48_Origin_X);
    double yj = (double)(ptEnd->y - WGS48_Origin_Y);
    for (list <PointStruct *>::iterator point = poly->coordinates->begin(); point != poly->coordinates->end(); point++)
    {
        // double xi = (*point)->x;
        // double yi = (*point)->y;
        double xi = (double)((*point)->x - WGS48_Origin_X);
        double yi = (double)((*point)->y - WGS48_Origin_Y);

        // vilslog::printf("RayCasting::checkInsidePolygon xi, yi(%f,%f)\n", xi, yi);

        bool intersect = ((yi > y) != (yj > y)) && (x < (xj - xi) * (y - yi) / (yj - yi) + xi);

        // vilslog::printf("RayCasting::checkInsidePolygon (xi,yi)(%f,%f) intersect[%d]\n", xi, yi, intersect);

        if (intersect)
            inside = !inside;
        xj = xi;
        yj = yi;
    }

    // vilslog::printf("RayCasting::checkInsidePolygon inside = %d\n", inside);

    return inside;
}

//
// crossing number for a point in a polygon
//
int RayCasting::checkHitPolygon(int posX1, int posY1, int posX2, int posY2, PolygonStruct * poly, int & edgePosX, int & edgePosY, double WGS48_Origin_X, double WGS48_Origin_Y)
{
    int cn = 0;    // the  crossing number counter

    // convert to earth Latitude and Longitude
    // float x1 = (float)centimeterTolatLng(posX1);
    // float y1 = (float)centimeterTolatLng(posY1);
    // float x2 = (float)centimeterTolatLng(posX2);
    // float y2 = (float)centimeterTolatLng(posY2);

    float y1 = (float)centimeterToLatitude(posY1);
    float x1 = (float)centimeterToLongitude(WGS48_Origin_Y, posX1);
    float y2 = (float)centimeterToLatitude(posY2);
    float x2 = (float)centimeterToLongitude(WGS48_Origin_Y, posX2);


    //printf("RayCasting::checkInsidePolygon node(%f,%f) -> (%.30f,%.30f)\n", node->posX, node->posY, x, y);

    PointStruct * ptEnd = poly->coordinates->back();
    // float xj = (float)ptEnd->x;
    // float yj = (float)ptEnd->y;
    float xj = (float)(ptEnd->x - WGS48_Origin_X);
    float yj = (float)(ptEnd->y - WGS48_Origin_Y);
    for (list <PointStruct *>::iterator point = poly->coordinates->begin(); point != poly->coordinates->end(); point++)
    {
        // float xi = (float)(*point)->x;
        // float yi = (float)(*point)->y;
        float xi = (float)((*point)->x - WGS48_Origin_X);
        float yi = (float)((*point)->y - WGS48_Origin_Y);

        float crossX = 0.0f;
        float crossY = 0.0f;
        cn = get_line_intersection(x1, y1, x2, y2, xj, yj, xi, yi, crossX, crossY);
        if ( cn > 0 )
        {
            // edgePosX = latLngTocentimeter((double)crossX);
            // edgePosY = latLngTocentimeter((double)crossY);
            edgePosX = (int)crossX;
            edgePosY = (int)crossY;
            //printf("RayCasting::checkHitPolygon cn[%d] edgePos[%d,%d]\n", cn, edgePosX, edgePosY);
            break;
        }
        xj = xi;
        yj = yi;
    }
    return cn;
}

//
// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
//
// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
int RayCasting::get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
    float p2_x, float p2_y, float p3_x, float p3_y, float & i_x, float & i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    float determinant = (-s2_x * s1_y + s1_x * s2_y);
    if (determinant < std::numeric_limits<double>::epsilon())
    {
        //printf("RayCasting::get_line_intersection determinant[%f] < epsilon[%f]\n", determinant, std::numeric_limits<double>::epsilon());
        return 0; // No collision
    }

    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / determinant;
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / determinant;

    //printf("RayCasting::get_line_intersection s[%f] t[%f]\n", s, t);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        i_x = p0_x + (t * s1_x);
        i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}
