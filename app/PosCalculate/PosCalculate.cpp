
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>      // std::setprecision

#include <sys/time.h>
#include <time.h>

#include "string.h"
#include "algorithm.h"
#include "../common.h"
#include "../PosCalculate/PosCalculate.h"
#include "../json/json.h"

using namespace std;

PosCalculate::PosCalculate(DeviceManager * pDevManager, SmoothingResult  * pSmResult, wqueue<DataItem*> & CheckPointJobQueue, wqueue<DataItem*> & DeviceEventJobQueue, DBManager * dbManager)
    : mDeviceManager(pDevManager), mSmoothResult(pSmResult), mCheckPointJobQueue(CheckPointJobQueue), mDeviceEventJobQueue(DeviceEventJobQueue), mDbManager(dbManager)
{
	residuals = new ResidualsFunction();
	non_linear = new NonLinearSolver(residuals);
    mDOPresolve = new DOPResolve();

    // it kinda takes a good guess.
	double initvalue[] = {1, 1, -1};
	non_linear->setParameters(initvalue);

    mAnchorCoordinate = new double *[MAX_ANCHOR_NUMBER];
    for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    {
        mAnchorCoordinate[i] = new double[3];
    }

    // mUpdateData = false;
    mUsingSectionGroupList = false;

    bStartLogging = false;
    mTimeoutLogging = 5;

    bPrintAoACirclePoint = false;
    bDumpAoACirclePoint = false;

    initProjectRangingNodeInfo();
}

PosCalculate::~PosCalculate()
{
    delete residuals;
    delete non_linear;
    delete mDOPresolve;

    for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    {
        delete [] mAnchorCoordinate[i];
    }
    delete [] mAnchorCoordinate;

    clear_ProjectRangingNodeInfoStruct_vector(&mProjectRangingNodeInfoVector);

}

void PosCalculate::initProjectRangingNodeInfo()
{
    clear_ProjectRangingNodeInfoStruct_vector(&mProjectRangingNodeInfoVector);

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectRangingNodeInfoStruct * newInfo = createRangingNodeInfo(projectid);

        mProjectRangingNodeInfoVector.push_back(newInfo);
    }
    prjidList->clear();
    delete prjidList;
}

ProjectRangingNodeInfoStruct * PosCalculate::createRangingNodeInfo(int projectid)
{
    ProjectRangingNodeInfoStruct * newInfo = new ProjectRangingNodeInfoStruct();

    newInfo->projectid = projectid;

    clear_ranging_vector(&newInfo->mAnchorTagRangingMapVector);
    clear_SectionDeviceInfo_vector(&newInfo->mSectionDeviceInfoVector);

    newInfo->mEnableAnchor1DMapping = 1;
    newInfo->mEnableOneAnchorCalculation = 1;
    newInfo->mStayInWallTimeout = 5000;

    newInfo->mSection0DGroupList = NULL;
    newInfo->mSection1DGroupList = NULL;
    newInfo->mAreaList = NULL;
    newInfo->mAreaCenterList = NULL;
    newInfo->mRaycasting = NULL;
    newInfo->mWallList = NULL;
    newInfo->mWallRaycasting = NULL;
    newInfo->mAnchorNodeList = NULL;
    newInfo->mLocatorNodeList = NULL;

    if ( !loadAnchorNode(newInfo) )
    {
        vilslog::printf ("PosCalculate::initProjectRangingNodeInfo() projectid[%d] loadAnchorNode failed!\n", projectid);
    }

    reloadAreaList(newInfo);

    return newInfo;
}

ProjectRangingNodeInfoStruct * PosCalculate::getProjectRangingNodeInfo(int projectid)
{
    ProjectRangingNodeInfoStruct * rangingNodeInfo = NULL;
    for (unsigned int i=0; i< mProjectRangingNodeInfoVector.size(); i++)
    {
        if (mProjectRangingNodeInfoVector[i]->projectid == projectid)
        {
            rangingNodeInfo = mProjectRangingNodeInfoVector[i];
            break;
        }
    }

    if( rangingNodeInfo == NULL )
    {
        rangingNodeInfo = createRangingNodeInfo(projectid);

        mProjectRangingNodeInfoVector.push_back(rangingNodeInfo);
    }

    return rangingNodeInfo;
}

//
// load Anchors and Locators Node
//
bool PosCalculate::loadAnchorNode(ProjectRangingNodeInfoStruct * rangingNodeInfo)
{
    if (rangingNodeInfo->mAnchorNodeList != NULL)
    {
        clear_node_list(rangingNodeInfo->mAnchorNodeList);
        delete rangingNodeInfo->mAnchorNodeList;
    }

    if (rangingNodeInfo->mLocatorNodeList != NULL)
    {
        clear_node_list(rangingNodeInfo->mLocatorNodeList);
        delete rangingNodeInfo->mLocatorNodeList;
    }
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(rangingNodeInfo->projectid);
    if (baseDb == NULL)
        return false;

    rangingNodeInfo->mAnchorNodeList = baseDb->read_nodes(NODE_TYPE_ANCHOR);
    rangingNodeInfo->mLocatorNodeList = baseDb->read_nodes(NODE_TYPE_LOCATOR);

    return true;
}

int PosCalculate::reloadAreaList(ProjectRangingNodeInfoStruct * rangingNodeInfo)
{
    if (rangingNodeInfo->mRaycasting == NULL)
    {
        rangingNodeInfo->mRaycasting = new RayCasting();
    }
    rangingNodeInfo->mRaycasting->stop();

    if (rangingNodeInfo->mWallRaycasting == NULL)
    {
        rangingNodeInfo->mWallRaycasting = new RayCasting();
    }
    rangingNodeInfo->mWallRaycasting->stop();

    //printf("PosCalculate::updateAreaLlist() mRaycasting->stop()\n");

    if (rangingNodeInfo->mAreaList != NULL)
    {
        clear_maplayerarea_list(rangingNodeInfo->mAreaList);
        delete rangingNodeInfo->mAreaList;
    }

    if (rangingNodeInfo->mWallList != NULL)
    {
        clear_maplayerarea_list(rangingNodeInfo->mWallList);
        delete rangingNodeInfo->mWallList;
    }
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(rangingNodeInfo->projectid);
    if (baseDb == NULL)
        return 0;

    rangingNodeInfo->mAreaList = baseDb->read_maplayerarea_not_wall();
    rangingNodeInfo->mWallList = baseDb->read_maplayerarea_type(999);
    // vilslog::printf("PosCalculate::updateAreaLlist() mAreaList size=%lu\n", mAreaList->size());
    // vilslog::printf("PosCalculate::updateAreaLlist() mWallList size=%lu\n", mWallList->size());

    if (rangingNodeInfo->mAreaCenterList != NULL)
    {
        //printf("PosCalculate::updateAreaLlist() mAreaCenterList != NULL\n");
        clear_areacenter_list(rangingNodeInfo->mAreaCenterList);
        delete rangingNodeInfo->mAreaCenterList;
    }

    rangingNodeInfo->mAreaCenterList = new list<AreaCenterStruct*>();

    //printf("PosCalculate::updateAreaLlist() new mAreaCenterList\n");

    //VILSConfig * tmpVILSConfig = NULL;
    MapLayerStruct * tmpCurMapLayer = NULL;
    std::list<MapLayerStruct*> * maplayers = baseDb->read_maplayers();

    //
    // find area center point
    //
    for (std::list <MapLayerAreaStruct *>::iterator i = rangingNodeInfo->mAreaList->begin (); i != rangingNodeInfo->mAreaList->end (); i++)
    {
        int count = 0;
        double WGS48_xi = 0.0;
        double WGS48_yi = 0.0;

        // vilslog::printf("PosCalculate::updateAreaLlist() projectid[%d] areaid[%s] areaname[%s]\n",
        //     (*i)->projectid, (*i)->areaid.c_str(), (*i)->areaname.c_str());

        //=========================================================
        // if (tmpVILSConfig == NULL)
        // {
        //     tmpVILSConfig = createVILSConfig();
        //     baseDb->read_config((*i)->projectid, tmpVILSConfig);
        // }
        // else
        // {
        //     if ( tmpVILSConfig->projectid == (*i)->projectid )
        //     {
        //         // keep same config
        //     }
        //     else
        //     {
        //         delete tmpVILSConfig;
        //         tmpVILSConfig = createVILSConfig();
        //         baseDb->read_config((*i)->projectid, tmpVILSConfig);
        //     }
        // }
        // if (tmpVILSConfig == NULL)
        // {
        //     continue;
        // }
        // double WGS48_Origin_X = std::stod(tmpVILSConfig->WGS48_Origin_X);
        // double WGS48_Origin_Y = std::stod(tmpVILSConfig->WGS48_Origin_Y);
        //
        // (*i)->WGS48_Origin_X = WGS48_Origin_X;
        // (*i)->WGS48_Origin_Y = WGS48_Origin_Y;
        //=========================================================
        double WGS48_Origin_X = 0.0;
        double WGS48_Origin_Y = 0.0;
        for (std::list <MapLayerStruct *>::iterator maplayer = maplayers->begin (); maplayer != maplayers->end (); maplayer++)
        {
            if ( (*maplayer)->layerid == (*i)->layerid )
            {
                tmpCurMapLayer = (*maplayer);
                WGS48_Origin_X = std::stod((*maplayer)->WGS48OriginX);
                WGS48_Origin_Y = std::stod((*maplayer)->WGS48OriginY);
                break;
            }
        }
        (*i)->WGS48_Origin_X = WGS48_Origin_X;
        (*i)->WGS48_Origin_Y = WGS48_Origin_Y;
        //=========================================================

        // vilslog::printf("PosCalculate::updateAreaLlist() projectid[%d] layerid[%d] areaid[%s] areaname[%s] WGS48_Origin_X[%f] WGS48_Origin_Y[%f]\n",
        //     rangingNodeInfo->projectid, (*i)->layerid, (*i)->areaid.c_str(), (*i)->areaname.c_str(), (*i)->WGS48_Origin_X, (*i)->WGS48_Origin_Y);

        for (list <PolygonStruct *>::iterator Polygon = (*i)->Polygons->begin(); Polygon != (*i)->Polygons->end(); Polygon++)
        {
            for (list <PointStruct *>::iterator point = (*Polygon)->coordinates->begin(); point != (*Polygon)->coordinates->end(); point++)
            {
                 // vilslog::printf("PosCalculate::updateAreaLlist() (*point)->x[%f] y[%f]\n", (*point)->x, (*point)->y);

                //xi += ((*point)->x - WGS48_Origin_X);
                //yi += ((*point)->y - WGS48_Origin_Y);
                WGS48_xi += (*point)->x;
                WGS48_yi += (*point)->y;
                count++;
            }
        }
        double WGS48_xi_center = WGS48_xi / (double)count;
        double WGS48_yi_center = WGS48_yi / (double)count;

        // 22.664353, 120.304440
        // double yi_orig = LatitudeToCentimeter(WGS48_Origin_Y);
        // double xi_orig = LongitudeToCentimeter(WGS48_Origin_Y, WGS48_Origin_X);
        // double yi_center = LatitudeToCentimeter(WGS48_yi_center);
        // double xi_center = LongitudeToCentimeter(WGS48_Origin_Y, WGS48_xi_center);
        // double yi_diff = yi_center - yi_orig;
        // double xi_diff = xi_center - xi_orig;


        // double test_value_1 = (0.000097 / 0.0000089987) * 100.0;
        // //double test_value_2 = 0.000097 * 11057400.0 ;
        // double test_value_2 = 0.000097 * 11112716.2812 ;
        // vilslog::printf("PosCalculate::updateAreaLlist() test_value_1[%f] test_value_2[%f]\n", test_value_1, test_value_2);

        // vilslog::printf("PosCalculate::updateAreaLlist() xi_diff[%f] yi_diff[%f]\n", xi_diff, yi_diff);

        double WGS48_xi_center_diff_orig = WGS48_xi_center - WGS48_Origin_X;
        double WGS48_yi_center_diff_orig = WGS48_yi_center - WGS48_Origin_Y;

        // vilslog::printf("PosCalculate::updateAreaLlist() areaid[%s] areaname[%s] count[%d] WGS48_xi[%f] WGS48_yi[%f]\n",
        //     (*i)->areaid.c_str(), (*i)->areaname.c_str(), count, WGS48_xi, WGS48_yi);
        // vilslog::printf("PosCalculate::updateAreaLlist() WGS48_xi_center[%f] WGS48_yi_center[%f] WGS48_xi_center_diff_orig[%f] WGS48_yi_center_diff_orig[%f]\n",
        //             WGS48_xi_center, WGS48_yi_center, WGS48_xi_center_diff_orig, WGS48_yi_center_diff_orig);

        AreaCenterStruct * areacenter = new AreaCenterStruct();
        areacenter->TagCount = 0;
        areacenter->areaid = (*i)->areaid;
        // double center_x = latLngTocentimeter(WGS48_xi_center_diff_orig);
        // double center_y = latLngTocentimeter(WGS48_yi_center_diff_orig);
        double center_y_new = LatitudeToCentimeter(WGS48_yi_center_diff_orig);
        double center_x_new = LongitudeToCentimeter(WGS48_yi_center, WGS48_xi_center_diff_orig);
        areacenter->center.x = center_x_new;
        areacenter->center.y = center_y_new;
        rangingNodeInfo->mAreaCenterList->push_back(areacenter);

        // vilslog::printf("PosCalculate::updateAreaLlist() projectid[%d] layerid[%d] areaid[%s] areaname[%s] center_new(%f,%f)\n\n",
        //     rangingNodeInfo->projectid, (*i)->layerid, areacenter->areaid.c_str(), (*i)->areaname.c_str(), center_x_new, center_y_new);
    }
    // vilslog::printf("PosCalculate::updateAreaLlist() projectid[%d] mAreaCenterList size=%lu\n", rangingNodeInfo->projectid, rangingNodeInfo->mAreaCenterList->size());

    // if (tmpVILSConfig != NULL)
    // {
    //     delete tmpVILSConfig;
    //     tmpVILSConfig = NULL;
    // }

    //======================================
    //tmpVILSConfig = createVILSConfig();
    //baseDb->read_config(rangingNodeInfo->projectid, tmpVILSConfig);
    //double WGS48_Origin_X = std::stod(tmpVILSConfig->WGS48_Origin_X);
    //double WGS48_Origin_Y = std::stod(tmpVILSConfig->WGS48_Origin_Y);
    if (tmpCurMapLayer != NULL)
    {
        double WGS48_Origin_X = std::stod(tmpCurMapLayer->WGS48OriginX);
        double WGS48_Origin_Y = std::stod(tmpCurMapLayer->WGS48OriginY);
        for (std::list <MapLayerAreaStruct *>::iterator i = rangingNodeInfo->mWallList->begin (); i != rangingNodeInfo->mWallList->end (); i++)
        {
            (*i)->WGS48_Origin_X = WGS48_Origin_X;
            (*i)->WGS48_Origin_Y = WGS48_Origin_Y;
        }
    }
    //delete tmpVILSConfig;
    //======================================

    clear_MapLayerStruct_list(maplayers);
    delete maplayers;

    rangingNodeInfo->mRaycasting->setAreaList(rangingNodeInfo->mAreaList);
    // mRaycasting->setOrigin(WGS48_Origin_X, WGS48_Origin_Y);

    //printf("PosCalculate::updateAreaLlist() mRaycasting->start()\n");
    rangingNodeInfo->mRaycasting->start();

    rangingNodeInfo->mWallRaycasting->setAreaList(rangingNodeInfo->mWallList);
    rangingNodeInfo->mWallRaycasting->start();

    //printf("PosCalculate::updateAreaLlist() finished!\n");
    return (int)rangingNodeInfo->mAreaList->size();
}

// int PosCalculate::updateAreaList()
// {
//     mUpdateData = true;
//     return 0;
// }

int PosCalculate::reloadAreaList()
{
    int count = 0;
    for (std::vector<ProjectRangingNodeInfoStruct*>::iterator rangingNodeInfo = mProjectRangingNodeInfoVector.begin(); rangingNodeInfo != mProjectRangingNodeInfoVector.end(); rangingNodeInfo++)
    {
        count += reloadAreaList( (*rangingNodeInfo) );
    }
    return count;
}

int PosCalculate::setLoRARanging(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, int rssi, const char * timestamp)
{
    int succces_count = 0;
    char Tag_id[64];
    char Locator_id[64];

    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, Tag_macaddress));

    //int LocatorFloor = mDeviceManager->getLocatorFloorByLoRAmacaddress(projectid, LoRA_macaddress);
    //int MapLayer = mDeviceManager->getLocatorMapLayerByLoRAmacaddress(projectid, LoRA_macaddress);
    //sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, LoRA_macaddress));

    NodeStruct * locatorNode = mDeviceManager->get_Locator_node(projectid, LoRA_macaddress);

    if (strlen(Tag_macaddress) == 0 || locatorNode == NULL)
    {
        vilslog::printf("PosCalculate::setLoRARanging failed! projectid[%d] Tag_macaddress[%s] Tag_id[%s] LoRA_macaddress[%s] locatorNode == NULL\n",
                            projectid, Tag_macaddress, Tag_id, LoRA_macaddress, locatorNode == NULL);
        return 0;
    }

    int MapLayer = locatorNode->maplayer;
    sprintf(Locator_id, "%s", locatorNode->nodeid);

    mDeviceManager->setTagAlive(projectid, Tag_macaddress);
    mDeviceManager->setTagOperationMode(projectid, Tag_macaddress, OPERTAION_MODE_LORA);
    mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, LoRA_macaddress);

    if ( Distance <= 50 )
    {
        // check Heart and Weight
        //int HeartBeat = mDeviceManager->getLocatorHeartBeat(projectid, LoRA_macaddress);
        //int Weight = mDeviceManager->getLocatorWeight(projectid, LoRA_macaddress);
        mDeviceManager->setTagHeartBeat(projectid, Tag_macaddress, locatorNode->heartbeat);
        mDeviceManager->setTagWeight(projectid, Tag_macaddress, locatorNode->weight);
        mDeviceManager->setTagInfoRangingTarget(projectid, Tag_macaddress, locatorNode->nodeid);
    }
    else
    {
        mDeviceManager->setTagInfoRangingTarget(projectid, Tag_macaddress, "");
        //int HeartBeat = mDeviceManager->getLocatorHeartBeat(projectid, LoRA_macaddress);
        //int Weight = mDeviceManager->getLocatorWeight(projectid, LoRA_macaddress);
        if (locatorNode->heartbeat >= 0)
            mDeviceManager->setTagHeartBeat(projectid, Tag_macaddress, 0);
        else
            mDeviceManager->setTagHeartBeat(projectid, Tag_macaddress, -1);

        if (locatorNode->weight >= 0)
            mDeviceManager->setTagWeight(projectid, Tag_macaddress, 0);
        else
            mDeviceManager->setTagWeight(projectid, Tag_macaddress, -1);
    }

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    //
    // find this tag
    //
    bool bFound = false;
    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        // check index
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), Tag_id) == 0 )
        {
            rangingNodeInfo->mAnchorTagRangingMapVector[i]->putRanging(Locator_id, MapLayer, 1, Distance, velocity, rssi, timestamp);

            //double tagVelocity = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocity();
            double tagVelocitykm = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocitykm();
            mDeviceManager->setTagVelocity(projectid, Tag_macaddress, tagVelocitykm);

            bFound = true;
            succces_count = 1;
            break;
        }
    }

    if (!bFound)
    {
        // int imu[6]={0,0,0,0,0,0};
        double tagcoord[3] = {0,0,0};
        mDeviceManager->getTagCoord(projectid, Tag_id, tagcoord);

        vilslog::printf ("PosCalculate::setRanging() Locator_id[%s] Tag_id[%s] (%d,%d,%d) Create new tag.\n",
            Locator_id, Tag_id, (int)tagcoord[0], (int)tagcoord[1], (int)tagcoord[2]);

        AnchorTagCoupleDataClass * newMap = new AnchorTagCoupleDataClass();
        newMap->init(tagcoord[0], tagcoord[1], tagcoord[2]);
        newMap->setTagName(Tag_id);
        newMap->putRanging(Locator_id, MapLayer, 1, Distance, velocity, rssi, timestamp);

        rangingNodeInfo->mAnchorTagRangingMapVector.push_back(newMap);
        succces_count = 1;
    }

    return succces_count;
}

int PosCalculate::setLoRARangingAOA(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, float angle, int rssi, const char * timestamp)
{
    int succces_count = 0;
    char Tag_id[64];
    char Locator_id[64];

    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, Tag_macaddress));

    //int LocatorFloor = mDeviceManager->getLocatorFloorByLoRAmacaddress(projectid, LoRA_macaddress);
    int MapLayer = mDeviceManager->getLocatorMapLayerByLoRAmacaddress(projectid, LoRA_macaddress);
    sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, LoRA_macaddress));

    if (strlen(Tag_macaddress) == 0 || strlen(Locator_id) == 0)
    {
        vilslog::printf("PosCalculate::setLoRARangingAOA() failed! projectid[%d] Tag_macaddress[%s] Tag_id[%s] LoRA_macaddress[%s] Locator_id[%s]\n",
                            projectid, Tag_macaddress, Tag_id, LoRA_macaddress, Locator_id);
        return 0;
    }

    mDeviceManager->setTagAlive(projectid, Tag_macaddress);
    mDeviceManager->setTagOperationMode(projectid, Tag_macaddress, OPERTAION_MODE_LORA);
    mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, LoRA_macaddress);

    if ( Distance <= 50 )
    {
        // check Heart and Weight
        int HeartBeat = mDeviceManager->getLocatorHeartBeat(projectid, LoRA_macaddress);
        int Weight = mDeviceManager->getLocatorWeight(projectid, LoRA_macaddress);
        mDeviceManager->setTagHeartBeat(projectid, Tag_macaddress, HeartBeat);
        mDeviceManager->setTagWeight(projectid, Tag_macaddress, Weight);
    }
    else
    {
        int HeartBeat = mDeviceManager->getLocatorHeartBeat(projectid, LoRA_macaddress);
        int Weight = mDeviceManager->getLocatorWeight(projectid, LoRA_macaddress);
        if (HeartBeat >= 0)
            mDeviceManager->setTagHeartBeat(projectid, Tag_macaddress, 0);
        else
            mDeviceManager->setTagHeartBeat(projectid, Tag_macaddress, -1);

        if (Weight >= 0)
            mDeviceManager->setTagWeight(projectid, Tag_macaddress, 0);
        else
            mDeviceManager->setTagWeight(projectid, Tag_macaddress, -1);
    }

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    //
    // find this tag
    //
    bool bFound = false;
    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), Tag_id) == 0 )
        {
            rangingNodeInfo->mAnchorTagRangingMapVector[i]->putRangingAOA(Locator_id, MapLayer, 1, Distance, velocity, angle, rssi, timestamp);

            //double tagVelocity = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocity();
            double tagVelocitykm = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocitykm();
            mDeviceManager->setTagVelocity(projectid, Tag_macaddress, tagVelocitykm);

            bFound = true;
            succces_count = 1;
            break;
        }
    }

    if (!bFound)
    {
        double tagcoord[3] = {0,0,0};
        mDeviceManager->getTagCoord(projectid, Tag_id, tagcoord);

        vilslog::printf ("PosCalculate::setLoRARangingAOA() Locator_id[%s] Tag_id[%s] (%d,%d,%d) Create new tag.\n",
            Locator_id, Tag_id, (int)tagcoord[0], (int)tagcoord[1], (int)tagcoord[2]);

        AnchorTagCoupleDataClass * newMap = new AnchorTagCoupleDataClass();
        newMap->init(tagcoord[0], tagcoord[1], tagcoord[2]);
        newMap->setTagName(Tag_id);
        newMap->putRangingAOA(Locator_id, MapLayer, 1, Distance, velocity, angle, rssi, timestamp);

        rangingNodeInfo->mAnchorTagRangingMapVector.push_back(newMap);
        succces_count = 1;
    }

    return succces_count;
}

int PosCalculate::setLoRAFenceAOA(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int level_x, int level_y, int level_time, int rssi, const char * timestamp)
{
    int succces_count = 0;
    char Tag_id[64];
    char Locator_id[64];

    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, Tag_macaddress));

    int MapLayer = mDeviceManager->getLocatorMapLayerByLoRAmacaddress(projectid, LoRA_macaddress);
    sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, LoRA_macaddress));

    // vilslog::printf("PosCalculate::setLoRAFenceAOA() projectid[%d] Tag_id[%s] MapLayer[%d] Locator_id[%s] level_x[%d] level_y[%d] level_time[%d]\n",
    //                     projectid, Tag_id, MapLayer, Locator_id, level_x, level_y, level_time);

    if (strlen(Tag_macaddress) == 0 || strlen(Locator_id) == 0)
    {
        vilslog::printf("PosCalculate::setLoRAFenceAOA() failed! projectid[%d] Tag_macaddress[%s] Tag_id[%s] LoRA_macaddress[%s] Locator_id[%s]\n",
                            projectid, Tag_macaddress, Tag_id, LoRA_macaddress, Locator_id);
        return 0;
    }

    mDeviceManager->setTagAlive(projectid, Tag_macaddress);
    mDeviceManager->setTagOperationMode(projectid, Tag_macaddress, OPERTAION_MODE_LORA);
    mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, LoRA_macaddress);

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    //
    // find this tag
    //
    bool bFound = false;
    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), Tag_id) == 0 )
        {
            rangingNodeInfo->mAnchorTagRangingMapVector[i]->putFenceAOA(Locator_id, MapLayer, 1, level_x, level_y, level_time, rssi, timestamp);

            double tagVelocitykm = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocitykm();
            mDeviceManager->setTagVelocity(projectid, Tag_macaddress, tagVelocitykm);

            bFound = true;
            succces_count = 1;
            break;
        }
    }

    if (!bFound)
    {
        double tagcoord[3] = {0,0,0};
        mDeviceManager->getTagCoord(projectid, Tag_id, tagcoord);

        vilslog::printf ("PosCalculate::setLoRAFenceAOA() Locator_id[%s] Tag_id[%s] (%d,%d,%d) Create new tag.\n",
            Locator_id, Tag_id, (int)tagcoord[0], (int)tagcoord[1], (int)tagcoord[2]);

        AnchorTagCoupleDataClass * newMap = new AnchorTagCoupleDataClass();
        newMap->init(tagcoord[0], tagcoord[1], tagcoord[2]);
        newMap->setTagName(Tag_id);
        newMap->putFenceAOA(Locator_id, MapLayer, 1, level_x, level_y, level_time, rssi, timestamp);

        rangingNodeInfo->mAnchorTagRangingMapVector.push_back(newMap);
        succces_count = 1;
    }
    return succces_count;
}

int PosCalculate::setRangingDiag(int projectid, int Anchor_idx, int Tag_idx, int Ranging_idx, int Distance, int * imu,
                    int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate)
{
    int succces_count = 0;
    char Tag_id[64];
    char Anchor_id[64];

    //printf("PosCalculate::setRangingDiag() Tag_idx[%d] Anchor_idx[%d] Distance[%d]\n",
    //                     Tag_idx,  Anchor_idx, Distance);

    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, Tag_idx));
    sprintf(Anchor_id, "%s", mDeviceManager->getAnchorId(projectid, Anchor_idx));
    //int anchorFloor = mDeviceManager->getAnchorFloorById(projectid, Anchor_id);
    int MapLayer = mDeviceManager->getAnchorMapLayerById(projectid, Anchor_id);

    // vilslog::printf("PosCalculate::setRangingDiag() Tag_id[%s] Anchor_idx[%d] Anchor_id[%s] Ranging_idx[%d] Distance[%d] MapLayer[%d]\n",
    //                     Tag_id, Anchor_idx, Anchor_id, Ranging_idx, Distance, MapLayer);

    if (strlen(Tag_id) == 0 || strlen(Anchor_id) == 0)
    {
        vilslog::printf("PosCalculate::setRangingDiag failed! Tag_id[%s], Anchor_idx[%d], Anchor_id[%s]\n",
                             Tag_id, Anchor_idx, Anchor_id);
        return 0;
    }

    mDeviceManager->setTagAliveById(projectid, Tag_id);
    mDeviceManager->setTagOperationModeById(projectid, Tag_id, OPERTAION_MODE_UWB);
    mDeviceManager->setAnchorAliveById(projectid, Anchor_id);

    //printf("PosCalculate::setRanging Tag_idx[%d] Anchor_idx[%d] Ranging_idx[%d] Distance[%d]\n",
    //                    Tag_idx, Anchor_idx, Ranging_idx, Distance);
    //printf("PosCalculate::setRanging T[%d] A[%d] idx[%d] D[%d] IMU[%d,%d,%d,%d,%d,%d]\n",
    //                    Tag_idx, Anchor_idx, Ranging_idx, Distance, imu[0], imu[1], imu[2], imu[3], imu[4], imu[5]);

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    //
    // find this tag
    //
    bool bFound = false;
    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        // check index
        //if ( mAnchorTagRangingMapVector[i]->getIndex() == Tag_idx )
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), Tag_id) == 0 )
        {
            //mAnchorTagRangingMapVector[i]->setTagName(Tag_id);
            //mAnchorTagRangingMapVector[i]->putAnchorStatus(Anchor_id, mTotalAnchorCount, mActiveAnchorCount);
            rangingNodeInfo->mAnchorTagRangingMapVector[i]->putRangingDiag(Anchor_id, MapLayer, Ranging_idx, Distance, imu,
                                    PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm,
                                    RangingCount, RangingFailCount, DataRate);

            //double tagVelocity = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocity();
            double tagVelocitykm = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocitykm();
            mDeviceManager->setTagVelocityById(projectid, Tag_id, tagVelocitykm);

            bFound = true;
            succces_count = 1;
            break;
        }
    }
    //printf ("PosCalculate::setRanging() Anchor_id[%s] Tag_id[%s] Distance[%d] bFound[%d]\n",
    //                    Anchor_id, Tag_id, Distance, bFound);

    if (!bFound)
    {
        double tagcoord[3] = {0,0,0};
        mDeviceManager->getTagCoord(projectid, Tag_id, tagcoord);

        // vilslog::printf ("PosCalculate::setRangingDiag() Anchor_id[%s] Tag_id[%s] (%d,%d,%d) Create new tag.\n",
        //     Anchor_id, Tag_id, (int)tagcoord[0], (int)tagcoord[1], (int)tagcoord[2]);

        AnchorTagCoupleDataClass * newMap = new AnchorTagCoupleDataClass();
        newMap->init(tagcoord[0], tagcoord[1], tagcoord[2]);
        newMap->setTagName(Tag_id);
        newMap->putRangingDiag(Anchor_id, MapLayer, Ranging_idx, Distance, imu,
                                PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm,
                                RangingCount, RangingFailCount, DataRate);

        rangingNodeInfo->mAnchorTagRangingMapVector.push_back(newMap);
        succces_count = 1;
    }

    //printf("PosCalculate::setRangingDiag() Tag_idx[%d] succces_count[%d]\n",
    //                     Tag_idx, succces_count);

    return succces_count;
}

int PosCalculate::setRangingDiag(int projectid, std::string AnchorMac, std::string TagMac, int Ranging_idx, int Distance, int * imu,
                    int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate)
{
    int succces_count = 0;
    char Tag_id[64];
    char Anchor_id[64];

    // vilslog::printf("PosCalculate::setRangingDiag() AnchorMac[%s] TagMac[%s] Distance[%d]\n",
    //                     AnchorMac.c_str(), TagMac.c_str(), Distance);

    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, TagMac.c_str()));
    sprintf(Anchor_id, "%s", mDeviceManager->getAnchorId(projectid, AnchorMac.c_str()));
    // int anchorFloor = mDeviceManager->getAnchorFloor(projectid, AnchorMac.c_str());
    int MapLayer = mDeviceManager->getAnchorMapLayer(projectid, AnchorMac.c_str());

    // vilslog::printf("PosCalculate::setRangingDiag() TagMac[%s] Tag_id[%s] AnchorMac[%s] Anchor_id[%s] Ranging_idx[%d] Distance[%d] MapLayer[%d]\n",
    //                     TagMac.c_str(), Tag_id, AnchorMac.c_str(), Anchor_id, Ranging_idx, Distance, MapLayer);

    if (strlen(Tag_id) == 0 || strlen(Anchor_id) == 0)
    {
        //
        // maybe is locator
        //
        sprintf(Anchor_id, "%s", mDeviceManager->getLocatorId(projectid, AnchorMac.c_str()));
        if (strlen(Tag_id) == 0 || strlen(Anchor_id) == 0)
        {
            vilslog::printf("PosCalculate::setRangingDiag failed! TagMac[%s] Tag_id[%s] AnchorMac[%s] Anchor_id[%s]\n",
                        TagMac.c_str(), Tag_id, AnchorMac.c_str(), Anchor_id);
            return 0;
        }
        MapLayer = mDeviceManager->getLocatorMapLayer(projectid, AnchorMac.c_str());
        mDeviceManager->setLocatorAlive(projectid, AnchorMac.c_str());
    }
    else
    {
        mDeviceManager->setAnchorAlive(projectid, AnchorMac.c_str());
    }

    mDeviceManager->setTagAlive(projectid, TagMac.c_str());
    mDeviceManager->setTagOperationMode(projectid, TagMac.c_str(), OPERTAION_MODE_UWB);
    //mDeviceManager->setAnchorAlive(AnchorMac.c_str());


    // printf("PosCalculate::setRanging idx[%d] D[%d] IMU[%d,%d,%d,%d,%d,%d]\n",
    //                    Ranging_idx, Distance, imu[0], imu[1], imu[2], imu[3], imu[4], imu[5]);

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    //
    // find this tag
    //
    bool bFound = false;
    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        //if ( mAnchorTagRangingMapVector[i]->getIndex() == Tag_idx )
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), Tag_id) == 0 )
        {
            //mAnchorTagRangingMapVector[i]->setTagName(Tag_id);
            //mAnchorTagRangingMapVector[i]->putAnchorStatus(Anchor_id, mTotalAnchorCount, mActiveAnchorCount);
            rangingNodeInfo->mAnchorTagRangingMapVector[i]->putRangingDiag(Anchor_id, MapLayer, Ranging_idx, Distance, imu,
                                    PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm,
                                    RangingCount, RangingFailCount, DataRate);

            //double tagVelocity = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocity();
            double tagVelocitykm = rangingNodeInfo->mAnchorTagRangingMapVector[i]->getVelocitykm();
            mDeviceManager->setTagVelocity(projectid, TagMac.c_str(), tagVelocitykm);

            bFound = true;
            succces_count = 1;
            break;
        }
    }
    // printf ("PosCalculate::setRanging() Anchor_id[%s] Tag_id[%s] Distance[%d] bFound[%d]\n",
    //                    Anchor_id, Tag_id, Distance, bFound);

    if (!bFound)
    {
        double tagcoord[3] = {0,0,0};
        mDeviceManager->getTagCoord(projectid, Tag_id, tagcoord);

        vilslog::printf ("PosCalculate::setRangingDiag() Anchor_id[%s] Tag_id[%s] (%d,%d,%d) Create new tag.\n",
            Anchor_id, Tag_id, (int)tagcoord[0], (int)tagcoord[1], (int)tagcoord[2]);

        AnchorTagCoupleDataClass * newMap = new AnchorTagCoupleDataClass();
        newMap->init(tagcoord[0], tagcoord[1], tagcoord[2]);
        newMap->setTagName(Tag_id);
        newMap->putRangingDiag(Anchor_id, MapLayer, Ranging_idx, Distance, imu,
                                PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm,
                                RangingCount, RangingFailCount, DataRate);

        rangingNodeInfo->mAnchorTagRangingMapVector.push_back(newMap);
        succces_count = 1;
    }

    //printf("PosCalculate::setRangingDiag() Tag_idx[%d] succces_count[%d]\n",
    //                     Tag_idx, succces_count);

    return succces_count;
}

void PosCalculate::setSectionGroupList(int projectid, std::list<SectionGroup *> * sectionGroupList)
{
    while(mUsingSectionGroupList)
    {
        // wait finish
        // vilslog::printf ("setSectionGroupList() projectid[%d] mUsingSectionGroupList[%d]\n", projectid, mUsingSectionGroupList);
        usleep(10000);// 10 ms
    }
    //
    // copy list
    //
    std::list<SectionGroup *> * newSection0DGroupList = new std::list<SectionGroup *>();
    std::list<SectionGroup *> * newSection1DGroupList = new std::list<SectionGroup *>();

    for (std::list<SectionGroup *>::iterator sectiongroup = sectionGroupList->begin(); sectiongroup != sectionGroupList->end(); sectiongroup++)
    {
        SectionGroup * newSectiongroup = new SectionGroup();
        newSectiongroup->GroupID = (*sectiongroup)->GroupID;
        newSectiongroup->projectid = (*sectiongroup)->projectid;
        newSectiongroup->SectionType = (*sectiongroup)->SectionType;
        newSectiongroup->AreaID = (*sectiongroup)->AreaID;
        newSectiongroup->Param1 = (*sectiongroup)->Param1;
        newSectiongroup->Param2 = (*sectiongroup)->Param2;
        newSectiongroup->Param3 = (*sectiongroup)->Param3;
        newSectiongroup->Param4 = (*sectiongroup)->Param4;
        newSectiongroup->Param5 = (*sectiongroup)->Param5;
        newSectiongroup->Param6 = (*sectiongroup)->Param6;
        newSectiongroup->maingroupid = (*sectiongroup)->maingroupid;
        newSectiongroup->nodeIDList = new std::list<std::string>();

        for (std::list<std::string>::iterator anchorID = (*sectiongroup)->nodeIDList->begin(); anchorID != (*sectiongroup)->nodeIDList->end(); anchorID++)
        {
            newSectiongroup->nodeIDList->push_back((*anchorID).c_str());
        }

        if( newSectiongroup->SectionType == SECTION_TYPE_ZERO )
        {
            // vilslog::printf ("setSectionGroupList() SECTION_TYPE_ZERO AreaID[%s] Param1[%d]\n", newSectiongroup->AreaID.c_str(), newSectiongroup->Param1);

            newSection0DGroupList->push_back(newSectiongroup);
        }
        else
        if( newSectiongroup->SectionType == SECTION_TYPE_ONE )
        {
            newSection1DGroupList->push_back(newSectiongroup);
        }
        else
        {
            // not valid group
            newSectiongroup->nodeIDList->clear();
            delete newSectiongroup->nodeIDList;
            delete newSectiongroup;
        }
    }

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    std::list<SectionGroup *> * oldSection0DGroupList = rangingNodeInfo->mSection0DGroupList;
    std::list<SectionGroup *> * oldSection1DGroupList = rangingNodeInfo->mSection1DGroupList;

    rangingNodeInfo->mSection0DGroupList = newSection0DGroupList;
    rangingNodeInfo->mSection1DGroupList = newSection1DGroupList;

    // release old one
    if (oldSection0DGroupList != NULL)
    {
        clear_SectionGroup_list(oldSection0DGroupList);
        delete oldSection0DGroupList;
    }
    if (oldSection1DGroupList != NULL)
    {
        clear_SectionGroup_list(oldSection1DGroupList);
        delete oldSection1DGroupList;
    }
    //===========================
}

int PosCalculate::getRangingTagCount(int projectid)
{
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    return (int)rangingNodeInfo->mAnchorTagRangingMapVector.size();
}

char * PosCalculate::getTagId(int projectid, int tagIndexInVector)
{
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);
    return rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName();
    // return mDeviceManager->getTagId(Tag_idx);
}

bool PosCalculate::getTagInfo(int projectid, int tagIndexInVector, char * TagId, double * IMU)
{
    bool bFound = false;
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);
    if (rangingNodeInfo != NULL)
    {
        bFound = true;
        std::string tagName = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName();
        memcpy(TagId, tagName.c_str(), 32);
        rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getIMU(IMU, 6);
    }
    return bFound;
    // return rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName();
}

bool PosCalculate::getIMU(int projectid, const char * TagId, double * IMU)
{
    bool bFound = false;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), TagId) == 0 )
        {
            bFound = true;
            rangingNodeInfo->mAnchorTagRangingMapVector[i]->getIMU(IMU, 6);
            break;
        }
    }

    return bFound;
}

// return 0: found 0D Section
// return 1: found 1D Section
// return 2: found 2D/3D position
// return -1: not found
int PosCalculate::rangingResult(int projectid, const char * TagId, double * results, int usingIMU, int IMUOffset, int SteadyStateOffset, int enable_GDOP, int GDOP_Threshold, int validdatatime, int enable_FixHeight)
{
    double preresults[3];
    bool sectionCheckFound = false;
    int tagIndexInVector = -1;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        // check index
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), TagId) == 0 )
        {
            tagIndexInVector = i;
            break;
        }
    }

    // if (mUpdateData)
    // {
    //     // wait finish
    //     vilslog::printf ("PosCalculate::rangingResult() mUpdateData[%d] reloadAreaList() \n", mUpdateData);
    //     //usleep(10000);// 10 ms
    //     reloadAreaList();
    //     mUpdateData = false;
    // }
    //
    // fetch previous result
    //
    mDeviceManager->getTagCoord(projectid, TagId, preresults);

    if (tagIndexInVector < 0)
        return -1;

    // vilslog::printf ("PosCalculate::rangingResult() TagId[%s] preresults[%d,%d]\n", TagId, (int)preresults[0], (int)preresults[1]);

    //RangingResultStruct validRangingData;
    RangingResultStruct * curValidRangingData = new RangingResultStruct();
    RangingResultStruct * curValidRangingData0D = new RangingResultStruct();
    int valid = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->prepareRangingData(curValidRangingData, validdatatime);
    int valid0D = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->prepareRangingData(curValidRangingData0D, 10000);

    if (valid <= 0 || valid0D <= 0 )
    {
        delete curValidRangingData;
        delete curValidRangingData0D;
        return -1;
    }

    // send to CheckPoint Queue
    send_to_Listener_queue(projectid, TagId, curValidRangingData);

    //printf ("rangingResult() tagIndexInVector[%d] valid[%d]\n", tagIndexInVector, valid);

    mUsingSectionGroupList = true;

    // return 0: found 0D Section
    // return 1: found 1D Section
    // return -1: not found
    int retSection = do_section_check(projectid, TagId, curValidRangingData, curValidRangingData0D);
    if (retSection >= 0)
        sectionCheckFound = true;

    // vilslog::printf ("PosCalculate::rangingResult() TagId[%s] sectionCheckFound[%d]\n", TagId, sectionCheckFound);
    if ( sectionCheckFound )
    {
        mDeviceManager->getTagCoord(projectid, TagId, results);
        rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setRawResult(results, 3);
        rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setResult(results, 3);

        //vilslog::printf ("PosCalculate::rangingResult() sectionCheckFound TagId[%s] TagCoord[%d, %d, %d]\n", TagId, (int)results[0], (int)results[1], (int)results[2]);
    }
    else
    {
        //vilslog::printf ("PosCalculate::rangingResult() TagId[%s] section Not Found!!\n", TagId);
    }
    mUsingSectionGroupList = false;

    int valid_AOA = do_AOA_check(projectid, TagId, curValidRangingData);

    int valid_3d_check = 0;

    if ( (sectionCheckFound != true) && (valid_AOA != 1) && (valid >= 3) )
    {
        valid_3d_check = do_3D_check(projectid, TagId, tagIndexInVector, curValidRangingData, usingIMU, IMUOffset, SteadyStateOffset, enable_GDOP, GDOP_Threshold, validdatatime, enable_FixHeight);
    }

    int tagMapLayer = resolveTagMapLayer(curValidRangingData);
    mDeviceManager->setTagInfoMapLayerById(projectid, TagId, tagMapLayer);

    // vilslog::printf ("rangingResult() TagId[%s] tagMapLayer[%d]\n", TagId, tagMapLayer);

    if ( sectionCheckFound || valid_3d_check > 0 || valid_AOA == 1)
    {
        // int tagFloor = resolveTagFloor(curValidRangingData);
        //int currentProjectId = resolveProjectId(tagFloor);

        mDeviceManager->setTagInfoGPSById(projectid, TagId, -1.0, -1.0);
        mDeviceManager->getTagCoord(projectid, TagId, results);

        //printf ("rangingResult() tagFloor[%d] currentProjectId[%d]\n", tagFloor, currentProjectId);
        // vilslog::printf ("rangingResult() TagId[%s] TagCoord[%d, %d, %d] -> [%d, %d, %d]\n",
        //     TagId, (int)preresults[0], (int)preresults[1], (int)preresults[2], (int)results[0], (int)results[1], (int)results[2]);

        int edgePosX = preresults[0];
        int edgePosY = preresults[1];
        MapLayerAreaStruct * area = rangingNodeInfo->mWallRaycasting->findNodeWallAreaStruct(tagMapLayer, preresults[0], preresults[1], results[0], results[1], edgePosX, edgePosY);
        if (area != NULL)
        {
            double InWallResult[3];

            if ( preresults[0] > results[0] )
                InWallResult[0] = edgePosX + 1;
            else
                InWallResult[0] = edgePosX -1;

            if ( preresults[1] > results[1] )
                InWallResult[1] = edgePosY + 1;
            else
                InWallResult[1] = edgePosY - 1;

            InWallResult[2] = results[2];

            mDeviceManager->setTagInWall(projectid, TagId, InWallResult[0], InWallResult[1], InWallResult[2], area);

            bool timeout = mDeviceManager->checkTagInWallTimeout(projectid, TagId, rangingNodeInfo->mStayInWallTimeout);
            if (timeout)
            {
                //printf ("rangingResult() timeout[%d]\n", (int)timeout);
            }
            else
            {
                int tagMapLayer = resolveTagMapLayer(curValidRangingData);
                mDeviceManager->setTagInfoS(projectid, TagId, InWallResult[0], InWallResult[1], InWallResult[2], tagMapLayer);
                mDeviceManager->setTagRangingResultType(projectid, TagId, 3);

                rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setRawResult(InWallResult, 3);
                rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setResult(InWallResult, 3);
            }
        }
        else
        {
            // int tagMapLayer = resolveTagMapLayer(curValidRangingData);
            // mDeviceManager->setTagInfoMapLayerById(projectid, TagId, tagMapLayer);
        }
    }
    else
    {
        // int tagMapLayer = resolveTagMapLayer(curValidRangingData);
        // mDeviceManager->setTagInfoMapLayerById(projectid, TagId, tagMapLayer);
    }

    if (bStartLogging)
    {
        static time_t start_time_t = getCurrentTimeInSec();
        static int count = 0;
        double rawresult[3];
        int smresult[3];

        if (count == 0)
        {
            //
            // dump anchors
            //
            //loadAnchorNode();
            dump_anchor_info();

            start_time_t = getCurrentTimeInSec();
            LOGGER->startdumpCSV("ranging");
            dump_ranging_result_header();
        }
        // vilslog::printf ("rangingResult() \n");

        mDeviceManager->getTagCoord(projectid, TagId, results);
        rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getRawResult(rawresult, 3);
        mSmoothResult->getResult(projectid, TagId, smresult);

        // vilslog::printf ("rangingResult() RawResult[%d, %d, %d]\n", (int)rawresult[0], (int)rawresult[1], (int)rawresult[2]);

        dump_ranging_result_data(tagIndexInVector, rawresult, results, smresult, curValidRangingData);

        time_t cur_time_t = getCurrentTimeInSec();

        double diifTime = std::difftime(cur_time_t, start_time_t);
        count++;
        if (count >= 10000 || diifTime > mTimeoutLogging)
        {
            LOGGER->stopdumpCSV();
            bStartLogging = false;
            count = 0;
        }
    }

    delete curValidRangingData;
    delete curValidRangingData0D;

    if ( sectionCheckFound )
    {
        return retSection;
    }
    else
    if ( valid_3d_check > 0 )
    {
        return 2;
    }
    else
    if ( valid_AOA == 1 )
    {
        return 2;
    }

    return -1;
}

int PosCalculate::getRawResult(int projectid, const char * TagId, double * rawresult)
{
    int tagIndexInVector = -1;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        // check index
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), TagId) == 0 )
        {
            tagIndexInVector = i;
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getRawResult(rawresult, 3);
            break;
        }
    }

    return tagIndexInVector;
}

int PosCalculate::setRawResult(int projectid, const char * TagId, double * rawresult)
{
    int tagIndexInVector = -1;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        // check index
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), TagId) == 0 )
        {

            // vilslog::printf("PosCalculate::setRawResult() TagId[%s] projectid[%d] rawresult[%d, %d, %d]\n",
            //                    TagId, projectid, (int)rawresult[0], (int)rawresult[1], (int)rawresult[2]);

            tagIndexInVector = i;
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setRawResult(rawresult, 3);
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setResult(rawresult, 3);
            break;
        }
    }

    return tagIndexInVector;
}
// int PosCalculate::checkTagArea(int projectid, NodeStruct * node)
// {
//     ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);
//
//     int results[3];
//     results[0] = node->smoothingPosX;
//     results[1] = node->smoothingPosY;
//     results[2] = node->smoothingPosZ;
//     AreaStruct * area = rangingNodeInfo->mRaycasting->findNodeAreaStruct(projectid, results[0], results[1]);
//     if (area == NULL)
//     {
//         //mDeviceManager->setTagInfoArea(projectid, node->nodeid, "", "", 0, "");// normal
//         mDeviceManager->setTagInfoArea(projectid, node, NULL, "");
//     }
//     else
//     {
//         bool bInside = false;
//         if ( strlen(node->AreaId) > 0 )
//         {
//             // check if still inside current area
//             bInside = rangingNodeInfo->mRaycasting->checkInsideArea(projectid, node->AreaId, results[0], results[1]);
//         }
//
//         if ( area->areaid.compare(node->exclude_rollcall_AreaId) == 0 )
//         {
//             vilslog::printf("PosCalculate::checkTagArea() Is exclude_AreaId[%s] TagId[%s] projectid[%d] areaid[%s] areaname[%s]\n",
//                    node->exclude_rollcall_AreaId, node->nodeid, projectid, area->areaid.c_str(), area->areaname.c_str());
//             return 0;
//         }
//
//         if (bInside)
//         {
//             //mDeviceManager->setTagInfoArea(projectid, node->nodeid, area->areaid.c_str(), area->areaname.c_str(), 0, node->AreaId);// normal
//             mDeviceManager->setTagInfoArea(projectid, node, area, node->AreaId);
//         }
//         else
//         {
//             //mDeviceManager->setTagInfoArea(projectid, node->nodeid, area->areaid.c_str(), area->areaname.c_str(), 0, "");// normal
//             mDeviceManager->setTagInfoArea(projectid, node, area, "");
//         }
//     }
//     return 0;
// }

int PosCalculate::checkLocatorArea(int projectid, NodeStruct * node)
{
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);
    int results[3];
    results[0] = node->posX;
    results[1] = node->posY;
    results[2] = node->posZ;

    // vilslog::printf ("PosCalculate::checkLocatorArea() nodeid[%s] [%d,%d,%d]\n",
    //     node->nodeid, (int)results[0], (int)results[1], (int)results[2]);

    MapLayerAreaStruct * area = rangingNodeInfo->mRaycasting->findNodeAreaStruct(node->maplayer, results[0], results[1]);
    if (area == NULL)
    {
        mDeviceManager->setLocatorInfoArea(projectid, node->nodeid, "", "");
    }
    else
    {
        mDeviceManager->setLocatorInfoArea(projectid, node->nodeid, area->areaid.c_str(), area->areaname.c_str());
        // vilslog::printf ("PosCalculate::checkLocatorArea() area->areaid[%s]\n", area->areaid.c_str());

        // vilslog::printf("PosCalculate::checkLocatorArea() nodeid[%s] AreaId[%s] AreaName[%s]\n",
        //        node->nodeid, node->AreaId, node->AreaName);
    }

    //
    // 檢查是否在病房內
    //
    MapLayerAreaStruct * areaType5 = rangingNodeInfo->mRaycasting->findNodeAreaStructWithtype(node->maplayer, results[0], results[1], 5);
    if (areaType5 == NULL)
    {
        mDeviceManager->setLocatorInfoAreaType5(projectid, node->nodeid, "", "");
    }
    else
    {
        mDeviceManager->setLocatorInfoAreaType5(projectid, node->nodeid, areaType5->areaid.c_str(), areaType5->areaname.c_str());
    }
    return 0;
}

std::string PosCalculate::getRangingString(int projectid, int validdatatime)
{
    std::ostringstream sendstream;
    int tagIndexInVector = 0;

    //printf("PosCalculate::getRangingString() validdatatime[%d]\n", validdatatime);
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        tagIndexInVector = i;
        RangingResultStruct * validRangingData = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getLastRangingData();
        int validCnt = validRangingData->validCount;

        //printf("PosCalculate::getRangingString() validCnt[%d]\n", validCnt);

        if ( validCnt > 0 )
        {
            sendstream<<rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName()<<"=[{";
            for(int j=0;j<validCnt;j++)
            {
                if (j > 0)
                    sendstream<<"},{";

                char TagAngle[32];
                memset(TagAngle, 0, 32);
                if (validRangingData->TagLevel_x[j] == 0 & validRangingData->TagLevel_y[j] == 0 & validRangingData->TagLevel_time[j] == 0)
                {
                    snprintf(TagAngle, 32, "%.02f", validRangingData->TagAngle[j]);
                }
                else
                {
                    snprintf(TagAngle, 32, "%d,%d,%d", validRangingData->TagLevel_x[j], validRangingData->TagLevel_y[j], validRangingData->TagLevel_time[j]);
                }

                sendstream<<"\""<<validRangingData->AnchorName[j]<<"\":\""<<validRangingData->RangingFiltered[j]<<"\",";
                //sendstream<<"\"a\":\""<< std::fixed << std::setprecision(2) <<validRangingData->TagAngle[j]<<"\",";
                sendstream<<"\"a\":\""<< TagAngle<<"\",";
                sendstream<<"\"time\":\""<<time_tToString(validRangingData->DataTimeStamp[j])<<"\"";
            }
        }
        else
        {
            RangingResultStruct validRangingData;
            validCnt = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->prepareAllRangingData(&validRangingData, validdatatime);

            //printf("PosCalculate::getRangingString() prepareAllRangingData validCnt[%d]\n", validCnt);

            if ( validCnt > 0 )
                sendstream<<rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName()<<"=[{";
            for(int j=0;j<validCnt;j++)
            {
                if (j > 0)
                    sendstream<<"},{";

                char TagAngle[32];
                memset(TagAngle, 0, 32);
                if (validRangingData.TagLevel_x[j] == 0 & validRangingData.TagLevel_y[j] == 0 & validRangingData.TagLevel_time[j] == 0)
                {
                    snprintf(TagAngle, 32, "%.02f", validRangingData.TagAngle[j]);
                }
                else
                {
                    snprintf(TagAngle, 32, "%d,%d,%d", validRangingData.TagLevel_x[j], validRangingData.TagLevel_y[j], validRangingData.TagLevel_time[j]);
                }

                sendstream<<"\""<<validRangingData.AnchorName[j]<<"\":\""<<validRangingData.RangingFiltered[j]<<"\",";
                //sendstream<<"\"a\":\""<<std::fixed << std::setprecision(2) <<validRangingData.TagAngle[j]<<"\",";
                sendstream<<"\"a\":\""<< TagAngle<<"\",";
                sendstream<<"\"time\":\""<<time_tToString(validRangingData.DataTimeStamp[j])<<"\"";
                // sendstream<<"\"time\":\""<<time_tToString(validRangingData.DataTimeInMS[j]/1000)<<"\"";
            }
        }

        if ( validCnt > 0 )
            sendstream<<"}]"<<endl;
    }
    std::string strRanging = sendstream.str();
    return strRanging;
}

std::string PosCalculate::getRangingStringEx(int projectid, int validdatatime)
{
    std::ostringstream sendstream;
    int tagIndexInVector = 0;

    //printf("PosCalculate::getRangingString() validdatatime[%d]\n", validdatatime);
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        tagIndexInVector = i;
        RangingResultStruct * validRangingData = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getLastRangingData();
        int validCnt = validRangingData->validCount;

        //printf("PosCalculate::getRangingString() validCnt[%d]\n", validCnt);

        if ( validCnt > 0 )
        {
            sendstream<<rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName()<<"={";
            for(int j=0;j<validCnt;j++)
            {
                if (j > 0)
                    sendstream<<",";
                sendstream<<"\""<<validRangingData->AnchorName[j]<<"\":\""<<validRangingData->RangingFiltered[j]<<"\"";
            }
         }
        else
        {
            RangingResultStruct validRangingData;
            validCnt = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->prepareAllRangingData(&validRangingData, validdatatime);

            //printf("PosCalculate::getRangingString() prepareAllRangingData validCnt[%d]\n", validCnt);

            if ( validCnt > 0 )
                sendstream<<rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName()<<"={";
            for(int j=0;j<validCnt;j++)
            {
                if (j > 0)
                    sendstream<<",";
                sendstream<<"\""<<validRangingData.AnchorName[j]<<"\":\""<<validRangingData.RangingFiltered[j]<<"\"";
            }
        }

        if ( validCnt > 0 )
            sendstream<<"}"<<endl;
    }
    std::string strRanging = sendstream.str();
    return strRanging;
}

std::string PosCalculate::getAoARangingCloudPoint(int projectId)
{
    //vilslog::printf("PosCalculate::getAoARangingCloudPoint() projectId[%d]\n", projectId);

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectId);

    //vilslog::printf("PosCalculate::getAoARangingCloudPoint() 1\n");
    return rangingNodeInfo->mAoARangingCloudPointString;
}

std::string PosCalculate::getAoAAngleVectorCloudPoint(int projectId)
{
    //vilslog::printf("PosCalculate::getAoAAngleVectorCloudPoint() projectId[%d]\n", projectId);

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectId);

    //vilslog::printf("PosCalculate::getAoAAngleVectorCloudPoint() 1\n");

    return rangingNodeInfo->mAoAAngleVectorCloudPointString;
}

std::string PosCalculate::getAoAAngleCloudPoint(int projectId)
{
    // vilslog::printf("PosCalculate::getAoAAngleCloudPoint() projectId[%d]\n", projectId);

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectId);

    // vilslog::printf("PosCalculate::getAoAAngleCloudPoint() 1\n");

    return rangingNodeInfo->mAoAAngleCloudPointString;
}

bool PosCalculate::do_trilateration_3D_new(int projectid, RangingResultStruct * validRangingData, double * results, bool invZ)
{
    double P1[3];
	double P2[3];
	double P3[3];
    double P4[3];
    int tdistance1 = 0;
    int tdistance2 = 0;
    int tdistance3 = 0;

#ifdef PRINT_RESULT_trilateration_3D_new
    double *tP1, *tP2, *tP3;
#endif

    if (validRangingData->validCount < 3)
        return false;


    //printf ("do_trilateration_3D_new() %s[%d] %s[%d] %s[%d] \n",
    //    validRangingData->AnchorName[0], validRangingData->Ranging[0],
    //    validRangingData->AnchorName[1], validRangingData->Ranging[1],
    //    validRangingData->AnchorName[2], validRangingData->Ranging[2]);

    int distance1 = validRangingData->RangingFiltered[0];
    int distance2 = validRangingData->RangingFiltered[1];
    int distance3 = validRangingData->RangingFiltered[2];


    //POINT 1
    bool bFound1 = mDeviceManager->getAnchorCoord(projectid, validRangingData->AnchorName[0], P1);
    if(!bFound1)
    {
        // maybe is locator
        bFound1 = mDeviceManager->getLocatorCoord(projectid, validRangingData->AnchorName[0], P1);
    }
    //POINT 2
    bool bFound2 = mDeviceManager->getAnchorCoord(projectid, validRangingData->AnchorName[1], P2);
    if(!bFound2)
    {
        // maybe is locator
        bFound2 = mDeviceManager->getLocatorCoord(projectid, validRangingData->AnchorName[1], P2);
    }
    //POINT 3
    bool bFound3 = mDeviceManager->getAnchorCoord(projectid, validRangingData->AnchorName[2], P3);
    if(!bFound3)
    {
        // maybe is locator
        bFound3 = mDeviceManager->getLocatorCoord(projectid, validRangingData->AnchorName[2], P3);
    }

    if (!bFound1 || !bFound2 || !bFound3)
    {
#ifdef PRINT_RESULT_trilateration_3D_new
        vilslog::printf ("do_trilateration_3D_new() 0 bFound1[%d] bFound2[%d] bFound3[%d]\n", bFound1, bFound2, bFound3);
#endif
        return false;
    }

    int distance4 = 0;
    bool bFound4 = false;
    if (validRangingData->validCount > 3)
    {
        distance4 = validRangingData->RangingFiltered[3];
        bFound4 = mDeviceManager->getAnchorCoord(projectid, validRangingData->AnchorName[3], P4);
        if(!bFound4)
        {
            // maybe is locator
            bFound4 = mDeviceManager->getLocatorCoord(projectid, validRangingData->AnchorName[3], P4);
        }
    }

    if (validRangingData->validCount > 3 && bFound4)
    {
        double results1[3] = {0, 0, 0};
        double results2[3] = {0, 0, 0};
        double results3[3] = {0, 0, 0};
        double results4[3] = {0, 0, 0};
        bool bSuccess1 = do_trilateration_3D(P1, P2, P3, distance1, distance2, distance3, results1, invZ);
        bool bSuccess2 = false;
        bool bSuccess3 = false;
        bool bSuccess4 = false;

        //printf("do_trilateration_3D_new results1[%d, %d, %d]\n", (int)results1[0], (int)results1[1], (int)results1[2]);

        if (validRangingData->validCount > 3)
        {
            bSuccess2 = do_trilateration_3D(P2, P3, P4, distance2, distance3, distance4, results2, invZ);
            //printf("do_trilateration_3D_new results2[%d, %d, %d]\n", (int)results2[0], (int)results2[1], (int)results2[2]);

            bSuccess3 = do_trilateration_3D(P1, P2, P4, distance1, distance2, distance4, results3, invZ);
            //printf("do_trilateration_3D_new results3[%d, %d, %d]\n", (int)results3[0], (int)results3[1], (int)results3[2]);

            bSuccess4 = do_trilateration_3D(P1, P3, P4, distance1, distance3, distance4, results4, invZ);
            //printf("do_trilateration_3D_new results4[%d, %d, %d]\n", (int)results4[0], (int)results4[1], (int)results4[2]);
        }

        //
        // find the point which more close to previous.
        //
        double dist1 = 999999;
        double dist2 = 999999;
        double dist3 = 999999;
        double dist4 = 999999;

        if (bSuccess1)
            dist1 = TwoPointDistance(results1, results);
        if (bSuccess2)
            dist2 = TwoPointDistance(results2, results);
        if (bSuccess3)
            dist3 = TwoPointDistance(results3, results);
        if (bSuccess4)
            dist4 = TwoPointDistance(results4, results);

        double min_dist = dist1;
        if (min_dist >= dist1)
        {
            min_dist = dist1;
            results[0] = results1[0];
            results[1] = results1[1];
            results[2] = results1[2];
            tdistance1 = distance1;
            tdistance2 = distance2;
            tdistance3 = distance3;
#ifdef PRINT_RESULT_trilateration_3D_new
            tP1 = P1;
            tP2 = P2;
            tP3 = P3;
#endif
        }
        if (min_dist > dist2)
        {
            min_dist = dist2;
            results[0] = results2[0];
            results[1] = results2[1];
            results[2] = results2[2];
            tdistance1 = distance2;
            tdistance2 = distance3;
            tdistance3 = distance4;
#ifdef PRINT_RESULT_trilateration_3D_new
            tP1 = P2;
            tP2 = P3;
            tP3 = P4;
#endif
        }
        if (min_dist > dist3)
        {
            min_dist = dist3;
            results[0] = results3[0];
            results[1] = results3[1];
            results[2] = results3[2];
            tdistance1 = distance1;
            tdistance2 = distance2;
            tdistance3 = distance4;
#ifdef PRINT_RESULT_trilateration_3D_new
            tP1 = P1;
            tP2 = P2;
            tP3 = P4;
#endif
        }
        if (min_dist > dist4)
        {
            min_dist = dist4;
            results[0] = results4[0];
            results[1] = results4[1];
            results[2] = results4[2];
            tdistance1 = distance1;
            tdistance2 = distance3;
            tdistance3 = distance4;
#ifdef PRINT_RESULT_trilateration_3D_new
            tP1 = P1;
            tP2 = P3;
            tP3 = P4;
#endif
        }

        if ( tdistance1 == 0 || tdistance2 == 0 || tdistance3 == 0 )
        {
#ifdef PRINT_RESULT_trilateration_3D_new
            vilslog::printf ("do_trilateration_3D_new() 4 [%d](%d,%d,%d)\n", tdistance1, (int)P1[0], (int)P1[1], (int)P1[2]);
            vilslog::printf ("do_trilateration_3D_new() 4 [%d](%d,%d,%d)\n", tdistance2, (int)P2[0], (int)P2[1], (int)P2[2]);
            vilslog::printf ("do_trilateration_3D_new() 4 [%d](%d,%d,%d)\n", tdistance3, (int)P3[0], (int)P3[1], (int)P3[2]);
#endif
            return false;
        }
        //
        // print anchors coord
        //
#ifdef PRINT_RESULT_trilateration_3D_new
        vilslog::printf ("do_trilateration_3D_new() 1 [%d](%d,%d,%d)\n", tdistance1, (int)tP1[0], (int)tP1[1], (int)tP1[2]);
        vilslog::printf ("do_trilateration_3D_new() 1 [%d](%d,%d,%d)\n", tdistance2, (int)tP2[0], (int)tP2[1], (int)tP2[2]);
        vilslog::printf ("do_trilateration_3D_new() 1 [%d](%d,%d,%d)\n", tdistance3, (int)tP3[0], (int)tP3[1], (int)tP3[2]);
        vilslog::printf ("do_trilateration_3D_new() 1 results(%d,%d,%d)\n", (int)results[0], (int)results[1], (int)results[2]);
        vilslog::printf ("do_trilateration_3D_new() 1 bSuccess[%d,%d,%d,%d]\n", bSuccess1, bSuccess2, bSuccess3, bSuccess4);
#endif //PRINT_RESULT
        return(bSuccess1 || bSuccess2 || bSuccess3 || bSuccess4);
    }
    else
    {
        if ( distance1 == 0 || distance2 == 0 || distance3 == 0 )
        {
#ifdef PRINT_RESULT_trilateration_3D_new
            vilslog::printf ("do_trilateration_3D_new() 3 [%d](%d,%d,%d)\n", distance1, (int)P1[0], (int)P1[1], (int)P1[2]);
            vilslog::printf ("do_trilateration_3D_new() 3 [%d](%d,%d,%d)\n", distance2, (int)P2[0], (int)P2[1], (int)P2[2]);
            vilslog::printf ("do_trilateration_3D_new() 3 [%d](%d,%d,%d)\n", distance3, (int)P3[0], (int)P3[1], (int)P3[2]);
#endif
            return false;
        }


        bool bSuccess1 = do_trilateration_3D(P1, P2, P3, distance1, distance2, distance3, results, invZ);

        //
        // print anchors coord
        //
#ifdef PRINT_RESULT_trilateration_3D_new
        vilslog::printf ("do_trilateration_3D_new() 2 [%d](%d,%d,%d)\n", distance1, (int)P1[0], (int)P1[1], (int)P1[2]);
        vilslog::printf ("do_trilateration_3D_new() 2 [%d](%d,%d,%d)\n", distance2, (int)P2[0], (int)P2[1], (int)P2[2]);
        vilslog::printf ("do_trilateration_3D_new() 2 [%d](%d,%d,%d)\n", distance3, (int)P3[0], (int)P3[1], (int)P3[2]);
        vilslog::printf ("do_trilateration_3D_new() 2 bSuccess1[%d] results(%d,%d,%d)\n", bSuccess1, (int)results[0], (int)results[1], (int)results[2]);
#endif //PRINT_RESULT
        return bSuccess1;
    }
}

bool PosCalculate::do_leastsquares_new(int projectid, RangingResultStruct * validRangingData, int enable_GDOP, int GDOP_Threshold, double * results)
{
    bool ret = true;
    double anchordistance[MAX_ANCHOR_NUMBER];
    double anchors_z_diff;

    anchors_z_diff = 0.0;
    for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    {
        mAnchorCoordinate[i][0] = 0.0;
        mAnchorCoordinate[i][1] = 0.0;
        mAnchorCoordinate[i][2] = 0.0;
        anchordistance[i] = 0.0;
    }
    //printf("do_leastsquares_new() validCount[%d]", validRangingData->validCount);
    int validCount = 0;
    for(int i=0;i<validRangingData->validCount && i<MAX_RANGING_RESULT;i++)
    {
        anchordistance[i] = validRangingData->RangingFiltered[i];

        if (strlen(validRangingData->AnchorName[i]) == 0)
        {
            continue;
        }

        //mDeviceManager->getAnchorCoord(validRangingData->AnchorName[i], mAnchorCoordinate[i]);
        getAnchorCoord(projectid, validRangingData->AnchorName[i], mAnchorCoordinate[i]);

        if (mAnchorCoordinate[i][2] > mAnchorCoordinate[0][2])
            anchors_z_diff += mAnchorCoordinate[i][2] - mAnchorCoordinate[0][2];
        else
            anchors_z_diff += mAnchorCoordinate[0][2] - mAnchorCoordinate[i][2];

        validCount++;

        if (strlen(validRangingData->AnchorName[i]) == 0)
        {
            vilslog::printf("(%d,%d,%d)", (int)mAnchorCoordinate[i][0], (int)mAnchorCoordinate[i][1], (int)mAnchorCoordinate[i][2]);
        }

        //printf(" %s[%0.2f]", validRangingData->AnchorName[i], anchordistance[i]);
    }
    //printf("\n");

    if (validCount < 4)
    {
        return false;
    }

#ifdef PRINT_RESULT_leastsquares
    vilslog::printf ("PosCalculate::do_leastsquares_new() previous results[%d,%d,%d]\n", (int)results[0], (int)results[1], (int)results[2]);
#endif

    non_linear->setParameters(results);
    non_linear->setData(validRangingData->validCount, mAnchorCoordinate, anchordistance);

	//coarse fit.
	ret = non_linear->fitData();

	//printf ("PosCalculate::do_leastsquares() ret = %d\n", ret);
	if ( ret )
	{
		double * params = non_linear->getParameters();

    	for(int i=0;i<3;i++)
    		results[i] = params[i];

#ifdef PRINT_RESULT_leastsquares
            vilslog::printf ("PosCalculate::do_leastsquares_new() current results[%d,%d,%d]\n", (int)results[0], (int)results[1], (int)results[2]);
#endif

        if (enable_GDOP == 1)
        {
            //
            // Using GDOP check if valid result
            //
            mDOPresolve->setData(params, validRangingData->validCount, mAnchorCoordinate, anchordistance);
            double GDOP = mDOPresolve->resolve();
            if (GDOP > GDOP_Threshold)
            {
                // Poor
                ret = false;
            }
        }
	}
    else
    {
#ifdef PRINT_RESULT_leastsquares
        vilslog::printf ("PosCalculate::do_leastsquares_new() ret[%d] failed!\n", ret);
#endif
    }

    return ret;
}

bool PosCalculate::checkInside(int projectid, RangingResultStruct * validRangingData, double * results)
{
    bool bInside = false;
    int max_anchor_height = -9999;
    int min_anchor_height = 9999;

    for(int i=0;i<validRangingData->validCount && i<MAX_RANGING_RESULT;i++)
    {
        if (strlen(validRangingData->AnchorName[i]) == 0)
        {
            continue;
        }

        //mDeviceManager->getAnchorCoord(validRangingData->AnchorName[i], mAnchorCoordinate[i]);
        getAnchorCoord(projectid, validRangingData->AnchorName[i], mAnchorCoordinate[i]);

        // vilslog::printf ("PosCalculate::checkInside() [%d] AnchorCoordinate[%d,%d,%d]\n",
        //    i, (int)mAnchorCoordinate[i][0], (int)mAnchorCoordinate[i][1], (int)mAnchorCoordinate[i][2]);

        if ( max_anchor_height < mAnchorCoordinate[i][2] )
            max_anchor_height = mAnchorCoordinate[i][2];

        if ( min_anchor_height > mAnchorCoordinate[i][2] )
            min_anchor_height = mAnchorCoordinate[i][2];
    }
    if ( (int)results[2] >= min_anchor_height && (int)results[2] <= max_anchor_height )
        bInside = true;

    // vilslog::printf ("PosCalculate::checkInside() max_anchor_height[%d] min_anchor_height[%d] results height[%d] bInside[%d]\n",
    //     max_anchor_height, min_anchor_height, (int)results[2], bInside);

    return bInside;
}

// int PosCalculate::resolveProjectId(int floor)
// {
//     int project_id = 1;
//
//     for (std::list<VILSConfig*>::iterator config = mAllConfigList->begin(); config != mAllConfigList->end(); config++)
//     {
//         if( (*config)->BuildingFloor == floor )
//         {
//             project_id = (*config)->projectid;
//             break;
//         }
//     }
//
//     return project_id;
// }

// return 0: found 0D Section
// return 1: found 1D Section
// return -1: not found
int PosCalculate::do_section_check(int projectid, const char * TagId, RangingResultStruct * validRangingData, RangingResultStruct * validRangingData0D)
{
    bool found = false;

    //
    // check  section ZERO first
    //
    found = do_section_0D_check(projectid, TagId, validRangingData, validRangingData0D);
    if ( found )
    {
        return 0;
    }

    found = do_section_1D_check(projectid, TagId, validRangingData);
    if ( found )
    {
        return 1;
    }

    return -1;
}

bool PosCalculate::do_section_0D_check(int projectid, const char * TagId, RangingResultStruct * validRangingData, RangingResultStruct * validRangingData0D)
{
    //printf ("PosCalculate::do_section_check() validRangingData->validCount[%d]\n", validRangingData->validCount);
    bool found = false;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    if (rangingNodeInfo->mSection0DGroupList == NULL || validRangingData->validCount <= 0)
    {
        return false;
    }

    int checkSection0D = -1;
    SectionGroup * matchSection = NULL;
    for (std::list<SectionGroup *>::iterator sectiongroup = rangingNodeInfo->mSection0DGroupList->begin(); sectiongroup != rangingNodeInfo->mSection0DGroupList->end(); sectiongroup++)
    {
        //  -1: not found
        //   0: found and in range
        //   1: found but out of range
        int retValue = do_check_Section_ZERO(projectid, TagId, (*sectiongroup), validRangingData);

        if ( retValue < 0)
        {
            // 使用 validRangingData0D
            retValue = do_check_Section_ZERO(projectid, TagId, (*sectiongroup), validRangingData0D);
        }

        if (retValue == 0)
        {
            checkSection0D = retValue;
            matchSection = (*sectiongroup);
            found = true;
            //vilslog::printf("do_section_0D_check() stop find another retValue[%d] GroupID[%d]\n", retValue, (*sectiongroup)->GroupID);
            break;// stop find another
        }
        else
        if (retValue > 0)
        {
            checkSection0D = retValue;
            matchSection = (*sectiongroup);
            found = true;
            // keep find another
            //vilslog::printf("do_section_0D_check() keep find another retValue[%d] GroupID[%d]\n", retValue, (*sectiongroup)->GroupID);
        }
    }

    if (found)
    {
        update_node_section0D(projectid, TagId, checkSection0D, matchSection, validRangingData);
    }
    else
    {
        // not found 0D section
        mDeviceManager->setTagInfoArea0D(projectid, TagId, "", 0);
    }

    return found;
}

bool PosCalculate::do_section_1D_check(int projectid, const char * TagId, RangingResultStruct * validRangingData)
{
    //printf ("PosCalculate::do_section_check() validRangingData->validCount[%d]\n", validRangingData->validCount);
    bool found = false;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    if (rangingNodeInfo->mSection1DGroupList == NULL || validRangingData->validCount <= 0)
    {
        return false;
    }

    //
    // check section ONE
    //
    for (std::list<SectionGroup *>::iterator sectiongroup = rangingNodeInfo->mSection1DGroupList->begin(); sectiongroup != rangingNodeInfo->mSection1DGroupList->end(); sectiongroup++)
    {
        found = do_check_Section_ONE(projectid, TagId, *sectiongroup, validRangingData);

        if (found)
        {
            break;
        }
    }

    return found;
}

// return value
//  -1: not found
//   0: found and in range
//   1: found but out of range
int PosCalculate::do_check_Section_ZERO(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData)
{
    if (sectiongroup->SectionType != SECTION_TYPE_ZERO)
        return -1;

    std::string nodeid = sectiongroup->nodeIDList->front();

    int checkSection0D = -1;
    for(int i=0;i<validRangingData->validCount && i<MAX_RANGING_RESULT;i++)
    {
        if ( nodeid.compare(validRangingData->AnchorName[i]) == 0)
        {
            if ( validRangingData->RangingFiltered[i] <= sectiongroup->Param1)
            {
                checkSection0D = 0; // 0: found and in range
            }
            else
            {
                checkSection0D = 1; // 1: found but out of range
            }
            break;
        }
    }
    return checkSection0D;
}

int PosCalculate::update_node_section0D(int projectid, const char * TagId, int checkSection0D, SectionGroup * sectiongroup, RangingResultStruct * validRangingData)
{
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    if (checkSection0D == 0)// 0: found and in range
    {
        for (std::list<MapLayerAreaStruct *>::iterator area = rangingNodeInfo->mAreaList->begin(); area != rangingNodeInfo->mAreaList->end(); area++)
        {
            if ( (*area)->areaid.compare(sectiongroup->AreaID) == 0 )
            {
                //=====================================
                //ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);
                //int area_type = rangingNodeInfo->mRaycasting->getAreaType((*area)->projectid, (*area)->areaid.c_str());
                //mDeviceManager->setTagInfoArea((*area)->projectid, TagId, (*area)->areaid.c_str(), (*area)->areaname.c_str(), sectiongroup->Param2, area_type, "");
                //=====================================
                mDeviceManager->setTagInfoArea0D(projectid, TagId, (*area)->areaid.c_str(), sectiongroup->Param2);
                //=====================================

                // clear RollCallNotInArea info
                mDeviceManager->setTagRollCallNotInArea(projectid, TagId, "");

                // move to area center
                for (std::list<AreaCenterStruct *>::iterator areacenter = rangingNodeInfo->mAreaCenterList->begin(); areacenter != rangingNodeInfo->mAreaCenterList->end(); areacenter++)
                {
                    if ( (*areacenter)->areaid.compare(sectiongroup->AreaID) == 0 )
                    {
                        int newPosition[3] = {(int)(*areacenter)->center.x, (int)(*areacenter)->center.y, 0};
                        find_empty_0D_position(projectid, TagId, (int)(*areacenter)->center.x, (int)(*areacenter)->center.y, newPosition);

                        int tagMapLayer = resolveTagMapLayer(validRangingData);
                        mDeviceManager->setTagInfoS(projectid, TagId, newPosition[0], newPosition[1], 0, tagMapLayer);
                        mDeviceManager->setTagRangingResultType(projectid, TagId, 0);

                        // vilslog::printf("PosCalculate::do_check_Section_ZERO() setTagInfoS projectid[%d] TagId[%s] center.x[%d] center.y[%d] tagMapLayer[%d]\n",
                        //    projectid, TagId, (int)(*areacenter)->center.x, (int)(*areacenter)->center.y, tagMapLayer);
                        break;
                    }
                }
                // vilslog::printf("PosCalculate::do_check_Section_ZERO() projectid[%d] TagId[%s] areaid[%s] areaname[%s] found Match!!!!!\n",
                //    projectid, TagId, (*area)->areaid.c_str(), (*area)->areaname.c_str());
                break;
            }
        }
    }
    else
    if ( checkSection0D == 1)// 1: found but out of range
    {
        // int ListIndex = removeFromSection0D(sectiongroup->GroupID, TagId);
        removeFromSection0D(projectid, sectiongroup->GroupID, TagId);

        mDeviceManager->setTagInfoArea0D(projectid, TagId, "", 0);

        //
        // 如果 Tag 仍在此區域內, 點名時會被排除, 同時 2dmap 上Tag圖示應該變色
        //
        mDeviceManager->setTagRollCallNotInArea(projectid, TagId, sectiongroup->AreaID.c_str());

// #ifdef SECTION_0D_EXCLUDE_AREA
//
//         //
//         // 測距距離 超過指定距離, 排除在此 Area.
//         //
//         mDeviceManager->setTagInfoNotInArea(sectiongroup->projectid, TagId, sectiongroup->AreaID.c_str());
// #endif
    }

    return 0;
}


bool PosCalculate::do_check_Section_ONE(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData)
{
    bool found = false;

#ifdef PRINT_DEBUG_SECTION_ONE
    vilslog::printf("do_check_Section_ONE() TagId[%s] nodeIDList->size()[%lu] validCount[%d]\n",
              TagId, sectiongroup->nodeIDList->size(), validRangingData->validCount);
#endif

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    //===================================
    if (rangingNodeInfo->mEnableOneAnchorCalculation == 1)
    {
        if (validRangingData->validCount == 1)
        {
            found = do_check_Section_ONE_on_one_anchor(projectid, TagId, sectiongroup, validRangingData);

#ifdef PRINT_DEBUG_SECTION_ONE
    vilslog::printf("do_check_Section_ONE() TagId[%s] mEnableOneAnchorCalculation[%d] found[%d]\n",
              TagId, rangingNodeInfo->mEnableOneAnchorCalculation, found);
#endif

        }
    }
    //===================================

    if (validRangingData->validCount >= 2)
    {
        found = do_check_Section_ONE_on_two_anchor(projectid, TagId, sectiongroup, validRangingData);

#ifdef PRINT_DEBUG_SECTION_ONE
    vilslog::printf("do_check_Section_ONE() TagId[%s] do_check_Section_ONE_on_two_anchor() found[%d]\n",
              TagId, found);
#endif
    }
    return found;
}

bool PosCalculate::do_check_Section_ONE_on_one_anchor(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData)
{
    bool found = false;

#ifdef PRINT_DEBUG_SECTION_ONE
    vilslog::printf("do_check_Section_ONE_on_one_anchor() TagId[%s] nodeIDList->size()[%lu] validCount[%d]\n",
              TagId, sectiongroup->nodeIDList->size(), validRangingData->validCount);
#endif
    //
    // check if has valid anchor
    //
    int validAnchor = 0;
    for (std::list<std::string>::iterator anchorID = sectiongroup->nodeIDList->begin(); anchorID != sectiongroup->nodeIDList->end(); anchorID++)
    {
        if ( strcmp((*anchorID).c_str(),  validRangingData->AnchorName[0]) == 0 )
        {
            validAnchor = 1;
            break;
        }
    }
    if (validAnchor == 0)
    {
#ifdef PRINT_DEBUG_SECTION_ONE
        vilslog::printf("do_check_Section_ONE_on_one_anchor() TagId[%s] GroupID[%d] validAnchor == 0 AnchorName[%s]\n",
              TagId, sectiongroup->GroupID, validRangingData->AnchorName[0]);
#endif
        return false;
    }

    double tagCurcoord[3] = {0,0,0};
    mDeviceManager->getTagCoord(projectid, TagId, tagCurcoord);

    double mvvec[3] = {0,0,0};
    mDeviceManager->getTagMomentum(projectid, TagId, mvvec);

    int ranging = (int)validRangingData->RangingFiltered[0];
    double P1[3];
    int ret = getAnchorCoord(projectid, validRangingData->AnchorName[0], P1);
    if ( ret == 0 )
    {
#ifdef PRINT_DEBUG_SECTION_ONE
        vilslog::printf("do_check_Section_ONE_on_one_anchor() TagId[%s] Can not find Anchor Coord.\n", TagId);
#endif
        return false;
    }

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

//#ifdef APPLY_ANCHOR_1D_MAPPING
    double P1_m[3] = {P1[0], P1[1], P1[2]};
    double Ranging_m = ranging;
    if (rangingNodeInfo->mEnableAnchor1DMapping == 1)
    {
        //====================
        //
        // mapping to 1D position
        //
        double P1_mapping[4];//[X,Y,Z,Ranging]
        bool success = map_to_1D_ranging(P1, ranging, sectiongroup->Param1, sectiongroup->Param2, sectiongroup->Param3, sectiongroup->Param4, sectiongroup->Param5, sectiongroup->Param6, P1_mapping);
        if (success == false )
        {
#ifdef PRINT_DEBUG_SECTION_ONE
            vilslog::printf("do_check_Section_ONE_on_one_anchor() TagId[%s] AnchorName[%s] convert_to_1D_ranging failed!!!\n",
                TagId, validRangingData->AnchorName[0]);
#endif
            return false;
        }

        //double P1_m[3] = {P1_mapping[0], P1_mapping[1], P1_mapping[2]};
        //double Ranging_m = P1_mapping[3];
        P1_m[0] = P1_mapping[0];
        P1_m[1] = P1_mapping[1];
        P1_m[2] = P1_mapping[2];
        Ranging_m = P1_mapping[3];
    //====================
    }
    //#else
    // else
    // {
    //     //double P1_m[3] = {P1[0], P1[1], P1[2]};
    //     //double Ranging_m = ranging;
    // }
//#endif

    //
    //  check valid ranging value
    //
    double anDist = TwoPointDistance(tagCurcoord, P1_m);
    double anvec[3] = {P1_m[0] - tagCurcoord[0], P1_m[1] - tagCurcoord[1], 0};
    double mv_dir = dotproduct(anvec, mvvec);

    double result[3] = {0, 0, 0};
    found = resolve_one_anchor(mv_dir, Ranging_m, anDist, mvvec, P1_m, tagCurcoord, result);
    if ( found )
    {
        int tagMapLayer = resolveTagMapLayer(validRangingData);

        double Param_1[3] = {(double)sectiongroup->Param1, (double)sectiongroup->Param2, (double)sectiongroup->Param3};
        double Param_2[3] = {(double)sectiongroup->Param4, (double)sectiongroup->Param5, (double)sectiongroup->Param6};

        //
        // make sure not exceed end point position
        //
        fix_exceed_endpoint(Param_1, Param_2, result);

        mDeviceManager->setTagInfoS(projectid, TagId, result[0], result[1], result[2], tagMapLayer);
        mDeviceManager->setTagRangingResultType(projectid, TagId, 1);

    }
    else
    {
#ifdef PRINT_DEBUG_SECTION_ONE
            vilslog::printf("do_check_Section_ONE_on_one_anchor() TagId[%s] resolve_one_anchor failed!!!\n", TagId);
#endif
    }
    return found;
}

bool PosCalculate::do_check_Section_ONE_on_two_anchor(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData)
{
    bool found = false;
    //printf("do_check_Section_ONE_on_two_anchor() Tag_idx[%d] nodeIDList->size()[%lu] validCount[%d]\n",
    //           Tag_idx, sectiongroup->nodeIDList->size(), validRangingData->validCount);


    double tagCurcoord[3] = {0,0,0};
    double tagPrecoord[3] = {0,0,0};
    mDeviceManager->getTagCoord(projectid, TagId, tagCurcoord);
    mDeviceManager->getTagPrePos(projectid, TagId, tagPrecoord);

    std::string FirstNode_ID = "";
    std::string SecondNode_ID = "";
    int FirstNode_Index = -1;
    int SecondNode_Index = -1;
    double FirstNode_Distance = 99999;
    double SecondNode_Distance = 99999;
    for(int i=0;i<validRangingData->validCount && i<MAX_RANGING_RESULT;i++)
    {
        for (std::list<std::string>::iterator anchorID = sectiongroup->nodeIDList->begin(); anchorID != sectiongroup->nodeIDList->end(); anchorID++)
        {
            if ( strcmp((*anchorID).c_str(),  validRangingData->AnchorName[i]) == 0 )
            {
                //
                // find the shortest distance
                //
                //====================
                double P1[3];
                //mDeviceManager->getAnchorCoord(validRangingData->AnchorName[i], P1);
                getAnchorCoord(projectid, validRangingData->AnchorName[i], P1);
                double Dist1 = TwoPointDistance(tagCurcoord, P1);
                //====================
                //double Dist1 = validRangingData->RangingFiltered[i];
                //====================

                if ( Dist1 < FirstNode_Distance )
                {
                    // move FirstNode to SecondNode
                    SecondNode_Distance = FirstNode_Distance;
                    SecondNode_Index = FirstNode_Index;
                    SecondNode_ID = FirstNode_ID;

                    FirstNode_Distance = Dist1;
                    FirstNode_Index = i;
                    FirstNode_ID = (*anchorID);
                }
                else
                if ( Dist1 < SecondNode_Distance )
                {
                    SecondNode_Distance = Dist1;
                    SecondNode_Index = i;
                    SecondNode_ID = (*anchorID);
                }
            }
        }
    }

    //printf("PosCalculate::do_check_Section_ONE_on_two_anchor() FirstNode_Index[%d] SecondNode_Index[%d]\n",
    //        FirstNode_Index, SecondNode_Index);

    if ( FirstNode_Index < 0 || SecondNode_Index < 0 )
    {
        return false;
    }

    //printf("PosCalculate::do_check_Section_ONE_on_two_anchor() FirstNode_ID[%s] SecondNode_ID[%s]\n",
    //        FirstNode_ID.c_str(), SecondNode_ID.c_str());

    if ( FirstNode_ID.length() == 0 || SecondNode_ID.length() == 0 )
    {
        return false;
    }

    double P1[3];
    //mDeviceManager->getAnchorCoord(FirstNode_ID.c_str(), P1);
    getAnchorCoord(projectid, FirstNode_ID.c_str(), P1);
    double P2[3];
    //mDeviceManager->getAnchorCoord(SecondNode_ID.c_str(), P2);
    getAnchorCoord(projectid, SecondNode_ID.c_str(), P2);

    double Dist1 = validRangingData->RangingFiltered[FirstNode_Index];
    double Dist2 = validRangingData->RangingFiltered[SecondNode_Index];
    double result[3] = {0, 0, 0};

    //printf("PosCalculate::do_check_Section_ONE_on_two_anchor() 2 \n");

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

//#ifdef APPLY_ANCHOR_1D_MAPPING
    double P1_m[3] = {P1[0], P1[1], P1[2]};
    double P2_m[3] = {P2[0], P2[1], P2[2]};
    double Ranging1_m = Dist1;
    double Ranging2_m = Dist2;
    if (rangingNodeInfo->mEnableAnchor1DMapping == 1)
    {
        //====================
        //
        // mapping to 1D position
        //
        double P1_mapping[4];//[X,Y,Z,mapping_Ranging]
        double P2_mapping[4];//[X,Y,Z,mapping_Ranging]
        bool success1 = map_to_1D_ranging(P1, Dist1, sectiongroup->Param1, sectiongroup->Param2, sectiongroup->Param3, sectiongroup->Param4, sectiongroup->Param5, sectiongroup->Param6, P1_mapping);
        bool success2 = map_to_1D_ranging(P2, Dist2, sectiongroup->Param1, sectiongroup->Param2, sectiongroup->Param3, sectiongroup->Param4, sectiongroup->Param5, sectiongroup->Param6, P2_mapping);
        if (success1 == false || success2 == false)
        {
            //printf("\tmap_to_1D_ranging failed!!! [%s]success1(%d) == 0 || [%s]success2(%d) == 0\n",
             //                   FirstNode_ID.c_str(), success1, SecondNode_ID.c_str(), success2);
            return false;
        }

        //double P1_m[3] = {P1_mapping[0], P1_mapping[1], P1_mapping[2]};
        //double P2_m[3] = {P2_mapping[0], P2_mapping[1], P2_mapping[2]};
        P1_m[0] = P1_mapping[0];
        P1_m[1] = P1_mapping[1];
        P1_m[2] = P1_mapping[2];
        P2_m[0] = P2_mapping[0];
        P2_m[1] = P2_mapping[1];
        P2_m[2] = P2_mapping[2];
        Ranging1_m = P1_mapping[3];
        Ranging2_m = P2_mapping[3];


        // vilslog::printf("do_check_Section_ONE_on_two_anchor() Tag[%d] P1[%d, %d, %d][%d] --> [%d, %d, %d][%d]\n",
        //     Tag_idx, (int)P1[0], (int)P1[1], (int)P1[2], (int)Dist1, (int)P1_m[0], (int)P1_m[1], (int)P1_m[2], (int)Ranging1_m);
        // vilslog::printf("do_check_Section_ONE_on_two_anchor() Tag[%d] P2[%d, %d, %d][%d] --> [%d, %d, %d][%d]\n",
        //     Tag_idx, (int)P2[0], (int)P2[1], (int)P2[2], (int)Dist2, (int)P2_m[0], (int)P2_m[1], (int)P2_m[2], (int)Ranging2_m);

    }
    // else
    // //#else
    // {
    //     double P1_m[3] = {P1[0], P1[1], P1[2]};
    //     double P2_m[3] = {P2[0], P2[1], P2[2]};
    //     double Ranging1_m = Dist1;
    //     double Ranging2_m = Dist2;
    // }
//#endif
    //====================

    //printf("PosCalculate::do_check_Section_ONE_on_two_anchor() 3 \n");

    found = resolve_two_anchor(P1_m, P2_m, Ranging1_m, Ranging2_m, result);
    if (found)
    {
        int tagMapLayer = resolveTagMapLayer(validRangingData);

        double Param_1[3] = {(double)sectiongroup->Param1, (double)sectiongroup->Param2, (double)sectiongroup->Param3};
        double Param_2[3] = {(double)sectiongroup->Param4, (double)sectiongroup->Param5, (double)sectiongroup->Param6};
//#ifdef APPLY_ANCHOR_1D_MAPPING
        if (rangingNodeInfo->mEnableAnchor1DMapping == 1)
        {
            //
            // make sure not exceed end point position
            //
            fix_exceed_endpoint(Param_1, Param_2, result);

            mDeviceManager->setTagInfoS(projectid, TagId, result[0], result[1], result[2], tagMapLayer);
            mDeviceManager->setTagRangingResultType(projectid, TagId, 1);

        }
        //#else
        else
        {
            double result_mapping[3];//[X,Y,Z]
            bool success = map_to_1D(result, sectiongroup->Param1, sectiongroup->Param2, sectiongroup->Param3, sectiongroup->Param4, sectiongroup->Param5, sectiongroup->Param6, result_mapping);
            if (success)
            {
                result[0] = result_mapping[0];
                result[1] = result_mapping[1];
                result[2] = result_mapping[2];
            }

            //
            // make sure not exceed end point position
            //
            fix_exceed_endpoint(Param_1, Param_2, result);

            mDeviceManager->setTagInfoS(projectid, TagId, result[0], result[1], result[2], tagMapLayer);
            mDeviceManager->setTagRangingResultType(projectid, TagId, 1);
        }
//#endif
        //printf("do_check_Section_ONE            Tag[%d] result[%d, %d, %d]\n",
        //        Tag_idx, (int)result[0], (int)result[1], (int)result[2]);
    }

   return found;
}

int PosCalculate::do_AOA_check(int projectid, const char * TagId, RangingResultStruct * validRangingData)
{
    int iRet = 0;

    std::list<AoARangingInfoStruct *> AoARangingInfoList;

    int tagMapLayer= resolveTagMapLayer(validRangingData);

    if (bPrintAoACirclePoint)
    {
        vilslog::printRaw("\n**********************************\n");
        vilslog::printRaw("PosCalculate::do_AOA_check() TagId[%s] validRangingData->validCount[%d]\n", TagId, validRangingData->validCount);
    }

    for(int i=0;i<validRangingData->validCount;i++)
    {
        // if (validRangingData->TagAngle[i] != 0.0f)
        // {
            AoANodeStruct * aoaNode = mDeviceManager->get_AoA_nodeById(projectid, validRangingData->AnchorName[i]);
            if (aoaNode != NULL)
            {
                AoARangingInfoStruct * info = new AoARangingInfoStruct();
                info->projectid = projectid;
                info->aoaNode = aoaNode;
                info->TagAngle = validRangingData->TagAngle[i];
                info->Ranging = validRangingData->RangingFiltered[i];
                info->MapLayer = tagMapLayer;
                info->TagLevel_x = validRangingData->TagLevel_x[i];
                info->TagLevel_y = validRangingData->TagLevel_y[i];
                info->TagLevel_time = validRangingData->TagLevel_time[i];

                float pitch_Radians = angle_to_rad((float)aoaNode->pitch);
                float yaw_Radians = angle_to_rad((float)aoaNode->yaw);

                info->aoaNorm[0] = (double)cos(yaw_Radians)*1000;
                info->aoaNorm[1] = (double)sin(yaw_Radians)*1000;
                info->aoaNorm[2] = (double)sin(pitch_Radians)*1000;

                AoARangingInfoList.push_back(info);

                if (bPrintAoACirclePoint)
                {
                    vilslog::printRaw("AOA Node nodeid[%s] macaddress[%s] pos[%d,%d,%d] pitch[%d] roll[%d] yaw[%d] gridsize[%d] angle[%f] Ranging[%d]\n",
                        aoaNode->nodeid, aoaNode->macaddress, aoaNode->posX, aoaNode->posY, aoaNode->posZ, aoaNode->pitch, aoaNode->roll, aoaNode->yaw, aoaNode->gridsize,
                        info->TagAngle, info->Ranging);

                }
            }
        // }
    }

    if ( (int)AoARangingInfoList.size() == 0 )
    {
        iRet = 0;
    }
    else
    if ( (int)AoARangingInfoList.size() == 1 )
    {
        iRet = do_one_AOA_check(TagId, &AoARangingInfoList);
    }
    else
    {
        // vilslog::printf ("PosCalculate::do_AOA_check() bDumpAoACirclePoint[%d] projectid[%d]\n", bDumpAoACirclePoint, projectid);

        if (bDumpAoACirclePoint)
        {
            ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);
            rangingNodeInfo->mAoARangingCloudPointStream.clear();
            rangingNodeInfo->mAoARangingCloudPointStream.str("");
            rangingNodeInfo->mAoARangingCloudPointStream.seekp(0); // for outputs: seek put ptr to start
            //rangingNodeInfo->mAoARangingCloudPointStream.seekg(0); // for inputs: seek get ptr to start

            rangingNodeInfo->mAoAAngleVectorCloudPointStream.clear();
            rangingNodeInfo->mAoAAngleVectorCloudPointStream.str("");
            rangingNodeInfo->mAoAAngleVectorCloudPointStream.seekp(0); // for outputs: seek put ptr to start
            //rangingNodeInfo->mAoAAngleCloudPointStream.seekg(0); // for inputs: seek get ptr to start

            rangingNodeInfo->mAoAAngleCloudPointStream.clear();
            rangingNodeInfo->mAoAAngleCloudPointStream.str("");
            rangingNodeInfo->mAoAAngleCloudPointStream.seekp(0); // for outputs: seek put ptr to start
            //rangingNodeInfo->mAoAAngleCloudPointStream.seekg(0); // for inputs: seek get ptr to start
        }

        // long long start1 = getSystemTime();
        iRet = do_multi_AOA_check(TagId, &AoARangingInfoList);

        // long long multi_AOA_check_time = getSystemTime() - start1;
        // vilslog::printf ("PosCalculate::do_AOA_check() multi_AOA_check_time[%d] \n", (int)multi_AOA_check_time);

        do_multi_AOA_angle_check(TagId, &AoARangingInfoList);

        // long long multi_AOA_angle_check_time = getSystemTime() - start1;
        // vilslog::printf ("PosCalculate::do_AOA_check() multi_AOA_angle_check_time[%d]\n", (int)multi_AOA_angle_check_time);

    }

    if (validRangingData->validCount > 0)
    {
        if(bPrintAoACirclePoint)
        {
            vilslog::printRaw("\n**********************************\n");
        }
        bPrintAoACirclePoint = false;

        if (bDumpAoACirclePoint)
        {
            ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);
            rangingNodeInfo->mAoARangingCloudPointString = rangingNodeInfo->mAoARangingCloudPointStream.str();
            //vilslog::printRaw("\n===mAoARangingCloudPointString.length() size[%d]====\n", rangingNodeInfo->mAoARangingCloudPointString.length());

            rangingNodeInfo->mAoAAngleVectorCloudPointString = rangingNodeInfo->mAoAAngleVectorCloudPointStream.str();
            //vilslog::printRaw("\n===mAoAAngleVectorCloudPointString.length() size[%d]====\n", rangingNodeInfo->mAoAAngleVectorCloudPointString.length());

            rangingNodeInfo->mAoAAngleCloudPointString = rangingNodeInfo->mAoAAngleCloudPointStream.str();
            //vilslog::printRaw("\n===mAoAAngleCloudPointString.length() size[%d]====\n", rangingNodeInfo->mAoAAngleCloudPointString.length());
        }
    }

    return iRet;
}

int PosCalculate::do_one_AOA_check(const char * TagId, std::list<AoARangingInfoStruct *> * AoARangingInfoList)
{
    std::list<AoARangingInfoStruct *>::iterator aoainfoit = AoARangingInfoList->begin();

    AoARangingInfoStruct * aoainfo = (*aoainfoit);

    if ( aoainfo->TagLevel_x != 0 || aoainfo->TagLevel_y != 0)
    {
        return do_one_AOA_Fence_check(TagId, aoainfo);
    }
    else
    if ( aoainfo->Ranging != 0)
    {
        return do_one_AOA_Angle_check(TagId, aoainfo);
    }

    return 0;
}

int PosCalculate::do_one_AOA_Fence_check(const char * TagId, AoARangingInfoStruct * aoainfo)
{
    // vilslog::printf("PosCalculate::do_one_AOA_Fence_check() TagId[%s] TagLevel_x[%d] TagLevel_y[%d] TagLevel_time[%d] gridsize[%d]\n",
    //     TagId, aoainfo->TagLevel_x, aoainfo->TagLevel_y, aoainfo->TagLevel_time, aoainfo->aoaNode->gridsize);
    //
    // vilslog::printf("PosCalculate::do_one_AOA_Fence_check() aoaNode pitch[%d] yaw[%d] roll[%d]\n",
    //     aoainfo->aoaNode->pitch, aoainfo->aoaNode->yaw, aoainfo->aoaNode->roll);

    double normal_vector[3] = {0,0,0};
    float pitch_Radians = angle_to_rad((float)aoainfo->aoaNode->pitch);
    float yaw_Radians = angle_to_rad((float)aoainfo->aoaNode->yaw);

    // vilslog::printf("PosCalculate::do_one_AOA_Fence_check() pitch_Radians[%f] yaw_Radians[%f]\n",
    //     pitch_Radians, yaw_Radians);

    double origAoA[3] = {0,0,0};
    origAoA[0] = (double)(cos(pitch_Radians) * aoainfo->TagLevel_x * aoainfo->aoaNode->gridsize);
    origAoA[1] = (double)(cos(pitch_Radians) * aoainfo->TagLevel_y * aoainfo->aoaNode->gridsize * -1);
    origAoA[2] = 0;

    // vilslog::printf("PosCalculate::do_one_AOA_Fence_check() origAoA[%d,%d,%d]\n",
    //     (int)origAoA[0], (int)origAoA[1], (int)origAoA[2]);

    normal_vector[0] = (double)cos(yaw_Radians)*1000;
    normal_vector[1] = (double)sin(yaw_Radians)*1000;
    normal_vector[2] = (double)sin(pitch_Radians)*1000;

    // vilslog::printf("PosCalculate::do_one_AOA_Fence_check() normal_vector[%f,%f,%f]\n",
    //     normal_vector[0], normal_vector[1], normal_vector[2]);

    double results[3] = {0,0,0};
    calc_AoA_result(aoainfo, normal_vector, origAoA, results);

    if (aoainfo->aoaNode->roll == 0)
    {
        results[2] = 100;
    }

    // vilslog::printf("PosCalculate::do_one_AOA_Fence_check() TagId[%s][%d,%d,%d]\n",
    //     TagId, (int)results[0], (int)results[1], (int)results[2]);

    mDeviceManager->setTagInfoS(aoainfo->projectid, TagId, results[0], results[1], results[2], aoainfo->MapLayer);
    mDeviceManager->setTagRangingResultType(1, TagId, 1);

    return 1;
}

int PosCalculate::do_one_AOA_Angle_check(const char * TagId, AoARangingInfoStruct * aoainfo)
{
    //int absAngle = aoainfo->aoaNode->yaw - (int)aoainfo->TagAngle; // AoA Coordinator to VILS Coordinator
    //int absAngle = (int)aoainfo->TagAngle;
    //int absAngle = (int)aoainfo->TagAngle + 90;
    float Radians = angle_to_rad(aoainfo->TagAngle);
    float Xoffset = aoainfo->Ranging * sin(Radians);

    // vilslog::printf("WebUDPParserService::do_one_AOA_Angle_check() yaw[%d] TagAngle[%d] Radians[%f]\n",
    //     aoainfo->aoaNode->yaw, (int)aoainfo->TagAngle, Radians);

    int AoAHeight = 100;// tag at 100 cm height
    double ALen = pow(AoAHeight, 2);
    double RLen = pow(aoainfo->Ranging, 2);
    // if ( ALen > RLen  )
    // {
    //     //vilslog::printf("WebUDPParserService::do_AOA_check() ALen[%d] > RLen[%d]\n", (int)ALen, (int)RLen);
    //     return 0;
    // }
    //int mapRanging = (int)sqrt( RLen - ALen );
    float XLen = pow(Xoffset, 2);
    if ( RLen - ALen - XLen < 0  )
    {
        //vilslog::printf("WebUDPParserService::do_AOA_check() ALen[%d] > RLen[%d]\n", (int)ALen, (int)RLen);
        ALen = 0;
    }

    float Yoffset = sqrt( RLen - ALen - XLen );

    // vilslog::printf("WebUDPParserService::do_AOA_check() RLen[%d] ALen[%d] XLen[%d] Xoffset[%f] Yoffset[%f]\n",
    //     (int)RLen, (int)ALen, (int)XLen, Xoffset, Yoffset);

    //float s = mapRanging * sin(Radians);
    //float c = mapRanging * cos(Radians);

    double origAoA[3] = {0,0,0};
    origAoA[0] = (double)Xoffset;
    origAoA[1] = (double)Yoffset;
    origAoA[2] = 0;

    double normal_vector[3] = {0,0,0};
    float pitch_Radians = angle_to_rad((float)aoainfo->aoaNode->pitch);
    float yaw_Radians = angle_to_rad((float)aoainfo->aoaNode->yaw);

    normal_vector[0] = (double)cos(yaw_Radians)*1000;
    normal_vector[1] = (double)sin(yaw_Radians)*1000;
    normal_vector[2] = (double)sin(pitch_Radians)*1000;

#ifdef PRINT_AOA_CLOUD_POINT
    printf("PosCalculate::do_one_AOA_check() aoaNode nodeid[%s] pitch_Radians[%f] yaw_Radians[%f] origAoA[%d,%d,%d] normal_vector[%d,%d,%d]\n",
        aoainfo->aoaNode->nodeid, pitch_Radians, yaw_Radians,
        (int)origAoA[0], (int)origAoA[1], (int)origAoA[2],
        (int)normal_vector[0], (int)normal_vector[1], (int)normal_vector[2]);
#endif
    double results[3] = {0,0,0};
    calc_AoA_result(aoainfo, normal_vector, origAoA, results);

    if (aoainfo->aoaNode->roll == 0)
    {
        results[2] = 100;
    }

#ifdef PRINT_AOA_CLOUD_POINT
    printf("WebUDPParserService::do_one_AOA_check() aoaNode nodeid[%s] pry[%d,%d,%d] results[%d,%d,%d]\n",
        aoainfo->aoaNode->nodeid,
        aoainfo->aoaNode->pitch, aoainfo->aoaNode->roll, aoainfo->aoaNode->yaw,
        (int)results[0], (int)results[1], (int)results[2]);
#endif

    if (bPrintAoACirclePoint)
    {
        vilslog::printRaw("do_one_AOA_check() results[%d,%d,%d]\n", (int)results[0], (int)results[1], (int)results[2]);
    }

    mDeviceManager->setTagInfoS(aoainfo->projectid, TagId, results[0], results[1], results[2], aoainfo->MapLayer);
    mDeviceManager->setTagRangingResultType(1, TagId, 1);

    return 1;
}

int PosCalculate::calc_AoA_result(AoARangingInfoStruct * aoainfo, double * normalVector, double * origAoA, double * results)
{
    int iRet = 0;

    // AoA Coordinator to VILS Coordinator
    float results_t[2] = {0,0};
    double results_totate[3] = {0,0,0};
    rotate_point(origAoA[0], origAoA[1], 0, 0, -90, results_t);
    results_totate[0] = results_t[0];
    results_totate[1] = results_t[1];
    results_totate[2] = origAoA[2];

    double results_map[3] = {0,0,0};
    rotate_point(results_totate[0], results_totate[1], 0, 0, aoainfo->aoaNode->yaw, results_t);
    results_map[0] = aoainfo->aoaNode->posX + results_t[0];
    results_map[1] = aoainfo->aoaNode->posY + results_t[1];
    results_map[2] = aoainfo->aoaNode->posZ + origAoA[2];


    // double normalVector_totate[3] = {0,0,0};
    // rotate_point(normalVector[0], normalVector[1], 0, 0, 90, results_t);
    // normalVector_totate[0] = results_t[0];
    // normalVector_totate[1] = results_t[1];
    // normalVector_totate[2] = normalVector[2];

    // vilslog::printf("WebUDPParserService::calc_AoA_result() aoaNode nodeid[%s] results_t[%d,%d] results_totate[%d,%d,%d] results_map[%d,%d,%d]\n",
    //     aoainfo->aoaNode->nodeid, (int)results_t[0], (int)results_t[1],
    //     (int)results_totate[0], (int)results_totate[1], (int)results_totate[2],
    //     (int)results_map[0], (int)results_map[1], (int)results_map[2]);


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

    // vilslog::printf("WebUDPParserService::calc_AoA_result() aoaNode nodeid[%s] pry[%d,%d,%d] results[%d,%d,%d] result_roll[%d,%d,%d]\n",
    //     aoainfo->aoaNode->nodeid,
    //     aoainfo->aoaNode->pitch, aoainfo->aoaNode->roll, aoainfo->aoaNode->yaw,
    //     (int)results[0], (int)results[1], (int)results[2],
    //     (int)result_roll[0], (int)result_roll[1], (int)result_roll[2]);

    return iRet;
}

int PosCalculate::do_multi_AOA_check(const char * TagId, std::list<AoARangingInfoStruct *> * AoARangingInfoList)
{
    int iRet = 0;
    int MapLayer = 0;
    int projectid = 1;
    std::vector<AoACirclePointStruct *> * circlePtList = new std::vector<AoACirclePointStruct *>();

    ProjectRangingNodeInfoStruct * rangingNodeInfo = NULL;
    if (bDumpAoACirclePoint)
    {
        // vilslog::printf("do_multi_AOA_check() bDumpAoACirclePoint projectid[%d]\n", projectid);

        rangingNodeInfo = getProjectRangingNodeInfo(projectid);
        rangingNodeInfo->mAoARangingCloudPointStream << "{\"tag\":\"" << TagId << "\",\"data\":[";

        // vilslog::printf("do_multi_AOA_check() bDumpAoACirclePoint AoARangingInfoList->size()[%d]\n", (int)AoARangingInfoList->size());
    }


    for (std::list<AoARangingInfoStruct *>::iterator aoainfoit = AoARangingInfoList->begin(); aoainfoit != AoARangingInfoList->end(); aoainfoit++)
    {
        if (bDumpAoACirclePoint)
        {
            if(rangingNodeInfo == NULL)
                rangingNodeInfo = getProjectRangingNodeInfo(projectid);
            if(aoainfoit != AoARangingInfoList->begin())
                rangingNodeInfo->mAoARangingCloudPointStream<<",";
        }

        std::list<PointStruct *> * ptList = list_cicle_point((*aoainfoit), true);

        AoACirclePointStruct * newAoACircle = new AoACirclePointStruct();
        newAoACircle->aoainfo = (*aoainfoit);
        newAoACircle->ptList = ptList;
        circlePtList->push_back(newAoACircle);

        MapLayer = newAoACircle->aoainfo->MapLayer;
        projectid = newAoACircle->aoainfo->projectid;
    }

    if (bDumpAoACirclePoint)
    {
        if(rangingNodeInfo == NULL)
            rangingNodeInfo = getProjectRangingNodeInfo(projectid);
        rangingNodeInfo->mAoARangingCloudPointStream<<"]";
    }

    double results[3] = {0,0,0};
    std::list<PointStruct *> * resultPtList = find_closest_point(circlePtList);
    for (std::list<PointStruct *>::iterator pt = resultPtList->begin(); pt != resultPtList->end(); pt++)
    {
        if ( pt == resultPtList->begin() )
        {
            results[0] = (*pt)->x;
            results[1] = (*pt)->y;
            results[2] = (*pt)->z;
        }
        else
        {
            results[0] = (results[0] + (*pt)->x) / 2.0;
            results[1] = (results[1] + (*pt)->y) / 2.0;
            results[2] = (results[2] + (*pt)->z) / 2.0;
        }
        iRet = 1;
    }

    if (iRet == 1)
    {
        mDeviceManager->setTagInfoS(projectid, TagId, results[0], results[1], results[2], MapLayer);
        mDeviceManager->setTagRangingResultType(1, TagId, 1);
    }

#ifdef PRINT_AOA_CLOUD_POINT
    vilslog::printf("PosCalculate::do_multi_AOA_check() results[%d,%d,%d] \n", (int)results[0], (int)results[1], (int)results[2]);
#endif

#ifdef PRINT_AOA_RESULT
    vilslog::printf("PosCalculate::do_multi_AOA_check() results[%d,%d,%d] \n", (int)results[0], (int)results[1], (int)results[2]);
#endif

    if (bPrintAoACirclePoint)
    {
        vilslog::printRaw("do_multi_AOA_check() results[%d,%d,%d]\n", (int)results[0], (int)results[1], (int)results[2]);
    }

    if (bDumpAoACirclePoint)
    {
        if(rangingNodeInfo == NULL)
            rangingNodeInfo = getProjectRangingNodeInfo(projectid);
        rangingNodeInfo->mAoARangingCloudPointStream<<",\"result\":[" << (int)results[0] <<"," << (int)results[1] <<"," << (int)results[2] <<"]";
        rangingNodeInfo->mAoARangingCloudPointStream<<"}";
    }

    for (std::vector<AoACirclePointStruct*>::iterator i = circlePtList->begin(); i != circlePtList->end(); i++)
    {
        clear_PointStruct_list((*i)->ptList);
        delete (*i)->ptList;
        delete (*i);
    }
    circlePtList->clear();
    delete circlePtList;

    clear_PointStruct_list(resultPtList);
    resultPtList->clear();
    delete resultPtList;

    return iRet;
}

int PosCalculate::do_multi_AOA_angle_check(const char * TagId, std::list<AoARangingInfoStruct *> * AoARangingInfoList)
{
    int iRet = 0;
    int MapLayer = 0;
    int projectid = 1;

    std::vector<AoACirclePointStruct *> * vectorPtList = new std::vector<AoACirclePointStruct *>();

    ProjectRangingNodeInfoStruct * rangingNodeInfo = NULL;
    if (bDumpAoACirclePoint)
    {
        rangingNodeInfo = getProjectRangingNodeInfo(projectid);
        rangingNodeInfo->mAoAAngleVectorCloudPointStream << "{\"tag\":\"" << TagId << "\",\"data\":[";
        rangingNodeInfo->mAoAAngleCloudPointStream << "{\"tag\":\"" << TagId << "\",\"data\":[";
    }

    for (std::list<AoARangingInfoStruct *>::iterator aoainfoit = AoARangingInfoList->begin(); aoainfoit != AoARangingInfoList->end(); aoainfoit++)
    {
        (*aoainfoit)->Ranging = 1000; // 10 m
        //(*aoainfoit)->Ranging = 170; // 10 m

        if (bDumpAoACirclePoint)
        {
            if(aoainfoit != AoARangingInfoList->begin())
            {
                rangingNodeInfo = getProjectRangingNodeInfo((*aoainfoit)->projectid);
                rangingNodeInfo->mAoAAngleVectorCloudPointStream << ",";
            }
        }

        std::list<PointStruct *> * ptList = list_cicle_point((*aoainfoit), false);

        AoACirclePointStruct * newAoACircle = new AoACirclePointStruct();
        newAoACircle->aoainfo = (*aoainfoit);
        newAoACircle->ptList = ptList;
        vectorPtList->push_back(newAoACircle);

        MapLayer = newAoACircle->aoainfo->MapLayer;
        projectid = newAoACircle->aoainfo->projectid;
    }

    double results[3] = {0};
    bool bFound = find_closest_angle_point(vectorPtList, results);

    if (bDumpAoACirclePoint)
    {
        if(rangingNodeInfo == NULL)
            rangingNodeInfo = getProjectRangingNodeInfo(projectid);
        rangingNodeInfo->mAoAAngleVectorCloudPointStream<<"]";
        rangingNodeInfo->mAoAAngleCloudPointStream<<"]";
    }

    if (bFound)
    {
        if (bPrintAoACirclePoint)
        {
            vilslog::printRaw("do_multi_AOA_angle_check() results[%d,%d,%d]\n", (int)results[0], (int)results[1], (int)results[2]);
        }

        if (bDumpAoACirclePoint)
        {
            if(rangingNodeInfo == NULL)
                rangingNodeInfo = getProjectRangingNodeInfo(projectid);
            rangingNodeInfo->mAoAAngleVectorCloudPointStream<<",\"result\":[" << (int)results[0] <<"," << (int)results[1] <<"," << (int)results[2] <<"]";
            rangingNodeInfo->mAoAAngleVectorCloudPointStream<<"}";
            rangingNodeInfo->mAoAAngleCloudPointStream<<",\"result\":[" << (int)results[0] <<"," << (int)results[1] <<"," << (int)results[2] <<"]";
            rangingNodeInfo->mAoAAngleCloudPointStream<<"}";
        }

#ifdef PRINT_AOA_RESULT
        vilslog::printRaw("PosCalculate::do_multi_AOA_angle_check() results[%d,%d,%d] \n", (int)results[0], (int)results[1], (int)results[2]);
#endif

        // create dummy point
        mDeviceManager->setTagIndex(projectid, "0710000000000999", -1, "Dummy AoA Tag");
        char * dummytagid = mDeviceManager->getTagId(projectid, "0710000000000999");
        mDeviceManager->setTagInfoS(projectid, dummytagid, results[0], results[1], results[2], MapLayer);
        mDeviceManager->setTagSmoothingPos(projectid, dummytagid, results[0], results[1], results[2]);
        mDeviceManager->setTagRangingResultType(1, dummytagid, 1);
        mDeviceManager->setTagAlive(projectid, "0710000000000999");
    }

    for (std::vector<AoACirclePointStruct*>::iterator i = vectorPtList->begin(); i != vectorPtList->end(); i++)
    {
        clear_PointStruct_list((*i)->ptList);
        delete (*i)->ptList;
        delete (*i);
    }
    vectorPtList->clear();
    delete vectorPtList;

    return iRet;
}

// int PosCalculate::do_two_AOA_angle(AoARangingInfoStruct * aoainfoFirst, AoARangingInfoStruct * aoainfoSecond)
// {
//     int iRet = 0;
//     // 1. 將2點轉換成相對座標系
//     double P1[3] = {0};
//     double P2[3] = {0};
//     double P3[3] = {0};
//     double P4[3] = {0};
//
//     P1[0] = aoainfoFirst->aoaNode->posX;
//     P1[1] = aoainfoFirst->aoaNode->posY;
//     P1[2] = aoainfoFirst->aoaNode->posZ;
//
//     P2[0] = aoainfoSecond->aoaNode->posX;
//     P2[1] = aoainfoSecond->aoaNode->posY;
//     P2[2] = aoainfoSecond->aoaNode->posZ;
//
//     // aoainfoFirst 法線上一點
//     float pitch_Radians = angle_to_rad((float)aoainfoFirst->aoaNode->pitch);
//     float yaw_Radians = angle_to_rad((float)aoainfoFirst->aoaNode->yaw);
//     P3[0] = aoainfoFirst->aoaNode->posX + (double)cos(yaw_Radians)*100;
//     P3[1] = aoainfoFirst->aoaNode->posY + (double)sin(yaw_Radians)*100;
//     P3[2] = aoainfoFirst->aoaNode->posZ + (double)sin(pitch_Radians)*100;
//
//     // aoainfoSecond 法線上一點
//     float pitch_Radians_2 = angle_to_rad((float)aoainfoSecond->aoaNode->pitch);
//     float yaw_Radians_2 = angle_to_rad((float)aoainfoSecond->aoaNode->yaw);
//     P4[0] = aoainfoSecond->aoaNode->posX + (double)cos(yaw_Radians_2)*100;
//     P4[1] = aoainfoSecond->aoaNode->posY + (double)sin(yaw_Radians_2)*100;
//     P4[2] = aoainfoSecond->aoaNode->posZ + (double)sin(pitch_Radians_2)*100;
//
//     //
//     // map 3 ponit to same plane
//     //
//     double R[16];
//     double invR[16];
//     findCoordTransformation(P1, P2, P3, R, invR);
//
//     double nP1[4] = {P1[0], P1[1], P1[2], 1};
//     double nP2[4] = {P2[0], P2[1], P2[2], 1};
//     double nP3[4] = {P3[0], P3[1], P3[2], 1};
//     double nP4[4] = {P4[0], P4[1], P4[2], 1};
//
//     double mP1[4];
//     double mP2[4];
//     double mP3[4];
//     double mP4[4];
//
//     // rotate to new plane
//     DotMatrix(4, invR, nP1, mP1);
//     DotMatrix(4, invR, nP2, mP2);
//     DotMatrix(4, invR, nP3, mP3);
//     DotMatrix(4, invR, nP4, mP4);
//
//     vilslog::printf("\nPosCalculate::do_two_AOA_angle() aoainfoFirst->aoaNode->pitch[%d] roll[%d] yaw[%d]\n",
//         (int)aoainfoFirst->aoaNode->pitch, (int)aoainfoFirst->aoaNode->roll, (int)aoainfoFirst->aoaNode->yaw);
//     vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP1[%d, %d, %d] mP1[%d, %d, %d]\n",
//         (int)nP1[0], (int)nP1[1], (int)nP1[2], (int)mP1[0], (int)mP1[1], (int)mP1[2]);
//     vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP2[%d, %d, %d] mP2[%d, %d, %d]\n",
//         (int)nP2[0], (int)nP2[1], (int)nP2[2], (int)mP2[0], (int)mP2[1], (int)mP2[2]);
//     vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP3[%d, %d, %d] mP3[%d, %d, %d]\n",
//         (int)nP3[0], (int)nP3[1], (int)nP3[2], (int)mP3[0], (int)mP3[1], (int)mP3[2]);
//     vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP4[%d, %d, %d] mP4[%d, %d, %d]\n",
//         (int)nP4[0], (int)nP4[1], (int)nP4[2], (int)mP4[0], (int)mP4[1], (int)mP4[2]);
//
//
//
//
//
//
//
//     // DotMatrix(4, R, mP1, nP1);
//     // DotMatrix(4, R, mP2, nP2);
//     // DotMatrix(4, R, mP3, nP3);
//     // DotMatrix(4, R, mP4, nP4);
//     //
//     // vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP1[%d, %d, %d] mP1[%d, %d, %d]\n",
//     //     (int)nP1[0], (int)nP1[1], (int)nP1[2], (int)mP1[0], (int)mP1[1], (int)mP1[2]);
//     // vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP2[%d, %d, %d] mP2[%d, %d, %d]\n",
//     //     (int)nP2[0], (int)nP2[1], (int)nP2[2], (int)mP2[0], (int)mP2[1], (int)mP2[2]);
//     // vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP3[%d, %d, %d] mP3[%d, %d, %d]\n",
//     //     (int)nP3[0], (int)nP3[1], (int)nP3[2], (int)mP3[0], (int)mP3[1], (int)mP3[2]);
//     // vilslog::printf("\nPosCalculate::do_two_AOA_angle() nP4[%d, %d, %d] mP4[%d, %d, %d]\n",
//     //     (int)nP4[0], (int)nP4[1], (int)nP4[2], (int)mP4[0], (int)mP4[1], (int)mP4[2]);
//     //
//     return iRet;
// }


std::list<PointStruct *> * PosCalculate::list_cicle_point(AoARangingInfoStruct * aoainfo, bool bDump)
{
    std::list<PointStruct *> * ptList = new std::list<PointStruct *>();
    //int absAngle = aoainfo->aoaNode->yaw - (int)aoainfo->TagAngle;
    //int absAngle = aoainfo->aoaNode->yaw - (int)aoainfo->TagAngle + 90; // AoA Coordinator to VILS Coordinator
    //int absAngle = (int)aoainfo->TagAngle;
    float Radians = angle_to_rad(aoainfo->TagAngle);
    int radius = abs(aoainfo->Ranging * cos(Radians));
    float Xoffset = aoainfo->Ranging * sin(Radians);

#ifdef PRINT_AOA_CLOUD_POINT
    vilslog::printRaw("\nPosCalculate::list_cicle_point() nodeid[%s] Ranging[%d] TagAngle[%f] radius[%d] Xoffset[%f]\n",
        aoainfo->aoaNode->nodeid, aoainfo->Ranging, aoainfo->TagAngle, radius, Xoffset);

    vilslog::printRaw("[\n");
#endif

    if(bPrintAoACirclePoint)
    {
        vilslog::printRaw("\nPosCalculate::list_cicle_point() nodeid[%s] Ranging[%d] TagAngle[%f] radius[%d] Xoffset[%f]\n",
            aoainfo->aoaNode->nodeid, aoainfo->Ranging, aoainfo->TagAngle, radius, Xoffset);

        vilslog::printRaw("[\n");
    }

    ProjectRangingNodeInfoStruct * rangingNodeInfo = NULL;
    if (bDumpAoACirclePoint)
    {
        rangingNodeInfo = getProjectRangingNodeInfo(aoainfo->projectid);
        if ( bDump )
            rangingNodeInfo->mAoARangingCloudPointStream << "{\"aoa\":\""<<aoainfo->aoaNode->nodeid<<"\",\"points\":[";
        else
            rangingNodeInfo->mAoAAngleVectorCloudPointStream << "{\"aoa\":\""<<aoainfo->aoaNode->nodeid<<"\",\"points\":[";
    }


    // move 1 cm a step
    //float minAngle = rad_to_angle(asin(1.0f/(float)aoainfo->Ranging));

    // move 10 cm a step
    float minAngle = rad_to_angle(asin(10.0f/(float)aoainfo->Ranging));

    // M_PI/180.0 = 0.017453
    // 180.0/M_PI = 57.295780
    //vilslog::printf("\nPosCalculate::list_cicle_point() minAngle[%f][%f]\n", minAngle, rad_to_angle(minAngle));

    int count = 0;
    PointStruct * ptPre = NULL;
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
        }

#ifdef PRINT_AOA_CLOUD_POINT
        if(count > 0)
            vilslog::printRaw(",");
        vilslog::printRaw("[%d,%d,%d]", (int)results[0], (int)results[1], (int)results[2]);
#endif

        if(bPrintAoACirclePoint)
        {
            if(count > 0)
                vilslog::printRaw(",");
            vilslog::printRaw("[%d,%d,%d]", (int)results[0], (int)results[1], (int)results[2]);
        }

        if (bDumpAoACirclePoint)
        {
            if(rangingNodeInfo == NULL)
                rangingNodeInfo = getProjectRangingNodeInfo(aoainfo->projectid);
            if ( bDump )
            {
                if(count > 0)
                    rangingNodeInfo->mAoARangingCloudPointStream << ",";
                rangingNodeInfo->mAoARangingCloudPointStream << "["<<(int)results[0] <<","<< (int)results[1] <<","<< (int)results[2] <<"]";
            }
            else
            {
                if(count > 0)
                    rangingNodeInfo->mAoAAngleVectorCloudPointStream << ",";
                rangingNodeInfo->mAoAAngleVectorCloudPointStream << "["<<(int)results[0] <<","<< (int)results[1] <<","<< (int)results[2] <<"]";
            }
        }

        PointStruct * newPt = new PointStruct();
        newPt->x = results[0];
        newPt->y = results[1];
        newPt->z = results[2];
        ptList->push_back(newPt);

        ptPre = newPt;
        count++;
    }

#ifdef PRINT_AOA_CLOUD_POINT
    vilslog::printRaw("\n];\n");
#else
    if(bPrintAoACirclePoint)
    {
        vilslog::printRaw("\n];\n");
    }
#endif
    if (bDumpAoACirclePoint)
    {
        if(rangingNodeInfo == NULL)
            rangingNodeInfo = getProjectRangingNodeInfo(aoainfo->projectid);
        if ( bDump )
            rangingNodeInfo->mAoARangingCloudPointStream << "]}";
        else
            rangingNodeInfo->mAoAAngleVectorCloudPointStream << "]}";
    }
    return ptList;
}

int PosCalculate::calc_rotate_point(AoARangingInfoStruct * aoainfo, float Xoffset, int radius, float angle, double * results)
{
    float angleRad = angle_to_rad(angle);
    float y = radius * sin(angleRad);
    float z = radius * cos(angleRad);

    double origAoA[3] = {0,0,0};
    origAoA[0] = Xoffset;
    origAoA[1] = (double)y;
    origAoA[2] = (double)z;
    // origAoA[0] = aoainfo->aoaNode->posX + Xoffset;
    // origAoA[1] = aoainfo->aoaNode->posY + (double)y;
    // origAoA[2] = aoainfo->aoaNode->posZ + (double)z;

    // double normal_vector[3] = {0,0,0};
    // float pitch_Radians = angle_to_rad((float)aoainfo->aoaNode->pitch);
    // float yaw_Radians = angle_to_rad((float)aoainfo->aoaNode->yaw);
    //
    // normal_vector[0] = (double)cos(yaw_Radians)*1000;
    // normal_vector[1] = (double)sin(yaw_Radians)*1000;
    // normal_vector[2] = (double)sin(pitch_Radians)*1000;

    calc_AoA_result(aoainfo, aoainfo->aoaNorm, origAoA, results);


    // //
    // // roll angle
    // //
    // double result_roll[3] = {0,0,0};
    // //int yaw = aoainfo->aoaNode->yaw + 90; // AoA Coordinator to VILS Coordinator
    // //int yaw = aoainfo->aoaNode->yaw;
    // int yaw = aoainfo->aoaNode->yaw - 90;
    // rotate_AoA_normal_vector(aoainfo->aoaNode->posX, aoainfo->aoaNode->posY, aoainfo->aoaNode->posZ,
    //         normal_vector[0], normal_vector[1], normal_vector[2],
    //         aoainfo->aoaNode->pitch, aoainfo->aoaNode->roll, yaw,
    //         results, result_roll);
    //
    // // vilslog::printf("WebUDPParserService::calc_circle_point() aoaNode nodeid[%s] pry[%d,%d,%d] results[%d,%d,%d] result_roll[%d,%d,%d]\n",
    // //     aoainfo->aoaNode->nodeid,
    // //     aoainfo->aoaNode->pitch, aoainfo->aoaNode->roll, aoainfo->aoaNode->yaw,
    // //     (int)results[0], (int)results[1], (int)results[2],
    // //     (int)result_roll[0], (int)result_roll[1], (int)result_roll[2]);
    //
    // results[0] = result_roll[0];
    // results[1] = result_roll[1];
    // results[2] = result_roll[2];
    return 0;
}

std::list<PointStruct *> * PosCalculate::find_closest_point(std::vector<AoACirclePointStruct *> * circlePtList)
{
    std::list<PointStruct *> * resultPtList = new std::list<PointStruct *>();

    if ( circlePtList->size() <= 1 )
        return resultPtList;

    for(unsigned int i=0;i<circlePtList->size();i++)
    {
        for(unsigned int j=i+1;j<circlePtList->size();j++)
        {
            AoACirclePointStruct * aoaCirPt1 = circlePtList->at(i);
            AoACirclePointStruct * aoaCirPt2 = circlePtList->at(j);

            std::list<PointStruct *> * ptList1 = aoaCirPt1->ptList;
            std::list<PointStruct *> * ptList2 = aoaCirPt2->ptList;

            double results[3] = {0,0,0};
            if ( find_closest_cross_point(ptList1, ptList2, results) )
            {
                if ( results[2] <= 0.0 || results[2] >= 250 )
                {
                    // vilslog::printf("PosCalculate::find_closest_point() skip results[%d,%d,%d] \n", (int)results[0], (int)results[1], (int)results[2]);
                }
                else
                {

                    PointStruct * pt = new PointStruct();
                    pt->x = results[0];
                    pt->y = results[1];
                    pt->z = results[2];
                    resultPtList->push_back(pt);

#ifdef PRINT_AOA_CLOUD_POINT
                    vilslog::printRaw("PosCalculate::find_closest_point() results[%d,%d,%d] \n", (int)results[0], (int)results[1], (int)results[2]);
#endif
#ifdef PRINT_AOA_RESULT
                    vilslog::printRaw("PosCalculate::find_closest_point() results[%d,%d,%d] \n", (int)results[0], (int)results[1], (int)results[2]);
#endif

                    if(bPrintAoACirclePoint)
                    {
                        vilslog::printRaw("find_closest_point() results[%d,%d,%d]\n", (int)results[0], (int)results[1], (int)results[2]);
                    }
                }
            }
        }
    }

    return resultPtList;
}

bool PosCalculate::find_closest_cross_point(std::list<PointStruct *> * ptList1, std::list<PointStruct *> * ptList2, double * results)
{
    bool bFound = false;
    double length = 100000000000;
    for (std::list<PointStruct*>::iterator pt1 = ptList1->begin(); pt1 != ptList1->end(); pt1++)
    {
        for (std::list<PointStruct*>::iterator pt2 = ptList2->begin(); pt2 != ptList2->end(); pt2++)
        {
            double diffX = (*pt1)->x - (*pt2)->x;
            double diffY = (*pt1)->y - (*pt2)->y;
            double diffZ = (*pt1)->z - (*pt2)->z;
            double distance = pow(diffX, 2) + pow(diffY, 2) + pow(diffZ, 2);
            // if ( length > distance )
            if ( length > distance )
            {

                // vilslog::printRaw("find_closest_point() distance[%d] pt1[%d,%d,%d] pt2[%d,%d,%d]\n",
                //     (int)distance, (int)(*pt1)->x, (int)(*pt1)->y, (int)(*pt1)->z, (int)(*pt2)->x, (int)(*pt2)->y, (int)(*pt2)->z);

                length = distance;
                results[0] = ((*pt1)->x + (*pt2)->x) / 2;
                results[1] = ((*pt1)->y + (*pt2)->y) / 2;
                results[2] = ((*pt1)->z + (*pt2)->z) / 2;
                bFound = true;
            }
        }
    }
    return bFound;
}

bool PosCalculate::find_closest_angle_point(std::vector<AoACirclePointStruct *> * vectorPtList, double * results)
{
    if ( vectorPtList->size() <= 1 )
        return false;

    std::vector<AoACirclePointStruct *> * circlePtList = new std::vector<AoACirclePointStruct *>();

    ProjectRangingNodeInfoStruct * rangingNodeInfo = NULL;

    int count = 0;
    for(unsigned int i=0;i<vectorPtList->size();i++)
    {
        for(unsigned int j=i+1;j<vectorPtList->size();j++)
        {
            AoACirclePointStruct * aoaCirPt1 = vectorPtList->at(i);
            AoACirclePointStruct * aoaCirPt2 = vectorPtList->at(j);

            if(bPrintAoACirclePoint)
            {
                vilslog::printRaw("find_closest_angle_point() aoaCirPt1->ptList->size()[%d] aoaCirPt2->ptList->size()[%d] \n",
                (int)aoaCirPt1->ptList->size(), (int)aoaCirPt2->ptList->size());
            }

            if (bDumpAoACirclePoint)
            {
                if (rangingNodeInfo == NULL)
                    rangingNodeInfo = getProjectRangingNodeInfo(aoaCirPt1->aoainfo->projectid);

                if(count != 0)
                    rangingNodeInfo->mAoAAngleCloudPointStream << ",";
                // rangingNodeInfo->mAoAAngleCloudPointStream << "[";
            }

            std::list<PointStruct *> * resultPtList = find_cross_circle_point(aoaCirPt1, aoaCirPt2);

            // if (bDumpAoACirclePoint)
            // {
            //     if(rangingNodeInfo == NULL)
            //         rangingNodeInfo = getProjectRangingNodeInfo(aoaCirPt1->aoainfo->projectid);
            //     rangingNodeInfo->mAoAAngleCloudPointStream << "]";
            // }

            AoACirclePointStruct * newAoACircle = new AoACirclePointStruct();
            newAoACircle->aoainfo = aoaCirPt1->aoainfo;
            newAoACircle->ptList = resultPtList;
            circlePtList->push_back(newAoACircle);

            count++;
        }
    }

    if(bPrintAoACirclePoint)
    {
        vilslog::printRaw("find_closest_angle_point() circlePtList->size()[%d] \n", (int)circlePtList->size());
    }

    bool bFound = find_match_angle_point(circlePtList, results);

    for (std::vector<AoACirclePointStruct*>::iterator i = circlePtList->begin(); i != circlePtList->end(); i++)
    {
        clear_PointStruct_list((*i)->ptList);
        delete (*i)->ptList;
        delete (*i);
    }
    circlePtList->clear();
    delete circlePtList;

    return bFound;
}

std::list<PointStruct *> * PosCalculate::find_cross_circle_point(AoACirclePointStruct * aoaCirPt1, AoACirclePointStruct * aoaCirPt2)
{
    std::list<PointStruct *> * resultPtList = new std::list<PointStruct *>();

    std::list<PointStruct *> * ptList1 = aoaCirPt1->ptList;
    std::list<PointStruct *> * ptList2 = aoaCirPt2->ptList;

    PointStruct * ptPre = NULL;

    // vilslog::printf("PosCalculate::find_cross_circle_point() ptList1->size()[%d] \n", (int)ptList1->size());
    // vilslog::printf("PosCalculate::find_cross_circle_point() ptList2->size()[%d] \n", (int)ptList2->size());
#ifdef PRINT_AOA_ANGLE_CLOUD_POINT
    vilslog::printRaw("\nPRINT_AOA_ANGLE_CLOUD_POINT\n");
    vilslog::printRaw("[\n");
#endif

    if(bPrintAoACirclePoint)
    {
        vilslog::printRaw("find_cross_circle_point() \n");
        vilslog::printRaw("[\n");
    }

    ProjectRangingNodeInfoStruct * rangingNodeInfo = NULL;
    if (bDumpAoACirclePoint)
    {
        rangingNodeInfo = getProjectRangingNodeInfo(aoaCirPt1->aoainfo->projectid);
        rangingNodeInfo->mAoAAngleCloudPointStream << "{\"aoa1\":\""<<aoaCirPt1->aoainfo->aoaNode->nodeid<<"\"";
        rangingNodeInfo->mAoAAngleCloudPointStream << ",\"aoa2\":\""<<aoaCirPt2->aoainfo->aoaNode->nodeid<<"\"";
        rangingNodeInfo->mAoAAngleCloudPointStream << ",\"points\":[";
    }

    int count = 0;
    for (std::list<PointStruct*>::iterator pt1 = ptList1->begin(); pt1 != ptList1->end(); pt1++)
    {
        double vector1[3] = { 0 };
        double results[3] = { 0 };

        vector1[0] = (*pt1)->x - aoaCirPt1->aoainfo->aoaNode->posX;
        vector1[1] = (*pt1)->y - aoaCirPt1->aoainfo->aoaNode->posY;
        vector1[2] = (*pt1)->z - aoaCirPt1->aoainfo->aoaNode->posZ;

        bool bFound = false;
        double length = 100000000000;
        for (std::list<PointStruct*>::iterator pt2 = ptList2->begin(); pt2 != ptList2->end(); pt2++)
        {
            double vector2[3] = { 0 };
            double crosspt1[3] = { 0 };
            double crosspt2[3] = { 0 };

            vector2[0] = (*pt2)->x - aoaCirPt2->aoainfo->aoaNode->posX;
            vector2[1] = (*pt2)->y - aoaCirPt2->aoainfo->aoaNode->posY;
            vector2[2] = (*pt2)->z - aoaCirPt2->aoainfo->aoaNode->posZ;

            double distance = find_two_line_distance((*pt1), vector1, (*pt2), vector2, crosspt1, crosspt2);

            // double diffX = vector1[0] - vector2[0];
            // double diffY = vector1[1] - vector2[1];
            // double diffZ = vector1[2] - vector2[2];
            // double distance = pow(diffX, 2) + pow(diffY, 2) + pow(diffZ, 2);
            if ( length > distance )
            {
                length = distance;
                results[0] = (crosspt1[0] + crosspt2[0]) / 2;
                results[1] = (crosspt1[1] + crosspt2[1]) / 2;
                results[2] = (crosspt1[2] + crosspt2[2]) / 2;
                bFound = true;
            }
        }

        if ( bFound && ((int)length == 0) )
        {
            if ( ptPre != NULL )
            {
                if ((int)ptPre->x == (int)results[0] &&
                    (int)ptPre->y == (int)results[1] &&
                    (int)ptPre->z == (int)results[2] )
                {
                    // same point
                    continue;
                }
            }

            if ( results[2] <= 0.0 || results[2] >= 250 )
            {
                //vilslog::printf("PosCalculate::find_cross_circle_point() skip results[%d,%d,%d] \n", (int)results[0], (int)results[1], (int)results[2]);
            }
            else
            {
                //
                // check if point one the another side
                //
                double vector1[3] = { 0 };
                vector1[0] = results[0] - aoaCirPt1->aoainfo->aoaNode->posX;
                vector1[1] = results[1] - aoaCirPt1->aoainfo->aoaNode->posY;
                vector1[2] = results[2] - aoaCirPt1->aoainfo->aoaNode->posZ;

                double dotRet1 = dotproduct(aoaCirPt1->aoainfo->aoaNorm, vector1);

                if ( dotRet1 < 0.0 )
                {
                    //vilslog::printf("dotRet1[%f] < 0 [%d,%d,%d] \n", dotRet1, (int)results[0], (int)results[1], (int)results[2]);
                    continue;
                }

                double vector2[3] = { 0 };
                vector2[0] = results[0] - aoaCirPt2->aoainfo->aoaNode->posX;
                vector2[1] = results[1] - aoaCirPt2->aoainfo->aoaNode->posY;
                vector2[2] = results[2] - aoaCirPt2->aoainfo->aoaNode->posZ;

                double dotRet2 = dotproduct(aoaCirPt2->aoainfo->aoaNorm, vector2);

                if ( dotRet2 < 0.0 )
                {
                    //vilslog::printf("dotRet2[%f] < 0 [%d,%d,%d] \n", dotRet2, (int)results[0], (int)results[1], (int)results[2]);
                    continue;
                }

#ifdef PRINT_AOA_ANGLE_CLOUD_POINT
                if (count != 0)
                    vilslog::printRaw(",");
                vilslog::printRaw("[%d,%d,%d]", (int)results[0], (int)results[1], (int)results[2]);
#endif

                if(bPrintAoACirclePoint)
                {
                    if (count != 0)
                        vilslog::printRaw(",");
                    vilslog::printRaw("[%d,%d,%d]", (int)results[0], (int)results[1], (int)results[2]);
                }

                if (bDumpAoACirclePoint)
                {
                    if(rangingNodeInfo == NULL)
                        rangingNodeInfo = getProjectRangingNodeInfo(aoaCirPt1->aoainfo->projectid);
                    if(count > 0)
                    {
                        rangingNodeInfo->mAoAAngleCloudPointStream << ",";
                    }
                    rangingNodeInfo->mAoAAngleCloudPointStream << "["<<(int)results[0] <<","<< (int)results[1] <<","<< (int)results[2] <<"]";
                }

                PointStruct * pt = new PointStruct();
                pt->x = results[0];
                pt->y = results[1];
                pt->z = results[2];
                resultPtList->push_back(pt);

                ptPre = pt;
                count++;
            }
        }
    }
#ifdef PRINT_AOA_ANGLE_CLOUD_POINT
    vilslog::printRaw("\n];\n");
#endif

    if(bPrintAoACirclePoint)
    {
        vilslog::printRaw("\n];\n");
    }

    if (bDumpAoACirclePoint)
    {
        if(rangingNodeInfo == NULL)
            rangingNodeInfo = getProjectRangingNodeInfo(aoaCirPt1->aoainfo->projectid);
        rangingNodeInfo->mAoAAngleCloudPointStream << "]}";
    }
    //vilslog::printf("PosCalculate::find_cross_circle_point() length[%d] \n", (int) sqrt(length));

    return resultPtList;
}

bool PosCalculate::find_match_angle_point(std::vector<AoACirclePointStruct *> * circlePtList, double * results)
{
    bool bFound = false;
    if ( (int)circlePtList->size() == 1 )
    {
        PointStruct * closestPt = NULL;
        double distance = 100000000;

        //
        // find height == 100 point
        //
        for (std::vector<AoACirclePointStruct*>::iterator i = circlePtList->begin(); i != circlePtList->end(); i++)
        {
            std::list<PointStruct *> * resultPtList = (*i)->ptList;

            for (std::list<PointStruct*>::iterator pt = resultPtList->begin(); pt != resultPtList->end(); pt++)
            {
                //vilslog::printf("PosCalculate::find_match_angle_point() pt[%d,%d,%d] \n", (int)((*pt)->x), (int)((*pt)->y), (int)((*pt)->z));

                double diffDist = fabs((*pt)->z - 100);
                if (distance > diffDist)
                {
                    distance = diffDist;
                    closestPt = (*pt);
                }

                if( (*pt)->z > 99 && (*pt)->z < 101 )
                {
                    results[0] = (*pt)->x;
                    results[1] = (*pt)->y;
                    results[2] = (*pt)->z;
                    bFound = true;

                    if(bPrintAoACirclePoint)
                    {
                        vilslog::printRaw("find_match_angle_point() pt[%d,%d,%d] \n", (int)((*pt)->x), (int)((*pt)->y), (int)((*pt)->z));
                    }
#ifdef PRINT_AOA_ANGLE_CLOUD_POINT
                    vilslog::printRaw("PosCalculate::find_match_angle_point() pt[%d,%d,%d] \n", (int)((*pt)->x), (int)((*pt)->y), (int)((*pt)->z));
#endif
                    //break;
                }
            }
        }

        if (!bFound && closestPt != NULL)
        {
            results[0] = closestPt->x;
            results[1] = closestPt->y;
            results[2] = 100;
            bFound = true;

            if(bPrintAoACirclePoint)
            {
                vilslog::printRaw("find_match_angle_point() closestPt[%d,%d,%d] \n", (int)(closestPt->x), (int)(closestPt->y), (int)(closestPt->z));
            }
#ifdef PRINT_AOA_ANGLE_CLOUD_POINT
            vilslog::printRaw("PosCalculate::find_match_angle_point() closestPt[%d,%d,%d] \n", (int)(closestPt->x), (int)(closestPt->y), (int)(closestPt->z));
#endif
        }
    }
    else
    {
        std::list<PointStruct *> * resultPtList = find_closest_point(circlePtList);

        //vilslog::printf("PosCalculate::find_match_angle_point() resultPtList->size()[%d] \n", (int)resultPtList->size());

        for (std::list<PointStruct *>::iterator pt = resultPtList->begin(); pt != resultPtList->end(); pt++)
        {
            if(bPrintAoACirclePoint)
            {
                vilslog::printRaw("find_match_angle_point() pt[%d,%d,%d] \n", (int)((*pt)->x), (int)((*pt)->y), (int)((*pt)->z));
            }
            if ( pt == resultPtList->begin() )
            {
                results[0] = (*pt)->x;
                results[1] = (*pt)->y;
                results[2] = (*pt)->z;
            }
            else
            {
                results[0] = (results[0] + (*pt)->x) / 2.0;
                results[1] = (results[1] + (*pt)->y) / 2.0;
                results[2] = (results[2] + (*pt)->z) / 2.0;
            }

            bFound = true;
        }

        if(bPrintAoACirclePoint)
        {
            vilslog::printRaw("find_match_angle_point() results[%d,%d,%d] \n",(int)results[0], (int)results[1], (int)results[2]);
        }
#ifdef PRINT_AOA_ANGLE_CLOUD_POINT
            vilslog::printRaw("find_match_angle_point() results[%d,%d,%d] \n",(int)results[0], (int)results[1], (int)results[2]);
#endif
#ifdef PRINT_AOA_RESULT
            vilslog::printRaw("find_match_angle_point() results[%d,%d,%d] \n",(int)results[0], (int)results[1], (int)results[2]);
#endif

        clear_PointStruct_list(resultPtList);
        resultPtList->clear();
        delete resultPtList;
    }

    if(bPrintAoACirclePoint)
    {
        if(!bFound)
        {
            vilslog::printRaw("find_match_angle_point() Not Found Point! \n");
        }
    }

#ifdef PRINT_AOA_ANGLE_CLOUD_POINT
    if(!bFound)
    {
        vilslog::printRaw("PosCalculate::find_match_angle_point() Not Found Point! \n");
    }
#endif

    return bFound;
}

int PosCalculate::do_3D_check(int projectid, const char * TagId, int tagIndexInVector, RangingResultStruct * validRangingData, int usingIMU, int IMUOffset, int SteadyStateOffset, int enable_GDOP, int GDOP_Threshold, int validdatatime, int enable_FixHeight)
{
    double results[3] = {0,0,0};
    double preresult[3] = {0,0,0};
    double tri3Dresult1[3] = {0,0,0};
    double tri3Dresult2[3] = {0,0,0};
    int valid = validRangingData->validCount;

#ifdef PRINTF_RANGINGRESULT_LOG
    vilslog::printf("PosCalculate::do_3D_check() ----------------start -----------------\n");
    vilslog::printf("PosCalculate::do_3D_check() TagId[%s] valid[%d]\n", TagId, valid);
    print_ranging_result_data(tagIndexInVector, validRangingData);
#endif

    if (valid < 3)
    {
        return -2;
    }
    else
    if (valid >= 3)
    {
        bool bSuccess = false;

        ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

        if ( valid > 3 )
        {
            //double results_new[3];
            //long long cal_do_leastsquares_start = getSystemTime();
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(results, 3);
            bSuccess = do_leastsquares_new(projectid, validRangingData, enable_GDOP, GDOP_Threshold, results);


            if ( (int)results[0] == 0 && (int)results[1] == 0 && (int)results[2] == 0)
            {
                bSuccess = false;
            }
            if ( (int)results[0] == 0 && (int)results[1] == 1 && (int)results[2] == 0)
            {
                bSuccess = false;
            }

            if (bSuccess)
            {
                if (enable_FixHeight == 1)
                {
                    bSuccess = checkInside(projectid, validRangingData, results);
                }
            }



            // long long cal_do_leastsquares = getSystemTime() - cal_do_leastsquares_start;
            // printf("PosCalculate::do_3D_check() cal_do_leastsquares time[%lld]\n", cal_do_leastsquares);
        }

        //
        // test only
        //
        // bSuccess = false;

        if (bSuccess)
        {
#ifdef PRINTF_RANGINGRESULT_LOG
            printf ("do_3D_check() do_leastsquares_new() TagId[%s] bSuccess[%d] results[%d, %d, %d]\n",
                 TagId, bSuccess, (int)results[0], (int)results[1], (int)results[2]);
#endif
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setRawResult(results, 3);
        }
        else
        {
#ifdef PRINTF_RANGINGRESULT_LOG
            if ( valid > 3 )
            {
                printf ("do_3D_check() do_leastsquares_new() TagId[%s] bSuccess[%d]\n", TagId, bSuccess);
            }
#endif

            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(preresult, 3);


            //long long cal_do_trilateration_3D_start = getSystemTime();
            //double results_new[3];
            //=====================
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(tri3Dresult1, 3);
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(tri3Dresult2, 3);
            bool bSuccess1 = do_trilateration_3D_new(projectid, validRangingData, tri3Dresult1, true);
            bool bSuccess2 = do_trilateration_3D_new(projectid, validRangingData, tri3Dresult2, false);
            // //=====================
            //bSuccess = do_trilateration_3D_new(&validRangingData, results, true);
            //
            // use pre result z value
            //
            //results[2] = preresult[2];
            //=====================
            bool bInside1 = true;
            bool bInside2 = true;
            if (enable_FixHeight == 1)
            {
                if (bSuccess1)
                    bInside1 = checkInside(projectid, validRangingData, tri3Dresult1);
                if (bSuccess1)
                    bInside2 = checkInside(projectid, validRangingData, tri3Dresult2);
            }

#ifdef PRINTF_RANGINGRESULT_LOG
            vilslog::printf ("do_3D_check() do_trilateration_3D_new() TagId[%s] bSuccess1[%d] bInside1[%d] results[%d, %d, %d]\n",
                TagId, bSuccess1, bInside1, (int)tri3Dresult1[0], (int)tri3Dresult1[1], (int)tri3Dresult1[2]);
            vilslog::printf ("do_3D_check() do_trilateration_3D_new() TagId[%s] bSuccess2[%d] bInside2[%d] results[%d, %d, %d]\n",
                TagId, bSuccess2, bInside2, (int)tri3Dresult2[0], (int)tri3Dresult2[1], (int)tri3Dresult2[2]);
#endif

            //long long cal_do_trilateration_3D = getSystemTime() - cal_do_trilateration_3D_start;
            //printf("PosCalculate::do_3D_check() cal_do_trilateration_3D time[%lld]\n", cal_do_trilateration_3D);
            if ((bSuccess1 && bInside1) && (bSuccess2 && bInside2))
            {
                double dist1 = TwoPointDistance(preresult, tri3Dresult1);
                double dist2 = TwoPointDistance(preresult, tri3Dresult2);
#ifdef PRINTF_RANGINGRESULT_LOG
                vilslog::printf ("do_3D_check() 3D_new() TagId[%s] dist1[%d] tri3Dresult1[%d, %d, %d]\n",
                   TagId, (int)dist1, (int)tri3Dresult1[0], (int)tri3Dresult1[1], (int)tri3Dresult1[2]);
                vilslog::printf ("do_3D_check() 3D_new() TagId[%s] dist2[%d] tri3Dresult2[%d, %d, %d]\n",
                   TagId, (int)dist2, (int)tri3Dresult2[0], (int)tri3Dresult2[1], (int)tri3Dresult2[2]);
#endif

                if ( dist1 < dist2 )
                {
                    results[0] = tri3Dresult1[0];
                    results[1] = tri3Dresult1[1];
                    results[2] = tri3Dresult1[2];
                }
                else
                {
                    results[0] = tri3Dresult2[0];
                    results[1] = tri3Dresult2[1];
                    results[2] = tri3Dresult2[2];
                }

                //
                // use pre result z value
                //
                //results[2] = preresult[2];

#ifdef PRINTF_RANGINGRESULT_LOG
                vilslog::printf ("do_3D_check() 3D_new() TagId[%s] (bSuccess1 && bSuccess2)[%d] results[%d, %d, %d]\n",
                  TagId, (bSuccess1 && bSuccess2), (int)results[0], (int)results[1], (int)results[2]);
#endif
                if ( (int)results[0] == 0 && (int)results[1] == 0 && (int)results[2] == 0)
                {
                    bSuccess = false;
                    //vilslog::printf ("do_3D_check() do_trilateration_3D_new All zero!!!  not success.\n");
                    rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(results, 3);
                }
                else
                {
                    rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setRawResult(results, 3);
                }
            }
            else
            if (bSuccess1)
            {
                //vilslog::printf ("do_3D_check() TagId[%s] do_trilateration_3D_new bSuccess2 falied!\n", TagId);

                results[0] = tri3Dresult1[0];
                results[1] = tri3Dresult1[1];
                results[2] = tri3Dresult1[2];

                if ( (int)tri3Dresult1[0] == 0 && (int)tri3Dresult1[1] == 0 && (int)tri3Dresult1[2] == 0)
                {
                    bSuccess = false;
                    valid = 0;
                }
                else
                {
                    rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setRawResult(tri3Dresult1, 3);
                }
            }
            else
            if (bSuccess2)
            {
                //vilslog::printf ("do_3D_check() TagId[%s] do_trilateration_3D_new bSuccess1 falied!\n", TagId);

                results[0] = tri3Dresult2[0];
                results[1] = tri3Dresult2[1];
                results[2] = tri3Dresult2[2];

                if ( (int)tri3Dresult2[0] == 0 && (int)tri3Dresult2[1] == 0 && (int)tri3Dresult2[2] == 0)
                {
                    bSuccess = false;
                    valid = 0;
                }
                else
                {
                    rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setRawResult(tri3Dresult2, 3);
                }
            }
            else
            {
#ifdef PRINTF_RANGINGRESULT_LOG
                printf ("do_3D_check() TagId[%s] do_trilateration_3D_new not success.\n", TagId);
#endif
                rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(results, 3);
                valid = 0;
            }
        }

        if (valid <= 0 )
        {
#ifdef PRINTF_RANGINGRESULT_LOG
            printf ("do_3D_check() TagId[%s] valid <= 0 return!\n", TagId);
#endif
            return valid;
        }

        if ( rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getRangingCount() < 3)
        {
            // maybe first poistion is far from origin.
            // so we need this result to move quickly toward this poistion.
        }
        else
        {
            //
            // find max velocity
            //
            double Velocity = validRangingData->TagVelocity[0];
            for(int iloop=1;iloop<valid;iloop++)
            {
                if (Velocity < validRangingData->TagVelocity[iloop])
                    Velocity = validRangingData->TagVelocity[iloop];
            }

            // compare two position check if too far
            double preresult[3];
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(preresult, 3);

            // double x_diff = preresult[0] - results[0];
            // double y_diff = preresult[1] - results[1];
            // double z_diff = preresult[2] - results[2];
            double x_diff = results[0] - preresult[0];
            double y_diff = results[1] - preresult[1];
            double z_diff = results[2] - preresult[2];
            double distance =  sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));

            // if ( strcmp(TagId, "Tag0022") == 0 )
            // {
            //     vilslog::printf("do_3D_check() TagId[%s] usingIMU[%d] distance[%d] Velocity[%d]\n",
            //         TagId, usingIMU, (int)distance, (int)Velocity);
            // }

            double movoffset = distance;
            if (usingIMU == 1)
            {
                movoffset = Velocity * IMUOffset;//  calculate from IMU data, convert to cm.
                if ( distance > movoffset )
                {
                    // using Velocity fix distance
                }
                else
                {
                    movoffset = distance;
                }
            }
            // if ( strcmp(TagId, "Tag0022") == 0 )
            // {
            //     vilslog::printf("do_3D_check() TagId[%s] movoffset[%d]\n", TagId, (int)movoffset);
            // }

            if ( Velocity <= 0.10 )
            {
                if ( movoffset > SteadyStateOffset )
                    movoffset = SteadyStateOffset;//cm
            }
            // if ( strcmp(TagId, "Tag0022") == 0 )
            // {
            //     vilslog::printf("do_3D_check() TagId[%s] SteadyStateOffset[%d] movoffset[%d]\n",
            //         TagId, SteadyStateOffset, (int)movoffset);
            // }
            //
            // interpolation
            //
            //float ratef =  fabs((float)(distance - movoffset) / (float)distance);
            float ratef =  fabs((float)(movoffset) / (float)distance);
            // double n_xval = results[0] + x_diff * ratef;
            // double n_yval = results[1] + y_diff * ratef;
            // double n_zval = results[2] + z_diff * ratef;
            double n_xval = preresult[0] + x_diff * ratef;
            double n_yval = preresult[1] + y_diff * ratef;
            double n_zval = preresult[2] + z_diff * ratef;

// #ifdef PRINTF_RANGINGRESULT_LOG
    // if ( strcmp(TagId, "Tag0022") == 0 )
    // {
    //     printf ("do_3D_check() TagId[%s] ratef=%f, movoffset=%f\n", TagId, ratef, movoffset);
    //     printf ("do_3D_check() TagId[%s] before preresult[%.02f, %.02f, %.02f]\n", TagId, preresult[0], preresult[1], preresult[2]);
    //     printf ("do_3D_check() TagId[%s] before results[%.02f, %.02f, %.02f]\n", TagId, results[0], results[1], results[2]);
    //     printf ("do_3D_check() TagId[%s] after  results[%.02f, %.02f, %.02f]\n", TagId, n_xval, n_yval, n_zval);
    // }
// #endif

            results[0] = n_xval;
            results[1] = n_yval;
            results[2] = n_zval;
        }

        //
        // skip strange data
        //
        if ( (int)results[0] > 100000 || (int)results[1] > 100000 || (int)results[2] > 100000)
        {
            return 0;
        }
        if ( (int)results[0] < -100000 || (int)results[1] < -100000 || (int)results[2] < -100000)
        {
            return 0;
        }

        if ( (int)results[0] == 0 && (int)results[1] == 0 && (int)results[2] == 0)
        {
            vilslog::printf ("do_3D_check() All ZERO skip strange data. TagId[%s] tagIndexInVector[%d] validCount[%d]\n",
                TagId, tagIndexInVector, validRangingData->validCount);
            return 0;
        }


        rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->setResult(results, 3);

#ifdef PRINTF_RANGINGRESULT_LOG
        vilslog::printf ("PosCalculate::do_3D_check() TagId[%s] before      results[%d, %d, %d] getRangingCount[%d]\n",
            TagId, (int)results[0], (int)results[1], (int)results[2], rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getRangingCount());

        vilslog::printf ("do_3D_check() TagId[%s] ApplyKalman before results[%d, %d, %d]\n",
           TagId, (int)results[0], (int)results[1], (int)results[2]);
#endif

        //
        // apply Kalman Filter
        //
        rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->update(validdatatime);

        if ( rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getRangingCount() >= 30)
        {
            rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getResult(results, 3);
        }


#ifdef PRINTF_RANGINGRESULT_LOG
       printf ("do_3D_check() TagId[%s] ApplyKalman  after results[%d, %d, %d]\n",
            TagId, (int)results[0], (int)results[1], (int)results[2]);
#endif

        int tagMapLayer = resolveTagMapLayer(validRangingData);
        mDeviceManager->setTagInfoS(projectid, TagId, results[0], results[1], results[2], tagMapLayer);
        mDeviceManager->setTagRangingResultType(projectid, TagId, 3);


        // vilslog::printf ("do_3D_check() TagId[%s] results[%d, %d, %d] tagMapLayer[%d]\n",
        //    TagId, (int)results[0], (int)results[1], (int)results[2], tagMapLayer);
    }

    return valid;
}

void PosCalculate::start_dump_ranging_result(int timeout)
{
    bStartLogging = true;
    mTimeoutLogging = timeout;
}

void PosCalculate::dump_ranging_result_header()
{
    char logString[2048];
    char tmpString[128];
    memset(logString, 0, 2048);
    sprintf(logString, "TimeStamp");

    snprintf(tmpString, 128, ",tag");
    strcat(logString, tmpString);

    int projectid = 1;
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    // Anchor name
    for (std::list<NodeStruct*>::iterator anchor = rangingNodeInfo->mAnchorNodeList->begin(); anchor != rangingNodeInfo->mAnchorNodeList->end(); anchor++)
    {
        snprintf(tmpString, 128, ",%s", (*anchor)->nodeid);
        strcat(logString, tmpString);
    }

    // Locator name
    for (std::list<NodeStruct*>::iterator locator = rangingNodeInfo->mLocatorNodeList->begin(); locator != rangingNodeInfo->mLocatorNodeList->end(); locator++)
    {
        snprintf(tmpString, 128, ",%s", (*locator)->nodeid);
        strcat(logString, tmpString);
    }

    // valid ranging count
    snprintf(tmpString, 128, ",valid");
    strcat(logString, tmpString);

    // raw result
    snprintf(tmpString, 128, ",rX,rY,rZ");
    strcat(logString, tmpString);

    // kalman filter result
    snprintf(tmpString, 128, ",kX,kY,kZ");
    strcat(logString, tmpString);

    // smoothing filter result
    snprintf(tmpString, 128, ",sX,sY,sZ");
    strcat(logString, tmpString);

    // IMU data
    snprintf(tmpString, 128, ",aX,aY,aZ,gX,gY,gZ");
    strcat(logString, tmpString);

    // AoA Tage Angle
    snprintf(tmpString, 128, ",angle");
    strcat(logString, tmpString);

    LOGGER->LogWOTimeStamp("%s", logString);
}

void PosCalculate::dump_ranging_result_data(int tagIndexInVector, double * rawresult, double * results, int * smresult, RangingResultStruct * validRangingData)
{
    char logString[2048];
    char tmpString[30];

    memset(logString, 0, 2048);

    // vilslog::printf("Tag[%d] rawresult[%d, %d, %d]",
    //     Tag_idx, (int)rawresult[0], (int)rawresult[1], (int)rawresult[2]);
    // vilslog::printf(" results[%d, %d, %d] validCount[%d]\n\t",
    //     (int)results[0], (int)results[1], (int)results[2], validRangingData->validCount);

    //LOGGER->Log("Tag[%d] rawresult[%d, %d, %d]", Tag_idx, (int)rawresult[0], (int)rawresult[1], (int)rawresult[2]);
    //LOGGER->Log(" results[%d, %d, %d] validCount[%d]\n\t", (int)results[0], (int)results[1], (int)results[2], validRangingData->validCount);

    int projectid = 1;
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    // tag name
    snprintf(tmpString, 30, ",%s", rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->getTagName());
    strcat(logString, tmpString);

    // Anchors
    int anchordistance = 0;
    for (std::list<NodeStruct*>::iterator anchor = rangingNodeInfo->mAnchorNodeList->begin(); anchor != rangingNodeInfo->mAnchorNodeList->end(); anchor++)
    {
        bool bFound = false;
        anchordistance = 0;
        for(int i=0;i<validRangingData->validCount && i<MAX_RANGING_RESULT;i++)
        {
            if ( strcmp((*anchor)->nodeid, validRangingData->AnchorName[i]) == 0)
            {
                //anchordistance = validRangingData->RangingFiltered[i];
                anchordistance = validRangingData->RangingRaw[i];
                bFound = true;
                break;
            }
        }
        if (anchordistance > 0 && bFound)
        {
            snprintf(tmpString, 30, ",%d", anchordistance);
            //printf ("%s[%d] ", (*anchor)->nodename, anchordistance);
            //LOGGER->Log("%s[%d] ", (*anchor)->nodename, anchordistance);
        }
        else
        {
            snprintf(tmpString, 30, ",");
        }
        strcat(logString, tmpString);
    }

    // Locators
    float tagAngle = 0.0f;
    int locatordistance = 0;
    for (std::list<NodeStruct*>::iterator locator = rangingNodeInfo->mLocatorNodeList->begin(); locator != rangingNodeInfo->mLocatorNodeList->end(); locator++)
    {
        bool bFound = false;
        locatordistance = 0;
        for(int i=0;i<validRangingData->validCount && i<MAX_RANGING_RESULT;i++)
        {
            if ( strcmp((*locator)->nodeid, validRangingData->AnchorName[i]) == 0)
            {
                locatordistance = validRangingData->RangingRaw[i];
                tagAngle = validRangingData->TagAngle[i];
                bFound = true;
                break;
            }
        }
        if (locatordistance > 0 && bFound)
        {
            snprintf(tmpString, 30, ",%d", locatordistance);
        }
        else
        {
            snprintf(tmpString, 30, ",");
        }
        strcat(logString, tmpString);
    }


    // valid ranging count
    snprintf(tmpString, 30, ",%d", validRangingData->validCount);
    strcat(logString, tmpString);

    // raw result
    snprintf(tmpString, 30, ",%d,%d,%d", (int)rawresult[0], (int)rawresult[1], (int)rawresult[2]);
    strcat(logString, tmpString);

    // kalman filter result
    snprintf(tmpString, 30, ",%d,%d,%d", (int)results[0], (int)results[1], (int)results[2]);
    strcat(logString, tmpString);

    // smoothing filter result
    snprintf(tmpString, 30, ",%d,%d,%d", smresult[0], smresult[1], smresult[2]);
    strcat(logString, tmpString);

    // IMU data
    snprintf(tmpString, 30, ",%d,%d,%d,%d,%d,%d", validRangingData->AnchorTagIMU[0][0], validRangingData->AnchorTagIMU[0][1], validRangingData->AnchorTagIMU[0][2],
                validRangingData->AnchorTagIMU[0][3], validRangingData->AnchorTagIMU[0][4], validRangingData->AnchorTagIMU[0][5]);
    strcat(logString, tmpString);

    // AoA Tag Angle
    snprintf(tmpString, 30, ",%.02f", tagAngle);
    strcat(logString, tmpString);

    //printf ("\n");
    LOGGER->Log("%s", logString);
}

void PosCalculate::print_ranging_result_data(int tagIndexInVector, RangingResultStruct * validRangingData)
{
    vilslog::printf("PosCalculate::print_ranging_result_data() tagIndexInVector[%d] validCount[%d] ", tagIndexInVector, validRangingData->validCount);
    for(int i=0;i<validRangingData->validCount;i++)
    {
        //printf("%s[%d] ", validRangingData->AnchorName[i], validRangingData->RangingRaw[i]);
        vilslog::printf("%s[%d] ", validRangingData->AnchorName[i], validRangingData->RangingFiltered[i]);
    }
    vilslog::printf("\n");
}

void PosCalculate::dump_anchor_info()
{
    LOGGER->startdumpCSV("anchors");

    char logString[512];
    memset(logString, 0, 512);

    sprintf(logString, "name,id,macaddress,x,y,z,version");
    LOGGER->LogWOTimeStamp("%s", logString);

    int projectid = 1;
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    // anchors
    for (std::list<NodeStruct*>::iterator anchor = rangingNodeInfo->mAnchorNodeList->begin(); anchor != rangingNodeInfo->mAnchorNodeList->end(); anchor++)
    {
        memset(logString, 0, 512);
        snprintf(logString, 512, "%s,%s,%s,%d,%d,%d,%s", (*anchor)->nodename, (*anchor)->nodeid,
            (*anchor)->macaddress, (int)(*anchor)->posX, (int)(*anchor)->posY, (int)(*anchor)->posZ, (*anchor)->version);
        LOGGER->LogWOTimeStamp("%s", logString);
    }

    // locators
    for (std::list<NodeStruct*>::iterator locator = rangingNodeInfo->mLocatorNodeList->begin(); locator != rangingNodeInfo->mLocatorNodeList->end(); locator++)
    {
        memset(logString, 0, 512);
        snprintf(logString, 512, "%s,%s,%s,%d,%d,%d,%s", (*locator)->nodename, (*locator)->nodeid,
            (*locator)->macaddress, (int)(*locator)->posX, (int)(*locator)->posY, (int)(*locator)->posZ, (*locator)->version);
        LOGGER->LogWOTimeStamp("%s", logString);
    }

    LOGGER->stopdumpCSV();
}

std::string PosCalculate::getRangingDiagnosisString(int projectid, const char * tag_macaddress, int validdatatime)
{
    std::ostringstream sendstream;

    //int Tag_idx = 0; //Power Tag only has 1 device, is my self.
    //Tag_idx = mDeviceManager->getTagIndex(tag_macaddress);
    char Tag_id[64];
    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, tag_macaddress));

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    bool bFound = false;
    int tagIndexInVector = 0;
    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), Tag_id) == 0 )
        {
            tagIndexInVector = i;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        std::string strRanging = sendstream.str();
        return strRanging;
    }

    RangingResultStruct validRangingData;
    int validCnt = rangingNodeInfo->mAnchorTagRangingMapVector[tagIndexInVector]->prepareAllRangingData(&validRangingData, validdatatime);

    // Tag ranging time , 重測次數 = 1
    // slotTime = 8 * anchor數量 + ((重測次數 * anchor數量) * 2) + 8;
    // resetTime = (tag數量 + 1) * slotTime;

    char TagIP[64];
    sprintf(TagIP, "%s", mDeviceManager->getTagIP(projectid, tag_macaddress));

    int tagCnt = mDeviceManager->getSameIPTagCount(projectid, TagIP);

//    vilslog::printf("PosCalculate::getRangingDiagnosisString() tag[%s] TagIP[%s] tagCnt[%d]\n", tag_macaddress, TagIP, tagCnt);

    int AnIdx = 0;
    for(int j=0;j<validCnt;j++)
    {
        const char * AnchorName = validRangingData.AnchorName[j];
        if (strlen(AnchorName) > 0)
        {
            int retryNum = 1;
            int anchorCnt = validRangingData.RangingAnchorCnt[j];
            int slotTime = (8 * anchorCnt) + ((retryNum * anchorCnt) * 2) + 8;
            int resetTime = (tagCnt + 1) * slotTime;

            sendstream << AnchorName << "={";
            sendstream<<"\"Ranging\":\""<<validRangingData.RangingRaw[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"PCnt\":\""<<validRangingData.TagPollCnt[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"AnCnt\":\""<<validRangingData.RangingAnchorCnt[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"TagRecv\":\""<<validRangingData.TagFinalRecvdBm[j] * -1<<"\"";
            sendstream << ",";
            sendstream<<"\"TagFP\":\""<<validRangingData.TagFirstPathdBm[j] * -1<<"\"";
            sendstream << ",";
            sendstream<<"\"AnRecv\":\""<<validRangingData.AnchorRecvdBm[j] * -1<<"\"";
            sendstream << ",";
            sendstream<<"\"AnFP\":\""<<validRangingData.AnchorFirstPathdBm[j] * -1<<"\"";
            sendstream << ",";
            sendstream<<"\"LostRate\":\""<<validRangingData.LostRate[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"DataRate\":\""<< std::fixed << std::setprecision(1) <<validRangingData.DataRate[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"DataCount\":\""<<validRangingData.DataCount[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"ValidRate\":\""<< std::fixed << std::setprecision(1) <<validRangingData.ValidRate[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"SlotTime\":\""<<slotTime<<"\"";
            sendstream << ",";
            sendstream<<"\"ResetTime\":\""<<resetTime<<"\"";
            sendstream << ",";
            sendstream<<"\"TagVelocity\":\""<< std::fixed << std::setprecision(2) <<validRangingData.TagVelocity[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"Angle\":\""<< std::fixed << std::setprecision(2) <<validRangingData.TagAngle[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"SD\":\""<< std::fixed << std::setprecision(2)<<validRangingData.StandardDeviation[j]<<"\"";
            sendstream << ",";
            sendstream<<"\"IMU\":\""<<validRangingData.AnchorTagIMU[j][0]<<","<<validRangingData.AnchorTagIMU[j][1]<<","<<validRangingData.AnchorTagIMU[j][2]<<","<<validRangingData.AnchorTagIMU[j][3]<<","<<validRangingData.AnchorTagIMU[j][4]<<","<<validRangingData.AnchorTagIMU[j][5]<<"\"";
            sendstream << "}" << endl;
            AnIdx++;
        }
    }

    std::string strRanging = sendstream.str();
    return strRanging;
}

int PosCalculate::setArguments(int projectid, int EnableNoiseMakup,
    float NoiseMakupRatio,
    float NoiseMakupWeight,
    int EnableKalman1D,
    int EnableRangingReliability,
    int RangingReliabilityMaxDistance,
    int RangingReliabilityMinCount,
    int RangingReliabilityAverageDiff,
    int EnableAnchor1DMapping,
    int EnableOneAnchorCalculation,
    int CalculationMethod,
    float WeightMovingAverageValue1,
    float WeightMovingAverageValue2,
    float WeightMovingAverageValue3,
    int StayInWallTimeout)
{
    int ret = 0;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    rangingNodeInfo->mEnableAnchor1DMapping = EnableAnchor1DMapping;
    rangingNodeInfo->mEnableOneAnchorCalculation = EnableOneAnchorCalculation;
    rangingNodeInfo->mStayInWallTimeout = StayInWallTimeout;

    for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
    {
        // check index
        // int Tag_idx = mAnchorTagRangingMapVector[i]->getIndex();
        // if ( Tag_idx >= 0)
        if( strlen(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName()) > 0)
        {
            rangingNodeInfo->mAnchorTagRangingMapVector[i]->setArguments(EnableNoiseMakup,
                NoiseMakupRatio,
                NoiseMakupWeight,
                EnableKalman1D,
                EnableRangingReliability,
                RangingReliabilityMaxDistance,
                RangingReliabilityMinCount,
                RangingReliabilityAverageDiff,
                CalculationMethod,
                WeightMovingAverageValue1,
                WeightMovingAverageValue2,
                WeightMovingAverageValue3);
        }
    }
    return ret;
}

void PosCalculate::update(int projectid, int validdatatime)
{
    for (std::vector<ProjectRangingNodeInfoStruct*>::iterator rangingNodeInfo = mProjectRangingNodeInfoVector.begin(); rangingNodeInfo != mProjectRangingNodeInfoVector.end(); rangingNodeInfo++)
    {
        for (unsigned int i=0; i< (*rangingNodeInfo)->mAnchorTagRangingMapVector.size(); i++)
        {
            (*rangingNodeInfo)->mAnchorTagRangingMapVector[i]->updateVelocity(validdatatime);
        }
    }
}

void PosCalculate::setPrintAoACirclePoint()
{
    bPrintAoACirclePoint = true;
}

void PosCalculate::setDumpAoACirclePoint(bool enable)
{
    // vilslog::printf("PosCalculate::setDumpAoACirclePoint()\n");
    bDumpAoACirclePoint = enable;
}

void PosCalculate::updatlocatorposition(int projectid, const char * LocatorId, std::string areaid)
{
    vilslog::printf("PosCalculate::updatlocatorposition() projectid[%d] LocatorId[%s] areaid[%s]\n",
        projectid, LocatorId, areaid.c_str());

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (std::list<AreaCenterStruct *>::iterator areacenter = rangingNodeInfo->mAreaCenterList->begin(); areacenter != rangingNodeInfo->mAreaCenterList->end(); areacenter++)
    {
        if ( (*areacenter)->areaid.compare(areaid) == 0 )
        {
            int newPosition[3] = {(int)(*areacenter)->center.x, (int)(*areacenter)->center.y, 0};

            vilslog::printf("PosCalculate::updatlocatorposition() newPosition[%d, %d, %d]\n",
                newPosition[0], newPosition[1], newPosition[2]);

            mDeviceManager->setLocatorInfoS(projectid, LocatorId, newPosition[0], newPosition[1], newPosition[2]);

            break;
        }
    }
}

int PosCalculate::getAnchorCoord(int projectid, const char * anchorid, double * P1)
{
    int ret = mDeviceManager->getAnchorCoord(projectid, anchorid, P1);
    if ( ret == 0 )
    {
        ret = mDeviceManager->getLocatorCoord(projectid, anchorid, P1);
    }
    return ret;
}

int PosCalculate::send_to_Listener_queue(int projectid, const char * TagId, RangingResultStruct * validRangingData)
{
    int validCnt = validRangingData->validCount;

    for(int i=0;i<validCnt;i++)
    {
        // vilslog::printf("send_to_Listener_queue() TagId[%s] AnchorName[%s] RangingFiltered[%d] DataTimeInMS[%s]\n",
        //    TagId, validRangingData->AnchorName[i], validRangingData->RangingFiltered[i], time_tToString(validRangingData->DataTimeInMS[i]/1000).c_str());

        DataItem * sendItemckp = new DataItem("RangingFiltered", projectid, TagId, validRangingData->AnchorName[i], validRangingData->RangingFiltered[i], 1, validRangingData->DataTimeStamp[i]);
        mCheckPointJobQueue.add(sendItemckp);

        DataItem * sendItemdevEvent = new DataItem("RangingFiltered", projectid, TagId, validRangingData->AnchorName[i], validRangingData->RangingFiltered[i], 1, validRangingData->DataTimeStamp[i]);
        mDeviceEventJobQueue.add(sendItemdevEvent);
    }
    return validCnt;
}

int PosCalculate::getSection0DCount(int projectid, int GroupID)
{
    int count = 0;
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (unsigned int i=0; i< rangingNodeInfo->mSectionDeviceInfoVector.size(); i++)
    {
        if ( rangingNodeInfo->mSectionDeviceInfoVector[i]->GroupID == GroupID )
        {
            count = (int)rangingNodeInfo->mSectionDeviceInfoVector[i]->nodeIDList.size();
            break;
        }
    }
    return count;
}

int PosCalculate::getSection0DIndex(int projectid, int GroupID, const char * TagId)
{
    int index = 0;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    bool bFoundGroup = false;
    for (unsigned int i=0; i< rangingNodeInfo->mSectionDeviceInfoVector.size(); i++)
    {
        if ( rangingNodeInfo->mSectionDeviceInfoVector[i]->GroupID == GroupID )
        {
            int cnt = 0;
            bool bFoundNode = false;
            for (std::list<std::string>::iterator nodeID = rangingNodeInfo->mSectionDeviceInfoVector[i]->nodeIDList.begin(); nodeID != rangingNodeInfo->mSectionDeviceInfoVector[i]->nodeIDList.end(); nodeID++)
            {
                if ( (*nodeID).compare(TagId) == 0 )
                {
                    bFoundNode = true;
                    break;
                }
                cnt++;
            }
            index = cnt;

            if (!bFoundNode)
            {
                rangingNodeInfo->mSectionDeviceInfoVector[i]->nodeIDList.push_back(TagId);
            }

            bFoundGroup = true;
            break;
        }
    }

    if (!bFoundGroup)
    {
        SectionDeviceInfo * newInfo = new SectionDeviceInfo();
        newInfo->GroupID = GroupID;
        newInfo->nodeIDList.push_back(TagId);
        rangingNodeInfo->mSectionDeviceInfoVector.push_back(newInfo);
    }

    return index;
}

int PosCalculate::removeFromSection0D(int projectid, int GroupID, const char * TagId)
{
    int index = -1;

    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    //bool bFoundGroup = false;
    for (unsigned int i=0; i< rangingNodeInfo->mSectionDeviceInfoVector.size(); i++)
    {
        if ( rangingNodeInfo->mSectionDeviceInfoVector[i]->GroupID == GroupID )
        {
            int cnt = 0;
            bool bFoundNode = false;
            for (std::list<std::string>::iterator nodeID = rangingNodeInfo->mSectionDeviceInfoVector[i]->nodeIDList.begin(); nodeID != rangingNodeInfo->mSectionDeviceInfoVector[i]->nodeIDList.end(); nodeID++)
            {
                if ( (*nodeID).compare(TagId) == 0 )
                {
                    //bFoundNode = true;
                    break;
                }
                cnt++;
            }
            index = cnt;

            // vilslog::printf("removeFromSection0D() TagId[%s] bFoundNode[%d]\n", TagId, bFoundNode);

            if (bFoundNode)
            {
                rangingNodeInfo->mSectionDeviceInfoVector[i]->nodeIDList.remove(TagId);
            }

            //bFoundGroup = true;
            break;
        }
    }

    return index;
}

bool PosCalculate::find_empty_0D_position(int projectid, const char * TagId, int startX, int startY, int * newPosition)
{
    bool bFound = false;
    int showCol = 3;
    int starIndex = -1;

    // vilslog::printf("find_empty_0D_position() TagId[%s] startX[%d] startY[%d]\n", TagId, startX, startY);

    int ListIndex = 0;
    while (!bFound)
    {
        int offset_num = ListIndex / 9;
        int ShowIndex = ListIndex - (offset_num * 9);

        int rowIndex = ShowIndex / showCol;
        int show_row = starIndex + rowIndex; // col size = 3
        int show_col = starIndex + (ShowIndex - (rowIndex * showCol));

        int offset_x = offset_num * 10;
        int offset_row = show_row * 50; // 50 cm
        int offset_col = show_col * 50 + offset_x; // 50 cm
        int curX = startX + offset_col;
        int curY = startY + offset_row;

        // vilslog::printf("find_empty_0D_position() curX[%d] curY[%d] offset_col[%d] offset_row[%d]\n", curX, curY, offset_col, offset_row);

        bool bExist = false;
        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            if ( (curX == (int)(*node)->posX) && (curY == (int)(*node)->posY) && ( strcmp(TagId,(*node)->nodeid) != 0 ) )
            {
                bExist = true;
                break;
            }
        }

        if (!bExist)
        {
            newPosition[0] = curX;
            newPosition[1] = curY;
            bFound = true;
            break;
        }
        ListIndex++;
    }

    return bFound ;
}

void PosCalculate::clear_ProjectRangingNodeInfoStruct_vector(std::vector<ProjectRangingNodeInfoStruct*> * plist)
{
    for (std::vector<ProjectRangingNodeInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_ranging_vector(&(*i)->mAnchorTagRangingMapVector);
        clear_SectionDeviceInfo_vector(&(*i)->mSectionDeviceInfoVector);

        if ((*i)->mSection0DGroupList != NULL)
        {
            clear_SectionGroup_list((*i)->mSection0DGroupList);
            delete (*i)->mSection0DGroupList;
        }
        if ((*i)->mSection1DGroupList != NULL)
        {
            clear_SectionGroup_list((*i)->mSection1DGroupList);
            delete (*i)->mSection1DGroupList;
        }
        if ((*i)->mRaycasting != NULL)
            delete (*i)->mRaycasting;
        if ((*i)->mWallRaycasting != NULL)
            delete (*i)->mWallRaycasting;

        if ((*i)->mAreaList != NULL)
        {
            clear_maplayerarea_list((*i)->mAreaList);
            delete (*i)->mAreaList;
        }
        if ((*i)->mAreaCenterList != NULL)
        {
            clear_areacenter_list((*i)->mAreaCenterList);
            delete (*i)->mAreaCenterList;
        }
        if ((*i)->mWallList != NULL)
        {
            clear_maplayerarea_list((*i)->mWallList);
            delete (*i)->mWallList;
        }
        if ((*i)->mAnchorNodeList != NULL)
        {
            clear_node_list((*i)->mAnchorNodeList);
            delete (*i)->mAnchorNodeList;
        }
        if ((*i)->mLocatorNodeList != NULL)
        {
            clear_node_list((*i)->mLocatorNodeList);
            delete (*i)->mLocatorNodeList;
        }

        delete *i;
    }
    plist->clear();
}
