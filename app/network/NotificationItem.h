/*
 * NotificationItem.h
 *
 *  Created on: 2019/08/02
 *      Author: ikki
 */
#ifndef __NotificationItem_h__
#define __NotificationItem_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../common.h"

using namespace std;

class NotificationItem
{
private:
public:
    NotificationItem(std::string type, int projectID, std::string account, std::string userid, std::string maingroups);
    // NotificationItem(int functiontype, int projectID, std::string title, std::string topic, std::string message);
    // NotificationItem(int functiontype, int projectID, std::string account, std::string topic, std::string functions, int dummy);
    NotificationItem(int functiontype, int projectID, int nodetype, std::string nodeid, std::string nodename, std::string userid, std::string maingroups, std::string macaddress, TriggerPushMessageStruct * triggerData);
    NotificationItem(int functiontype, int projectID, int nodetype, std::string nodeid, std::string nodename, std::string userid, std::string maingroups, std::string macaddress, std::string title, std::string message);
    NotificationItem(int functiontype, int projectID, int nodetype, std::string nodeid, std::string nodename, std::string userid, std::string maingroups, std::string macaddress, std::string title, std::string message, std::string waitkey);
    NotificationItem(int functiontype, int projectID, std::list<std::string> targetlist, std::string accountid, std::string userid, std::string maingroups, std::string title, std::string message, std::string waitkey);
    NotificationItem(int functiontype, int projectID, std::list<std::string> * sendtargetlist, std::list<std::string> targetlist, std::list<std::string> subgrouplist, std::list<std::string> surveylist, std::list<std::string> deadlinelist, std::string accountid, std::string userid, std::string maingroups, std::string title, std::string message, std::string waitkey);

    ~NotificationItem();
    std::string m_type;
    int m_functiontype;
    int m_projectID;
    int m_nodetype;

    std::string m_nodeid;
    std::string m_nodename;
    std::string m_maingroups;
    std::string m_macaddress;
    std::string m_accountid;
    std::string m_userid;
    std::string m_title;
    std::string m_topic;
    std::string m_message;
    std::string m_imageuid;
    std::string m_formid;

    // m_type = update_GAE_Topic
    std::string m_functions;
    std::string m_waitkey;

    TriggerPushMessageStruct mTriggerData;

    std::list<std::string> m_sendtargetlist;
    std::list<std::string> m_targetlist;
    std::list<std::string> m_subgrouplist;
    std::list<std::string> m_surveylist;
    std::list<std::string> m_deadlinelist;

    // for assisteventrecord
    std::string m_eventday;
    std::string m_eventtime;
};

#endif
