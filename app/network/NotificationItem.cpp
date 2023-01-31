/*
 *  NotificationItem.cpp
 *
 *  Created on: 2019/08/02
 *      Author: ikki
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include "string.h"
 #include <iostream>
 #include <sstream>

#include "NotificationItem.h"

// updatePushNotify
NotificationItem::NotificationItem(std::string type, int projectID, std::string account, std::string userid, std::string maingroups)
    : m_type(type), m_projectID(projectID), m_accountid(account), m_userid(userid), m_maingroups(maingroups)
{

}

NotificationItem::NotificationItem(int functiontype, int projectID, int nodetype, std::string nodeid, std::string nodename, std::string userid, std::string maingroups, std::string macaddress, TriggerPushMessageStruct * triggerData)
    : m_functiontype(functiontype), m_projectID(projectID), m_nodetype(nodetype), m_nodeid(nodeid), m_nodename(nodename), m_userid(userid), m_maingroups(maingroups), m_macaddress(macaddress)
{
    mTriggerData.enable = triggerData->enable;
    mTriggerData.nodeid = triggerData->nodeid;
    mTriggerData.targettype = triggerData->targettype;
    mTriggerData.distance = triggerData->distance;
    mTriggerData.title = triggerData->title;
    mTriggerData.message = triggerData->message;
}

NotificationItem::NotificationItem(int functiontype, int projectID, int nodetype, std::string nodeid, std::string nodename, std::string userid, std::string maingroups, std::string macaddress, std::string title, std::string message)
    : m_functiontype(functiontype), m_projectID(projectID), m_nodetype(nodetype), m_nodeid(nodeid), m_nodename(nodename), m_userid(userid), m_maingroups(maingroups), m_macaddress(macaddress), m_title(title), m_message(message)
{

}

NotificationItem::NotificationItem(int functiontype, int projectID, int nodetype, std::string nodeid, std::string nodename, std::string userid, std::string maingroups, std::string macaddress, std::string title, std::string message, std::string waitkey)
    : m_functiontype(functiontype), m_projectID(projectID), m_nodetype(nodetype), m_nodeid(nodeid), m_nodename(nodename), m_userid(userid), m_maingroups(maingroups), m_macaddress(macaddress), m_title(title), m_message(message), m_waitkey(waitkey)
{

}

NotificationItem::NotificationItem(int functiontype, int projectID, std::list<std::string> targetlist, std::string accountid, std::string userid, std::string maingroups, std::string title, std::string message, std::string waitkey)
    : m_functiontype(functiontype), m_projectID(projectID), m_accountid(accountid), m_userid(userid), m_maingroups(maingroups), m_title(title), m_message(message), m_waitkey(waitkey)
{
    for (std::list<std::string>::iterator target = targetlist.begin(); target != targetlist.end(); target++)
        m_sendtargetlist.push_back((*target));
    for (std::list<std::string>::iterator target = targetlist.begin(); target != targetlist.end(); target++)
        m_targetlist.push_back((*target));
}

NotificationItem::NotificationItem(int functiontype, int projectID, std::list<std::string> * sendtargetlist, std::list<std::string> targetlist, std::list<std::string> subgrouplist, std::list<std::string> surveylist, std::list<std::string> deadlinelist, std::string accountid, std::string userid, std::string maingroups, std::string title, std::string message, std::string waitkey)
    : m_functiontype(functiontype), m_projectID(projectID), m_accountid(accountid), m_userid(userid), m_maingroups(maingroups), m_title(title), m_message(message), m_waitkey(waitkey)
{
    for (std::list<std::string>::iterator target = sendtargetlist->begin(); target != sendtargetlist->end(); target++)
        m_sendtargetlist.push_back((*target));
    for (std::list<std::string>::iterator target = targetlist.begin(); target != targetlist.end(); target++)
        m_targetlist.push_back((*target));
    for (std::list<std::string>::iterator subgroup = subgrouplist.begin(); subgroup != subgrouplist.end(); subgroup++)
        m_subgrouplist.push_back((*subgroup));
    for (std::list<std::string>::iterator survey = surveylist.begin(); survey != surveylist.end(); survey++)
        m_surveylist.push_back((*survey));
    for (std::list<std::string>::iterator deadline = deadlinelist.begin(); deadline != deadlinelist.end(); deadline++)
        m_deadlinelist.push_back((*deadline));
}

NotificationItem::~NotificationItem()
{
    m_sendtargetlist.clear();
    m_targetlist.clear();
    m_subgrouplist.clear();
    m_surveylist.clear();
    m_deadlinelist.clear();
}
