/*
 *  GAEService.h
 *
 *  Created on: 2020/04/20
 *      Author: ikki
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

#include "GAEService.h"

using namespace std;

GAEService::GAEService(wqueue<DataItem*> & GAEJobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mGAEJobQueue(GAEJobQueue), mMySQLJobQueue(MySQLJobQueue), mNotifiactionQueue(NotifiactionQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

GAEService::~GAEService()
{
}

bool GAEService::status()
{
    return mInitial;
}

std::string GAEService::statusString()
{
    return mStatus;
}

void * GAEService::run()
{
    while(1)
    {
        try
        {
            mSyncGAETopicInfo_time = 0;

            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("GAEService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "GAEService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void GAEService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long syncTime = 59000;
    //long long syncTopicTime = 59970000;
    long long syncTopicTime = 59990000;
    long long syncUserCardTime = 59960000;
    long long syncCardUserTime = 59950000;
    long long syncDILinkTime = 59980000;

    mInitial = true;

    mStatus = "GAEService()::run() Initial successfully!";
    vilslog::printf ("GAEService()::run() Initial successfully!\n");

    int projectid = 1;
    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    m_client = new WebSocketClient(vilssysConfig->VILSUID, mGAEJobQueue);
    m_client->start();

    vilslog::printf ("\n\n\nGAEService()::run() Initial successfully!\n\n\n");

    while (1)
    {
        DataItem * item = mGAEJobQueue.removetimed(1000);
        if( item != NULL )
        {
            processDataItem(item);
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        syncTime += difft;
        syncTopicTime += difft;
        syncUserCardTime += difft;
        syncCardUserTime += difft;
        syncDILinkTime += difft;

        //if ( syncTime >= 3600000) //  1 hour
        //if ( syncTime >= 600000) //  10 minutes
        if ( syncTime >= 60000) // 1 minute
        {
            std::string time = currentTimeForSQL();

            if (time.compare("00:00") == 0)
            {
                vilslog::printf ("GAEService()::run() time[%s] startSync()\n", time.c_str());

                SyncAllProject();
                updateGAEDIInfo();
            }
            syncTime = 0;
        }

        //if ( syncTopicTime >= 600000) //  10 minutes
        if ( syncTopicTime >= 60000000) //  600 minutes
        {
            // vilslog::printf ("GAEService()::run() updateGAETopicInfo()\n");

            updateGAETopicInfo();
            syncTopicTime = 0;
        }

        //if (syncUserCardTime >= 600000) //  10 minutes
        if ( syncUserCardTime >= 60000000) //  600 minutes
        {
            // vilslog::printf ("GAEService()::run() updateGAEUserCard()\n");

            updateGAEUserCard();
            syncUserCardTime = 0;
        }

        //if (syncCardUserTime >= 600000) //  10 minutes
        if ( syncCardUserTime >= 60000000) //  600 minutes
        {
            // vilslog::printf ("GAEService()::run() updateGAECardUser()\n");

            updateGAECardUser();
            syncCardUserTime = 0;
        }

        //if (syncDILinkTime >= 600000) //  10 minutes
        if ( syncDILinkTime >= 60000000) //  600 minutes
        {
            // vilslog::printf ("GAEService()::run() updateGAEDIInfo()\n");

            updateGAEDIInfo();
            syncDILinkTime = 0;
        }

        start1 = end1;
    }
}

int GAEService::processDataItem(DataItem * dataItem)
{
    if ( strcmp(dataItem->getType(), "updatesurveyresult") == 0 )
    {
        // vilslog::printf("GAEService::processDataItem() updatesurveyresult\n");

        SyncAllProject();
    }
    else
    if ( strcmp(dataItem->getType(), "GAESurveyResultData") == 0 )
    {
        // vilslog::printf("GAEService::processDataItem() GAESurveyResultData\n");

        int projectid = dataItem->getProjectID();

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            GAESurveyResultData * oldResult = baseDb->get_SurveyResult(dataItem->mGAESurveyResultData.hashkey, dataItem->mGAESurveyResultData.messagehashkey);

            std::list<SurveyEventRecordStruct*> * recordLeaveFormList = baseDb->get_LeaveForm_SurveyEventRecord();
            check_applicationform_leave(projectid, recordLeaveFormList, oldResult, &dataItem->mGAESurveyResultData);

            std::list<SurveyEventRecordStruct*> * recordAssistList = baseDb->get_Assist_SurveyEventRecords();
            check_applicationform_assist(projectid, recordAssistList, oldResult, &dataItem->mGAESurveyResultData);

            std::list<SurveyEventRecordStruct*> * recordCurriculumList = baseDb->get_Curriculum_SurveyEventRecords();
            check_applicationform_curriculum(projectid, recordCurriculumList, oldResult, &dataItem->mGAESurveyResultData);

#ifdef ENABLE_KIA_SERVICE
            check_KIA_WCInfo(projectid, oldResult, &dataItem->mGAESurveyResultData);
#endif

            WorkItem * work = new WorkItem("GAESurveyResultData", projectid, &dataItem->mGAESurveyResultData);
            mMySQLJobQueue.add(work);

            oldResult->results.clear();
            delete oldResult;
            clear_SurveyEventRecordStruct_list(recordLeaveFormList);
            delete recordLeaveFormList;
            clear_SurveyEventRecordStruct_list(recordAssistList);
            delete recordAssistList;
            clear_SurveyEventRecordStruct_list(recordCurriculumList);
            delete recordCurriculumList;
        }
    }
    else
    if ( strcmp(dataItem->getType(), "GAEMessageStatusData") == 0 )
    {
        // vilslog::printf("GAEService::processDataItem() GAEMessageStatusData\n");

        int projectid = dataItem->getProjectID();
        WorkItem * work = new WorkItem("GAEMessageStatusData", projectid, &dataItem->mGAEMessageStatusData);
        mMySQLJobQueue.add(work);
    }
    else
    if ( strcmp(dataItem->getType(), "updateGAEuserinfo") == 0 )
    {
        // vilslog::printf("GAEService::processDataItem() updateGAEuserinfo\n");

        int projectid = dataItem->getProjectID();
        int userid = dataItem->getUserId();

        if ( userid <= 0 )
        {
            updateGAETopicInfo(projectid);
        }
        else
        {
            updateGAEUserInfo(projectid, userid);
        }
    }
    else
    if ( strcmp(dataItem->getType(), "updateUserSubGroup") == 0 )
    {
        int projectid = dataItem->getProjectID();
        updateGAETopicInfo(projectid);
    }
    else
    if ( strcmp(dataItem->getType(), "updateUserMainGroup") == 0 )
    {
        int projectid = dataItem->getProjectID();
        updateGAETopicInfo(projectid);
    }

    return 0;
}

bool GAEService::SyncAllProject()
{
    SyncMessageStatus(1);
    SyncSurveyResult(1);
    // updateGAETopicInfo();
    return true;
}

int GAEService::SyncMessageStatus(int projectid)
{
    int count = 0;

    std::string FromDatetime = currentDateTimeForPushNotification(1);
    std::string ToDatetime = currentDateTimeForPushNotification(0);

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    char queryString[128];
    memset(queryString, 0, 128);
    sprintf(queryString, "fromdatetime=%s&todatetime=%s&vilsuid=%s", FromDatetime.c_str(), ToDatetime.c_str(), vilssysConfig->VILSUID.c_str());

    // vilslog::printf ("GAEService::SyncMessageStatus() queryString[%s]\n", queryString);

    std::string content = startSync(projectid, "getmessagestatus", queryString);

    // vilslog::printf ("GAEService::SyncMessageStatus() content[%s]\n", content.c_str());

    if ( content.length() == 0 )
    {
        return 0;
    }

    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    try{
        parsingSuccessful = reader->parse( content.c_str(), JSONresult);

        if (parsingSuccessful)
        {
            if ( JSONresult.isArray() )
            {
                //vilslog::printf ("GAEService::SyncMessageStatus() JSONresult.isArray()\n");

                for(int i=0;i<(int)JSONresult.size();i++)
                {
                    GAEMessageStatusData * msgStatus = new GAEMessageStatusData();
                    msgStatus->messageid = "";
                    msgStatus->topic = "";
                    msgStatus->messagehashkey = "";
                    msgStatus->token = "";
                    msgStatus->read = "0";
                    msgStatus->finished = "0";
                    msgStatus->datetime = "";

                    Json::Value result = JSONresult[i];

                    if ( result.isMember("messageid") )
                    {
                        //vilslog::printf ("GAEService::SyncMessageStatus() messageid[%s]\n", result["messageid"].asCString());
                        msgStatus->messageid = result["messageid"].asCString();
                    }

                    if ( result.isMember("messagehashkey") )
                    {
                        //vilslog::printf ("GAEService::SyncMessageStatus() messageid[%s]\n", result["messageid"].asCString());
                        msgStatus->messagehashkey = result["messagehashkey"].asCString();
                    }

                    if ( result.isMember("topic") )
                    {
                        //vilslog::printf ("GAEService::SyncMessageStatus() messageid[%s]\n", result["messageid"].asCString());
                        msgStatus->topic = result["topic"].asCString();
                    }

                    if ( result.isMember("token") )
                    {
                        //vilslog::printf ("GAEService::SyncMessageStatus() token[%s]\n", result["token"].asCString());
                        msgStatus->token = result["token"].asCString();
                    }

                    if ( result.isMember("read") )
                    {
                        //vilslog::printf ("GAEService::SyncMessageStatus() read[%s]\n", result["read"].asCString());
                        msgStatus->read = result["read"].asCString();
                    }

                    if ( result.isMember("finished") )
                    {
                        //vilslog::printf ("GAEService::SyncMessageStatus() finished[%s]\n", result["finished"].asCString());
                        msgStatus->finished = result["finished"].asCString();
                    }

                    if ( result.isMember("datetime") )
                    {
                        //vilslog::printf ("GAEService::SyncMessageStatus() datetime[%s]\n\n", result["datetime"].asCString());
                        msgStatus->datetime = result["datetime"].asCString();
                    }

                    WorkItem * work = new WorkItem("GAEMessageStatusData", projectid, msgStatus);
                    mMySQLJobQueue.add(work);

                    delete msgStatus;
                }
            }
            else
            {
                vilslog::printf ("GAEService::SyncMessageStatus() JSONresult.isArray() False!!\n");
            }
        }
        else
        {
            vilslog::printf ("GAEService::SyncMessageStatus() parsingSuccessful[%d]\n", parsingSuccessful);
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("GAEService::SyncMessageStatus() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;

    return count;
}

int GAEService::SyncSurveyResult(int projectid)
{
    std::list<GAESurveyResultData*> * surveyResultList = fetchSurveyResult(projectid);

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 0;

    std::list<SurveyEventRecordStruct*> * recordLeaveFormList = baseDb->get_LeaveForm_SurveyEventRecord();
    std::list<SurveyEventRecordStruct*> * recordAssistList = baseDb->get_Assist_SurveyEventRecords();
    std::list<SurveyEventRecordStruct*> * recordCurriculumList = baseDb->get_Curriculum_SurveyEventRecords();

    //
    // check levae form
    //
    for (std::list<GAESurveyResultData*>::iterator surveyData = surveyResultList->begin(); surveyData != surveyResultList->end(); surveyData++)
    {
        // 123
        GAESurveyResultData * oldResult = baseDb->get_SurveyResult((*surveyData)->hashkey, (*surveyData)->messagehashkey);

        check_applicationform_leave(projectid, recordLeaveFormList, oldResult, (*surveyData));

        check_applicationform_assist(projectid, recordAssistList, oldResult, (*surveyData));

        check_applicationform_curriculum(projectid, recordCurriculumList, oldResult, (*surveyData));

#ifdef ENABLE_KIA_SERVICE
        check_KIA_WCInfo(projectid, oldResult, (*surveyData));
#endif

        WorkItem * work = new WorkItem("GAESurveyResultData", projectid, (*surveyData));
        mMySQLJobQueue.add(work);

        oldResult->results.clear();
        delete oldResult;
    }

    clear_SurveyEventRecordStruct_list(recordLeaveFormList);
    delete recordLeaveFormList;

    clear_SurveyEventRecordStruct_list(recordAssistList);
    delete recordAssistList;

    clear_SurveyEventRecordStruct_list(recordCurriculumList);
    delete recordCurriculumList;

    clear_GAESurveyResultData_list(surveyResultList);
    delete surveyResultList;

    return 0;
}

std::list<GAESurveyResultData*> * GAEService::fetchSurveyResult(int projectid)
{
    std::list<GAESurveyResultData*> * surveyResultList = new std::list<GAESurveyResultData*>();

    std::string FromDatetime = currentDateTimeForPushNotification(1);
    std::string ToDatetime = currentDateTimeForPushNotification(0);

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    char queryString[128];
    memset(queryString, 0, 128);
    sprintf(queryString, "fromdatetime=%s&todatetime=%s&vilsuid=%s", FromDatetime.c_str(), ToDatetime.c_str(), vilssysConfig->VILSUID.c_str());

    // vilslog::printf ("GAEService::SyncSurveyResult() queryString[%s]\n", queryString);

    std::string content = startSync(projectid, "getsurveyresult", queryString);

    if ( content.length() == 0 )
    {
        return surveyResultList;
    }

    // vilslog::printf ("GAEService::SyncSurveyResult() content[%s]\n", content.c_str());

    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    try{
        parsingSuccessful = reader->parse( content.c_str(), JSONresult);

        if (parsingSuccessful)
        {
            if ( JSONresult.isArray() )
            {
                // vilslog::printf ("GAEService::SyncSurveyResult() JSONresult.isArray()\n");

                for(int i=0;i<(int)JSONresult.size();i++)
                {
                    GAESurveyResultData * surveyData = new GAESurveyResultData();

                    Json::Value result = JSONresult[i];

                    if ( result.isMember("topic") )
                    {
                        // vilslog::printf ("GAEService::SyncSurveyResult() topic[%s]\n", result["topic"].asCString());
                        surveyData->topic = result["topic"].asCString();
                    }

                    if ( result.isMember("messagehashkey") )
                    {
                        // vilslog::printf ("GAEService::SyncSurveyResult() messagehashkey[%s]\n", result["messagehashkey"].asCString());
                        surveyData->messagehashkey = result["messagehashkey"].asCString();
                    }

                    if ( result.isMember("hash") )
                    {
                        // vilslog::printf ("GAEService::SyncSurveyResult() hash[%s]\n", result["hash"].asCString());
                        surveyData->hashkey = result["hash"].asCString();
                    }

                    if ( result.isMember("token") )
                    {
                        // vilslog::printf ("GAEService::SyncSurveyResult() hash[%s]\n", result["hash"].asCString());
                        surveyData->token = result["token"].asCString();
                    }

                    if ( result.isMember("results") )
                    {
                        // vilslog::printf ("GAEService::SyncSurveyResult() result[results].isArray()[%d]\n", result["results"].isArray());
                        if ( result["results"].isArray() )
                        {
                            for(int i=0;i<(int)result["results"].size();i++)
                            {
                                Json::Value resultvalue = result["results"][i];
                                // vilslog::printf ("GAEService::SyncSurveyResult() results[%d][%s]\n", i, resultvalue.asCString());
                                surveyData->results.push_back(resultvalue.asString());
                            }
                        }
                    }

                    if ( result.isMember("comment") )
                    {
                        // vilslog::printf ("GAEService::SyncSurveyResult() comment[%s]\n", result["comment"].asCString());
                        surveyData->comment = result["comment"].asCString();
                    }

                    if ( result.isMember("datetime") )
                    {
                        // vilslog::printf ("GAEService::SyncSurveyResult() datetime[%s]\n\n", result["datetime"].asCString());
                        surveyData->datetime = result["datetime"].asCString();
                    }


                    //
                    // 檢查 同一訊息 最新回覆
                    //
                    bool bFound = false;
                    for (std::list<GAESurveyResultData*>::iterator result = surveyResultList->begin(); result != surveyResultList->end(); result++)
                    {
                        if ( ((*result)->hashkey.compare(surveyData->hashkey) == 0) &&
                             ((*result)->messagehashkey.compare(surveyData->messagehashkey) == 0) &&
                             ((*result)->token.compare(surveyData->token) == 0))
                        {
                            // 檢查時間
                            if ( (*result)->datetime.compare(surveyData->datetime) < 0)
                            {
                                // 取代
                                vilslog::printf ("GAEService::fetchSurveyResult() 重複回覆 取代 hash[%s] messagehashkey[%s] datetime[%s] comment[%s]\n",
                                    surveyData->hashkey.c_str(), surveyData->messagehashkey.c_str(), surveyData->datetime.c_str(), surveyData->comment.c_str());

                                (*result)->hashkey = surveyData->hashkey;
                                (*result)->token = surveyData->token;
                                (*result)->topic = surveyData->topic;
                                (*result)->messagehashkey = surveyData->messagehashkey;
                                (*result)->comment = surveyData->comment;
                                (*result)->datetime = surveyData->datetime;
                                (*result)->results.clear();

                                for (std::list<std::string>::iterator item = surveyData->results.begin(); item != surveyData->results.end(); item++)
                                {
                                    (*result)->results.push_back((*item));
                                }
                            }
                            bFound = true;
                            break;
                        }
                    }

                    if (!bFound)
                    {
                        surveyResultList->push_back(surveyData);
                    }
                }
            }
            else
            {
                vilslog::printf ("GAEService::SyncSurveyResult() JSONresult.isArray() False!!\n");
            }
        }
        else
        {
            vilslog::printf ("GAEService::SyncSurveyResult() parsingSuccessful[%d]\n", parsingSuccessful);
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("GAEService::SyncSurveyResult() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;

    return surveyResultList;
}

bool GAEService::check_applicationform_leave(int projectid, std::list<SurveyEventRecordStruct*> * recordList, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return false;

    // vilslog::printf ("GAEService::check_applicationform_leave() surveyData->hash[%s]\n", surveyData->hash.c_str());

    bool NewResult = false;
    for (std::list<SurveyEventRecordStruct*>::iterator record = recordList->begin(); record != recordList->end(); record++)
    {
        if ( (*record)->hashkey.compare(surveyData->hashkey) == 0 )
        {
            // vilslog::printf ("GAEService::check_applicationform_leave() eventid[%d] surveyid[%d] formid[%s] hashkey[%s]\n",
            //     (*record)->eventid, (*record)->surveyid, (*record)->formid.c_str(), (*record)->hashkey.c_str());

            // GAESurveyResultData * oldResult = baseDb->get_SurveyResult((*record)->hashkey, surveyData->messagehashkey);

            std::list<std::string>::iterator newValue = surveyData->results.begin();

            int signoffstatus = 0;
            std::string status = "簽核中";
            if ( oldResult->results.size() == 0)
            {
                NewResult = true;

                int index = 0;
                for (std::list<std::string>::iterator newValue = surveyData->results.begin(); newValue != surveyData->results.end(); newValue++, index++)
                {
                    if ((index == 0) && ((*newValue).compare("1") == 0))
                    {
                        status = "簽核中";
                        signoffstatus = 0;
                    }
                    else
                    if ((index == 1) && ((*newValue).compare("1") == 0))
                    {
                        status = "已簽核";
                        signoffstatus = 1;
                    }
                    else
                    if ((index == 2) && ((*newValue).compare("1") == 0))
                    {
                        status = "未核准";
                        signoffstatus = 99;
                    }
                }
            }
            else
            {
                int index = 0;
                for (std::list<std::string>::iterator oldValue = oldResult->results.begin(); oldValue != oldResult->results.end(); oldValue++, newValue++, index++)
                {
                    if ( (*oldValue).compare(*newValue) != 0 )
                    {
                        // found new value

                        // vilslog::printf ("GAEService::check_applicationform_leave() found new value index[%d] value[%s]->[%s]\n", index, (*oldValue).c_str(), (*newValue).c_str());
                        NewResult = true;
                        // break;
                    }

                    if ((index == 0) && ((*newValue).compare("1") == 0))
                    {
                        status = "簽核中";
                        signoffstatus = 0;
                    }
                    else
                    if ((index == 1) && ((*newValue).compare("1") == 0))
                    {
                        status = "已簽核";
                        signoffstatus = 1;
                    }
                    else
                    if ((index == 2) && ((*newValue).compare("1") == 0))
                    {
                        status = "未核准";
                        signoffstatus = 99;
                    }
                }
            }

            if ( NewResult )
            {
                //
                // send out notify
                //
                ApplicationFormLeaveStruct * formRecord = baseDb->get_LeaveForm((*record)->formid);
                TargetTopicStruct * targetTopic = baseDb->get_TargetTopicFromTopic(surveyData->topic);

                std::string userid = to_string(targetTopic->userid);
                std::string maingroups = to_string(targetTopic->maingroupid);

                std::list<std::string> targetlist;
                //targetlist.push_back(formRecord->accountid);
                targetlist.push_back(to_string(formRecord->userid)); // 請假人員
                std::string randomKey = random_string(16);
                std::string title = "請假單審核結果通知";

                std::ostringstream sendstream;
                sendstream << "審核結果:"<<status<<endl;
                sendstream << "假勤項目:"<<formRecord->type<<endl;
                sendstream << "請假事由:"<<formRecord->reason<<endl;
                sendstream << "請假時間:"<<formRecord->startdate<< " " <<formRecord->starttime<<" 到 "<<formRecord->enddate<< " " <<formRecord->endtime<<endl;
                sendstream << "申請人:"<<formRecord->applyusername<<endl;
                sendstream << "簽核人員:"<<targetTopic->name<<endl;
                if (surveyData->comment.length() > 0)
                    sendstream << "簽核人員說明:"<<surveyData->comment<<endl;
                else
                    sendstream << "簽核人員說明:無"<<endl;

                std::string context = sendstream.str();

                // vilslog::printf ("GAEService::check_applicationform_leave() accountid[%s] context[%s]\n", formRecord->accountid.c_str(), context.c_str());

                NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_2222, projectid, targetlist, targetTopic->accountid, userid, maingroups, title, context, randomKey);
                mNotifiactionQueue.add(notifactionitem);

                formRecord->signoffstatus = signoffstatus;// 0:簽核中 1:已簽核 99:未核准
                formRecord->signoffuserid = targetTopic->userid;

                // vilslog::printf ("GAEService::check_applicationform_leave() signoffstatus[%d] signoffaccountid[%s]\n", signoffstatus, formRecord->signoffaccountid.c_str());


                WorkItem * work = new WorkItem("updateApplicationFormLeaveStruct", projectid, formRecord);
                mMySQLJobQueue.add(work);

                // baseDb->update_LeaveForm(formRecord);

                delete formRecord;
                delete targetTopic;
            }

            // oldResult->results.clear();
            // delete oldResult;
            break;
        }
    }

    return NewResult;
}

bool GAEService::check_applicationform_assist(int projectid, std::list<SurveyEventRecordStruct*> * recordList, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return false;

    // vilslog::printf ("GAEService::check_applicationform_assist() surveyData->hashkey[%s] messagehashkey[%s]\n",
    //     surveyData->hashkey.c_str(), surveyData->messagehashkey.c_str());

    bool NewResult = false;
    for (std::list<SurveyEventRecordStruct*>::iterator record = recordList->begin(); record != recordList->end(); record++)
    {
        if ( (*record)->hashkey.compare(surveyData->hashkey) == 0 )
        {
            // vilslog::printf ("GAEService::check_applicationform_assist() eventid[%d] surveyid[%d] formid[%s] hashkey[%s]\n",
            //     (*record)->eventid, (*record)->surveyid, (*record)->formid.c_str(), (*record)->hashkey.c_str());

            // GAESurveyResultData * oldResult = baseDb->get_SurveyResult((*record)->hashkey, surveyData->messagehashkey);

            std::list<std::string>::iterator newValue = surveyData->results.begin();

            int signoffstatus = 0;
            std::string status = "簽核中";
            if ( oldResult->results.size() == 0)
            {
                NewResult = true;

                int index = 0;
                for (std::list<std::string>::iterator newValue = surveyData->results.begin(); newValue != surveyData->results.end(); newValue++, index++)
                {
                    if ((index == 0) && ((*newValue).compare("1") == 0))
                    {
                        status = "簽核中";
                        signoffstatus = 0;
                    }
                    else
                    if ((index == 1) && ((*newValue).compare("1") == 0))
                    {
                        status = "已簽核";
                        signoffstatus = 1;
                    }
                    else
                    if ((index == 2) && ((*newValue).compare("1") == 0))
                    {
                        status = "未核准";
                        signoffstatus = 99;
                    }
                }
            }
            else
            {
                int index = 0;
                for (std::list<std::string>::iterator oldValue = oldResult->results.begin(); oldValue != oldResult->results.end(); oldValue++, newValue++, index++)
                {
                    if ( (*oldValue).compare(*newValue) != 0 )
                    {
                        // found new value

                        // vilslog::printf ("GAEService::check_applicationform_assist() found new value index[%d] value[%s]->[%s]\n", index, (*oldValue).c_str(), (*newValue).c_str());
                        NewResult = true;
                        // break;
                    }

                    if ((index == 0) && ((*newValue).compare("1") == 0))
                    {
                        status = "簽核中";
                        signoffstatus = 0;
                    }
                    else
                    if ((index == 1) && ((*newValue).compare("1") == 0))
                    {
                        status = "已簽核";
                        signoffstatus = 1;
                    }
                    else
                    if ((index == 2) && ((*newValue).compare("1") == 0))
                    {
                        status = "未核准";
                        signoffstatus = 99;
                    }
                }
            }

            if ( NewResult )
            {
                // vilslog::printf ("GAEService::check_applicationform_assist() send out notify \n");
                //
                // send out notify
                //
                AssistEventStruct * assistevent = baseDb->get_AssistEvent((*record)->formid);
                AssistStruct * assist = baseDb->read_Assist(assistevent->assistid);
                TargetTopicStruct * targetTopic = baseDb->get_TargetTopicFromTopic(surveyData->topic);

                std::string userid = to_string(targetTopic->userid);
                std::string maingroups = to_string(targetTopic->maingroupid);

                std::list<std::string> targetlist;
                //targetlist.push_back(assistevent->accountid);
                targetlist.push_back(to_string(assistevent->userid));
                std::string randomKey = random_string(16);
                std::string title = "申請協助事項審核結果通知";

                std::ostringstream sendstream;
                sendstream << "審核結果:"<<status<<endl;
                sendstream << "事項名稱:"<<assist->title<<endl;
                sendstream << "申請者:"<<assistevent->applyusername<<"/"<<assistevent->applyuserrelation<<endl;
                sendstream << "申請說明:"<<assistevent->description<<endl;
                sendstream << "申請時間:"<<assistevent->datetime<<endl;
                sendstream << "簽核人員:"<<targetTopic->name<<endl;
                if (surveyData->comment.length() > 0)
                    sendstream << "簽核人員說明:"<<surveyData->comment<<endl;
                else
                    sendstream << "簽核人員說明:無"<<endl;

                std::string context = sendstream.str();

                // vilslog::printf ("GAEService::check_applicationform_assist() accountid[%s] context[%s]\n", assistevent->accountid.c_str(), context.c_str());

                NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_204, projectid, targetlist, targetTopic->accountid, userid, maingroups, title, context, randomKey);
                mNotifiactionQueue.add(notifactionitem);


                assistevent->signoffstatus = signoffstatus;// 0:簽核中 1:已簽核 99:未核准
                assistevent->signoffuserid = targetTopic->userid;
                //assistevent->signoffaccountid = targetTopic->accountid;

                // vilslog::printf ("GAEService::check_applicationform_assist() signoffstatus[%d] signoffaccountid[%s]\n",
                //     signoffstatus, assistevent->signoffaccountid.c_str());

                WorkItem * work = new WorkItem("updateAssistEventStruct", projectid, assistevent);
                mMySQLJobQueue.add(work);

                //baseDb->update_AssistEvent(assistevent);

                delete assist;
                delete assistevent;
                delete targetTopic;
            }

            // oldResult->results.clear();
            // delete oldResult;
            break;
        }
    }

    return NewResult;
}

bool GAEService::check_applicationform_curriculum(int projectid, std::list<SurveyEventRecordStruct*> * recordList, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return false;

    // vilslog::printf ("GAEService::check_applicationform_curriculum() surveyData->hashkey[%s] messagehashkey[%s]\n",
    //     surveyData->hashkey.c_str(), surveyData->messagehashkey.c_str());

    bool NewResult = false;
    for (std::list<SurveyEventRecordStruct*>::iterator record = recordList->begin(); record != recordList->end(); record++)
    {
        if ( (*record)->hashkey.compare(surveyData->hashkey) == 0 )
        {
            // vilslog::printf ("GAEService::check_applicationform_curriculum() eventid[%d] surveyid[%d] userid[%d] formid[%s] hashkey[%s]\n",
            //     (*record)->eventid, (*record)->surveyid, (*record)->userid, (*record)->formid.c_str(), (*record)->hashkey.c_str());

            // GAESurveyResultData * oldResult = baseDb->get_SurveyResult((*record)->hashkey, surveyData->messagehashkey);

            std::list<std::string>::iterator newValue = surveyData->results.begin();

            int signoffstatus = 0;
            std::string status = "簽核中";
            if ( oldResult->results.size() == 0)
            {
                NewResult = true;

                int index = 0;
                for (std::list<std::string>::iterator newValue = surveyData->results.begin(); newValue != surveyData->results.end(); newValue++, index++)
                {
                    if ((index == 0) && ((*newValue).compare("1") == 0))
                    {
                        status = "簽核中";
                        signoffstatus = 0;
                    }
                    else
                    if ((index == 1) && ((*newValue).compare("1") == 0))
                    {
                        status = "已簽核";
                        signoffstatus = 1;
                    }
                    else
                    if ((index == 2) && ((*newValue).compare("1") == 0))
                    {
                        status = "未核准";
                        signoffstatus = 99;
                    }
                }
            }
            else
            {
                int index = 0;
                for (std::list<std::string>::iterator oldValue = oldResult->results.begin(); oldValue != oldResult->results.end(); oldValue++, newValue++, index++)
                {
                    if ( (*oldValue).compare(*newValue) != 0 )
                    {
                        // found new value

                        // vilslog::printf ("GAEService::check_applicationform_curriculum() found new value index[%d] value[%s]->[%s]\n", index, (*oldValue).c_str(), (*newValue).c_str());
                        NewResult = true;
                        // break;
                    }

                    if ((index == 0) && ((*newValue).compare("1") == 0))
                    {
                        status = "簽核中";
                        signoffstatus = 0;
                    }
                    else
                    if ((index == 1) && ((*newValue).compare("1") == 0))
                    {
                        status = "已簽核";
                        signoffstatus = 1;
                    }
                    else
                    if ((index == 2) && ((*newValue).compare("1") == 0))
                    {
                        status = "未核准";
                        signoffstatus = 99;
                    }
                }
            }

            if ( NewResult )
            {
                // vilslog::printf ("GAEService::check_applicationform_curriculum() send out notify \n");
                //
                // send out notify
                //
                CurriculumRecordStruct * curriculumrecord = baseDb->get_CurriculumRecord((*record)->formid);
                CurriculumApplyStruct * curriculumapply = baseDb->get_CurriculumApply((*record)->formid, (*record)->userid);
                CurriculumStruct * curriculum = baseDb->get_Curriculum(curriculumrecord->curriculumid);
                CurriculumTypeStruct * curriculumtype = baseDb->get_CurriculumType(curriculum->curriculumtypeid);
                TargetTopicStruct * targetTopic = baseDb->get_TargetTopicFromTopic(surveyData->topic);

                std::string userid = to_string(targetTopic->userid);
                std::string maingroups = to_string(targetTopic->maingroupid);

                std::list<std::string> targetlist;
                targetlist.push_back(to_string((*record)->userid));
                std::string randomKey = random_string(16);
                std::string title = "申請選課課程審核結果通知";

                std::ostringstream sendstream;
                sendstream << "審核結果:"<<status<<endl;
                sendstream << "課程名稱:"<<curriculumtype->name<<endl;
                sendstream << "課程編號:"<<curriculum->number<<endl;
                sendstream << "簽核人員:"<<targetTopic->name<<endl;
                if (surveyData->comment.length() > 0)
                    sendstream << "簽核人員說明:"<<surveyData->comment<<endl;
                else
                    sendstream << "簽核人員說明:無"<<endl;

                std::string context = sendstream.str();

                // vilslog::printf ("GAEService::check_applicationform_curriculum() context[%s]\n", context.c_str());

                NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_204, projectid, targetlist, targetTopic->accountid, userid, maingroups, title, context, randomKey);
                mNotifiactionQueue.add(notifactionitem);


                curriculumapply->status = signoffstatus;// 0:簽核中 1:已簽核 99:未核准

                // vilslog::printf ("GAEService::check_applicationform_curriculum() signoffstatus[%d]\n", signoffstatus);

                WorkItem * work = new WorkItem("updateCurriculumApplyStruct", projectid, curriculumapply);
                mMySQLJobQueue.add(work);

                delete curriculumrecord;
                delete curriculum;
                delete curriculumapply;
                delete targetTopic;
            }


            // oldResult->results.clear();
            // delete oldResult;
            break;
        }
    }

    return NewResult;
}

#ifdef ENABLE_KIA_SERVICE
bool GAEService::check_KIA_WCInfo(int projectid, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData)
{
    bool NewResult = false;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return NewResult;

    // vilslog::printf ("GAEService::check_KIA_WCInfo() surveyData topic[%s] messagehashkey[%s]\n",
    //     surveyData->topic.c_str(), surveyData->messagehashkey.c_str());

    PushNotifyMessageRecordStruct * MsgRecord = baseDb->get_PushNotifyMessageRecord(surveyData->messagehashkey);
    if (MsgRecord == NULL)
    {
        return NewResult;
    }

    // vilslog::printf ("GAEService::check_KIA_WCInfo() MsgRecord PushNotifyMessageRecorddbId[%d] messagehashkey[%s] surveyids[%s] userid[%d]\n",
    //     MsgRecord->id, MsgRecord->messagehashkey.c_str(), MsgRecord->surveyids.c_str(), MsgRecord->userid);

    SurveyEventRecordStruct * surveyRecord = baseDb->get_SurveyEventRecord(MsgRecord->id);

    if ( surveyRecord == NULL)
    {
        delete MsgRecord;
        return NewResult;
    }

    // vilslog::printf ("GAEService::check_KIA_WCInfo() surveyRecord hashkey[%s] eventid[%d] surveyid[%d] userid[%d]\n",
    //     surveyRecord->hashkey.c_str(), surveyRecord->eventid, surveyRecord->surveyid, surveyRecord->userid);

    std::list<SurveyItemStruct*> * surveyItemList = baseDb->readSurveyItems(surveyRecord->surveyid);

    if ( surveyRecord == NULL)
    {
        delete surveyRecord;
        delete MsgRecord;
        return NewResult;
    }

    //
    // 檢查 是否有不同資料
    //
    std::list<SurveyItemStruct*>::iterator surveyItem = surveyItemList->begin();

    bool bFoundNewResult = false;
    std::string description = "";

    if ( oldResult->results.size() == 0)
    {
        for (std::list<std::string>::iterator newValue = surveyData->results.begin(); newValue != surveyData->results.end(); newValue++, surveyItem++)
        {
            if ((*newValue).compare("1") == 0)
            {
                bFoundNewResult = true;
                description = (*surveyItem)->description;
            }
        }
    }
    else
    {
        std::list<std::string>::iterator newValue = surveyData->results.begin();
        for (std::list<std::string>::iterator oldValue = oldResult->results.begin(); oldValue != oldResult->results.end(); oldValue++, newValue++, surveyItem++)
        {
            if ( (*oldValue).compare(*newValue) != 0 )
            {
                if ((*newValue).compare("1") == 0)
                {
                    bFoundNewResult = true;
                    description = (*surveyItem)->description;

                    vilslog::printf ("GAEService::check_KIA_WCInfo() oldValue[%s] newValue[%s] surveyItem->description[%s]\n",
                        (*oldValue).c_str(), (*newValue).c_str(), (*surveyItem)->description.c_str());

                }
            }
        }
    }

    if (bFoundNewResult)
    {
        vilslog::printf ("GAEService::check_KIA_WCInfo() MsgRecord PushNotifyMessageRecorddbId[%d] messagehashkey[%s] surveyids[%s] userid[%d]\n",
            MsgRecord->id, MsgRecord->messagehashkey.c_str(), MsgRecord->surveyids.c_str(), MsgRecord->userid);

        vilslog::printf ("GAEService::check_KIA_WCInfo() surveyRecord hashkey[%s] eventid[%d] surveyid[%d] userid[%d]\n",
            surveyRecord->hashkey.c_str(), surveyRecord->eventid, surveyRecord->surveyid, surveyRecord->userid);

        // 新資料
        UserStruct * user = baseDb->getUserByUserId(surveyRecord->userid);
        if ( user != NULL)
        {
            //
            // 推播給子集合管理者
            //
            std::list<std::string> * sendtargetlist = getSubgGroupManager(projectid, user);

            vilslog::printf ("GAEService::check_KIA_WCInfo() description[%s] sendtargetlist[%s]\n",
                description.c_str(), convert_list_to_string(sendtargetlist).c_str());

            if (sendtargetlist != NULL)
            {
                std::list<std::string> targetlist;
                for (std::list<std::string>::iterator target = sendtargetlist->begin(); target != sendtargetlist->end(); target++)
                {
                    targetlist.push_back((*target));
                }

                std::ostringstream sendstream;
                sendstream << "地點:"<<user->name<<endl;
                sendstream << "事項名稱:"<<MsgRecord->title<<endl;
                sendstream << "意見的編號(GID):"<<surveyRecord->formid<<endl;
                sendstream << "回報結果:"<<description<<endl;
                if (surveyData->comment.length() > 0)
                    sendstream << "回報內容:"<<surveyData->comment<<endl;
                else
                    sendstream << "回報內容:無"<<endl;
                sendstream << "回報時間:"<<currentDateTimeForSQL()<<endl;

                std::string userid = to_string(user->userid);
                std::string title = "回報事件處理通知";
                std::string context = sendstream.str();
                std::string maingroups = convert_list_to_string(user->maingroups);
                std::string randomKey = random_string(16);

                NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_204, projectid, targetlist, user->account, userid, maingroups, title, context, randomKey);

                mNotifiactionQueue.add(notifactionitem);

                targetlist.clear();
                sendtargetlist->clear();
                delete sendtargetlist;
            }

            clear_UserStruct(user);
            delete user;
        }
    }

    surveyItemList->clear();
    delete surveyItemList;

    delete surveyRecord;
    delete MsgRecord;
    return NewResult;
}
#endif

int GAEService::updateGAETopicInfo()
{
    time_t t_Current = time(0);

    time_t t_diff = t_Current - mSyncGAETopicInfo_time;
    int diff_min = ((int)t_diff/60);
    if ( diff_min > 1 )
    {
        mSyncGAETopicInfo_time = t_Current;

        std::list<int> * prjidList = mDbManager->getAllProjectConfig();
        for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
        {
            int projectid = (*prj);

            //startSyncWithRetry(projectid, 3);

            // https://smimsvils.appspot.com/vilsapi?type=getmessagestatus&messageid=1587115188857821
            // https://test-dot-smimsvils.appspot.com/vilsapi?type=getmessagestatus&fromdatetime=20200419151548&todatetime=20200420151548
            //SyncMessageStatus(projectid);


            // http://smimsvils.appspot.com/vilsapi?type=getsurveyresult&hash=AhgXQ78urAI8
            //SyncSurveyResult(projectid);

            // http://smimsvils.appspot.com/vilsapi?type=gettopicinfo&target=all&prefix=SMIMSKAO
            // SyncTopicInfo(projectid);
            updateGAETopicInfo(projectid);

            // updateTopicInfoName(projectid);
        }

        prjidList->clear();
        delete prjidList;
    }
    return 0;
}

int GAEService::updateGAEUserCard()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        std::list<UserStruct*> * userList = baseDb->get_Users();
        std::list<TargetTopicStruct*> * recordList = baseDb->get_TargetTopics();

        // vilslog::printf ("GAEService::updateGAEUserCard() userList->size()[%d]\n", (int)userList->size());

        std::ostringstream sendstream;
        sendstream.str("");
        sendstream.clear();
        sendstream<<"prefix=" << vilssysConfig->PushNotifyPrefix;
        sendstream<<"&vilsuid=" << vilssysConfig->VILSUID;
        sendstream<<"&data=[";

        int sendcount = 0;
        for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
        {
            if (sendcount != 0)
            {
                sendstream<<",";
            }

            int maingroup = 1;
            if ((*user)->maingroups->size() > 0)
            {
                std::list<std::string>::iterator maingroupIt = (*user)->maingroups->begin();
                maingroup = convert_string_to_int((*maingroupIt));
            }

            // 轉換 舊 topic 到新的
            TargetTopicStruct * newTopic = get_USER_newTopic(projectid, vilssysConfig->PushNotifyPrefix, (*user), maingroup, recordList);

            std::string rfidcards = convert_list_to_string((*user)->rfidcards);
            std::string nfccards = convert_list_to_string((*user)->nfccards);
            std::string uwbcards = convert_list_to_string((*user)->uwbcards);

            sendstream<<"{\"topic\":\""<<newTopic->topic<<"\",\"userid\":"<<(*user)->userid<<",\"name\":\""<<(*user)->name<<"\"";
            if (rfidcards.length() > 0)
            {
                sendstream<<",\"rfidcards\":\""<<rfidcards<<"\"";
            }
            if (nfccards.length() > 0)
            {
                sendstream<<",\"nfccards\":\""<<nfccards<<"\"";
            }
            if (uwbcards.length() > 0)
            {
                sendstream<<",\"uwbcards\":\""<<uwbcards<<"\"";
            }
            sendstream<<"}";

            delete newTopic;

            sendcount++;
            if ( sendcount >= 10 )
            {
                sendstream<<"]";

                std::string sendString = sendstream.str();

                // vilslog::printf ("GAEService::updateGAEUserCard() 1 sendString[%s]\n", sendString.c_str());

                sendPOSTRequest("updateusercard", sendString.c_str());

                sendcount = 0;
                sendstream.str("");
                sendstream.clear();
                sendstream<<"prefix=" << vilssysConfig->PushNotifyPrefix;
                sendstream<<"&vilsuid=" << vilssysConfig->VILSUID;
                sendstream<<"&data=[";
            }
        }

        if ( sendcount > 0 )
        {
            sendstream<<"]";

            std::string sendString = sendstream.str();

            // vilslog::printf ("GAEService::updateGAEUserCard() 2 sendString[%s]\n", sendString.c_str());

            sendPOSTRequest("updateusercard", sendString.c_str());
        }

        clear_UserStruct_list(userList);
        delete userList;

        clear_TargetTopicStruct_list(recordList);
        delete recordList;
    }
    prjidList->clear();
    delete prjidList;
    return 0;
}

int GAEService::updateGAECardUser()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        std::list<UserStruct*> * userList = baseDb->get_Users();
        std::list<TargetTopicStruct*> * recordList = baseDb->get_TargetTopics();

        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        updateGAECardUser_uwbcard(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, tagNodeList, userList, recordList);

        std::list<NFCCardStruct*> * nfccardList = mDeviceManager->get_NFCCard_list(projectid);
        updateGAECardUser_nfccard(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, nfccardList, userList, recordList);

        std::list<RFIDCardStruct*> * rfidcardList = mDeviceManager->get_RFIDCard_list(projectid);
        updateGAECardUser_rfidcard(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, rfidcardList, userList, recordList);

        clear_UserStruct_list(userList);
        delete userList;

        clear_TargetTopicStruct_list(recordList);
        delete recordList;
    }
    prjidList->clear();
    delete prjidList;
    return 0;
}

int GAEService::updateGAECardUser_uwbcard(int projectid, std::string prefix, std::string VILSUID, std::list<NodeStruct*> * tagNodeList, std::list<UserStruct*> * userList, std::list<TargetTopicStruct*> * recordList)
{
    // vilslog::printf ("GAEService::updateGAECardUser_uwbcard() tagNodeList->size()[%d]\n", (int)tagNodeList->size());

    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&cardtype=uwbcard";
    sendstream<<"&data=[";

    int sendcount = 0;
    for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
    {
        if ( (*node)->userid <= 0 )
        {
            continue;
        }

        std::string username = getUserName((*node)->userid, userList);

        if (username.length() == 0)
        {
            continue;
        }

        if (sendcount != 0)
        {
            sendstream<<",";
        }

        std::string target = prefix;
        target.append("_TAG_");
        target.append((*node)->macaddress);

        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*node)->nodename, (*node)->macaddress, 0, (*node)->maingroupid, recordList);

        sendstream<<"{\"topic\":\""<<newTopic->topic<<"\"";
        sendstream<<",\"cardid\":\""<<(*node)->macaddress<<"\"";
        sendstream<<",\"cardname\":\""<<(*node)->nodename<<"\"";
        sendstream<<",\"userid\":"<<(*node)->userid;
        sendstream<<",\"username\":\""<<username<<"\"";
        sendstream<<"}";

        delete newTopic;

        sendcount++;
        if ( sendcount >= 10 )
        {
            sendstream<<"]";
            std::string sendString = sendstream.str();
            sendPOSTRequest("updatecarduser", sendString.c_str());

            sendcount = 0;
            sendstream.str("");
            sendstream.clear();
            sendstream<<"prefix=" << prefix;
            sendstream<<"&vilsuid=" << VILSUID;
            sendstream<<"&cardtype=uwbcard";
            sendstream<<"&data=[";
        }
    }

    if ( sendcount > 0 )
    {
        sendstream<<"]";
        std::string sendString = sendstream.str();
        sendPOSTRequest("updatecarduser", sendString.c_str());
    }

    return 0;
}

int GAEService::updateGAECardUser_nfccard(int projectid, std::string prefix, std::string VILSUID, std::list<NFCCardStruct*> * nfccardList, std::list<UserStruct*> * userList, std::list<TargetTopicStruct*> * recordList)
{
    // vilslog::printf ("GAEService::updateGAECardUser_nfccard() nfccardList->size()[%d]\n", (int)nfccardList->size());

    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&cardtype=nfccard";
    sendstream<<"&data=[";

    int sendcount = 0;
    for (std::list<NFCCardStruct*>::iterator card = nfccardList->begin(); card != nfccardList->end(); card++)
    {
        if ( (*card)->userid <= 0 )
        {
            continue;
        }

        std::string username = getUserName((*card)->userid, userList);

        if (username.length() == 0)
        {
            continue;
        }

        if (sendcount != 0)
        {
            sendstream<<",";
        }

        std::string target = prefix;
        target.append("_CARD_");
        target.append((*card)->cardid);
        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*card)->cardid, (*card)->cardid, 0, (*card)->maingroupid, recordList);

        sendstream<<"{\"topic\":\""<<newTopic->topic<<"\"";
        sendstream<<",\"cardid\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"cardname\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"userid\":"<<(*card)->userid;
        sendstream<<",\"username\":\""<<username<<"\"";
        sendstream<<"}";

        delete newTopic;

        sendcount++;
        if ( sendcount >= 10 )
        {
            sendstream<<"]";
            std::string sendString = sendstream.str();
            sendPOSTRequest("updatecarduser", sendString.c_str());

            sendcount = 0;
            sendstream.str("");
            sendstream.clear();
            sendstream<<"prefix=" << prefix;
            sendstream<<"&vilsuid=" << VILSUID;
            sendstream<<"&cardtype=nfccard";
            sendstream<<"&data=[";
        }
    }

    if ( sendcount > 0 )
    {
        sendstream<<"]";
        std::string sendString = sendstream.str();
        sendPOSTRequest("updatecarduser", sendString.c_str());
    }

    return 0;
}

int GAEService::updateGAECardUser_rfidcard(int projectid, std::string prefix, std::string VILSUID, std::list<RFIDCardStruct*> * rfidcardList, std::list<UserStruct*> * userList, std::list<TargetTopicStruct*> * recordList)
{
    // vilslog::printf ("GAEService::updateGAECardUser_rfidcard() rfidcardList->size()[%d]\n", (int)rfidcardList->size());

    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&cardtype=rfidcard";
    sendstream<<"&data=[";

    int sendcount = 0;
    for (std::list<RFIDCardStruct*>::iterator card = rfidcardList->begin(); card != rfidcardList->end(); card++)
    {
        if ( (*card)->userid <= 0 )
        {
            continue;
        }

        std::string username = getUserName((*card)->userid, userList);

        if (username.length() == 0)
        {
            continue;
        }

        if (sendcount != 0)
        {
            sendstream<<",";
        }

        std::string target = prefix;
        target.append("_CARD_");
        target.append((*card)->cardid);
        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*card)->cardid, (*card)->cardid, 0, (*card)->maingroupid, recordList);

        sendstream<<"{\"topic\":\""<<newTopic->topic<<"\"";
        sendstream<<",\"cardid\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"cardname\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"userid\":"<<(*card)->userid;
        sendstream<<",\"username\":\""<<username<<"\"";
        sendstream<<"}";

        delete newTopic;

        sendcount++;
        if ( sendcount >= 10 )
        {
            sendstream<<"]";
            std::string sendString = sendstream.str();
            sendPOSTRequest("updatecarduser", sendString.c_str());

            sendcount = 0;
            sendstream.str("");
            sendstream.clear();
            sendstream<<"prefix=" << prefix;
            sendstream<<"&vilsuid=" << VILSUID;
            sendstream<<"&cardtype=rfidcard";
            sendstream<<"&data=[";
        }
    }

    if ( sendcount > 0 )
    {
        sendstream<<"]";
        std::string sendString = sendstream.str();
        sendPOSTRequest("updatecarduser", sendString.c_str());
    }

    return 0;
}

int GAEService::updateGAEDIInfo()
{
    char request[4096];
    memset(request, 0, 4096);
    const char *MessageFmt = "data={\"prefix\":\"%s\",\"link\":\"%sdi/\",\"mainpage\":\"%s\",\"vilsuid\":\"%s\",\"welcometitle\":\"%s\",\"welcomebody\":\"%s\"}";

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

        memset(request, 0, 4096);

        //snprintf(request, 4096, MessageFmt, vilssysConfig->PushNotifyPrefix.c_str(), DI_WEBPAGE, CENTER_WEBPAGE);
        snprintf(request, 4096, MessageFmt, vilssysConfig->PushNotifyPrefix.c_str(),
        vilssysConfig->VILSServer.c_str(), vilssysConfig->VILSServer.c_str(), vilssysConfig->VILSUID.c_str(),
        vilssysConfig->PushNotifyWelcomeTitle.c_str(), vilssysConfig->PushNotifyWelcomeBody.c_str());

        // vilslog::printf("GAEService::updateGAEDIInfo() projectid[%d] request[%s]\n", projectid, request);

        sendPOSTRequest("updatedilink", request);

    }
    prjidList->clear();
    delete prjidList;

    return 0;
}

int GAEService::updateGAETopicInfo(int projectid)
{
    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 0;

    std::list<TargetTopicStruct*> * recordList = baseDb->get_TargetTopics();
    std::list<UserStruct*> * userList = baseDb->get_Users();

    updateGAE_TAG_TopicInfo(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, recordList);
    updateGAE_TAG_USER_TopicInfo(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, recordList);
    updateGAE_LOCATOR_TopicInfo(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, recordList);
    updateGAE_USER_TopicInfo(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, recordList, userList);
    updateGAE_UHFCARD_TopicInfo(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, recordList, userList);

    updateGAE_SUBGROUP_TopicInfo(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, recordList);
    updateGAE_MAINGROUP_TopicInfo(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, recordList);

    clear_TargetTopicStruct_list(recordList);
    delete recordList;

    clear_UserStruct_list(userList);
    delete userList;

    return 0;
}

int GAEService::updateGAEUserInfo(int projectid, int userid)
{
    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 0;

    std::list<TargetTopicStruct*> * recordList = baseDb->get_TargetTopics();
    std::list<UserStruct*> * userList = baseDb->get_Users();

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        if ( (*user)->userid == userid )
        {
            //
            // target topic
            //
            int maingroup = 1;
            if ((*user)->maingroups->size() > 0)
            {
                std::list<std::string>::iterator maingroupIt = (*user)->maingroups->begin();
                maingroup = convert_string_to_int((*maingroupIt));
            }

            // 轉換 舊 topic 到新的
            TargetTopicStruct * newTopic = get_USER_newTopic(projectid, vilssysConfig->PushNotifyPrefix, (*user), maingroup, recordList);
            // TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*user)->name, (*user)->account, (*user)->userid, maingroup, recordList);
            sendList->push_back(newTopic);

            updateGAETargetTopicInfoList(vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, sendList);

            //
            // User Card
            //
            updateGAEUserCard(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, (*user), newTopic);

            //
            // Card User
            //
            updateGAECardUser(projectid, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, (*user), newTopic);

            break;
        }
    }


    clear_TargetTopicStruct_list(sendList);
    delete sendList;

    clear_TargetTopicStruct_list(recordList);
    delete recordList;

    clear_UserStruct_list(userList);
    delete userList;

    return 0;
}

int GAEService::updateGAEUserCard(int projectid, std::string prefix, std::string VILSUID, UserStruct * user, TargetTopicStruct * topic)
{
    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&data=[";

    std::string target = prefix;
    target.append("_USER_");
    target.append(to_string(user->userid));
    // target.append(user->account);

    std::string rfidcards = convert_list_to_string(user->rfidcards);
    std::string nfccards = convert_list_to_string(user->nfccards);
    std::string uwbcards = convert_list_to_string(user->uwbcards);

    sendstream<<"{\"topic\":\""<<topic->topic<<"\",\"userid\":"<<user->userid<<",\"name\":\""<<user->name<<"\"";
    if (rfidcards.length() > 0)
    {
        sendstream<<",\"rfidcards\":\""<<rfidcards<<"\"";
    }
    if (nfccards.length() > 0)
    {
        sendstream<<",\"nfccards\":\""<<nfccards<<"\"";
    }
    if (uwbcards.length() > 0)
    {
        sendstream<<",\"uwbcards\":\""<<uwbcards<<"\"";
    }
    sendstream<<"}";

    sendstream<<"]";

    std::string sendString = sendstream.str();

    // vilslog::printf ("GAEService::updateGAEUserCard() sendString[%s]\n", sendString.c_str());

    sendPOSTRequest("updateusercard", sendString.c_str());

    return 0;
}

int GAEService::updateGAECardUser(int projectid, std::string prefix, std::string VILSUID, UserStruct * user, TargetTopicStruct * topic)
{
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
    updateGAECardUser_uwbcard(projectid, prefix, VILSUID, tagNodeList, user, topic);

    std::list<NFCCardStruct*> * nfccardList = mDeviceManager->get_NFCCard_list(projectid);
    updateGAECardUser_nfccard(projectid, prefix, VILSUID, nfccardList, user, topic);

    std::list<RFIDCardStruct*> * rfidcardList = mDeviceManager->get_RFIDCard_list(projectid);
    updateGAECardUser_rfidcard(projectid, prefix, VILSUID, rfidcardList, user, topic);

    return 0;
}

int GAEService::updateGAECardUser_uwbcard(int projectid, std::string prefix, std::string VILSUID, std::list<NodeStruct*> * tagNodeList, UserStruct * user, TargetTopicStruct * topic)
{
    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&cardtype=uwbcard";
    sendstream<<"&data=[";

    int sendcount = 0;
    for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
    {
        if ( (*node)->userid != user->userid )
        {
            continue;
        }

        if (sendcount != 0)
        {
            sendstream<<",";
        }

        std::string target = prefix;
        target.append("_TAG_");
        target.append((*node)->macaddress);

        sendstream<<"{\"topic\":\""<<topic->topic<<"\"";
        sendstream<<",\"cardid\":\""<<(*node)->macaddress<<"\"";
        sendstream<<",\"cardname\":\""<<(*node)->nodename<<"\"";
        sendstream<<",\"userid\":"<<(*node)->userid;
        sendstream<<",\"username\":\""<<user->name<<"\"";
        sendstream<<"}";

        sendcount++;
        if ( sendcount >= 10 )
        {
            sendstream<<"]";
            std::string sendString = sendstream.str();

            // vilslog::printf ("GAEService::updateGAECardUser_uwbcard() sendString[%s]\n", sendString.c_str());

            sendPOSTRequest("updatecarduser", sendString.c_str());

            sendcount = 0;
            sendstream.str("");
            sendstream.clear();
            sendstream<<"prefix=" << prefix;
            sendstream<<"&vilsuid=" << VILSUID;
            sendstream<<"&cardtype=uwbcard";
            sendstream<<"&data=[";
        }
    }

    if ( sendcount > 0 )
    {
        sendstream<<"]";
        std::string sendString = sendstream.str();

        // vilslog::printf ("GAEService::updateGAECardUser_uwbcard() sendString[%s]\n", sendString.c_str());

        sendPOSTRequest("updatecarduser", sendString.c_str());
    }

    return 0;
}

int GAEService::updateGAECardUser_nfccard(int projectid, std::string prefix, std::string VILSUID, std::list<NFCCardStruct*> * nfccardList, UserStruct * user, TargetTopicStruct * topic)
{
    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&cardtype=nfccard";
    sendstream<<"&data=[";

    int sendcount = 0;
    for (std::list<NFCCardStruct*>::iterator card = nfccardList->begin(); card != nfccardList->end(); card++)
    {
        if ( (*card)->userid != user->userid )
        {
            continue;
        }

        if (sendcount != 0)
        {
            sendstream<<",";
        }

        std::string target = prefix;
        target.append("_CARD_");
        target.append((*card)->cardid);

        sendstream<<"{\"topic\":\""<<topic->topic<<"\"";
        sendstream<<",\"cardid\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"cardname\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"userid\":"<<(*card)->userid;
        sendstream<<",\"username\":\""<<user->name<<"\"";
        sendstream<<"}";

        sendcount++;
        if ( sendcount >= 10 )
        {
            sendstream<<"]";
            std::string sendString = sendstream.str();

            // vilslog::printf ("GAEService::updateGAECardUser_nfccard() sendString[%s]\n", sendString.c_str());

            sendPOSTRequest("updatecarduser", sendString.c_str());

            sendcount = 0;
            sendstream.str("");
            sendstream.clear();
            sendstream<<"prefix=" << prefix;
            sendstream<<"&vilsuid=" << VILSUID;
            sendstream<<"&cardtype=nfccard";
            sendstream<<"&data=[";
        }
    }

    if ( sendcount > 0 )
    {
        sendstream<<"]";
        std::string sendString = sendstream.str();

        // vilslog::printf ("GAEService::updateGAECardUser_nfccard() sendString[%s]\n", sendString.c_str());

        sendPOSTRequest("updatecarduser", sendString.c_str());
    }

    return 0;
}

int GAEService::updateGAECardUser_rfidcard(int projectid, std::string prefix, std::string VILSUID, std::list<RFIDCardStruct*> * rfidcardList, UserStruct * user, TargetTopicStruct * topic)
{
    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&cardtype=rfidcard";
    sendstream<<"&data=[";

    int sendcount = 0;
    for (std::list<RFIDCardStruct*>::iterator card = rfidcardList->begin(); card != rfidcardList->end(); card++)
    {
        if ( (*card)->userid != user->userid )
        {
            continue;
        }

        std::string target = prefix;
        target.append("_CARD_");
        target.append((*card)->cardid);

        sendstream<<"{\"topic\":\""<<topic->topic<<"\"";
        sendstream<<",\"cardid\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"cardname\":\""<<(*card)->cardid<<"\"";
        sendstream<<",\"userid\":"<<(*card)->userid;
        sendstream<<",\"username\":\""<<user->name<<"\"";
        sendstream<<"}";

        sendcount++;
        if ( sendcount >= 10 )
        {
            sendstream<<"]";
            std::string sendString = sendstream.str();

            // vilslog::printf ("GAEService::updateGAECardUser_rfidcard() sendString[%s]\n", sendString.c_str());

            sendPOSTRequest("updatecarduser", sendString.c_str());

            sendcount = 0;
            sendstream.str("");
            sendstream.clear();
            sendstream<<"prefix=" << prefix;
            sendstream<<"&vilsuid=" << VILSUID;
            sendstream<<"&cardtype=rfidcard";
            sendstream<<"&data=[";
        }
    }

    if ( sendcount > 0 )
    {
        sendstream<<"]";
        std::string sendString = sendstream.str();

        // vilslog::printf ("GAEService::updateGAECardUser_rfidcard() sendString[%s]\n", sendString.c_str());

        sendPOSTRequest("updatecarduser", sendString.c_str());
    }

    return 0;
}

int GAEService::updateGAE_TAG_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList)
{
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
    {
        std::string target = PushNotifyPrefix;
        target.append("_TAG_");
        target.append((*existed)->macaddress);

        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*existed)->nodename, (*existed)->macaddress, 0, (*existed)->maingroupid, recordList);
        sendList->push_back(newTopic);
    }

    updateGAETargetTopicInfoList(PushNotifyPrefix, VILSUID, sendList);
    //int sendCount = updateGAETargetTopicInfoList(PushNotifyPrefix, sendList);
    // vilslog::printf("GAEService::updateGAE_TAG_TopicInfo() recordList->size()[%d] sendCount[%d]\n", (int)recordList->size(), sendCount);

    clear_TargetTopicStruct_list(sendList);
    delete sendList;

    return 0;
}

int GAEService::updateGAE_LOCATOR_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList)
{
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
    {
        std::string target = PushNotifyPrefix;
        target.append("_LOCATOR_");
        target.append((*existed)->macaddress);

        //update_to_GAE(projectid, target, (*existed)->nodename, recordList);

        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*existed)->nodename, (*existed)->macaddress, 0, (*existed)->maingroupid, recordList);
        sendList->push_back(newTopic);
    }

    updateGAETargetTopicInfoList(PushNotifyPrefix, VILSUID, sendList);
    //int sendCount = updateGAETargetTopicInfoList(PushNotifyPrefix, sendList);

    // vilslog::printf("GAEService::updateGAE_LOCATOR_TopicInfo() recordList->size()[%d] sendCount[%d]\n", (int)recordList->size(), sendCount);

    clear_TargetTopicStruct_list(sendList);
    delete sendList;

    return 0;
}

int GAEService::updateGAE_TAG_USER_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList)
{
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
    {
        std::string target = PushNotifyPrefix;
        target.append("_USER_");
        target.append((*existed)->macaddress);

        if ((*existed)->userid <= 0)
        {
            continue;
        }

        //update_to_GAE(projectid, target, (*existed)->nodename, recordList);

        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*existed)->nodename, (*existed)->macaddress, 0, (*existed)->maingroupid, recordList);
        sendList->push_back(newTopic);

    }

    updateGAETargetTopicInfoList(PushNotifyPrefix, VILSUID, sendList);
    //int sendCount = updateGAETargetTopicInfoList(PushNotifyPrefix, sendList);

    // vilslog::printf("GAEService::updateGAE_TAG_USER_TopicInfo() recordList->size()[%d] sendCount[%d]\n", (int)recordList->size(), sendCount);

    clear_TargetTopicStruct_list(sendList);
    delete sendList;

    return 0;
}

int GAEService::updateGAE_USER_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList, std::list<UserStruct*> * userList)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 1;

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        // std::string target = PushNotifyPrefix;
        // target.append("_USER_");
        // target.append(to_string((*user)->userid));
        //target.append((*user)->account);

        //update_to_GAE(projectid, target, (*user)->name, recordList);

        // vilslog::printf("GAEService::updateGAE_USER_TopicInfo() target[%s] account[%s]\n", target.c_str(), (*user)->account.c_str());
        int maingroup = 1;
        if ((*user)->maingroups->size() > 0)
        {
            std::list<std::string>::iterator maingroupIt = (*user)->maingroups->begin();
            maingroup = convert_string_to_int((*maingroupIt));
        }

        // 轉換 舊 topic 到新的
        TargetTopicStruct * newTopic = get_USER_newTopic(projectid, PushNotifyPrefix, (*user), maingroup, recordList);
        // TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*user)->name, (*user)->account, (*user)->userid, maingroup, recordList);

        // vilslog::printf("GAEService::updateGAE_USER_TopicInfo() name[%s] userid[%d] accountid[%s]\n", newTopic->name.c_str(), newTopic->userid, newTopic->accountid.c_str());

        sendList->push_back(newTopic);
    }

    updateGAETargetTopicInfoList(PushNotifyPrefix, VILSUID, sendList);
    //int sendCount = updateGAETargetTopicInfoList(PushNotifyPrefix, sendList);

    // vilslog::printf("GAEService::updateGAE_USER_TopicInfo() recordList->size()[%d] sendCount[%d]\n", (int)recordList->size(), sendCount);

    clear_TargetTopicStruct_list(sendList);
    delete sendList;


    return 0;
}

int GAEService::updateGAE_UHFCARD_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList, std::list<UserStruct*> * userList)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 1;

    std::list<RFIDCardStruct*> * cardList = baseDb->get_RFIDCards();

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<RFIDCardStruct*>::iterator card = cardList->begin(); card != cardList->end(); card++)
    {
        std::string target = PushNotifyPrefix;
        target.append("_CARD_");
        target.append((*card)->cardid);

        std::string cardname = (*card)->cardid;
        bool bFound = false;
        for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
        {
            for (std::list<std::string>::iterator rfidcard = (*user)->rfidcards->begin(); rfidcard != (*user)->rfidcards->end(); rfidcard++)
            {
                if ( (*card)->cardid.compare((*rfidcard)) == 0 )
                {
                    cardname = (*user)->name;
                    bFound = true;
                    break;
                }
            }
            if (bFound)
            {
                break;
            }
        }

        //update_to_GAE(projectid, target, cardname, recordList);

        TargetTopicStruct * newTopic = get_newTopic(projectid, target, cardname, (*card)->cardid, 0, (*card)->maingroupid, recordList);
        sendList->push_back(newTopic);
    }

    clear_RFIDCardStruct_list(cardList);
    delete cardList;

    updateGAETargetTopicInfoList(PushNotifyPrefix, VILSUID, sendList);
    //int sendCount = updateGAETargetTopicInfoList(PushNotifyPrefix, sendList);

    // vilslog::printf("GAEService::updateGAE_UHFCARD_TopicInfo() recordList->size()[%d] sendCount[%d]\n", (int)recordList->size(), sendCount);

    clear_TargetTopicStruct_list(sendList);
    delete sendList;

    return 0;
}

int GAEService::updateGAE_SUBGROUP_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 1;

    std::list<UserSubGroupStruct*> * userSubGroupList = baseDb->get_UserSubGroups();

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<UserSubGroupStruct*>::iterator subgroup = userSubGroupList->begin(); subgroup != userSubGroupList->end(); subgroup++)
    {
        std::string target = PushNotifyPrefix;
        target.append("_SUBGROUP_");
        target.append(std::to_string((*subgroup)->subgroupid));

        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*subgroup)->subgroupname, std::to_string((*subgroup)->subgroupid), 0, (*subgroup)->maingroupid, recordList);
        sendList->push_back(newTopic);
    }

    updateGAETargetTopicInfoList(PushNotifyPrefix, VILSUID, sendList);

    clear_UserSubGroupStruct_list(userSubGroupList);
    delete userSubGroupList;

    clear_TargetTopicStruct_list(sendList);
    delete sendList;

    return 0;
}

int GAEService::updateGAE_MAINGROUP_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 1;

    std::list<UserMainGroupStruct*> * userMainGroupList = baseDb->get_UserMainGroups();

    std::list<TargetTopicStruct*> * sendList = new std::list<TargetTopicStruct*>();

    for (std::list<UserMainGroupStruct*>::iterator maingroup = userMainGroupList->begin(); maingroup != userMainGroupList->end(); maingroup++)
    {
        std::string target = PushNotifyPrefix;
        target.append("_MAINGROUP_");
        target.append(std::to_string((*maingroup)->id));

        TargetTopicStruct * newTopic = get_newTopic(projectid, target, (*maingroup)->name, std::to_string((*maingroup)->id), 0, (*maingroup)->id, recordList);
        sendList->push_back(newTopic);
    }

    updateGAETargetTopicInfoList(PushNotifyPrefix, VILSUID, sendList);

    clear_UserMainGroupStruct_list(userMainGroupList);
    delete userMainGroupList;

    clear_TargetTopicStruct_list(sendList);
    delete sendList;

    return 0;
}

TargetTopicStruct * GAEService::get_existTopic(int projectid, std::string target, std::list<TargetTopicStruct*> * recordList)
{
    TargetTopicStruct * existTopic = NULL;

    // find existed topic record
    for (std::list<TargetTopicStruct*>::iterator record = recordList->begin(); record != recordList->end(); record++)
    {
        if ( target.compare((*record)->target) == 0 )
        {
            existTopic = new TargetTopicStruct();
            existTopic->target = (*record)->target;
            existTopic->topic = (*record)->topic;
            existTopic->name = (*record)->name;
            existTopic->accountid = (*record)->accountid;
            existTopic->maingroupid = (*record)->maingroupid;
            existTopic->userid = (*record)->userid;
            existTopic->datetime = (*record)->datetime;
            break;
        }
    }

    return existTopic;
}

TargetTopicStruct * GAEService::get_newTopic(int projectid, std::string target, std::string name, std::string accountid, int userid, int maingroup, std::list<TargetTopicStruct*> * recordList)
{
    TargetTopicStruct * newTopic = new TargetTopicStruct();

    // find exited topic record
    bool bFound = false;
    for (std::list<TargetTopicStruct*>::iterator record = recordList->begin(); record != recordList->end(); record++)
    {
        if ( target.compare((*record)->target) == 0 )
        {
            newTopic->target = (*record)->target;
            newTopic->topic = (*record)->topic;
            newTopic->name = (*record)->name;
            newTopic->accountid = (*record)->accountid;
            newTopic->maingroupid = (*record)->maingroupid;
            newTopic->userid = (*record)->userid;
            newTopic->datetime = (*record)->datetime;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        newTopic->topic = random_string(16);
    }
    newTopic->target = target;

    if ( newTopic->name.compare(name) != 0 )
    {
        newTopic->name = name;
        newTopic->accountid = accountid;
        newTopic->maingroupid = maingroup;
        newTopic->userid = userid;
        WorkItem * work = new WorkItem("TargetTopicStruct", projectid, newTopic);
        mMySQLJobQueue.add(work);
    }

    if ( newTopic->accountid.length() == 0 )
    {
        newTopic->accountid = accountid;
        newTopic->maingroupid = maingroup;
        newTopic->userid = userid;
        WorkItem * work = new WorkItem("TargetTopicStruct", projectid, newTopic);
        mMySQLJobQueue.add(work);
    }

    return newTopic;
}

TargetTopicStruct * GAEService::get_USER_newTopic(int projectid, std::string PushNotifyPrefix, UserStruct * user, int maingroup, std::list<TargetTopicStruct*> * recordList)
{
    std::string targetNew = PushNotifyPrefix;
    targetNew.append("_USER_");
    targetNew.append(to_string(user->userid));

    TargetTopicStruct * newTopic = get_existTopic(projectid, targetNew, recordList);

    if (newTopic == NULL)
    {
        std::string targetOld = PushNotifyPrefix;
        targetOld.append("_USER_");
        targetOld.append(user->account);

        TargetTopicStruct * oldTopic = get_existTopic(projectid, targetOld, recordList);
        if (oldTopic == NULL)
        {
            newTopic = get_newTopic(projectid, targetNew, user->name, user->account, user->userid, maingroup, recordList);
        }
        else
        {
            // copy topic
            newTopic = new TargetTopicStruct();
            newTopic->target = targetNew;
            newTopic->topic = oldTopic->topic;
            newTopic->name = user->name;
            newTopic->accountid = user->account;
            newTopic->maingroupid = maingroup;
            newTopic->userid = user->userid;

            vilslog::printf("GAEService::get_USER_newTopic() Copy Old Topic targetNew[%s] topic[%s]\n", targetNew.c_str(), oldTopic->topic.c_str());

            WorkItem * work = new WorkItem("TargetTopicStruct", projectid, newTopic);
            mMySQLJobQueue.add(work);

            delete oldTopic;
        }
    }
    else
    {
        // update name
        newTopic->target = targetNew;

        if ( newTopic->name.compare(user->name) != 0 )
        {
            newTopic->name = user->name;
            newTopic->accountid = user->account;
            newTopic->maingroupid = maingroup;
            newTopic->userid = user->userid;
            WorkItem * work = new WorkItem("TargetTopicStruct", projectid, newTopic);
            mMySQLJobQueue.add(work);
        }

        if ( newTopic->accountid.length() == 0 )
        {
            newTopic->accountid = user->account;
            newTopic->maingroupid = maingroup;
            newTopic->userid = user->userid;
            WorkItem * work = new WorkItem("TargetTopicStruct", projectid, newTopic);
            mMySQLJobQueue.add(work);
        }
    }
    return newTopic;
}


// int GAEService::updateTopicInfoName(int projectid)
// {
//     int count = 0;
//
//     return count;
// }

// bool GAEService::startSyncWithRetry(int projectid, int retry_Count)
// {
//     int count = 0;
//     bool bSuccess = false;
//     while(!bSuccess && count < retry_Count)
//     {
//         bSuccess = startSync(projectid);
//         sleep(2);
//         count++;
//     }
//
//     return true;
// }

std::string GAEService::startSync(int projectid, const char * type, const char * parameter)
{
    //bool bSuccess = false;
    vilslog::printf("GAEService::startSync() projectid[%d]\n", projectid);

    // http://47.100.182.173/scapi_ibl/api/searcher?type=1&keyword=%
    // http://mopsfundweb.appspot.com/fetchlastfund?action=hotfund
    // http://vils-iot-datacenter.appspot.com/

    //const char *host = "vils-iot-datacenter.appspot.com"; // 目標 URI
    //const char *command = "";

    const char *host = PSUH_NOTIFY_SERVER; // 目標 URI
    //const char *host = mERPServer.c_str();

    //const char *command = "scapi_ibl/api/searcher?type=1&keyword=%"; // 長者資料
    //const char *command = "scapi_ibl/api/searcher?type=2&keyword=%"; // 員工資料
    const char *command = "vilsapi"; // API
    const char *PORT_NUM = PSUH_NOTIFY_SERVER_PORT; // HTTP port

    char request[0xfff]; // 請求訊息
    const char *requestLineFmt = "GET /%s/?type=%s&%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    char buffer[1024];
    int bufferLen = 1024;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    //snprintf(buffer, bufferLen, requestLineFmt, "getmessagestatus", "messageid", "1587115188857821");
    snprintf(buffer, bufferLen, requestLineFmt, command, type, parameter);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    strcat(request, CRLF);

    // vilslog::printf("GAEService::startSync() projectid[%d] request[%s]\n", projectid, request);

    std::string http_response_string = sendRequest(host, PORT_NUM, request);
    // vilslog::printf("GAEService::startSync() projectid[%d] http_response_string[%s]\n", projectid, http_response_string.c_str());

    vilslog::printf("GAEService::startSync() projectid[%d] finished!\n", projectid);

    return http_response_string;
}

// bool GAEService::updateGAETargetTopicInfo(TargetTopicStruct * newTopic)
// {
//     char request[4096];
//     memset(request, 0, 4096);
//
//     //const char *MessageFmt = "type=updatetargettopic&target=%s&topic=%s&name=%s";
//     const char *MessageFmt = "type=updatetargettopic&target=%s&topic=%s&name=%s";
//
//     snprintf(request, 4096, MessageFmt, newTopic->target.c_str(), newTopic->topic.c_str(), newTopic->name.c_str());
//
//     return sendPOSTRequest("updatetargettopic", request);
// }

int GAEService::updateGAETargetTopicInfoList(std::string prefix, std::string VILSUID, std::list<TargetTopicStruct*> * topiclist)
{
    std::ostringstream sendstream;
    sendstream.str("");
    sendstream.clear();
    sendstream<<"prefix=" << prefix;
    sendstream<<"&vilsuid=" << VILSUID;
    sendstream<<"&data=[";

    int totalcount = 0;
    int sendcount = 0;
    for (std::list<TargetTopicStruct*>::iterator topic = topiclist->begin(); topic != topiclist->end(); topic++)
    {
        if (sendcount != 0)
        {
            sendstream<<",";
        }

        sendstream<<"{\"target\":\""<<(*topic)->target<<"\"";
        sendstream<<",\"topic\":\""<<(*topic)->topic<<"\"";
        sendstream<<",\"name\":\""<<(*topic)->name<<"\"";
        sendstream<<",\"accountid\":\""<<(*topic)->accountid<<"\"";
        sendstream<<"}";

        long length = sendstream.tellp();

        // vilslog::printf("GAEService::updateGAETargetTopicInfoList() length[%d]\n", length);

        totalcount++;
        sendcount++;
        if ( length >= 2048 )
        {
            sendstream<<"]";
            std::string sendString = sendstream.str();

            // vilslog::printf("GAEService::updateGAETargetTopicInfoList() sendString[%s]\n", sendString.c_str());

            sendPOSTRequest("updatetargettopiclist", sendString.c_str());

            sendcount = 0;
            sendstream.str("");
            sendstream.clear();
            sendstream<<"prefix=" << prefix;
            sendstream<<"&vilsuid=" << VILSUID;
            sendstream<<"&data=[";
        }
    }

    if ( sendcount > 0 )
    {
        sendstream<<"]";
        std::string sendString = sendstream.str();

        // vilslog::printf("GAEService::updateGAETargetTopicInfoList() sendString[%s]\n", sendString.c_str());

        sendPOSTRequest("updatetargettopiclist", sendString.c_str());
    }

    return totalcount;
}

bool GAEService::sendPOSTRequest(const char * type, const char * sendbody)
{
    const char *host = PSUH_NOTIFY_SERVER; // 目標 URI
    //const char *host = "test-dot-smimsvils.appspot.com"; // 目標 URI
    //const char *host = "ios-dot-smimsvils.appspot.com"; // 目標 URI
    const char *command = "vilsapi"; // API
    const char *PORT_NUM = PSUH_NOTIFY_SERVER_PORT; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    const char *requestLineFmt = "POST /%s/ HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    const char *headerConnection = "Connection: close\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *MessageFmt = "type=%s&%s";
    //const char *MessageFmt = "type=updatetargettopic&target=%s&topic=%s&name=%s";

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    //snprintf(buffer, bufferLen, requestLineFmt, host, PORT_NUM, command);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    strcat(request, ContentType);
    strcat(request, headerConnection);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);
    snprintf(sendmessage, 4096, MessageFmt, type, sendbody);

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);

    // vilslog::printf("GAEService::sendPOSTRequest() request[%s]\n", request);


    //
    // sen out request
    //
    std::string http_response_string = sendRequest(host, PORT_NUM, request);
    // if ( http_response_string.length() > 0 )
    // {
    //     return true;
    // }
    // else
    // {
    //     //vilslog::printf("sendPushNotification() failed!\n");
    //     return messageid;
    // }

    // vilslog::printf("GAEService::sendPOSTRequest() http_response_string[%s]\n", http_response_string.c_str());

    return true;
}

std::string GAEService::sendRequest(const char * host, const char * port, const char * request)
{
    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標
    char response[4096]; // 回應訊息
    std::string http_response_string = "";

    // 以 memset 清空 hints 結構
    //memset(&hints, 0, sizeof(struct addrinfo));
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    //hints.ai_family = AF_INET; // 使用 IPv4
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    //if ((gaiStatus = getaddrinfo(host, "http", &hints, &result)) != 0)
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        vilslog::printf("GAEService::sendRequest() getaddrinfo() failed!\n");
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }

    // 分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);
    if ( cfd < 0)
    {
        vilslog::printf("GAEService::sendRequest() socket() failed!\n");
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }

    // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        vilslog::printf("GAEService::sendRequest() connect() failed!\n");
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }

    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    //vilslog::printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) < 0)
    {
        vilslog::printf("GAEService::sendRequest() send() failed!\n");
        errPrint("Send");
        return http_response_string;
    }

    // 接收回應
    size_t size = 4096;
    size_t total = 0, n = 0;

    memset(response, 0, size);

    // 格式化輸出回應訊息
    // vilslog::printf ("----------\nResponse:\n");

    bool parse_header = true;
    bool chunked = false;
    int header_end_index = 0;
    std::list<HttpHeader*> * httpHeaderlist = NULL;

    // Transfer-Encoding: chunked\r\n
    // 如果是chunked, 則主體內容實際長度並不同Content-Length (最終長度是相同). 這種格式是將內容分成多個chunked, 每個chunked格式為:
    // chunked byte數\r\n
    // 實際資料\r\n
    // 注意都有\r\n喔. chunked byte數的格式為16進位字串

    while((n = recv(cfd, response, size, 0)) > 0)
    {
        total += n;

        // vilslog::printf ("\n-------\ntotal[%d] n[%d]\n", total, n);
        // vilslog::printf ("response[%c][%c][%c][%c][%c][%c][%c][%c]\n",
        // response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7]);
        // vilslog::printf ("response[%d][%d][%d][%d][%d][%d][%d][%d]\n",
        // response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7]);

        if (parse_header)
        {
            httpHeaderlist = parse_http_header(response, n);
            if (httpHeaderlist != NULL)
            {
                //
                // check if content chunked
                //
                for (std::list<HttpHeader *>::iterator header = httpHeaderlist->begin(); header != httpHeaderlist->end(); header++)
                {
                    if ((*header)->parameter.compare("Transfer-Encoding") == 0)
                    {
                        string::size_type found = (*header)->value.find("chunked", 0);
                        if (found != string::npos)
                        {
                            chunked = true;
                        }
                    }
                    else
                    if ((*header)->parameter.compare("header_end_index") == 0)
                    {
                        header_end_index = atoi((*header)->value.c_str());
                    }
                }
                parse_header = false;
            }
        }
        else
        {
            header_end_index = 0;
        }

        std::string content = "";
        if (chunked)
        {
            // f66\r\n
            std::string tmpStr = response + header_end_index;
            string::size_type chunkedEnd = tmpStr.find("\r\n", 0);

            if (chunkedEnd != string::npos)
            {
                //printf ("chunkedEnd[%d] != string::npos\n", (int)chunkedEnd);
                if (chunkedEnd > 6)
                {
                    // maybe not content  chunked size info
                    response[(int)chunkedEnd] = '\0';

                    content = response + header_end_index;
                }
                else
                {
                    content = response + header_end_index + chunkedEnd + 2;
                }
            }
            else
            {
                //printf ("chunkedEnd == string::npos\n");
                content = response + header_end_index;
            }
        }
        else
        {
            //printf ("!chunkedEnd\n");
            content = response + header_end_index;
        }

        // vilslog::printf ("\n-------\ncontent:%s\n---------------\n", content.c_str());

        http_response_string.append(content);

        //printf ("\n-------------------\nresponse:%d\n%s\n------------------\n",
        //   (int)http_response_string.length(), http_response_string.c_str());

        if (n >= 4)
        {
            // read not use data
            //if ( response[0] == '\n')


            // vilslog::printf ("response[%c][%c][%c][%c][%c][%c][%c][%c]\n",
            // response[n-8], response[n-7], response[n-6], response[n-5],
            // response[n-4], response[n-3], response[n-2], response[n-1]);
            // vilslog::printf ("response[%d][%d][%d][%d][%d][%d][%d][%d]\n",
            // response[n-8], response[n-7], response[n-6], response[n-5],
            // response[n-4], response[n-3], response[n-2], response[n-1]);


            //
            // find http end \r\n\r\n
            //
            //if (response[n-4] == '\r' && response[n-3] == '\n' &&response[n-2] == '\r' &&response[n-1] == '\n')
            if (response[n-2] == '\r' &&response[n-1] == '\n')
            {
                //vilslog::printf ("Found Http end!!!\n");
                break;
            }
        }

        memset(response, 0, size);
    }

    // vilslog::printf ("Final total[%d] length[%d]\n", total, http_response_string.length());
    // vilslog::printf ("\n-------------------\nresponse:\n%s\n------------------\n", http_response_string.c_str());


    /*
    string::size_type found = http_response_string.find("{", 0);
    if (found != string::npos)
    {
        std::string http_response_json = http_response_string.substr(found);

        // vilslog::printf ("http_response_json.length[%d]\n", http_response_json.length());
        // printf ("\n-------------------\n%s\n-------------------\n", http_response_json.c_str());

        Json::Reader * reader = new Json::Reader();
        Json::Value JSONresult;
        bool parsingSuccessful = false;

        try{
            parsingSuccessful = reader->parse( http_response_json.c_str(), JSONresult);

            if (parsingSuccessful)
            {
                if ( JSONresult.isMember("key") )
                {
                    vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] key[%s]\n",
                       parsingSuccessful, JSONresult["key"].asCString());
                }

                if ( JSONresult.isMember("messageid") )
                {
                    vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] messageid[%s]\n",
                       parsingSuccessful, JSONresult["messageid"].asCString());
                }

                if ( JSONresult.isMember("token") )
                {
                    vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] token[%s]\n",
                       parsingSuccessful, JSONresult["token"].asCString());
                }

                if ( JSONresult.isMember("read") )
                {
                    vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] read[%s]\n",
                       parsingSuccessful, JSONresult["read"].asCString());
                }

                if ( JSONresult.isMember("finished") )
                {
                    vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] finished[%s]\n",
                       parsingSuccessful, JSONresult["finished"].asCString());
                }

                if ( JSONresult.isMember("datetime") )
                {
                    vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] datetime[%s]\n",
                       parsingSuccessful, JSONresult["datetime"].asCString());
                }

                if ( JSONresult.isMember("comment") )
                {
                    vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] comment[%s]\n",
                       parsingSuccessful, JSONresult["comment"].asCString());
                }

                if ( JSONresult.isMember("results") )
                {
                    if ( JSONresult["results"].isArray() )
                    {
                        for(int i=0;i<(int)JSONresult["results"].size();i++)
                        {
                            Json::Value result = JSONresult["results"][i];
                            vilslog::printf ("GAEService::sendRequest() results[%d][%s]\n", i, result.asCString());
                        }
                    }
                }

                bSuccess = true;
                // else
                // if ( JSONresult.isMember("success") )
                // {
                //      vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d] success[%d]\n",
                //         parsingSuccessful, JSONresult["success"].asBool());
                //
                //      //
                //      // update data
                //      //
                //      if ( JSONresult["success"].asBool() )
                //      {
                //          bSuccess = update_data(projectid, JSONresult);
                //      }
                //      else
                //      {
                //          bSuccess = false;
                //      }
                // }
                // else
                // {
                //     vilslog::printf ("GAEService::sendRequest() can not find \"success\" \n");
                //     bSuccess = false;
                // }
            }
            else
            {
                vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d]\n", parsingSuccessful);
                bSuccess = false;
            }
        }
        catch(exception exc)
        {
            vilslog::printf ("GAEService::sendRequest() reader->parse() catch exception[%s]\n", exc.what());
            bSuccess = false;
        }

        delete reader;
    }
    */

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // if (recv(cfd, response, 0xffff, 0) < 0)
    //    errPrint("Receive");

    // 格式化輸出回應訊息
    // vilslog::printf ("----------\nResponse:\n----------\n%s\n", response);


    // Json::Reader * reader = new Json::Reader();
    // Json::Value JSONresult;
    // bool parsingSuccessful = false;
    //
    // try{
    //     parsingSuccessful = reader->parse( response, JSONresult);
    // }
    // catch(...)
    // {
    //     vilslog::printf ("GAEService::sendRequest() reader->parse() catch exception...\n");
    // }
    //
    // if (parsingSuccessful)
    // {
    //     if ( JSONresult.isMember("success") )
    //     {
    //          vilslog::printf ("GAEService::sendRequest() success[%s]\n", JSONresult["success"].asCString());
    //     }
    // }
    // else
    // {
    //     vilslog::printf ("GAEService::sendRequest() parsingSuccessful[%d]\n", parsingSuccessful);
    // }
    //
    // delete reader;

    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return http_response_string;

}


std::list<HttpHeader*> * GAEService::parse_http_header(const char * response, size_t n)
{
    // HTTP/1.1 200 OK\r\n
    // Server: nginx/1.10.3 (Ubuntu)\r\n
    // Date: Fri, 16 Aug 2019 01:36:41 GMT\r\n
    // Content-Type: application/json; charset=utf-8\r\n
    // Transfer-Encoding: chunked\r\n
    // Connection: keep-alive\r\n
    // Access-Control-Allow-Origin: *\r\n
    // \r\n
    // f66\r\n
    // {"success"...

    char buffer[4096];
    std::list<HttpHeader*> * httpHeaderlist = NULL;

    memset(buffer, 0, 4096);
    int datalen = get_line(response, (int)n, 0, buffer, 4096);
    buffer[datalen] = 0;

    if (response[0] == 'H' && response[1] == 'T' && response[2] == 'T' && response[3] == 'P')
    {
        httpHeaderlist = new std::list<HttpHeader*>();
    }
    else
    {
        return NULL;
    }

    int start_idx = 0;
    bool bFinished = false;
    while(!bFinished)
    {
        memset(buffer, 0, 4096);
        int datalen = get_line(response, (int)n, start_idx, buffer, 4096);
        buffer[datalen] = 0;

        // pair 屬性名稱: 屬性值\r\n
        std::string bufString = buffer;
        string::size_type loc = bufString.find(":", 0);
        if (loc != string::npos)
        {
            HttpHeader * header = new HttpHeader();

            header->parameter = bufString.substr(0, loc);
            header->value = bufString.substr(loc+2, bufString.length() - 2 - 2);

            // vilslog::printf ("GAEService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            httpHeaderlist->push_back(header);
        }

        start_idx += datalen;

        if(datalen == 0)
        {
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("GAEService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        if(datalen == 2)
        {
            // header end \r\n\r\n
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("GAEService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        {
            //vilslog::printf ("GAEService::parse_http_header() buffer[%s]\n", buffer);
        }
    }
    return httpHeaderlist;
}

int GAEService::get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len)
{
    int length = 0;
    bool bFound = false;
    // find 0x10 0x13
    for (int i = start_idx; i < data_len; i++)
    {
        if(length < buffer_len)
        {
            linebuffer[length] = databuffer[i];
            length++;
        }

        if (databuffer[i] == '\r')
        {
            if ((i+1) < (data_len))
            {
                if (databuffer[i+1] == '\n')
                {
                    linebuffer[length] = databuffer[i];
                    length++;
                    bFound = true;
                }
            }
        }

        if(bFound)
            break;

    }

    return length;
}

void GAEService::errExit(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    printf("GAEService::errExit() Error: %s\n", buff.c_str());
    exit(EXIT_FAILURE);
}

void GAEService::errPrint(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    vilslog::printf("GAEService::errPrint() Error: %s\n", buff.c_str());
}

bool GAEService::update_data(int projectid, Json::Value JSONresult)
{
    Json::Value dataValue(Json::arrayValue);

    if ( JSONresult.isMember("data") )
    {
        dataValue = JSONresult.get("data", dataValue);

        if (dataValue.isArray())
        {
            //vilslog::printf ("GAEService::update_data() dataValue.isArray()\n");
        }
        else
        {
            vilslog::printf ("GAEService::update_data() dataValue is not Array\n");
            return false;
        }
    }
    else
    {
        return false;
    }

    int count = dataValue.size();
    for(int i=0;i<count;i++)
    {
        Json::Value data = dataValue[i];

        vilslog::printf ("GAEService::update_data() count[%d] i[%d]\n", count, i);
    }

    return true;
}

std::string GAEService::getJSONString(Json::Value JSONresult, const char * key)
{
    std::string value = "";

    if ( JSONresult.isMember(key) && !JSONresult[key].empty())
        value = JSONresult[key].asString();

    return value;
}

int GAEService::getJSONInt(Json::Value JSONresult, const char * key)
{
    int value = -1;

    if ( JSONresult.isMember(key) && !JSONresult[key].empty())
        value = JSONresult[key].asInt();

    return value;
}

NodeStruct * GAEService::parseNodeInfo(int projectid, std::string mine_prefix, std::string target)
{
    NodeStruct * nodeinfo = NULL;

    std::vector<std::string> nodeinfoVector = convert_string_to_vector(target, '_');

    if ( (int)nodeinfoVector.size() >= 3)
    {
        std::string prefix = nodeinfoVector[0];

        for(int i=1;i<(int)nodeinfoVector.size() - 2;i++)
        {
            if ( i > 0)
            {
                prefix.append("_");
            }
            prefix.append(nodeinfoVector[i]);
        }

        // vilslog::printf ("GAEService::parseNodeInfo() projectid[%d] mine_prefix[%s] prefix[%s]\n", projectid, mine_prefix.c_str(), prefix.c_str());

        if (mine_prefix.compare(prefix) != 0)
        {
            // vilslog::printf ("GAEService::parseNodeInfo() mine_prefix.compare(prefix) != 0\n");

            return NULL;
        }

        std::string macaddress = nodeinfoVector[(int)nodeinfoVector.size() - 1];
        std::string type = nodeinfoVector[(int)nodeinfoVector.size() - 2];

        if ( (type.compare("TAG") == 0) || (type.compare("USER") == 0) )
        {
            nodeinfo = mDeviceManager->get_Tag_node_ByMacaddress(projectid, macaddress.c_str());
        }
        else
        if ( type.compare("LOCATOR") == 0)
        {
            nodeinfo = mDeviceManager->get_Locator_node_ByMacaddress(projectid, macaddress.c_str());
        }
    }
    nodeinfoVector.clear();
    return nodeinfo;
}

std::list<std::string> * GAEService::getSubgGroupManager(int projectid, UserStruct * user)
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
