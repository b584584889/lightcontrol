
/*
 * PowerTagCalculate.cpp
 *
 *  Created on: 2020/02/12
 *      Author: ikki
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>      // std::setprecision

#include <sys/time.h>
#include <time.h>

#include "PowerTagCalculate.h"

#include "../PosCalculate/algorithm.h"
#include "../common.h"

using namespace std;

PowerTagCalculate::PowerTagCalculate(DeviceManager * pDevManager, DBManager * dbManager)
    : mDeviceManager(pDevManager), mDbManager(dbManager)
{
    mTestAngle = 0.0f;
    initProjectRangingNodeInfo();
}

PowerTagCalculate::~PowerTagCalculate()
{
    clear_ProjectRangingNodeInfoStruct_vector(&mProjectRangingNodeInfoVector);
}

int PowerTagCalculate::aoaResult(int projectid, std::string aoaId)
{
    int succces = 0;
    // vilslog::printf("PowerTagCalculate::aoaResult()projectid[%d] aoaId[%s] \n", projectid, aoaId.c_str());

    AoAAngleInfoStruct * aoAAngleInfoData = new AoAAngleInfoStruct();
    aoAAngleInfoData->validCount = 0;

    int validCount = prepareAoAAngleData(projectid, aoaId, aoAAngleInfoData);

    if (validCount >= 3)
    {
        succces = checkAoAResult(projectid, aoaId, aoAAngleInfoData);
    }

    delete aoAAngleInfoData;

    return succces;
}

int PowerTagCalculate::setLoRARangingAOA(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, float angle, int rssi, const char * timestamp)
{
    int succces_count = 0;
    char Tag_id[64];
    char Locator_id[64];

    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, Tag_macaddress));

    int MapLayer = mDeviceManager->getLocatorMapLayerByLoRAmacaddress(projectid, LoRA_macaddress);
    sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, LoRA_macaddress));

    if (strlen(Tag_macaddress) == 0 || strlen(Locator_id) == 0)
    {
        vilslog::printf("PowerTagCalculate::setLoRARangingAOA() failed! projectid[%d] Tag_macaddress[%s] Tag_id[%s] LoRA_macaddress[%s] Locator_id[%s]\n",
                            projectid, Tag_macaddress, Tag_id, LoRA_macaddress, Locator_id);
        return 0;
    }

    //mDeviceManager->setTagAlive(projectid, Tag_macaddress);
    //mDeviceManager->setTagOperationMode(projectid, Tag_macaddress, OPERTAION_MODE_LORA);
    //mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, LoRA_macaddress);

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

        vilslog::printf ("PowerTagCalculate::setLoRARangingAOA() Locator_id[%s] Tag_id[%s] (%d,%d,%d) Create new tag.\n",
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

bool PowerTagCalculate::setAoATag(int projectid, std::string tagId)
{
    bool bFound = false;

    for (std::list<PowerTagInfoStruct*>::iterator queue = mPowerTagInfoList.begin(); queue != mPowerTagInfoList.end(); queue++)
    {
        if ( (*queue)->projectid == projectid )
        {
            if ( tagId.compare((*queue)->tagId) == 0 )
            {
                bFound = true;
                (*queue)->issuetime = getCurrentTimeInSec();
                break;
            }
        }
    }

    if (!bFound)
    {
        PowerTagInfoStruct * newInfo = new PowerTagInfoStruct();
        newInfo->projectid = projectid;
        newInfo->tagId = tagId;
        newInfo->issuetime = getCurrentTimeInSec();
        mPowerTagInfoList.push_back(newInfo);

        vilslog::printf(" PowerTagCalculate::setAoATag() mPowerTagInfoList.mPowerTagInfoList(%s)\n", tagId.c_str());
    }

    return bFound;
}

void PowerTagCalculate::initProjectRangingNodeInfo()
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

ProjectRangingNodeInfoStruct * PowerTagCalculate::createRangingNodeInfo(int projectid)
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
        vilslog::printf ("PowerTagCalculate::initProjectRangingNodeInfo() projectid[%d] loadAnchorNode failed!\n", projectid);
    }

    return newInfo;
}

ProjectRangingNodeInfoStruct * PowerTagCalculate::getProjectRangingNodeInfo(int projectid)
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
bool PowerTagCalculate::loadAnchorNode(ProjectRangingNodeInfoStruct * rangingNodeInfo)
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

void PowerTagCalculate::clear_ProjectRangingNodeInfoStruct_vector(std::vector<ProjectRangingNodeInfoStruct*> * plist)
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

int PowerTagCalculate::prepareAoAAngleData(int projectid, std::string aoaId, AoAAngleInfoStruct * aoAAngleInfoData)
{
    ProjectRangingNodeInfoStruct * rangingNodeInfo = getProjectRangingNodeInfo(projectid);

    for (std::list<PowerTagInfoStruct*>::iterator queue = mPowerTagInfoList.begin(); queue != mPowerTagInfoList.end(); queue++)
    {
        if ( (*queue)->projectid == projectid )
        {
            for (unsigned int i=0; i< rangingNodeInfo->mAnchorTagRangingMapVector.size(); i++)
            {
                // check index
                if ( strcmp(rangingNodeInfo->mAnchorTagRangingMapVector[i]->getTagName(), (*queue)->tagId.c_str()) == 0 )
                {
                    RangingResultStruct * curValidRangingData = new RangingResultStruct();
                    int validCnt = rangingNodeInfo->mAnchorTagRangingMapVector[i]->prepareRangingData(curValidRangingData, 2000);

                    for(int j=0;j<validCnt;j++)
                    {
                        if (strcmp(aoaId.c_str(), curValidRangingData->AnchorName[j]) == 0 )
                        {
                            aoAAngleInfoData->tagId[aoAAngleInfoData->validCount] = (*queue)->tagId;
                            aoAAngleInfoData->tagAngle[aoAAngleInfoData->validCount] = curValidRangingData->TagAngle[j];
                            aoAAngleInfoData->validCount++;
                            break;
                        }
                    }
                    delete curValidRangingData;

                    // vilslog::printf("PowerTagCalculate::aoaResult() projectid[%d] aoaId[%s] tagId[%s] aoAAngleInfoData->validCount[%d]\n",
                    //     projectid, aoaId.c_str(), (*queue)->tagId.c_str(), aoAAngleInfoData->validCount);

                    // vilslog::printf("PowerTagCalculate::aoaResult() projectid[%d] aoaId[%s] tagId[%s] validCnt[%d]\n",
                    //     projectid, aoaId.c_str(), (*queue)->tagId.c_str(), validCnt);
                }
            }
        }
    }

    return aoAAngleInfoData->validCount;
}

int PowerTagCalculate::checkAoAResult(int projectid, std::string aoaId, AoAAngleInfoStruct * aoAAngleInfoData)
{
    AoARangingInfoStruct * aoainfo = prepareAoARangingInfo();

    // for(int i=0;i<aoAAngleInfoData->validCount;i++)
    // {
    //     vilslog::printf("PowerTagCalculate::checkAoAResult() projectid[%d] aoaId[%s] tagId[%s] tagAngle[%f]\n",
    //         projectid, aoaId.c_str(), aoAAngleInfoData->tagId[i].c_str(), aoAAngleInfoData->tagAngle[i]);
    // }

    double AoAAnchor1[3] = {0};
    double AoAAnchor2[3] = {0};
    double AoAAnchor3[3] = {0};

    mDeviceManager->getTagCoord(projectid, aoAAngleInfoData->tagId[0].c_str(), AoAAnchor1);
    mDeviceManager->getTagCoord(projectid, aoAAngleInfoData->tagId[1].c_str(), AoAAnchor2);
    mDeviceManager->getTagCoord(projectid, aoAAngleInfoData->tagId[2].c_str(), AoAAnchor3);

    // vilslog::printf("PowerTagCalculate::checkAoAResult() AoAAnchor1[%f,%f,%f] AoAAnchor2[%f,%f,%f] AoAAnchor3[%f,%f,%f]\n",
    //     AoAAnchor1[0], AoAAnchor1[1], AoAAnchor1[2], AoAAnchor2[0], AoAAnchor2[1], AoAAnchor2[2], AoAAnchor3[0], AoAAnchor3[1], AoAAnchor3[2]);

    // vilslog::printRaw("\n\n");
    //long long start1 = getSystemTime();

    aoainfo->TagAngle = aoAAngleInfoData->tagAngle[0];
    std::list<PointStruct *> * ptList1 = list_cicle_point(aoainfo);

    // long long start2 = getSystemTime();
    // long long list1Using = start2 - start1;

    // vilslog::printRaw("\n\n");
    // vilslog::printRaw("find_cross_circle_point 1 list1Using[%d]\n", list1Using);
    std::list<PointStruct *> * resultPtList1 = find_cross_circle_point(aoainfo->aoaNode, ptList1, AoAAnchor1[2], false);

    aoainfo->TagAngle = aoAAngleInfoData->tagAngle[1];
    std::list<PointStruct *> * ptList2 = list_cicle_point(aoainfo);

    // long long start3 = getSystemTime();
    // long long list2Using = start3 - start2;

    // vilslog::printRaw("\n\n");
    // vilslog::printRaw("find_cross_circle_point 2 list2Using[%d]\n", list2Using);
    std::list<PointStruct *> * resultPtList2 = find_cross_circle_point(aoainfo->aoaNode, ptList2, AoAAnchor2[2], false);

    aoainfo->TagAngle = aoAAngleInfoData->tagAngle[2];
    std::list<PointStruct *> * ptList3 = list_cicle_point(aoainfo);

    // long long start4 = getSystemTime();
    // long long list3Using = start4 - start3;

    // vilslog::printRaw("\n\n");
    // vilslog::printRaw("find_cross_circle_point 3 list3Using[%d]\n", list3Using);
    std::list<PointStruct *> * resultPtList3 = find_cross_circle_point(aoainfo->aoaNode, ptList3, AoAAnchor3[2], false);

    double results[4];
    bool bFound = find_AoA_position(AoAAnchor1, AoAAnchor2, AoAAnchor3, resultPtList1, resultPtList2, resultPtList3, results);

    if (bFound)
    {
        mDeviceManager->setAoAInfoById(projectid, aoaId.c_str(), (int)results[0], (int)results[1], (int)results[2]);
        mDeviceManager->setAoAInfoSById(projectid, aoaId.c_str(), aoainfo->aoaNode->pitch, aoainfo->aoaNode->roll, (int)results[3] * -1);
        vilslog::printRaw("PowerTagCalculate::checkAoAResult() results[%f,%f,%f] yaw[%f]\n", results[0], results[1], results[2], results[3] * -1);
    }
    else
    {
        vilslog::printRaw("PowerTagCalculate::checkAoAResult() Not Foudn!!!!\n");
    }

    // long long start5 = getSystemTime();
    // long long find_AoA_position_Using = start5 - start4;
    // vilslog::printRaw("PowerTagCalculate::checkAoAResult() find_AoA_position_Using[%d]\n", find_AoA_position_Using);

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

    delete aoainfo->aoaNode;
    delete aoainfo;
    return 0;
}

AoARangingInfoStruct * PowerTagCalculate::prepareAoARangingInfo()
{
    AoANodeStruct * aoaNode = new AoANodeStruct();
    aoaNode->posX = 0;
    aoaNode->posY = 0;
    aoaNode->posZ = 0;
    aoaNode->pitch = 90;
    aoaNode->roll = 0;
    aoaNode->yaw = 0;

    mTestAngle += 1.0f;
    if (mTestAngle >= 360)
        mTestAngle = 0.0f;

    AoARangingInfoStruct * aoainfo = new AoARangingInfoStruct();
    aoainfo->projectid = 1;
    aoainfo->aoaNode = aoaNode;
    aoainfo->TagAngle = 0;
    aoainfo->Ranging = 1000;
    aoainfo->MapLayer = 1;

    float pitch_Radians = angle_to_rad((float)aoaNode->pitch);
    float yaw_Radians = angle_to_rad((float)aoaNode->yaw);

    aoainfo->aoaNorm[0] = (double)cos(yaw_Radians)*1000;
    aoainfo->aoaNorm[1] = (double)sin(yaw_Radians)*1000;
    aoainfo->aoaNorm[2] = (double)sin(pitch_Radians)*1000;

    return aoainfo;
}

std::list<PointStruct *> * PowerTagCalculate::list_cicle_point(AoARangingInfoStruct * aoainfo)
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

int PowerTagCalculate::calc_rotate_point(AoARangingInfoStruct * aoainfo, float Xoffset, int radius, float angle, double * results)
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

int PowerTagCalculate::calc_AoA_result(AoARangingInfoStruct * aoainfo, double * normalVector, double * origAoA, double * results)
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

std::list<PointStruct *> * PowerTagCalculate::find_cross_circle_point(AoANodeStruct * aoaNode, std::list<PointStruct *> * ptList, int z_height, bool dumplog)
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

bool PowerTagCalculate::find_AoA_position(double * AoAAnchor1, double * AoAAnchor2, double * AoAAnchor3,
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
        double R[9] = {0};
        findRotationMatrix(AoAAnchor1, AoAAnchor2, AoAAnchor3, P1, P2, P3, R);

        double mP1[3];
        double mP2[3];
        double mP3[3];
        DotMatrix(3, R, P1, mP1);
        DotMatrix(3, R, P2, mP2);
        DotMatrix(3, R, P3, mP3);

        // vilslog::printRaw("mP1[%d,%d,%d] mP2[%d,%d,%d] mP3[%d,%d,%d]\n",
        //         (int)mP1[0], (int)mP1[1], (int)mP1[2],
        //         (int)mP2[0], (int)mP2[1], (int)mP2[2],
        //         (int)mP3[0], (int)mP3[1], (int)mP3[2]);

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

        double P5[3];
        double mP5[3];
        P5[0] = 100;
        P5[1] = 0;
        P5[2] = 0;
        DotMatrix(3, R, P5, mP5);

        double angle = atan2(mP5[1], mP5[0]) * 180.0 / PI;
        // vilslog::printRaw("P5[%f,%f,%f] mP5[%f,%f,%f] angle[%f]\n", P5[0], P5[1], P5[2], mP5[0], mP5[1], mP5[2], angle);

        results[3] = angle;
    }

    return bFound;
}
