/*
 *  WorkItem.cpp
 *
 *  Created on: 2017/02/10
 *      Author: ikki
 */

#include "WorkItem.h"
#include "../common.h"

WorkItem::WorkItem(const char* type, int projectID)
      : m_type(type), m_project_id(projectID)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, const char* nodeid, int nodetype, double voltage, double temperature, int count)
      : m_type(type), m_project_id(projectID), m_NodeType(nodetype), m_nodeid(nodeid), m_voltage(voltage), m_temperature(temperature), m_count(count)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, const char* nodeid, double posX, double posY, double posZ, const char* area_id)
      : m_type(type), m_project_id(projectID), m_nodeid(nodeid), m_posX(posX), m_posY(posY), m_posZ(posZ), m_area_id(area_id)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, const char* nodeid, double posX, double posY, double posZ,
                double accelX, double accelY, double accelZ,
                double gyroX, double gyroY, double gyroZ, const char* area_id)
                : m_type(type), m_project_id(projectID), m_nodeid(nodeid), m_posX(posX), m_posY(posY), m_posZ(posZ),
                m_accelX(accelX), m_accelY(accelY), m_accelZ(accelZ),
                m_gyroX(gyroX), m_gyroY(gyroY), m_gyroZ(gyroZ), m_area_id(area_id)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, int nodetype)
    : m_type(type), m_project_id(projectID), m_NodeType(nodetype)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, int nodetype, const char* macaddress)
    : m_type(type), m_project_id(projectID), m_NodeType(nodetype), m_macaddress(macaddress)
{
    init();
}

// "alert0D" "alertcancel" "alert"
WorkItem::WorkItem(const char* type, int projectID, int nodetype, const char* macaddress, const char * jsondata)
    : m_type(type), m_project_id(projectID), m_NodeType(nodetype), m_macaddress(macaddress), m_jsondata(jsondata)
{
    init();
}

// "alert0D" "alertcancel" "alert"
WorkItem::WorkItem(const char* type, int projectID, int nodetype, const char* macaddress, const char * jsondata, std::string empid)
    : m_type(type), m_project_id(projectID), m_NodeType(nodetype), m_macaddress(macaddress), m_jsondata(jsondata), m_empid(empid)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, const char* macaddress, const char* nodeid, const char* nodename, int buildingid, int floor, double posX, double posY, double posZ)
      : m_type(type), m_project_id(projectID), m_nodeid(nodeid), m_macaddress(macaddress), m_nodename(nodename), m_posX(posX), m_posY(posY), m_posZ(posZ), m_buildingid(buildingid), m_floor(floor)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, int nodetype, int tafid, int buildingid, int floor, double posX, double posY, double posZ)
    : m_type(type), m_project_id(projectID), m_NodeType(nodetype), m_posX(posX), m_posY(posY), m_posZ(posZ), m_tafid(tafid), m_buildingid(buildingid), m_floor(floor)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, int tafid, string nodename, string action, string area_name, string issuetime)
    : m_type(type), m_project_id(projectID), m_nodename(nodename), m_action(action), m_area_name(area_name), m_issuetime(issuetime), m_tafid(tafid)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, std::string macaddress, std::string serno, int dummy)
    : m_type(type), m_project_id(projectID), m_macaddress(macaddress), m_serno(serno)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, std::string macaddress, std::string anchorids)
    : m_type(type), m_project_id(projectID), m_macaddress(macaddress), m_anchorids(anchorids)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, std::string macaddress, int range)
    : m_type(type), m_project_id(projectID), m_macaddress(macaddress), m_range(range)
{
    init();
}

WorkItem::WorkItem(const char* type, int projectID, NodeStruct * node)
    : m_type(type), m_project_id(projectID)
{
    init();

    m_node.nodetype = node->nodetype;
    snprintf(m_node.nodeid, 32, "%s", node->nodeid);
    snprintf(m_node.nodename, 32, "%s", node->nodename);
    snprintf(m_node.panid, 32, "%s", node->panid);
    snprintf(m_node.time, 32, "%s", node->time);
    snprintf(m_node.macaddress, 32, "%s", node->macaddress);

    m_node.tafid = node->tafid;
    m_node.buildingid = node->buildingid;
    m_node.buildingfloor = node->buildingfloor;
    m_node.maplayer = node->maplayer;

    m_node.posX = node->posX;
    m_node.posY = node->posY;
    m_node.posZ = node->posZ;
    m_node.prePosX = node->prePosX;
    m_node.prePosY = node->prePosY;
    m_node.prePosZ = node->prePosZ;
    m_node.smoothingPosX = node->smoothingPosX;
    m_node.smoothingPosY = node->smoothingPosY;
    m_node.smoothingPosZ = node->smoothingPosZ;
    m_node.momentumX = node->momentumX;
    m_node.momentumY = node->momentumY;
    m_node.momentumZ = node->momentumZ;
    m_node.velocity = node->velocity;

    m_node.index = node->index;
    m_node.alive = node->alive;
    m_node.asleep = node->asleep;

    snprintf(m_node.voltage, 32, "%s", node->voltage);
    snprintf(m_node.temperature, 32, "%s", node->temperature);
    snprintf(m_node.version, 32, "%s", node->version);
    snprintf(m_node.rangingList, 128, "%s", node->rangingList);
    m_node.blacklist = node->blacklist;
    m_node.whitelist = node->whitelist;
    m_node.joinRangingRange = node->joinRangingRange;
    m_node.joinNotRangingRange = node->joinNotRangingRange;
    m_node.aliveTime = node->aliveTime;

    snprintf(m_node.IP, 32, "%s", node->IP);
    snprintf(m_node.AppVersion, 32, "%s", node->AppVersion);
    snprintf(m_node.PID, 32, "%s", node->PID);

    snprintf(m_node.AreaId, 32, "%s", node->AreaId);
    snprintf(m_node.AreaName, 256, "%s", node->AreaName);

    snprintf(m_node.AreaId0D, 32, "%s", node->AreaId0D);
    m_node.AreaType0D = node->AreaType0D;

    snprintf(m_node.AreaType5Id, 32, "%s", node->AreaType5Id);
    snprintf(m_node.AreaType5Name, 256, "%s", node->AreaType5Name);

    m_node.RangingResultType = node->RangingResultType;
    m_node.area_unknown = node->area_unknown;
    snprintf(m_node.exclude_rollcall_AreaId, 32, "%s", node->exclude_rollcall_AreaId);

    m_node.alert = node->alert;
    m_node.alert0D = node->alert0D;
    m_node.alertIssuTime = node->alertIssuTime;

    snprintf(m_node.LoRAMacaddress, 32, "%s", node->LoRAMacaddress);

    // for websocket event
    m_node.voltage_changed = node->voltage_changed;
    m_node.voltage_low = node->voltage_low;
    m_node.coord_changed = node->coord_changed;

    m_node.updateTimestamp = node->updateTimestamp;
    m_node.poweronTimestamp = node->poweronTimestamp;
    m_node.poweroffTimestamp = node->poweroffTimestamp;

    // for Wall
    m_node.wallPosX = node->wallPosX;
    m_node.wallPosY = node->wallPosY;
    m_node.wallPosZ = node->wallPosZ;
    m_node.wallTimeStamp = node->wallTimeStamp;

    m_node.Longitude = node->Longitude;
    m_node.Latitude = node->Latitude;

    m_node.Opertaion_Mode = node->Opertaion_Mode;
    m_node.pingCount = node->pingCount;

    m_node.weight = node->weight;
    m_node.heartbeat = node->heartbeat;
    m_node.rangingTargetid = node->rangingTargetid;// 與誰在測距

    m_node.updateWeightTimestamp = node->updateWeightTimestamp;
    m_node.updateHeartBeatTimestamp = node->updateHeartBeatTimestamp;

    m_node.enableSensorAlert = node->enableSensorAlert;

    m_node.gwid = node->gwid;

    m_node.resident_serno = node->resident_serno;
    m_node.empid = node->empid;

    m_node.userid = node->userid; // link to Table User.userid
    m_node.maingroupid = node->maingroupid; // link to Table UserMainGroup.id
    m_node.verifykey = node->verifykey;
}

WorkItem::WorkItem(const char* type, int projectID, CheckPointPathStruct checkPointPath)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_CheckPointPath.projectid = checkPointPath.projectid;
    m_CheckPointPath.GroupID = checkPointPath.GroupID;
    m_CheckPointPath.PathID = checkPointPath.PathID;
    m_CheckPointPath.PathIndex = checkPointPath.PathIndex;
    m_CheckPointPath.StartNodeId = checkPointPath.StartNodeId;
    m_CheckPointPath.StartNodeDistance = checkPointPath.StartNodeDistance;
    m_CheckPointPath.StartNodeMinTime = checkPointPath.StartNodeMinTime;
    m_CheckPointPath.StartNodeMaxTime = checkPointPath.StartNodeMaxTime;
    m_CheckPointPath.EndNodeId = checkPointPath.EndNodeId;
    m_CheckPointPath.EndNodeDistance = checkPointPath.EndNodeDistance;
    m_CheckPointPath.EndNodeMinTime = checkPointPath.EndNodeMinTime;
    m_CheckPointPath.EndNodeMaxTime = checkPointPath.EndNodeMaxTime;
    m_CheckPointPath.TravelTime = checkPointPath.TravelTime;

}

WorkItem::WorkItem(const char* type, int projectID, CheckPointPathDailyRecord DailyRecord)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_DailyRecord.DailyRecordID = DailyRecord.DailyRecordID;
    m_DailyRecord.projectid = DailyRecord.projectid;
    m_DailyRecord.GroupID = DailyRecord.GroupID;
    m_DailyRecord.PathID = DailyRecord.PathID;

    // For Runtime Check
    m_DailyRecord.Checked = DailyRecord.Checked; // Tag already enter ane leave
    m_DailyRecord.Finished = DailyRecord.Finished; // Tag already enter ane leave
    m_DailyRecord.Exceed = DailyRecord.Exceed; // Tag already enter ane leave
    m_DailyRecord.StartNodeTagData.TagRecordNodeId = DailyRecord.StartNodeTagData.TagRecordNodeId;
    m_DailyRecord.StartNodeTagData.TagNodeId = DailyRecord.StartNodeTagData.TagNodeId;
    m_DailyRecord.StartNodeTagData.TagStatus = DailyRecord.StartNodeTagData.TagStatus;
    m_DailyRecord.StartNodeTagData.TagEnterTime = DailyRecord.StartNodeTagData.TagEnterTime;
    m_DailyRecord.StartNodeTagData.TagLeaveTime = DailyRecord.StartNodeTagData.TagLeaveTime;
    m_DailyRecord.EndNodeTagData.TagRecordNodeId = DailyRecord.EndNodeTagData.TagRecordNodeId;
    m_DailyRecord.EndNodeTagData.TagNodeId = DailyRecord.EndNodeTagData.TagNodeId;
    m_DailyRecord.EndNodeTagData.TagStatus = DailyRecord.EndNodeTagData.TagStatus;
    m_DailyRecord.EndNodeTagData.TagEnterTime = DailyRecord.EndNodeTagData.TagEnterTime;
    m_DailyRecord.EndNodeTagData.TagLeaveTime = DailyRecord.EndNodeTagData.TagLeaveTime;

    m_DailyRecord.DateTime = DailyRecord.DateTime;
}

WorkItem::WorkItem(const char* type, int projectID, CheckPointGroupStruct checkPointGroup)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_CheckPointGroup.projectid = checkPointGroup.projectid;
    m_CheckPointGroup.GroupID = checkPointGroup.GroupID;
    m_CheckPointGroup.GroupName = checkPointGroup.GroupName;
    m_CheckPointGroup.StartTime = checkPointGroup.StartTime;
    m_CheckPointGroup.EndTime = checkPointGroup.EndTime;
    m_CheckPointGroup.issuetime = checkPointGroup.issuetime;
}

WorkItem::WorkItem(const char* type, int projectID, CheckPointGroupDailyRecord DailyRecord)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_GroupDailyRecord.DailyRecordID = DailyRecord.DailyRecordID;
    m_GroupDailyRecord.projectid = DailyRecord.projectid;
    m_GroupDailyRecord.GroupID = DailyRecord.GroupID;

    // For Runtime Check
    m_GroupDailyRecord.Finished = DailyRecord.Finished; // Tag already enter ane leave
    m_GroupDailyRecord.Exceed = DailyRecord.Exceed; // Tag already enter ane leave
    m_GroupDailyRecord.DateTime = DailyRecord.DateTime;
}

WorkItem::WorkItem(const char* type, int projectID, CheckPointTagBehaviorData TagBehaviorData)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_TagBehaviorData.projectid = TagBehaviorData.projectid;
    m_TagBehaviorData.GroupID = TagBehaviorData.GroupID;
    m_TagBehaviorData.PathID = TagBehaviorData.PathID;
    m_TagBehaviorData.PathIndex = TagBehaviorData.PathIndex;
    m_TagBehaviorData.AnchorNodeId = TagBehaviorData.AnchorNodeId;
    m_TagBehaviorData.TagNodeId = TagBehaviorData.TagNodeId;
    m_TagBehaviorData.Distance = TagBehaviorData.Distance;
    m_TagBehaviorData.DateTime = TagBehaviorData.DateTime;

}

WorkItem::WorkItem(const char* type, int projectID, InspectionGroupStruct inspectionGroup)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_InspectionGroup.GroupID = inspectionGroup.GroupID;
    m_InspectionGroup.GroupName = inspectionGroup.GroupName;
    m_InspectionGroup.StartTime = inspectionGroup.StartTime;
    m_InspectionGroup.EndTime = inspectionGroup.EndTime;
    m_InspectionGroup.AlertTime = inspectionGroup.AlertTime;
    m_InspectionGroup.issuetime = inspectionGroup.issuetime;
}

WorkItem::WorkItem(const char* type, int projectID, InspectionPointDailyRecord DailyRecord)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_InspectionPointDailyRecord.DailyRecordID = DailyRecord.DailyRecordID;
    m_InspectionPointDailyRecord.GroupID = DailyRecord.GroupID;
    m_InspectionPointDailyRecord.PointID = DailyRecord.PointID;
    m_InspectionPointDailyRecord.Score = DailyRecord.Score;

    // For Runtime Check
    m_InspectionPointDailyRecord.Checked = DailyRecord.Checked; // Tag already enter ane leave
    m_InspectionPointDailyRecord.Finished = DailyRecord.Finished; // Tag already enter ane leave
    m_InspectionPointDailyRecord.Exceed = DailyRecord.Exceed; // Tag already enter ane leave
    m_InspectionPointDailyRecord.Inspector = DailyRecord.Inspector;
    m_InspectionPointDailyRecord.RecordNodeId = DailyRecord.RecordNodeId;
    m_InspectionPointDailyRecord.Status = DailyRecord.Status;
    m_InspectionPointDailyRecord.EnterTime = DailyRecord.EnterTime;
    m_InspectionPointDailyRecord.LeaveTime = DailyRecord.LeaveTime;

    m_InspectionPointDailyRecord.DateTime = DailyRecord.DateTime;
    m_InspectionPointDailyRecord.SendAlertMessage = DailyRecord.SendAlertMessage;
}

WorkItem::WorkItem(const char* type, int projectID, InspectionGroupDailyRecord DailyRecord)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_InspectionGroupDailyRecord.DailyRecordID = DailyRecord.DailyRecordID;
    m_InspectionGroupDailyRecord.GroupID = DailyRecord.GroupID;
    m_InspectionGroupDailyRecord.Score = DailyRecord.Score;

    // For Runtime Check
    m_InspectionGroupDailyRecord.Finished = DailyRecord.Finished; // Tag already enter ane leave
    m_InspectionGroupDailyRecord.Exceed = DailyRecord.Exceed; // Tag already enter ane leave
    m_InspectionGroupDailyRecord.Checked = DailyRecord.Checked; // Tag already enter ane leave
    m_InspectionGroupDailyRecord.DateTime = DailyRecord.DateTime;
}

WorkItem::WorkItem(const char* type, int projectID, InspectorBehaviorData InspectorBehaviorData)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_InspectorBehaviorData.GroupID = InspectorBehaviorData.GroupID;
    m_InspectorBehaviorData.PointID = InspectorBehaviorData.PointID;
    m_InspectorBehaviorData.PointIndex = InspectorBehaviorData.PointIndex;
    m_InspectorBehaviorData.AnchorNodeId = InspectorBehaviorData.AnchorNodeId;
    m_InspectorBehaviorData.Inspector = InspectorBehaviorData.Inspector;
    m_InspectorBehaviorData.Distance = InspectorBehaviorData.Distance;
    m_InspectorBehaviorData.DateTime = InspectorBehaviorData.DateTime;
}

WorkItem::WorkItem(const char* type, int projectID, EventStruct * event)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_Event.projectid = event->projectid;
    m_Event.nodeid = event->nodeid;
    m_Event.eventid = event->eventid;
    m_Event.action = event->action;
    m_Event.areaid = event->areaid;
    m_Event.issuetime = event->issuetime;
    m_Event.nodename = event->nodename;
    m_Event.areaname = event->areaname;
    m_Event.issuename = event->issuename;
}

WorkItem::WorkItem(const char* type, int projectID, FiredEventStruct * eventFired)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_EventFired.firedeventid = eventFired->firedeventid;
    m_EventFired.projectid = eventFired->projectid;
    m_EventFired.eventid = eventFired->eventid;
    m_EventFired.maingroupid = eventFired->maingroupid;
    m_EventFired.empid = eventFired->empid;
    m_EventFired.comment = eventFired->comment;
    m_EventFired.prettyjson = eventFired->prettyjson;
    m_EventFired.eventjson = eventFired->eventjson;
    m_EventFired.timestart = eventFired->timestart;
    m_EventFired.timeend = eventFired->timeend;
    m_EventFired.readdatetime = eventFired->readdatetime;
    m_EventFired.datetime = eventFired->datetime;

    m_EventFired.firstdatetime = eventFired->firstdatetime;
}

WorkItem::WorkItem(const char* type, int projectID, SectionGroup * sectiongroup)
    : m_type(type), m_project_id(projectID)
{
    init();

    m_Sectiongroup.projectid = sectiongroup->projectid;
    m_Sectiongroup.GroupID = sectiongroup->GroupID;
    m_Sectiongroup.SectionType = sectiongroup->SectionType;
    m_Sectiongroup.AreaID = sectiongroup->AreaID;
    m_Sectiongroup.Param1 = sectiongroup->Param1;
    m_Sectiongroup.Param2 = sectiongroup->Param2;
    m_Sectiongroup.Param3 = sectiongroup->Param3;
    m_Sectiongroup.Param4 = sectiongroup->Param4;
    m_Sectiongroup.Param5 = sectiongroup->Param5;
    m_Sectiongroup.Param6 = sectiongroup->Param6;
    m_Sectiongroup.maingroupid = sectiongroup->maingroupid;
    m_Sectiongroup.nodeIDList = new std::list<std::string>();

    if (sectiongroup->nodeIDList != NULL)
    {
        for (std::list<std::string>::iterator str = sectiongroup->nodeIDList->begin(); str != sectiongroup->nodeIDList->end(); str++)
        {
            m_Sectiongroup.nodeIDList->push_back((*str));
        }
    }
}

WorkItem::WorkItem(const char* type, int projectID, VILSConfig config)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_Config.projectid = config.projectid;
    m_Config.projectname = config.projectname;
    m_Config.projectdesc = config.projectdesc;
    // m_Config.iot_userid = config.iot_userid;
    // m_Config.iot_projectid = config.iot_projectid;
    // m_Config.iot_serverurl = config.iot_serverurl;
    // m_Config.iot_serverport = config.iot_serverport;
    // m_Config.iot_access_token = config.iot_access_token;
    m_Config.WGS48_Origin_X = config.WGS48_Origin_X;
    m_Config.WGS48_Origin_Y = config.WGS48_Origin_Y;
    m_Config.MAP_North = config.MAP_North;
    // m_Config.Building_Origin_X = config.Building_Origin_X;
    // m_Config.Building_Origin_Y = config.Building_Origin_Y;
    // m_Config.Building_X_Length = config.Building_X_Length;
    // m_Config.Building_Y_Length = config.Building_Y_Length;
    m_Config.BuildingID = config.BuildingID;
    m_Config.BuildingFloor = config.BuildingFloor;
    // m_Config.uploadresult = config.uploadresult;
    m_Config.datetime = config.datetime;
}

WorkItem::WorkItem(const char* type, int projectID, VILSSystemConfig * sysConfig)
    : m_type(type), m_project_id(projectID)
{
    init();

    m_SysConfig.SolveTagPosition_Time = sysConfig->SolveTagPosition_Time;
    m_SysConfig.SendTAFDB_Time = sysConfig->SendTAFDB_Time;
    m_SysConfig.SendTagAnchorList_Time = sysConfig->SendTagAnchorList_Time;
    m_SysConfig.SendDeviceStatus_Time = sysConfig->SendDeviceStatus_Time;
    m_SysConfig.UpdateNodeFromSMIMSIOTSERVER_Time = sysConfig->UpdateNodeFromSMIMSIOTSERVER_Time;

    m_SysConfig.web_udp_port = sysConfig->web_udp_port;
    m_SysConfig.center_udp_port = sysConfig->center_udp_port;
    m_SysConfig.coord_udp_port = sysConfig->coord_udp_port;
    m_SysConfig.center_broadcast_time = sysConfig->center_broadcast_time;
    m_SysConfig.center_broadcast_port = sysConfig->center_broadcast_port;
    m_SysConfig.web_socket_port = sysConfig->web_socket_port;
    m_SysConfig.UsingProtocol = sysConfig->UsingProtocol;
    m_SysConfig.VILSServer = sysConfig->VILSServer;
    m_SysConfig.VILSUID = sysConfig->VILSUID;
    m_SysConfig.ERPServer = sysConfig->ERPServer;
    m_SysConfig.SSOIP = sysConfig->SSOIP;

    m_SysConfig.PushNotifyPrefix = sysConfig->PushNotifyPrefix;
    m_SysConfig.PushNotifyWelcomeTitle = sysConfig->PushNotifyWelcomeTitle;
    m_SysConfig.PushNotifyWelcomeBody = sysConfig->PushNotifyWelcomeBody;
    m_SysConfig.EnablePushNotify = sysConfig->EnablePushNotify;

    m_SysConfig.TagAlertTimeout = sysConfig->TagAlertTimeout;
    m_SysConfig.TagAlertVoltageLow = sysConfig->TagAlertVoltageLow;

    //1.Ranging Data Preprocessing
    m_SysConfig.ValidDataTime = sysConfig->ValidDataTime;
    m_SysConfig.MaxRangindDistance = sysConfig->MaxRangindDistance;
    m_SysConfig.EnableNoiseMakup = sysConfig->EnableNoiseMakup;
    m_SysConfig.NoiseMakupRatio = sysConfig->NoiseMakupRatio;
    m_SysConfig.NoiseMakupWeight = sysConfig->NoiseMakupWeight;
    m_SysConfig.EnableKalman1D = sysConfig->EnableKalman1D;
    m_SysConfig.EnableRangingReliability = sysConfig->EnableRangingReliability;
    m_SysConfig.RangingReliabilityMaxDistance = sysConfig->RangingReliabilityMaxDistance;
    m_SysConfig.RangingReliabilityMinCount = sysConfig->RangingReliabilityMinCount;
    m_SysConfig.RangingReliabilityAverageDiff = sysConfig->RangingReliabilityAverageDiff;

    //2.Position Calculation
    m_SysConfig.EnableAnchor1DMapping = sysConfig->EnableAnchor1DMapping;// 0:disable 1:enable
    m_SysConfig.EnableOneAnchorCalculation = sysConfig->EnableOneAnchorCalculation;// 0:disable 1:enable
    m_SysConfig.EnableGDOP = sysConfig->EnableGDOP;
    m_SysConfig.GDOPThreshold = sysConfig->GDOPThreshold;
    m_SysConfig.CalculationMethod = sysConfig->CalculationMethod;
    m_SysConfig.EnableIMUVelocity = sysConfig->EnableIMUVelocity;
    m_SysConfig.IMUVelocityOffset = sysConfig->IMUVelocityOffset;
    m_SysConfig.SteadyStateOffset = sysConfig->SteadyStateOffset;
    m_SysConfig.WeightMovingAverageValue1 = sysConfig->WeightMovingAverageValue1;
    m_SysConfig.WeightMovingAverageValue2 = sysConfig->WeightMovingAverageValue2;
    m_SysConfig.WeightMovingAverageValue3 = sysConfig->WeightMovingAverageValue3;
    m_SysConfig.StayInWallTimeout = sysConfig->StayInWallTimeout;
    m_SysConfig.EnableFixHeight = sysConfig->EnableFixHeight;

    //3.Position Result
    m_SysConfig.SmoothingResult = sysConfig->SmoothingResult;
    m_SysConfig.SmoothingResultOffset = sysConfig->SmoothingResultOffset;
    m_SysConfig.SmoothingResultStep = sysConfig->SmoothingResultStep;
    m_SysConfig.SmoothingResultFirstStateOffsetRatio = sysConfig->SmoothingResultFirstStateOffsetRatio;
    m_SysConfig.SmoothingResultFirstStateRunTime = sysConfig->SmoothingResultFirstStateRunTime;
    m_SysConfig.SmoothingResultSecondStateOffsetRatio = sysConfig->SmoothingResultSecondStateOffsetRatio;
    m_SysConfig.SmoothingResultSecondStateRunTime = sysConfig->SmoothingResultSecondStateRunTime;

    //
    // LoRAWAN
    //
    m_SysConfig.LoRAWANTPOIC = sysConfig->LoRAWANTPOIC;
    m_SysConfig.MQTTHostIp = sysConfig->MQTTHostIp;
    m_SysConfig.MQTTHostPort = sysConfig->MQTTHostPort;
    m_SysConfig.MQTTHostUsername = sysConfig->MQTTHostUsername;
    m_SysConfig.MQTTHostPassword = sysConfig->MQTTHostPassword;
    m_SysConfig.MQTTClientId = sysConfig->MQTTClientId;
}

WorkItem::WorkItem(const char* type, int projectID, CourseStruct * course)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_Course.dbId = course->dbId;
    m_Course.projectid = course->projectid;
    m_Course.allDay = course->allDay;
    m_Course.resourceId = course->resourceId;
    m_Course.courseId = course->courseId;
    m_Course.title = course->title;
    m_Course.description = course->description;
    m_Course.start = course->start;
    m_Course.end = course->end;
    m_Course.startTime = course->startTime;
    m_Course.endTime = course->endTime;
    m_Course.daysOfWeek = new std::list<int>();
    m_Course.startRecur = course->startRecur;
    m_Course.endRecur = course->endRecur;
    m_Course.backgroundColor = course->backgroundColor;
    m_Course.borderColor = course->borderColor;
    m_Course.maingroupid = course->maingroupid;
}

WorkItem::WorkItem(const char* type, int projectID, CourseRollCallRecordStruct * courseRollCallRecord)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_CourseRollCallRecord.projectid = courseRollCallRecord->projectid;
    m_CourseRollCallRecord.courseId = courseRollCallRecord->courseId;
    m_CourseRollCallRecord.infoOfDate = courseRollCallRecord->infoOfDate;
    m_CourseRollCallRecord.tags = new std::list<std::string>();

    if (courseRollCallRecord->tags != NULL)
    {
        for (std::list<std::string>::iterator str = courseRollCallRecord->tags->begin(); str != courseRollCallRecord->tags->end(); str++)
        {
            m_CourseRollCallRecord.tags->push_back((*str));
        }
    }
}

WorkItem::WorkItem(const char* type, int projectID, AreaInOutEventStruct * inoutEvent)
    : m_type(type), m_project_id(projectID)
{
    init();
    mInOutEvent.action = inoutEvent->action;
    mInOutEvent.AreaType0D = inoutEvent->AreaType0D;
    mInOutEvent.projectid = inoutEvent->projectid;
    mInOutEvent.tafid = inoutEvent->tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    mInOutEvent.MapAreaType = inoutEvent->MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    mInOutEvent.NodeId = inoutEvent->NodeId;
    mInOutEvent.NodeName = inoutEvent->NodeName;
    mInOutEvent.NodeMacaddress = inoutEvent->NodeMacaddress;
    mInOutEvent.AreaID = inoutEvent->AreaID;
    mInOutEvent.AreaName = inoutEvent->AreaName;
    mInOutEvent.DataTime = inoutEvent->DataTime;
}

WorkItem::WorkItem(const char* type, int projectID, GPSInfoStruct * GPSInfo)
    : m_type(type), m_project_id(projectID)
{
    init();
    mGPSInfo.nodetype = GPSInfo->nodetype;
    mGPSInfo.nodeid = GPSInfo->nodeid;
    mGPSInfo.Longitude = GPSInfo->Longitude;
    mGPSInfo.Latitude = GPSInfo->Latitude;
    mGPSInfo.updateTimestamp = GPSInfo->updateTimestamp;
}

WorkItem::WorkItem(const char* type, int projectID, LoRaGatewayInfoStruct * gateway)
    : m_type(type), m_project_id(projectID)
{
    init();
    mLoRaGateway.sf = gateway->sf;
    mLoRaGateway.projectid = gateway->projectid;
    mLoRaGateway.gwip = gateway->gwip;
    mLoRaGateway.gwid = gateway->gwid;
    mLoRaGateway.repeater = gateway->repeater;
    mLoRaGateway.maplayer = gateway->maplayer;
    mLoRaGateway.posX = gateway->posX;
    mLoRaGateway.posY = gateway->posY;
    mLoRaGateway.posZ = gateway->posZ;
    mLoRaGateway.alive = gateway->alive;// alive count
    mLoRaGateway.MQTTalive = gateway->MQTTalive;// MQTT alive count
    mLoRaGateway.pingCount = gateway->pingCount;
    mLoRaGateway.aliveTime = gateway->aliveTime;
    mLoRaGateway.maingroupid = gateway->maingroupid;
    mLoRaGateway.verifykey = gateway->verifykey;

    // mLoRaGateway.channel = gateway->channel;
    // copy list
    for (std::list<std::string>::iterator channel = gateway->channels.begin(); channel != gateway->channels.end(); channel++)
    {
        mLoRaGateway.channels.push_back((*channel));
    }
}

WorkItem::WorkItem(const char* type, int projectID, AnalyzeTrackRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mAnalyzeTrackRecord.projectid = record->projectid;
    mAnalyzeTrackRecord.nodetype = record->nodetype;
    mAnalyzeTrackRecord.nodeid = record->nodeid;
    mAnalyzeTrackRecord.datatype = record->datatype;
    mAnalyzeTrackRecord.analyzeddone = record->analyzeddone; // 1:done
    mAnalyzeTrackRecord.analyzeddate = record->analyzeddate; // date for already analyzed, foramt: YYYY-MM-DD
}

WorkItem::WorkItem(const char* type, int projectID, TagDayTrackRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mDayTrackRecord.projectid = record->projectid;
    mDayTrackRecord.nodeid = record->nodeid;
    mDayTrackRecord.nodetype = record->nodetype;
    mDayTrackRecord.datatype = record->datatype;
    mDayTrackRecord.count = record->count;
    mDayTrackRecord.day = record->day;
    mDayTrackRecord.max_time = record->max_time;
    mDayTrackRecord.min_time = record->min_time;
    mDayTrackRecord.total_time = record->total_time;
    mDayTrackRecord.area_ids = record->area_ids;
    mDayTrackRecord.area_names = record->area_names;
    mDayTrackRecord.course_id = record->course_id;
    mDayTrackRecord.course_info = record->course_info;
}

WorkItem::WorkItem(const char* type, int projectID, DeviceActivityRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mDeviceActivityRecord.nodetype = record->nodetype;
    mDeviceActivityRecord.nodeid = record->nodeid;
    mDeviceActivityRecord.alive = record->alive;
    mDeviceActivityRecord.issueday = record->issueday;
    mDeviceActivityRecord.issuetime = record->issuetime;
}

WorkItem::WorkItem(const char* type, int projectID, AreaRollCallRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mAreaRollCallRecord.projectid = record->projectid;
    mAreaRollCallRecord.areaid = record->areaid;
    mAreaRollCallRecord.nodeid = record->nodeid;
    mAreaRollCallRecord.issueday = record->issueday;
    mAreaRollCallRecord.issuetime = record->issuetime;
    mAreaRollCallRecord.totaltime = record->totaltime;
    mAreaRollCallRecord.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, OperationModeRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mOperationModeRecord.mode = record->mode;
    mOperationModeRecord.nodeid = record->nodeid;
    mOperationModeRecord.issueday = record->issueday;
    mOperationModeRecord.issuetime = record->issuetime;
    mOperationModeRecord.totaltime = record->totaltime;
    mOperationModeRecord.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, UserResidentStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mResidentRecord.id = record->id;
    mResidentRecord.serno = record->serno;
    mResidentRecord.gender = record->gender;
    mResidentRecord.name = record->name;
    mResidentRecord.roomNo = record->roomNo;
    mResidentRecord.residentType = record->residentType;
    mResidentRecord.nurseNo = record->nurseNo;
    mResidentRecord.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, UserEmployeeStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mEmployeeRecord.id = record->id;
    mEmployeeRecord.empid = record->empid;
    mEmployeeRecord.cname = record->cname;
    mEmployeeRecord.orgid = record->orgid;
    mEmployeeRecord.orgName = record->orgName;
    mEmployeeRecord.area = record->area;
    mEmployeeRecord.jobTitle = record->jobTitle;
    mEmployeeRecord.gender = record->gender;
    mEmployeeRecord.pwd = record->pwd;
    mEmployeeRecord.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, DeviceActivityAliveCountStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mDeviceActivityAliveRecord.nodetype = record->nodetype;
    mDeviceActivityAliveRecord.devicecount = record->devicecount;
    mDeviceActivityAliveRecord.aliveString = record->aliveString;
    mDeviceActivityAliveRecord.issueday = record->issueday;
    mDeviceActivityAliveRecord.issuetime = record->issuetime;
}

WorkItem::WorkItem(const char* type, int projectID, AreaActivityUserCountStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mAreaActivityUserCountRecord.count = record->count;
    mAreaActivityUserCountRecord.totaltime = record->totaltime;
    mAreaActivityUserCountRecord.areaid = record->areaid;
    mAreaActivityUserCountRecord.areaname = record->areaname;
    mAreaActivityUserCountRecord.issueday = record->issueday;
    mAreaActivityUserCountRecord.issuetime = record->issuetime;
    mAreaActivityUserCountRecord.start_datetime = record->start_datetime;
    mAreaActivityUserCountRecord.end_datetime = record->end_datetime;
}

WorkItem::WorkItem(const char* type, int projectID, EmployeeEventReportStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mEmployeeEventReportRecord.eventid_alert = record->eventid_alert;
    mEmployeeEventReportRecord.eventid_cancel = record->eventid_cancel;
    mEmployeeEventReportRecord.emp_empid = record->emp_empid;
    mEmployeeEventReportRecord.emp_cname = record->emp_cname;
    mEmployeeEventReportRecord.emp_orgName = record->emp_orgName;
    mEmployeeEventReportRecord.emp_jobTitle = record->emp_jobTitle;
    mEmployeeEventReportRecord.comment = record->comment;
    mEmployeeEventReportRecord.action = record->action;
    mEmployeeEventReportRecord.nodeid = record->nodeid;
    mEmployeeEventReportRecord.locatorid = record->locatorid;
    mEmployeeEventReportRecord.resident_serno = record->resident_serno;
    mEmployeeEventReportRecord.resident_name = record->resident_name;
    mEmployeeEventReportRecord.resident_roomNo = record->resident_roomNo;
    mEmployeeEventReportRecord.eventtime = record->eventtime;
    mEmployeeEventReportRecord.reponsetime = record->reponsetime;
}

WorkItem::WorkItem(const char* type, int projectID, OperationModeActivityStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mOperationModeActivityRecord.mode = record->mode;
    mOperationModeActivityRecord.count = record->count;
    mOperationModeActivityRecord.totaltime = record->totaltime;
    mOperationModeActivityRecord.issueday = record->issueday;
    mOperationModeActivityRecord.issuetime = record->issuetime;
}

WorkItem::WorkItem(const char* type, int projectID, TagMoveOffsetInfoStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mTagMoveOffsetInfoRecord.TagId = record->TagId;
    mTagMoveOffsetInfoRecord.moveoffset = record->moveoffset;
    mTagMoveOffsetInfoRecord.prePosX = record->prePosX;
    mTagMoveOffsetInfoRecord.prePosY = record->prePosY;
    mTagMoveOffsetInfoRecord.prePosZ = record->prePosZ;
    mTagMoveOffsetInfoRecord.issuetime = record->issuetime;
}

WorkItem::WorkItem(const char* type, int projectID, NodeMoveOffsetInfoStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mNodeMoveOffsetInfoRecord.nodetype = record->nodetype;
    mNodeMoveOffsetInfoRecord.nodeid = record->nodeid;
    mNodeMoveOffsetInfoRecord.moveoffset = record->moveoffset;
    mNodeMoveOffsetInfoRecord.minutes = record->minutes;
    mNodeMoveOffsetInfoRecord.issueday = record->issueday;
    mNodeMoveOffsetInfoRecord.issuetime = record->issuetime;
    mNodeMoveOffsetInfoRecord.start_datetime = record->start_datetime;
    mNodeMoveOffsetInfoRecord.end_datetime = record->end_datetime;
}

WorkItem::WorkItem(const char* type, int projectID, PushNotifyMessageStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mPushNotifyMessage.type = record->type;
    mPushNotifyMessage.nodeid = record->nodeid;
    mPushNotifyMessage.targetid = record->targetid;
    mPushNotifyMessage.topic = record->topic;
    mPushNotifyMessage.messagehashkey = record->messagehashkey;
    mPushNotifyMessage.messageid = record->messageid;
    mPushNotifyMessage.maingroups = record->maingroups;
}

WorkItem::WorkItem(const char* type, int projectID, PushNotifyMessageRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mPushNotifyMessageRecord.type = record->type;
    mPushNotifyMessageRecord.nodeid = record->nodeid;
    mPushNotifyMessageRecord.title = record->title;
    mPushNotifyMessageRecord.message = record->message;
    mPushNotifyMessageRecord.messagehashkey = record->messagehashkey;
    mPushNotifyMessageRecord.targetids = record->targetids;
    mPushNotifyMessageRecord.subgroupids = record->subgroupids;
    mPushNotifyMessageRecord.surveyids = record->surveyids;
    mPushNotifyMessageRecord.hashkeys = record->hashkeys;
    mPushNotifyMessageRecord.deadlines = record->deadlines;
    mPushNotifyMessageRecord.userid = record->userid;
    mPushNotifyMessageRecord.imageuid = record->imageuid;
    mPushNotifyMessageRecord.formid = record->formid;
    mPushNotifyMessageRecord.eventday = record->eventday;
    mPushNotifyMessageRecord.eventtime = record->eventtime;
    mPushNotifyMessageRecord.maingroups = record->maingroups;
}

WorkItem::WorkItem(const char* type, int projectID, MapLayerStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mMapLayerRecord.projectid = record->projectid;
    mMapLayerRecord.layerid = record->layerid;
    mMapLayerRecord.layer_name = record->layer_name;
    mMapLayerRecord.layer_description = record->layer_description;
    mMapLayerRecord.WGS48OriginX = record->WGS48OriginX;
    mMapLayerRecord.WGS48OriginY = record->WGS48OriginY;
    mMapLayerRecord.MAPNorth = record->MAPNorth;
    mMapLayerRecord.BuildingID = record->BuildingID;
    mMapLayerRecord.BuildingFloor = record->BuildingFloor;
    mMapLayerRecord.POSX = record->POSX;
    mMapLayerRecord.POSY = record->POSY;
    mMapLayerRecord.SCALE = record->SCALE;
    mMapLayerRecord.COEFF = record->COEFF;
    mMapLayerRecord.FILE = record->FILE;
    mMapLayerRecord.MAPWIDTH = record->MAPWIDTH;
    mMapLayerRecord.MAPHEIGHT = record->MAPHEIGHT;
    mMapLayerRecord.maingroupid = record->maingroupid;
}

WorkItem::WorkItem(const char* type, int projectID, MapLayerAreaStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mMapLayerAreaRecord.layerid = record->layerid;
    mMapLayerAreaRecord.areaid = record->areaid;
    mMapLayerAreaRecord.areaname = record->areaname;
    mMapLayerAreaRecord.geojson = record->geojson;
    mMapLayerAreaRecord.type = record->type;
    mMapLayerAreaRecord.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, AoANodeStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mAoANodeRecord.projectid = record->projectid;

    snprintf(mAoANodeRecord.nodeid, 32, "%s", record->nodeid);
    snprintf(mAoANodeRecord.nodename, 32, "%s", record->nodename);
    snprintf(mAoANodeRecord.panid, 32, "%s", record->panid);
    snprintf(mAoANodeRecord.macaddress, 32, "%s", record->macaddress);

    mAoANodeRecord.maplayer = record->maplayer;
    mAoANodeRecord.posX = record->posX;
    mAoANodeRecord.posY = record->posY;
    mAoANodeRecord.posZ = record->posZ;
    mAoANodeRecord.pitch = record->pitch;
    mAoANodeRecord.roll = record->roll;
    mAoANodeRecord.yaw = record->yaw;
    mAoANodeRecord.gridsize = record->gridsize;

    mAoANodeRecord.maingroupid = record->maingroupid;
    mAoANodeRecord.verifykey = record->verifykey;
}

WorkItem::WorkItem(const char* type, int projectID, FenceAlertStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mFenceAlertRecord.GroupID = record->GroupID;
    mFenceAlertRecord.nodeid = record->nodeid;
    mFenceAlertRecord.distance = record->distance;
}

WorkItem::WorkItem(const char* type, int projectID, GAESurveyResultData * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mGAESurveyResultData.hashkey = record->hashkey;
    mGAESurveyResultData.token = record->token;
    mGAESurveyResultData.topic = record->topic;
    mGAESurveyResultData.messagehashkey = record->messagehashkey;
    mGAESurveyResultData.comment = record->comment;
    mGAESurveyResultData.datetime = record->datetime;

    for (std::list<std::string>::iterator result = record->results.begin(); result != record->results.end(); result++)
    {
        mGAESurveyResultData.results.push_back((*result));
    }
}

WorkItem::WorkItem(const char* type, int projectID, GAEMessageStatusData * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mGAEMessageStatusData.messageid = record->messageid;
    mGAEMessageStatusData.topic = record->topic;
    mGAEMessageStatusData.messagehashkey = record->messagehashkey;
    mGAEMessageStatusData.token = record->token;
    mGAEMessageStatusData.read = record->read;
    mGAEMessageStatusData.finished = record->finished;
    mGAEMessageStatusData.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, TargetTopicStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mTargetTopicRecord.target = record->target;
    mTargetTopicRecord.topic = record->topic;
    mTargetTopicRecord.name = record->name;
    mTargetTopicRecord.accountid = record->accountid;
    mTargetTopicRecord.datetime = record->datetime;
    mTargetTopicRecord.maingroupid = record->maingroupid;
    mTargetTopicRecord.userid = record->userid;
}

WorkItem::WorkItem(const char* type, int projectID, CARDRollCallRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mCARDRollCallRecord.rollcallid = record->rollcallid;
    mCARDRollCallRecord.rollcalleventid = record->rollcalleventid;
    mCARDRollCallRecord.cardid = record->cardid;
    mCARDRollCallRecord.userid = record->userid;
    mCARDRollCallRecord.nodeid = record->nodeid;

    for (std::list<int>::iterator value = record->rfiddeviceids.begin(); value != record->rfiddeviceids.end(); value++)
    {
        mCARDRollCallRecord.rfiddeviceids.push_back((*value));
    }

    for (std::list<std::string>::iterator value = record->nfclocators.begin(); value != record->nfclocators.end(); value++)
    {
        mCARDRollCallRecord.nfclocators.push_back((*value));
    }

    //mCARDRollCallRecord.rfiddeviceid = record->rfiddeviceid;
    //mCARDRollCallRecord.nfclocator = record->nfclocator;

    mCARDRollCallRecord.present = record->present;
    mCARDRollCallRecord.datetime = record->datetime;
    mCARDRollCallRecord.updatetime = record->updatetime;
    mCARDRollCallRecord.maingroups = record->maingroups;
}

WorkItem::WorkItem(const char* type, int projectID, CARDRollCallEventStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mCARDRollCallEvent.eventid = record->eventid;
    mCARDRollCallEvent.rollcallid = record->rollcallid;
    mCARDRollCallEvent.rollcallname = record->rollcallname;
    mCARDRollCallEvent.StartDateTime = record->StartDateTime;
    mCARDRollCallEvent.EndDateTime = record->EndDateTime;
    mCARDRollCallEvent.totalcount = record->totalcount;
    mCARDRollCallEvent.presentcount = record->presentcount;
    mCARDRollCallEvent.datetime = record->datetime;
    mCARDRollCallEvent.updatetime = record->updatetime;
    mCARDRollCallEvent.maingroups = record->maingroups;
}

WorkItem::WorkItem(const char* type, int projectID, CARDRollCallSubGroupEventStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mCARDRollCallSubGroupEvent.eventid = record->eventid;
    mCARDRollCallSubGroupEvent.rollcallid = record->rollcallid;
    mCARDRollCallSubGroupEvent.subgroupid = record->subgroupid;
    mCARDRollCallSubGroupEvent.rollcallname = record->rollcallname;
    mCARDRollCallSubGroupEvent.StartDateTime = record->StartDateTime;
    mCARDRollCallSubGroupEvent.EndDateTime = record->EndDateTime;
    mCARDRollCallSubGroupEvent.totalcount = record->totalcount;
    mCARDRollCallSubGroupEvent.presentcount = record->presentcount;
    mCARDRollCallSubGroupEvent.datetime = record->datetime;
    mCARDRollCallSubGroupEvent.updatetime = record->updatetime;

    for (std::list<int>::iterator userid = record->userids.begin(); userid != record->userids.end(); userid++)
    {
        mCARDRollCallSubGroupEvent.userids.push_back((*userid));
    }
    for (std::list<int>::iterator userid = record->presentuserids.begin(); userid != record->presentuserids.end(); userid++)
    {
        mCARDRollCallSubGroupEvent.presentuserids.push_back((*userid));
    }
    mCARDRollCallSubGroupEvent.maingroups = record->maingroups;
}

WorkItem::WorkItem(const char* type, int projectID, CARDScanRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mCARDScanRecord.id = record->id;
    mCARDScanRecord.cardtype = record->cardtype;
    mCARDScanRecord.cardid = record->cardid;
    mCARDScanRecord.LoRAMacaddress = record->LoRAMacaddress;
    mCARDScanRecord.ssid = record->ssid;
    mCARDScanRecord.wifimac = record->wifimac;
    mCARDScanRecord.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, RFIDDeviceStruct * device)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_RFIDDevice.alive = device->alive;
    m_RFIDDevice.deviceid = device->deviceid;
    m_RFIDDevice.macaddress = device->macaddress;
    m_RFIDDevice.name = device->name;
    m_RFIDDevice.type = device->type;
    m_RFIDDevice.ip = device->ip;
    m_RFIDDevice.gwid = device->gwid;
    m_RFIDDevice.LoRAMacaddress = device->LoRAMacaddress;
    m_RFIDDevice.datetime = device->datetime;
    m_RFIDDevice.aliveTime = device->aliveTime;
    m_RFIDDevice.maingroupid = device->maingroupid;
    m_RFIDDevice.verifykey = device->verifykey;
}

WorkItem::WorkItem(const char* type, int projectID, NFCDeviceStruct * device)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_NFCDevice.alive = device->alive;
    m_NFCDevice.deviceid = device->deviceid;
    m_NFCDevice.macaddress = device->macaddress;
    m_NFCDevice.name = device->name;
    m_NFCDevice.type = device->type;
    m_NFCDevice.gwid = device->gwid;
    m_NFCDevice.LoRAMacaddress = device->LoRAMacaddress;
    m_NFCDevice.version = device->version;
    m_NFCDevice.datetime = device->datetime;
    m_NFCDevice.aliveTime = device->aliveTime;
    m_NFCDevice.ssid = device->ssid;
    m_NFCDevice.wifimac = device->wifimac;
    m_NFCDevice.maingroupid = device->maingroupid;
    m_NFCDevice.verifykey = device->verifykey;
}

WorkItem::WorkItem(const char* type, int projectID, RFIDCardStruct * card)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_RFIDCard.alive = card->alive;
    m_RFIDCard.cardid = card->cardid;
    m_RFIDCard.name = card->name;
    m_RFIDCard.type = card->type;
    m_RFIDCard.maplayer = card->maplayer;
    m_RFIDCard.userid = card->userid;
    m_RFIDCard.unknown = card->unknown;
    m_RFIDCard.deviceid = card->deviceid;
    m_RFIDCard.datetime = card->datetime;
    m_RFIDCard.updatetime = card->updatetime;
    m_RFIDCard.maingroupid = card->maingroupid;
}

WorkItem::WorkItem(const char* type, int projectID, NFCCardStruct * card)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_NFCCard.alive = card->alive;
    m_NFCCard.cardid = card->cardid;
    m_NFCCard.name = card->name;
    m_NFCCard.type = card->type;
    m_NFCCard.maplayer = card->maplayer;
    m_NFCCard.userid = card->userid;
    m_NFCCard.unknown = card->unknown;
    m_NFCCard.deviceid = card->deviceid;
    m_NFCCard.datetime = card->datetime;
    m_NFCCard.updatetime = card->updatetime;
    m_NFCCard.maingroupid = card->maingroupid;
}

WorkItem::WorkItem(const char* type, int projectID, EDGEDeviceStruct * device)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_EdgeDevice.alive = device->alive;
    m_EdgeDevice.deviceid = device->deviceid;
    m_EdgeDevice.macaddress = device->macaddress;
    m_EdgeDevice.name = device->name;
    m_EdgeDevice.type = device->type;
    m_EdgeDevice.ip = device->ip;
    m_EdgeDevice.LoRAMacaddress = device->LoRAMacaddress;
    m_EdgeDevice.gwid = device->gwid;
    m_EdgeDevice.version = device->version;
    m_EdgeDevice.datetime = device->datetime;
    m_EdgeDevice.aliveTime = device->aliveTime;
    m_EdgeDevice.maingroupid = device->maingroupid;
    m_EdgeDevice.verifykey = device->verifykey;
}

WorkItem::WorkItem(const char* type, int projectID, UserStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_UserData.userid = record->userid;
    m_UserData.groupid = record->groupid;
    m_UserData.type = record->type;
    m_UserData.empid = record->empid;
    m_UserData.account = record->account;
    m_UserData.name = record->name;
    m_UserData.unit = record->unit;
    m_UserData.title = record->title;
    m_UserData.gender = record->gender;
    m_UserData.pwd = record->pwd;
    m_UserData.datetime = record->datetime;

    m_UserData.maingroups = copyStringList(record->maingroups);
    m_UserData.datagroups = copyStringList(record->datagroups);
    m_UserData.subgroups = copyStringList(record->subgroups);
    m_UserData.rfidcards = copyStringList(record->rfidcards);
    m_UserData.nfccards = copyStringList(record->nfccards);
    m_UserData.uwbcards = copyStringList(record->uwbcards);
    m_UserData.sensors = copyStringList(record->sensors);
    m_UserData.imageuid = record->imageuid;

    m_UserData.maplayer = record->maplayer;
    m_UserData.posX = record->posX;
    m_UserData.posY = record->posY;
    m_UserData.posZ = record->posZ;
    m_UserData.Longitude = record->Longitude;
    m_UserData.Latitude = record->Latitude;

    m_UserData.AreaId = record->AreaId;
    m_UserData.AreaName = record->AreaName;
    m_UserData.AreaId0D = record->AreaId0D;
    m_UserData.AreaType0D = record->AreaType0D;
    m_UserData.AreaType5Id = record->AreaType5Id;
    m_UserData.AreaType5Name = record->AreaType5Name;
    m_UserData.RangingResultType = record->RangingResultType;
    m_UserData.area_unknown = record->area_unknown;
    m_UserData.exclude_rollcall_AreaId = record->exclude_rollcall_AreaId;

}

WorkItem::WorkItem(const char* type, int projectID, UserSubGroupStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_UserSubGroup.subgroupid = record->subgroupid;
    m_UserSubGroup.subgroupname = record->subgroupname;
    m_UserSubGroup.maingroupid = record->maingroupid;
    m_UserSubGroup.subgroupid = record->subgroupid;
    m_UserSubGroup.permission = record->permission;
    m_UserSubGroup.imageuid = record->imageuid;
    m_UserSubGroup.accountidList = copyStringList(record->accountidList);
    m_UserSubGroup.useridList = copyStringList(record->useridList);
    m_UserSubGroup.type = record->type;
    m_UserSubGroup.validdate = record->validdate;
    m_UserSubGroup.curriculumrecordid = record->curriculumrecordid;
}

WorkItem::WorkItem(const char* type, int projectID, UserMainGroupStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_UserMainGroup.id = record->id;
    m_UserMainGroup.autojoin = record->autojoin;
    m_UserMainGroup.name = record->name;
    m_UserMainGroup.permission = record->permission;
    m_UserMainGroup.welcommessage = record->welcommessage;
    m_UserMainGroup.accountidList = copyStringList(record->accountidList);
    m_UserMainGroup.useridList = copyStringList(record->useridList);
    m_UserMainGroup.imageuid = record->imageuid;
}

WorkItem::WorkItem(const char* type, int projectID, LoginTokenStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_LoginToken.userid = record->userid;
    m_LoginToken.accountid = record->accountid;
    m_LoginToken.token = record->token;
    m_LoginToken.loginkey = record->loginkey;
}

WorkItem::WorkItem(const char* type, int projectID, AssistEventStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mAssistEvent.id = record->id;
    mAssistEvent.assistid = record->assistid;
    mAssistEvent.userid = record->userid;
    mAssistEvent.description = record->description;
    // mAssistEvent.accountid = record->accountid;
    for (std::list<std::string>::iterator eventtime = record->eventtime.begin(); eventtime != record->eventtime.end(); eventtime++)
    {
        mAssistEvent.eventtime.push_back((*eventtime));
    }
    mAssistEvent.weekday = record->weekday;
    for (std::list<std::string>::iterator validdate = record->validdate.begin(); validdate != record->validdate.end(); validdate++)
    {
        mAssistEvent.validdate.push_back((*validdate));
    }
    for (std::list<std::string>::iterator invaliddate = record->invaliddate.begin(); invaliddate != record->invaliddate.end(); invaliddate++)
    {
        mAssistEvent.invaliddate.push_back((*invaliddate));
    }
    mAssistEvent.deadline = record->deadline;
    mAssistEvent.imageids = record->imageids;
    mAssistEvent.fileids = record->fileids;
    mAssistEvent.applyusername = record->applyusername;
    mAssistEvent.applyuserrelation = record->applyuserrelation;
    mAssistEvent.signoffstatus = record->signoffstatus;
    mAssistEvent.signoffuserid = record->signoffuserid;
    mAssistEvent.maingroups = record->maingroups;
    // mAssistEvent.signoffaccountid = record->signoffaccountid;
    mAssistEvent.datetime = record->datetime;
}

WorkItem::WorkItem(const char* type, int projectID, AssistEventRecordStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mAssistEventRecord.id = record->id;
    mAssistEventRecord.assistid = record->assistid;
    mAssistEventRecord.assisteventid = record->assisteventid;
    mAssistEventRecord.PushNotifyMessageRecorddbId = record->PushNotifyMessageRecorddbId;
    mAssistEventRecord.userid = record->userid;
    // mAssistEventRecord.accountid = record->accountid;
    mAssistEventRecord.messagehashkey = record->messagehashkey;
    for (std::list<std::string>::iterator result = record->results.begin(); result != record->results.end(); result++)
    {
        mAssistEventRecord.results.push_back((*result));
    }
    mAssistEventRecord.eventday = record->eventday;
    mAssistEventRecord.eventtime = record->eventtime;
    mAssistEventRecord.maingroups = record->maingroups;
    mAssistEventRecord.datetime = record->datetime;
    mAssistEventRecord.updatetime = record->updatetime;
}

WorkItem::WorkItem(const char* type, int projectID, ApplicationFormLeaveStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    mApplicationFormLeave.formid = record->formid;
    mApplicationFormLeave.applytime = record->applytime;
    mApplicationFormLeave.userid = record->userid;
    mApplicationFormLeave.subgroups = record->subgroups;
    mApplicationFormLeave.type = record->type;
    mApplicationFormLeave.reason = record->reason;
    mApplicationFormLeave.startdate = record->startdate;
    mApplicationFormLeave.starttime = record->starttime;
    mApplicationFormLeave.enddate = record->enddate;
    mApplicationFormLeave.endtime = record->endtime;
    mApplicationFormLeave.reissue = record->reissue;
    mApplicationFormLeave.applyusername = record->applyusername;
    mApplicationFormLeave.applyuserrelation = record->applyuserrelation;
    mApplicationFormLeave.imageids = record->imageids;
    mApplicationFormLeave.fileids = record->fileids;
    mApplicationFormLeave.signoffstatus = record->signoffstatus;// 0:簽核中 1:已簽核 99:未核准
    mApplicationFormLeave.signoffuserid = record->signoffuserid;
    mApplicationFormLeave.maingroups = record->maingroups;
    mApplicationFormLeave.datetime = record->datetime;
    mApplicationFormLeave.updatetime = record->updatetime;
}

WorkItem::WorkItem(const char* type, int projectID, SensorInfoStruct * info)
    : m_type(type), m_project_id(projectID)
{
    init();
    mSensorInfo.id = info->id;
    mSensorInfo.name = info->name;
    mSensorInfo.day = info->day;
    mSensorInfo.value = info->value;
    mSensorInfo.jsonvalue = info->jsonvalue;
    mSensorInfo.Longitude = info->Longitude;
    mSensorInfo.Latitude = info->Latitude;
    mSensorInfo.datetime = info->datetime;
    mSensorInfo.maingroups = info->maingroups; // UserMainGroup.id
}

WorkItem::WorkItem(const char* type, int projectID, SensorDeviceStruct * device)
    : m_type(type), m_project_id(projectID)
{
    init();
    mSensorDevice.alive = device->alive;
    mSensorDevice.maingroupid = device->maingroupid; // UserMainGroup.id
    mSensorDevice.id = device->id;
    mSensorDevice.name = device->name;
    mSensorDevice.type = device->type;
    mSensorDevice.jsonvalue = device->jsonvalue;
    mSensorDevice.Longitude = device->Longitude;
    mSensorDevice.Latitude = device->Latitude;
    mSensorDevice.Battery = device->Battery;
    mSensorDevice.Version = device->Version;
    mSensorDevice.datetime = device->datetime;
    mSensorDevice.verifykey = device->verifykey;
}

WorkItem::WorkItem(const char* type, int projectID, SensorFiredEventStruct * event)
    : m_type(type), m_project_id(projectID)
{
    init();
    mSensorFiredEvent.id = event->id;
    mSensorFiredEvent.sensorid = event->sensorid;
    mSensorFiredEvent.type = event->type;
    mSensorFiredEvent.jsonvalue = event->jsonvalue;
    mSensorFiredEvent.datetime = event->datetime;
    mSensorFiredEvent.maingroupid = event->maingroupid; // UserMainGroup.id
}

WorkItem::WorkItem(const char* type, int projectID, FlyInfoStruct * info)
    : m_type(type), m_project_id(projectID)
{
    init();
    mFlyInfo.FDATE = info->FDATE;
    mFlyInfo.expectTime = info->expectTime;
    mFlyInfo.airLineCode = info->airLineCode;
    mFlyInfo.airLineNum = info->airLineNum;
    mFlyInfo.type = info->type;
    mFlyInfo.jsonvalue = info->jsonvalue;
    mFlyInfo.triggerTime = info->triggerTime;
    mFlyInfo.datetime = info->datetime;
    mFlyInfo.maingroupid = info->maingroupid; // UserMainGroup.id
    mFlyInfo.sensorid = info->sensorid;
    mFlyInfo.airBoardingGate = info->airBoardingGate;
}

WorkItem::WorkItem(const char* type, int projectID, WCInfoStruct * info)
    : m_type(type), m_project_id(projectID)
{
    init();
    mWCInfo.GID = info->GID;
    mWCInfo.place = info->place;
    mWCInfo.item = info->item;
    mWCInfo.notetime = info->notetime;
    mWCInfo.sendNotify = info->sendNotify;
    mWCInfo.maingroupid = info->maingroupid;
}

WorkItem::WorkItem(const char* type, int projectID, SoccerRecordingStruct * record)
    : m_type(type), m_project_id(projectID)
{
    init();
    m_SoccerRecording.recordid = record->recordid;
    m_SoccerRecording.maingroupid = record->maingroupid;
    m_SoccerRecording.recordnumber = record->recordnumber;
    m_SoccerRecording.starttime = record->starttime;
    m_SoccerRecording.endtime = record->endtime;
}

WorkItem::WorkItem(const char* type, int projectID, CurriculumApplyStruct * info)
    : m_type(type), m_project_id(projectID)
{
    init();
    mCurriculumApply.userid = info->userid;
    mCurriculumApply.curriculumrecordid = info->curriculumrecordid;
    mCurriculumApply.status = info->status;
    mCurriculumApply.datetime = info->datetime;
}

WorkItem::~WorkItem()
{
    if (m_Sectiongroup.nodeIDList != NULL)
    {
        m_Sectiongroup.nodeIDList->clear();
        delete m_Sectiongroup.nodeIDList;
    }
    if (m_Course.daysOfWeek != NULL)
    {
        m_Course.daysOfWeek->clear();
        delete m_Course.daysOfWeek;
    }
    if (m_CourseRollCallRecord.tags != NULL)
    {
        m_CourseRollCallRecord.tags->clear();
        delete m_CourseRollCallRecord.tags;
    }
    mLoRaGateway.channels.clear();
    mGAESurveyResultData.results.clear();

    clear_UserStruct(&m_UserData);

    if (m_UserSubGroup.accountidList != NULL)
    {
        m_UserSubGroup.accountidList->clear();
        delete m_UserSubGroup.accountidList;
    }
    if (m_UserSubGroup.useridList != NULL)
    {
        m_UserSubGroup.useridList->clear();
        delete m_UserSubGroup.useridList;
    }
    if (m_UserMainGroup.accountidList != NULL)
    {
        m_UserMainGroup.accountidList->clear();
        delete m_UserMainGroup.accountidList;
    }
    if (m_UserMainGroup.useridList != NULL)
    {
        m_UserMainGroup.useridList->clear();
        delete m_UserMainGroup.useridList;
    }

    mCARDRollCallSubGroupEvent.userids.clear();
    mCARDRollCallSubGroupEvent.presentuserids.clear();

    mAssistEvent.eventtime.clear();
    mAssistEvent.validdate.clear();
    mAssistEvent.invaliddate.clear();

    mAssistEventRecord.results.clear();

    m_FireFightingInfo.StandbyDeviceIds->clear();
    m_FireFightingInfo.NFCDeviceIdList->clear();

    delete m_FireFightingInfo.StandbyDeviceIds;
    delete m_FireFightingInfo.NFCDeviceIdList;
}

void WorkItem::init()
{
    m_Sectiongroup.nodeIDList = NULL;
    m_Course.daysOfWeek = NULL;
    m_CourseRollCallRecord.tags = NULL;

    m_UserData.maingroups = NULL;
    m_UserData.datagroups = NULL;
    m_UserData.subgroups = NULL;
    m_UserData.rfidcards = NULL;
    m_UserData.nfccards = NULL;
    m_UserData.uwbcards = NULL;
    m_UserData.sensors = NULL;

    m_UserSubGroup.accountidList = NULL;
    m_UserSubGroup.useridList = NULL;
    m_UserMainGroup.accountidList = NULL;
    m_UserMainGroup.useridList = NULL;

    m_FireFightingInfo.StandbyDeviceIds = new std::list<std::string>();
    m_FireFightingInfo.NFCDeviceIdList = new std::list<int>();
}

void WorkItem::setNodeId(const char* nodeid)
{
    m_nodeid = nodeid;
}

void WorkItem::setMacaddress(const char* macaddress)
{
    m_macaddress = macaddress;
}

void WorkItem::setRecorderID(std::string recorderid)
{
    m_recorderid = recorderid;
}

void WorkItem::setWeight(std::string weight)
{
    m_weight = weight;
}

void WorkItem::setHeartBeat(std::string heartbeat)
{
    m_heartbeat = heartbeat;
}

void WorkItem::setBloodSugar(std::string bloodsugar)
{
    m_bloodsugar = bloodsugar;
}

void WorkItem::setSystolicBloodPressure(std::string systolicbloodpressure)
{
    m_systolicbloodpressure = systolicbloodpressure;
}

void WorkItem::setDiastolicBloodPressure(std::string diastolicbloodpressure)
{
    m_diastolicbloodpressure = diastolicbloodpressure;
}

void WorkItem::setBodyTemperature(std::string bodytemperature)
{
    m_bodytemperature = bodytemperature;
}

void WorkItem::setBodyFat(std::string bodyfat)
{
    m_bodyfat = bodyfat;
}

void WorkItem::setBloodOxygen(std::string bloodoxygen)
{
    m_bloodoxygen = bloodoxygen;
}

void WorkItem::setDateTime(std::string datetime)
{
    m_datetime = datetime;
}

void WorkItem::setMaingroupid(int maingroupid)
{
    m_maingroupid = maingroupid;
}

void WorkItem::setMaingroups(std::string maingroups)
{
    m_maingroups = maingroups;
}

void WorkItem::setStatus(std::string status)
{
    m_status = status;
}

void WorkItem::setMaplayer(int maplayer)
{
    m_maplayer = maplayer;
}

void WorkItem::setRole(int role)
{
    m_role = role;
}

void WorkItem::setAccountId(std::string accountid)
{
    m_accountid = accountid;
}

void WorkItem::setUserId(int userid)
{
    m_userid = userid;
}

void WorkItem::setUserPwd(std::string pwd)
{
    m_pwd = pwd;
}

void WorkItem::setRFIDCards(std::string rfidcards)
{
    m_rfidcards = rfidcards;
}

void WorkItem::setNFCCards(std::string nfccards)
{
    m_nfccards = nfccards;
}

void WorkItem::setUWBCards(std::string uwbcards)
{
    m_uwbcards = uwbcards;
}

void WorkItem::setLongitudeLatitude(std::string Longitude, std::string Latitude)
{
    m_Longitude = Longitude;
    m_Latitude = Latitude;
}

void WorkItem::setPosition(double posX, double posY, double posZ)
{
    m_posX = posX;
    m_posY = posY;
    m_posZ = posZ;
}

const char* WorkItem::getType()
{
    return m_type.c_str();
}

int WorkItem::getNodeType()
{
    return m_NodeType;
}

int WorkItem::getProjectId()
{
    return m_project_id;
}

int WorkItem::getTAFID()
{
    return m_tafid;
}

int WorkItem::getBuildingId()
{
    return m_buildingid;
}

int WorkItem::getFloor()
{
    return m_floor;
}

int WorkItem::getMaplayer()
{
    return m_maplayer;
}

const char* WorkItem::getMacaddress()
{
    return m_macaddress.c_str();
}

const char* WorkItem::getNodeId()
{
    return m_nodeid.c_str();
}

double WorkItem::getVoltage()
{
    return m_voltage;
}

double WorkItem::getTemperature()
{
    return m_temperature;
}

int WorkItem::getCount()
{
    return m_count;
}

double WorkItem::getPosX()
{
    return m_posX;
}

double WorkItem::getPosY()
{
    return m_posY;
}

double WorkItem::getPosZ()
{
    return m_posZ;
}

double WorkItem::getAccelX()
{
    return m_accelX;
}

double WorkItem::getAccelY()
{
    return m_accelY;
}

double WorkItem::getAccelZ()
{
    return m_accelZ;
}

double WorkItem::getGyroX()
{
    return m_gyroX;
}

double WorkItem::getGyroY()
{
    return m_gyroY;
}

double WorkItem::getGyroZ()
{
    return m_gyroZ;
}

const char* WorkItem::getNodeName()
{
    return m_nodename.c_str();
}

const char* WorkItem::getAction()
{
    return m_action.c_str();
}

const char* WorkItem::getAreaName()
{
    return m_area_name.c_str();
}

const char* WorkItem::getAreaId()
{
    return m_area_id.c_str();
}

const char* WorkItem::getJsonData()
{
    return m_jsondata.c_str();
}

const char* WorkItem::getAnchorids()
{
    return m_anchorids.c_str();
}

const char* WorkItem::getIssueTime()
{
    return m_issuetime.c_str();
}

int WorkItem::getRange()
{
    return m_range;
}

const char* WorkItem::getSerno()
{
    return m_serno.c_str();
}

const char* WorkItem::getempid()
{
    return m_empid.c_str();
}

std::string WorkItem::getRecorderID()
{
    return m_recorderid;
}

std::string WorkItem::getWeight()
{
    return m_weight;
}

std::string WorkItem::getHeartBeat()
{
    return m_heartbeat;
}

std::string WorkItem::getBloodSugar()
{
    return m_bloodsugar;
}

std::string WorkItem::getSystolicBloodPressure()
{
    return m_systolicbloodpressure;
}

std::string WorkItem::getDiastolicBloodPressure()
{
    return m_diastolicbloodpressure;
}

std::string WorkItem::getBodyTemperature()
{
    return m_bodytemperature;
}

std::string WorkItem::getBodyFat()
{
    return m_bodyfat;
}

std::string WorkItem::getBloodOxygen()
{
    return m_bloodoxygen;
}

std::string WorkItem::getDateTime()
{
    return m_datetime;
}

int WorkItem::getRole()
{
    return m_role;
}

std::string WorkItem::getAccountId()
{
    return m_accountid;
}

int WorkItem::getUserId()
{
    return m_userid;
}

std::string WorkItem::getUserPwd()
{
    return m_pwd;
}

std::string WorkItem::getRFIDCards()
{
    return m_rfidcards;
}

std::string WorkItem::getNFCCards()
{
    return m_nfccards;
}

std::string WorkItem::getUWBCards()
{
    return m_uwbcards;
}

std::string WorkItem::getLongitude()
{
    return m_Longitude;
}

std::string WorkItem::getLatitude()
{
    return m_Latitude;
}

int WorkItem::getMaingroupid()
{
    return m_maingroupid;
}

std::string WorkItem::getMaingroups()
{
    return m_maingroups;
}

std::string WorkItem::getStatus()
{
    return m_status;
}
