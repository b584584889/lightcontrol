


#ifndef PosCalculate_H
#define PosCalculate_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "SmoothingResult.h"
#include "RayCasting.h"
#include "../config.h"
#include "../common.h"
#include "../DeviceManager/DeviceManager.h"
#include "../DOP/DOPResolve.h"
#include "../logger/Logger.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../mysql/MySQLDBAccess.h"

//#define PRINTF_RANGINGRESULT_LOG
//#define PRINT_RESULT_trilateration_3D_new
//#define PRINT_RESULT_leastsquares
//#define PRINT_AOA_CLOUD_POINT
//#define PRINT_AOA_ANGLE_CLOUD_POINT
// #define PRINT_AOA_RESULT

// #define PRINT_DEBUG_SECTION_ONE

typedef struct
{
    int projectid;
	std::vector<AnchorTagCoupleDataClass *> mAnchorTagRangingMapVector;
    std::vector<SectionDeviceInfo *> mSectionDeviceInfoVector;

    int mEnableAnchor1DMapping;
    int mEnableOneAnchorCalculation;
    int mStayInWallTimeout;

    std::list<SectionGroup *> * mSection0DGroupList;
    std::list<SectionGroup *> * mSection1DGroupList;

    RayCasting * mRaycasting;
    RayCasting * mWallRaycasting;
    std::list<AreaCenterStruct*> * mAreaCenterList;
    std::list<MapLayerAreaStruct*> * mAreaList;
    std::list<MapLayerAreaStruct*> * mWallList;
	std::list<NodeStruct*> * mAnchorNodeList;
    std::list<NodeStruct*> * mLocatorNodeList;

    std::ostringstream mAoARangingCloudPointStream;
    std::ostringstream mAoAAngleVectorCloudPointStream;
    std::ostringstream mAoAAngleCloudPointStream;

    std::string mAoARangingCloudPointString;
    std::string mAoAAngleVectorCloudPointString;
    std::string mAoAAngleCloudPointString;

} ProjectRangingNodeInfoStruct;

typedef struct
{
    int projectid;
    AoANodeStruct * aoaNode;
    double aoaNorm[3]; // AOA normal Vector
    float TagAngle;
    int Ranging;
    int MapLayer;
    int TagLevel_x; // AOA Level
    int TagLevel_y; // AOA Level
    int TagLevel_time; // AOA Level
} AoARangingInfoStruct;

typedef struct
{
    AoARangingInfoStruct * aoainfo;
    std::list<PointStruct *> * ptList;
} AoACirclePointStruct;

class PosCalculate
{
public:
	PosCalculate(DeviceManager * pDevManager, SmoothingResult * pSmResult, wqueue<DataItem*> & CheckPointJobQueue, wqueue<DataItem*> & DeviceEventJobQueue, DBManager * dbManager);
	~PosCalculate();

    //int updateAreaList();
    int reloadAreaList();

    // int setRanging(int Anchor_idx, int Tag_idx, int Ranging_idx, int Distance, int * imu);
	// int setRanging(const char * Anchor_macaddress, const char * Tag_macaddress, int Distance);
    int setLoRARanging(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, int rssi, const char * timestamp);
    int setLoRARangingAOA(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, float angle, int rssi, const char * timestamp);
    int setLoRAFenceAOA(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int level_x, int level_y, int level_time, int rssi, const char * timestamp);

    int setRangingDiag(int projectid, int Anchor_idx, int Tag_idx, int Ranging_idx, int Distance, int * imu,
                        int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate);
    int setRangingDiag(int projectid, std::string AnchorMac, std::string TagMac, int Ranging_idx, int Distance, int * imu,
                        int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate);
    void setSectionGroupList(int projectid, std::list<SectionGroup *> * sectionGroupList);

    int getRangingTagCount(int projectid);
    // int getTagIndex(int index);
    char * getTagId(int projectid, int tagIndexInVector);
    bool getTagInfo(int projectid, int tagIndexInVector, char * TagId, double * IMU);
    // bool getIMU(int index, double * IMU);
    bool getIMU(int projectid, const char * TagId, double * IMU);
    // int update(int validdatatime);
	//int rangingResult(int Tag_idx, double * results, int usingIMU, int enable_GDOP, int GDOP_Threshold, int validdatatime);
    int rangingResult(int projectid, const char * TagId, double * results, int usingIMU, int IMUOffset, int SteadyStateOffset, int enable_GDOP, int GDOP_Threshold, int validdatatime, int enable_FixHeight);
    int getRawResult(int projectid, const char * TagId, double * rawresult);
    int setRawResult(int projectid, const char * TagId, double * rawresult);

    //int checkTagArea(int projectid, const char * TagId, double * results, const char * AreaId, const char * exclude_AreaId);
    // int checkTagArea(int projectid, NodeStruct * node);
    int checkLocatorArea(int projectid, NodeStruct * node);
    std::string getRangingString(int projectId, int validdatatime);
    std::string getRangingStringEx(int projectId, int validdatatime);
    std::string getAoARangingCloudPoint(int projectId);
    std::string getAoAAngleVectorCloudPoint(int projectId);
    std::string getAoAAngleCloudPoint(int projectId);

    void start_dump_ranging_result(int timeout);

    std::string getRangingDiagnosisString(int projectid, const char * tag_macaddress, int validdatatime);
	//int getValidAnchors(int Tag_idx, int validdatatime, int * ValidAnchors);
	//int getValidAnchors(int Tag_idx, int validdatatime, int * ValidAnchors, int * ValidAnchorDist);
	// int prepareRangingData(int Tag_idx, RangingResultStruct * validRangingData, int validdatatime);
    int setArguments(int projectid, int EnableNoiseMakup,
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
        int StayInWallTimeout);

    void update(int projectid, int validdatatime);
    void setPrintAoACirclePoint();
    void setDumpAoACirclePoint(bool enable);
    void updatlocatorposition(int projectid, const char * LocatorId, std::string areaid);

private:

    DeviceManager * mDeviceManager;
    SmoothingResult  * mSmoothResult;
    wqueue<DataItem*> & mCheckPointJobQueue;
    wqueue<DataItem*> & mDeviceEventJobQueue;
    DBManager * mDbManager;
    // wqueue<DataItem*> & mCoursesJobQueue;
    //VILSConfig * mVILSConfig;
    //MySQLDBAccess * mMysqlDB;
	// std::list<VILSConfig*> * mAllConfigList;

    bool bStartLogging;
    int mTimeoutLogging;

    bool bPrintAoACirclePoint;
    bool bDumpAoACirclePoint;

	ResidualsFunction * residuals;
	NonLinearSolver * non_linear;
    double ** mAnchorCoordinate;
    DOPResolve * mDOPresolve;

	std::vector<ProjectRangingNodeInfoStruct *> mProjectRangingNodeInfoVector;

	// bool mUpdateData;
    bool mUsingSectionGroupList;

    // std::vector<AnchorTagCoupleDataClass *> mAnchorTagRangingMapVector;
    // std::vector<SectionDeviceInfo *> mSectionDeviceInfoVector;
	//
    // int mEnableAnchor1DMapping;
    // int mEnableOneAnchorCalculation;
    // int mStayInWallTimeout;
	//
    // std::list<SectionGroup *> * mSectionGroupList;
	//
    // RayCasting * mRaycasting;
    // RayCasting * mWallRaycasting;
    // std::list<AreaCenterStruct*> * mAreaCenterList;
    // std::list<AreaStruct*> * mAreaList;
    // std::list<AreaStruct*> * mWallList;
	// std::list<NodeStruct*> * mAnchorNodeList;
    // std::list<NodeStruct*> * mLocatorNodeList;

	void initProjectRangingNodeInfo();
	ProjectRangingNodeInfoStruct * createRangingNodeInfo(int projectid);
	ProjectRangingNodeInfoStruct * getProjectRangingNodeInfo(int projectid);
    // void resolveCurrentTagArea();
    bool loadAnchorNode(ProjectRangingNodeInfoStruct * rangingNodeInfo);
    int reloadAreaList(ProjectRangingNodeInfoStruct * rangingNodeInfo);

    bool do_trilateration_3D_new(int projectid, RangingResultStruct * validRangingData, double * results, bool invZ);
    bool do_leastsquares_new(int projectid, RangingResultStruct * validRangingData, int enable_GDOP, int GDOP_Threshold, double * results);
    bool checkInside(int projectid, RangingResultStruct * validRangingData, double * results);

    int do_section_check(int projectid, const char * TagId, RangingResultStruct * validRangingData, RangingResultStruct * validRangingData0D);
    bool do_section_0D_check(int projectid, const char * TagId, RangingResultStruct * validRangingData, RangingResultStruct * validRangingData0D);
    bool do_section_1D_check(int projectid, const char * TagId, RangingResultStruct * validRangingData);
    int do_check_Section_ZERO(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData);
    int update_node_section0D(int projectid, const char * TagId, int checkSection0D, SectionGroup * sectiongroup, RangingResultStruct * validRangingData);
    // int do_check_Section_ZERO_old(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData);
    bool do_check_Section_ONE(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData);
    bool do_check_Section_ONE_on_one_anchor(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData);
    bool do_check_Section_ONE_on_two_anchor(int projectid, const char * TagId, SectionGroup * sectiongroup, RangingResultStruct * validRangingData);

	int do_AOA_check(int projectid, const char * TagId, RangingResultStruct * validRangingData);
    int do_one_AOA_check(const char * TagId, std::list<AoARangingInfoStruct *> * AoARangingInfoList);
    int do_one_AOA_Fence_check(const char * TagId, AoARangingInfoStruct * aoainfo);
    int do_one_AOA_Angle_check(const char * TagId, AoARangingInfoStruct * aoainfo);
    int calc_AoA_result(AoARangingInfoStruct * aoainfo, double * normalVector, double * origAoA, double * results);
    int do_multi_AOA_check(const char * TagId, std::list<AoARangingInfoStruct *> * AoARangingInfoList);
    int do_multi_AOA_angle_check(const char * TagId, std::list<AoARangingInfoStruct *> * AoARangingInfoList);
    // int do_two_AOA_angle(AoARangingInfoStruct * aoainfoFirst, AoARangingInfoStruct * aoainfoSecond);

    std::list<PointStruct *> * list_cicle_point(AoARangingInfoStruct * aoainfo, bool bDump);
    int calc_rotate_point(AoARangingInfoStruct * aoainfo, float Xoffset, int radius, float angle, double * results);
    std::list<PointStruct *> * find_closest_point(std::vector<AoACirclePointStruct *> * circlePtList);
    bool find_closest_cross_point(std::list<PointStruct *> * ptList1, std::list<PointStruct *> * ptList2, double * results);
    bool find_closest_angle_point(std::vector<AoACirclePointStruct *> * vectorPtList, double * results);
    std::list<PointStruct *> * find_cross_circle_point(AoACirclePointStruct * aoaCirPt1, AoACirclePointStruct * aoaCirPt2);
    bool find_match_angle_point(std::vector<AoACirclePointStruct *> * circlePtList, double * results);

    int do_3D_check(int projectid, const char * TagId, int tagIndexInVector, RangingResultStruct * validRangingData, int usingIMU, int IMUOffset, int SteadyStateOffset, int enable_GDOP, int GDOP_Threshold, int validdatatime, int enable_FixHeight);

    void dump_ranging_result_header();
    void dump_ranging_result_data(int tagIndexInVector, double * rawresult, double * results, int * smresult, RangingResultStruct * validRangingData);
    void print_ranging_result_data(int tagIndexInVector, RangingResultStruct * validRangingData);
    void dump_anchor_info();
    int getAnchorCoord(int projectid, const char * anchorid, double * AnchorCoord);
    //void updateAnchorStatus();
    //int check_valid_anchor_ranging_value(double mv_direction, double rangingvlaue, double anTagDistance);
    int send_to_Listener_queue(int projectid, const char * TagId, RangingResultStruct * validRangingData);

    int getSection0DCount(int projectid, int GroupID);
    int getSection0DIndex(int projectid, int GroupID, const char * TagId);
    int removeFromSection0D(int projectid, int GroupID, const char * TagId);
    bool find_empty_0D_position(int projectid, const char * TagId, int startX, int startY, int * newPosition);

    void clear_ProjectRangingNodeInfoStruct_vector(std::vector<ProjectRangingNodeInfoStruct*> * plist);
};

#endif
