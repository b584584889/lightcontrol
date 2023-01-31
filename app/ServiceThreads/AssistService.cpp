/*
 *  AssistService.cpp
 *
 *  Created on: 2020/09/30
 *      Author: ikki
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <math.h>
#include <algorithm>    // std::max

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include "AssistService.h"

using namespace std;

AssistService::AssistService(wqueue<NotificationItem*> & NotifiactionQueue, wqueue<DataItem*> & AssistJobQueue, wqueue<WorkItem*> & MySQLJobQueue, DBManager * dbManager)
    : mNotifiactionQueue(NotifiactionQueue), mAssistJobQueue(AssistJobQueue), mMySQLJobQueue(MySQLJobQueue), mDbManager(dbManager)
{
}

AssistService::~AssistService()
{
}

bool AssistService::status()
{
    return mInitial;
}

std::string AssistService::statusString()
{
    return mStatus;
}

void * AssistService::run()
{
    while(1)
    {
        try
        {
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("AssistService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "AssistService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void AssistService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long checkAssistTime = 50000;
    long long reloadAssistTime = 0;

    mInitial = true;
    mAssistRunning = false;

    mStatus = "AssistService()::run() Initial Courses successfully!";
    vilslog::printf ("AssistService()::run() Initial Courses successfully!\n");

    initProjectAssistInfo();

    while (1)
    {
        DataItem * item = mAssistJobQueue.removetimed(1000);
        if( item != NULL )
        {
            processDataItem(item);
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        checkAssistTime += difft;
        reloadAssistTime += difft;

        if ( checkAssistTime >= 60000) // 60 seconds
        {
            CheckProjectAssistEvent();
            checkAssistTime = 0;
        }

        if ( reloadAssistTime >= 600000 ) // 600 seconds
        {
            if ( !mAssistRunning )
            {
                reloadAssistList();
            }
            reloadAssistTime = 0;
        }

        start1 = end1;
    }
}

void AssistService::initProjectAssistInfo()
{
    clear_ProjectAssistInfoStruct_vector(&mProjectAssistInfoVector);

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        ProjectAssistInfoStruct * newInfo = new ProjectAssistInfoStruct();
        newInfo->projectid = projectid;
        newInfo->sendNotifyTime = baseDb->get_sys_sendNotifyTime(1); // 1200 -> 20 hours
        newInfo->mAssistList = baseDb->read_Assists();
        newInfo->mAssistEventList = baseDb->read_AssistEvents();

        vilslog::printf("AssistService::initProjectAssistInfo() newInfo->mAssistList->size()[%d]\n", (int)newInfo->mAssistList->size());
        vilslog::printf("AssistService::initProjectAssistInfo() newInfo->mAssistEventList->size()[%d]\n", (int)newInfo->mAssistEventList->size());

        mProjectAssistInfoVector.push_back(newInfo);
    }

    prjidList->clear();
    delete prjidList;

}

int AssistService::processDataItem(DataItem * dataItem)
{
    if ( strcmp(dataItem->getType(), "updateAssist") == 0 )
    {
        // vilslog::printf("AssistService::processDataItem() updateAssist\n");

        reloadAssistList();
    }
    return 0;
}

void AssistService::CheckProjectAssistEvent()
{
    int intime_count = 0;
    std::string todayDate = TodayDateType1();
    int todayWeekday = TodayDateWeekday();

    for (std::vector<ProjectAssistInfoStruct*>::iterator i = mProjectAssistInfoVector.begin(); i != mProjectAssistInfoVector.end(); i++)
    {
        for (std::list<AssistEventStruct*>::iterator event = (*i)->mAssistEventList->begin(); event != (*i)->mAssistEventList->end(); event++)
        {
            bool todayNeedAssist = CheckTodayNeedAssist((*event), todayDate, todayWeekday);
            if (todayNeedAssist)
            {
                int ret = CheckAssistEvent((*i), (*event), todayDate);
                if ( ret == 2 )
                {
                    intime_count++;
                }
            }
        }
    }
}

bool AssistService::CheckTodayNeedAssist(AssistEventStruct * event, std::string todayDate, int todayWeekday)
{
    if ( todayDate.compare(event->deadline) > 0 )
    {
        // vilslog::printf("CheckTodayNeedAssist() event->deadline[%s] todayDate[%s]\n", event->deadline.c_str(), todayDate.c_str());
        return false;
    }

    for (std::list<int>::iterator weekday = event->weekday.begin(); weekday != event->weekday.end(); weekday++)
    {
        if ( (*weekday) == todayWeekday)
        {
            //
            // 檢查 是否排除 今天
            //
            for (std::list<std::string>::iterator invaliddate = event->invaliddate.begin(); invaliddate != event->invaliddate.end(); invaliddate++)
            {
                if ( (*invaliddate).compare(todayDate) == 0 )
                {
                    // 找到 排除 日期
                    // vilslog::printf("CheckTodayNeedAssist() todayNeedAssist[%d] invaliddate[%s] todayDate[%s]\n", false, (*invaliddate).c_str(), todayDate.c_str());
                    return false;
                }
            }
            // 找不到 排除 日期
            // vilslog::printf("CheckTodayNeedAssist() todayNeedAssist[%d] todayDate[%s]\n", true, todayDate.c_str());
            return true;
        }
    }

    //
    // 檢查是否 需要執行
    //
    for (std::list<std::string>::iterator validdate = event->validdate.begin(); validdate != event->validdate.end(); validdate++)
    {
        if ( (*validdate).compare(todayDate) == 0 )
        {
            // 找到 需要執行 日期
            // vilslog::printf("CheckTodayNeedAssist() todayNeedAssist[%d] validdate[%s] todayDate[%s]\n", true, (*validdate).c_str(), todayDate.c_str());
            return true;
        }
    }

    // vilslog::printf("CheckTodayNeedAssist() todayNeedAssist[%d] todayDate[%s]\n", false, todayDate.c_str());
    return false;
}

int AssistService::CheckAssistEvent(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate)
{
    int ret = 0;
    time_t t_Current = time(0);

    // vilslog::printf("CheckAssistEvent() description[%s] eventtime.size()[%d]\n",
    //     event->description.c_str(), event->eventtime.size());

    for (std::list<std::string>::iterator eventtime = event->eventtime.begin(); eventtime != event->eventtime.end(); eventtime++)
    {
        time_t t_Start = StringToInspectionDatetime((*eventtime));

        if (t_Current < t_Start)
        {
            // in check time interval
            time_t t_diff = t_Start - t_Current;
            int diff_min = ((int)t_diff/60);

            // vilslog::printf("CheckAssistEvent() [not yet] description[%s] eventtime[%s] t_Start[%lu] diff_min[%d]\n",
            //     event->description.c_str(), (*eventtime).c_str(), t_Start, diff_min);

            //if ( diff_min <= 480 ) // 8 hours
            if ( diff_min <= prjAssistInfo->sendNotifyTime ) // 1200 -> 20 hours
            {
                // send assist event
                check_assist_event(prjAssistInfo, event, todayDate, (*eventtime), true);
            }

            ret = 1;
        }
        else
        if (t_Current >= t_Start)
        {
            // check excess time event
            // time_t t_diff = t_Current - t_Start;
            // vilslog::printf("CheckAssistEvent() [excess] description[%s] eventtime[%s] t_Start[%lu]\n",
            //     event->description.c_str(), (*eventtime).c_str(), t_Start);

            check_assist_event(prjAssistInfo, event, todayDate, (*eventtime), false);

            ret = 2;
        }
    }

    return ret;
}

int AssistService::check_assist_event(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate, std::string eventtime, bool createEvent)
{
    int ret = 0;

    //
    // check if send event
    //
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(prjAssistInfo->projectid);
    if (baseDb == NULL)
        return ret;

    AssistEventRecordStruct * assisteventrecord = baseDb->get_AssistEventRecord(event->assistid, event->id, todayDate, eventtime);
    if (assisteventrecord->PushNotifyMessageRecorddbId > 0)
    {
        // vilslog::printf("AssistService::create_assist_event() alread sent!! assistid[%d] assisteventid[%d] PushNotifyMessageRecorddbId[%d] todayDate[%s] eventtime[%s]\n",
        //     assisteventrecord->assistid, assisteventrecord->id, assisteventrecord->PushNotifyMessageRecorddbId, todayDate.c_str(), eventtime.c_str());

        check_assist_result(prjAssistInfo, event, todayDate, eventtime, assisteventrecord);
    }
    else
    if (createEvent)
    {
        create_assist_event(prjAssistInfo, event, todayDate, eventtime, assisteventrecord);
    }

    assisteventrecord->results.clear();
    delete assisteventrecord;
    return ret;
}

int AssistService::check_assist_result(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate, std::string eventtime, AssistEventRecordStruct * assisteventrecord)
{
    int ret = 0;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(prjAssistInfo->projectid);
    if (baseDb == NULL)
        return ret;

    // vilslog::printf("AssistService::check_assist_result() assistid[%d] assisteventid[%d] PushNotifyMessageRecorddbId[%d] todayDate[%s] eventtime[%s]\n",
    //     assisteventrecord->assistid, assisteventrecord->id, assisteventrecord->PushNotifyMessageRecorddbId, todayDate.c_str(), eventtime.c_str());

    //
    // 檢查 SurveyEventRecord 紀錄
    //
    SurveyEventRecordStruct * surveyevent = baseDb->get_SurveyEventRecord(assisteventrecord->PushNotifyMessageRecorddbId);

    if ( surveyevent != NULL )
    {
        // vilslog::printf("AssistService::check_assist_result() surveyevent != NULL PushNotifyMessageRecorddbId[%d]\n",
        //     surveyevent->PushNotifyMessageRecorddbId);

        GAESurveyResultData * surveyresult = baseDb->get_SurveyResult(surveyevent->hashkey, assisteventrecord->messagehashkey);

        if (surveyresult->hashkey.length() > 0 && surveyresult->messagehashkey.length() > 0)
        {
            // vilslog::printf("AssistService::check_assist_result() Found surveyresult!!! messagehashkey[%s] surveyresult->results.size()[%d] assisteventrecord->results.size()[%d]\n",
            //     surveyresult->messagehashkey.c_str(), (int)surveyresult->results.size(), (int)assisteventrecord->results.size());

            //
            // save result
            //
            bool bNewResult = false;
            std::string surveyresult_result = convertListToString(&surveyresult->results);
            std::string assisteventrecord_result = convertListToString(&assisteventrecord->results);


            // vilslog::printf("AssistService::check_assist_result() surveyresult_result[%s] assisteventrecord_result[%s]\n",
            //     surveyresult_result.c_str(), assisteventrecord_result.c_str());

            if ( surveyresult_result.compare(assisteventrecord_result) != 0 )
            {
                bNewResult = true;
            }

            if (bNewResult)
            {
                //
                // new result
                //
                vilslog::printf("AssistService::check_assist_result() Found new surveyresult!!! messagehashkey[%s] surveyresult->results.size()[%d] assisteventrecord->results.size()[%d]\n",
                    surveyresult->messagehashkey.c_str(), (int)surveyresult->results.size(), (int)assisteventrecord->results.size());

                int index = 0;
                std::string report_status = "未完成";
                assisteventrecord->results.clear();
                for (std::list<std::string>::iterator surveyres = surveyresult->results.begin(); surveyres != surveyresult->results.end(); surveyres++)
                {
                    assisteventrecord->results.push_back((*surveyres));

                    if ( index == 0 )
                    {
                        if ( (*surveyres).compare("1") == 0 )
                        {
                            report_status = "已完成";
                        }
                    }
                    index++;
                }

                //
                // 取得 負責人
                //
                TargetTopicStruct * targettopic = baseDb->get_TargetTopicFromTopic(surveyresult->topic);
                assisteventrecord->userid = targettopic->userid;
                // assisteventrecord->accountid = targettopic->accountid;

                vilslog::printf("AssistService::check_assist_result() Found new surveyresult!!!surveyresult->topic[%s] userid[%d]\n",
                    surveyresult->topic.c_str(), assisteventrecord->userid);


                WorkItem * workitem = new WorkItem("updateAssistEventRecordStruct", prjAssistInfo->projectid, assisteventrecord);
                mMySQLJobQueue.add(workitem);

                delete targettopic;

                AssistStruct * assistinfo = NULL;
                for (std::list<AssistStruct*>::iterator assist = prjAssistInfo->mAssistList->begin(); assist != prjAssistInfo->mAssistList->end(); assist++)
                {
                    if ((*assist)->id == event->assistid)
                    {
                        assistinfo = (*assist);
                        break;
                    }
                }

                //
                // 負責人 上傳照片
                //
                std::list<std::string> * imageids = baseDb->getStorageImagesFromMessagehashkey(assisteventrecord->messagehashkey);
                std::string report_imageids = convertListToString(imageids);
                imageids->clear();
                delete imageids;

                std::string username = "";
                std::string userid = "0";
                std::string accountid = "";
                std::string maingroups = "";
                UserStruct * user = baseDb->getUserByUserId(event->userid);
                if (user != NULL)
                {
                    username = user->name;
                    accountid = user->account;
                    userid = to_string(user->userid);
                    maingroups = convert_list_to_string(user->maingroups);
                }

                //
                // 負責人 上傳回應
                //
                std::string report_comment = surveyresult->comment;
                if (report_comment.length() == 0 )
                    report_comment = "無";

                std::ostringstream sendstream;
                std::string title = "協助事項狀況回報";

                time_t t_eventtime = StringToInspectionDatetime(eventtime);
                std::list<std::string> * sendtargetlist = new std::list<std::string>();
                std::list<std::string> targetlist;
                sendtargetlist->push_back(userid);
                targetlist.push_back(userid);

                sendstream << "人員:"<<username<<endl;
                sendstream << "申請者:"<<event->applyusername<<"/"<<event->applyuserrelation<<endl;
                sendstream << "申請時間:"<<event->datetime<<endl;
                sendstream << "預約時間:"<<time_tToString(t_eventtime)<< endl;
                sendstream << "事項名稱:"<<assistinfo->title<<endl;
                sendstream << "申請說明:"<<assistinfo->description<<endl;
                sendstream << "回報狀況:"<<report_status<<endl;
                sendstream << "回報說明:"<<report_comment<<endl;

                std::string context = sendstream.str();

                // console.log("AssistService::check_assist_result() context[%s]\n", context.c_str());

                // ===========================================
                // 傳送給 申請人員
                std::string randomKey1 = random_string(16);
                std::list<std::string> subgrouplist;
                std::list<std::string> surveylist;
                std::list<std::string> deadlinelist;
                NotificationItem * notifactionitem1 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_206, prjAssistInfo->projectid, sendtargetlist, targetlist, subgrouplist, surveylist, deadlinelist, accountid, userid, maingroups, title, context, randomKey1);
                notifactionitem1->m_imageuid = report_imageids;
                notifactionitem1->m_formid = to_string(event->id);
                notifactionitem1->m_eventday = todayDate;
                notifactionitem1->m_eventtime = eventtime;
                mNotifiactionQueue.add(notifactionitem1);
                // ===========================================

                targetlist.clear();
                if (sendtargetlist != NULL)
                {
                    sendtargetlist->clear();
                    delete sendtargetlist;
                }
            }
        }
        // else
        // {
        //     vilslog::printf("\n\nAssistService::check_assist_result() no surveyresult!!!\n\n");
        // }

        surveyresult->results.clear();
        delete surveyresult;
        delete surveyevent;
    }

    return ret;
}

int AssistService::create_assist_event(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate, std::string eventtime, AssistEventRecordStruct * assisteventrecord)
{
    int ret = 0;

    //
    // check if send event
    //
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(prjAssistInfo->projectid);
    if (baseDb == NULL)
        return ret;

    if (assisteventrecord->PushNotifyMessageRecorddbId < 0)
    {

        // create new one
        vilslog::printf("AssistService::create_assist_event() [create new one] assistid[%d] assisteventid[%d] todayDate[%s] eventtime[%s]\n",
            event->assistid, event->id, todayDate.c_str(), eventtime.c_str());

        if (prjAssistInfo->mAssistList != NULL)
        {
            // 找出 負責人員
            AssistStruct * assistinfo = NULL;
            std::list<std::string> * sendtargetlist = NULL;
            std::list<std::string> targetlist;
            std::string username = "";
            std::string userid = "0";
            std::string accountid = "";
            std::string maingroups = "";
            std::string assignusers = "";

            for (std::list<AssistStruct*>::iterator assist = prjAssistInfo->mAssistList->begin(); assist != prjAssistInfo->mAssistList->end(); assist++)
            {
                if ((*assist)->id == event->assistid)
                {
                    assistinfo = (*assist);
                    assignusers = (*assist)->assignusers;
                    break;
                }
            }

            UserStruct * user = baseDb->getUserByUserId(event->userid);
            if (user != NULL)
            {
                username = user->name;
                accountid = user->account;
                userid = to_string(user->userid);
                maingroups = convert_list_to_string(user->maingroups);
            }

            if ( assignusers.length() == 0 )
            {
                //
                // 推播給子集合管理者
                //
                sendtargetlist = getSubgGroupManager(prjAssistInfo->projectid, user);

                if (user != NULL)
                {
                    clear_UserStruct(user);
                    delete user;
                }
            }
            else
            {
                //
                // 推播給指定人員
                //
                sendtargetlist = convert_string_to_list(assignusers, ',');
            }
            for (std::list<std::string>::iterator target = sendtargetlist->begin(); target != sendtargetlist->end(); target++)
            {
                targetlist.push_back((*target));
            }

            std::ostringstream sendstream;
            std::string title = "預約協助事項";

            time_t t_eventtime = StringToInspectionDatetime(eventtime);

            sendstream << "人員:"<<username<<endl;
            sendstream << "申請者:"<<event->applyusername<<"/"<<event->applyuserrelation<<endl;
            sendstream << "申請時間:"<<event->datetime<<endl;
            sendstream << "預約時間:"<<time_tToString(t_eventtime)<< endl;
            sendstream << "事項名稱:"<<assistinfo->title<<endl;
            sendstream << "申請說明:"<<assistinfo->description<<endl;

            std::string context = sendstream.str();

            vilslog::printf("AssistService::create_assist_event() context[%s]\n", context.c_str());

            if (sendtargetlist != NULL)
            {
                // ===========================================
                // 新增紀錄
                if (assisteventrecord->id < 0)
                {
                    AssistEventRecordStruct * newRecord = new AssistEventRecordStruct();
                    newRecord->assistid = event->assistid;
                    newRecord->assisteventid = event->id;
                    newRecord->userid = 0;
                    newRecord->PushNotifyMessageRecorddbId = -1;
                    newRecord->messagehashkey = "";
                    newRecord->maingroups = maingroups;
                    newRecord->eventday = todayDate;
                    newRecord->eventtime = eventtime;

                    WorkItem * workitem = new WorkItem("createAssistEventRecordStruct", prjAssistInfo->projectid, newRecord);
                    mMySQLJobQueue.add(workitem);
                }
                // ===========================================


                // ===========================================
                // 傳送給 負責人員
                std::string randomKey1 = random_string(16);
                std::list<std::string> subgrouplist;
                std::list<std::string> surveylist;
                std::list<std::string> deadlinelist;
                std::string surveyid = ApplicationForm_ASSISTEVENT_ID;
                surveylist.push_back(surveyid);
                deadlinelist.push_back(currentDateTimeForSQL(-1));
                NotificationItem * notifactionitem1 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_204, prjAssistInfo->projectid, sendtargetlist, targetlist, subgrouplist, surveylist, deadlinelist, accountid, userid, maingroups, title, context, randomKey1);
                notifactionitem1->m_imageuid = event->imageids;
                notifactionitem1->m_formid = to_string(event->id);
                notifactionitem1->m_eventday = todayDate;
                notifactionitem1->m_eventtime = eventtime;
                mNotifiactionQueue.add(notifactionitem1);
                // ===========================================
            }

            targetlist.clear();
            if (sendtargetlist != NULL)
            {
                sendtargetlist->clear();
                delete sendtargetlist;
            }
        }

        ret = 1;
    }

    return ret;
}

void AssistService::reloadAssistList()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        bool bFound = false;
        for (std::vector<ProjectAssistInfoStruct*>::iterator i = mProjectAssistInfoVector.begin(); i != mProjectAssistInfoVector.end(); i++)
        {
            if ( (*i)->projectid == projectid )
            {
                clear_AssistInfoStruct((*i));

                (*i)->mAssistList = baseDb->read_Assists();
                (*i)->mAssistEventList = baseDb->read_AssistEvents();

                bFound = true;
                break;
            }
        }
        if (!bFound)
        {

            ProjectAssistInfoStruct * newInfo = new ProjectAssistInfoStruct();
            newInfo->projectid = projectid;
            //newInfo->sendNotifyTime = 1200; // 1200 -> 20 hours
            newInfo->sendNotifyTime = baseDb->get_sys_sendNotifyTime(1); // 1200 -> 20 hours
            newInfo->mAssistList = baseDb->read_Assists();
            newInfo->mAssistEventList = baseDb->read_AssistEvents();

            mProjectAssistInfoVector.push_back(newInfo);
        }
    }

    prjidList->clear();
    delete prjidList;
}

void AssistService::clear_ProjectAssistInfoStruct_vector(std::vector<ProjectAssistInfoStruct*> * plist)
{
    for (std::vector<ProjectAssistInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_AssistInfoStruct((*i));
        delete (*i);
    }
    plist->clear();
}

void AssistService::clear_AssistInfoStruct(ProjectAssistInfoStruct * info)
{
    if (info->mAssistList != NULL)
    {
        info->mAssistList->clear();
        delete info->mAssistList;
        info->mAssistList = NULL;
    }
    if (info->mAssistEventList != NULL)
    {
        clear_AssistEventStruct_list(info->mAssistEventList);
        info->mAssistEventList->clear();
        delete info->mAssistEventList;
        info->mAssistEventList = NULL;
    }
}

void AssistService::clear_AssistEventStruct_list(std::list<AssistEventStruct*> * plist)
{
    for (std::list<AssistEventStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->eventtime.clear();
        (*i)->weekday.clear();
        (*i)->validdate.clear();
        (*i)->invaliddate.clear();
        delete (*i);
    }
    plist->clear();
}


std::list<std::string> * AssistService::getSubgGroupManager(int projectid, UserStruct * user)
{
    std::list<std::string> * targetlist = new std::list<std::string>();

    if (user == NULL)
    {
        return targetlist;
    }

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    std::list<UserSubGroupStruct*> * userSubGroupList = NULL;
    if (baseDb != NULL)
    {
    	userSubGroupList = baseDb->get_UserSubGroups();

        for (std::list<std::string>::iterator userSubGroupid = user->subgroups->begin(); userSubGroupid != user->subgroups->end(); userSubGroupid++)
        {
            int curUserGroupid = convert_string_to_int((*userSubGroupid));

            for (std::list<UserSubGroupStruct *>::iterator userSubGroup = userSubGroupList->begin(); userSubGroup != userSubGroupList->end(); userSubGroup++)
            {
                if( (*userSubGroup)->subgroupid == curUserGroupid )
                {
                    // 找到 子集合管理者
                    //for (std::list<std::string>::iterator accountid_it = (*userSubGroup)->accountidList->begin(); accountid_it != (*userSubGroup)->accountidList->end(); accountid_it++)
                    for (std::list<std::string>::iterator userid_it = (*userSubGroup)->useridList->begin(); userid_it != (*userSubGroup)->useridList->end(); userid_it++)
                    {
                        //
                        // 不重複加入
                        //
                        bool bExisted = false;
                        for (std::list<std::string>::iterator userid_existed = targetlist->begin(); userid_existed != targetlist->end(); userid_existed++)
                        {
                            if ( (*userid_it).compare((*userid_existed)) == 0 )
                            {
                                bExisted = true;
                                break;
                            }
                        }
                        if (!bExisted)
                        {
                            targetlist->push_back((*userid_it));
                        }
                    }
                    break;
                }
            }
        }
    }

    if (userSubGroupList != NULL)
    {
        clear_UserSubGroupStruct_list(userSubGroupList);
        delete userSubGroupList;
    }

    return targetlist;
}
