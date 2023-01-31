/*
 *  KIAService.h
 *
 *  Created on: 2021/04/27
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

#include <curl/curl.h>

#include "KIAService.h"

using namespace std;

static size_t WCInfoWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

KIAService::KIAService(wqueue<WorkItem*> & MySQLJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, DBManager * dbManager)
    : mMySQLJobQueue(MySQLJobQueue), mNotifiactionQueue(NotifiactionQueue), mDbManager(dbManager)
{
}

KIAService::~KIAService()
{
}

bool KIAService::status()
{
    return mInitial;
}

std::string KIAService::statusString()
{
    return mStatus;
}

void * KIAService::run()
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
            vilslog::printf("KIAService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "KIAService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void KIAService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long updateyWCInfoTime = 6000;

    mWCInfoList = new std::list<WCInfoStruct*>();

    mInitial = true;

    mStatus = "KIAService()::run() Initial Courses successfully!";
    vilslog::printf ("KIAService()::run() Initial Courses successfully!\n");

    while (1)
    {
        end1 = getSystemTime();
        difft = end1 - start1;

        updateyWCInfoTime += difft;

        if ( updateyWCInfoTime >= 6000) //  1 min
        {
            updateWCInfo();

            sendNotify();

            updateyWCInfoTime = 0;
        }

        start1 = end1;
        usleep(10000);
    }
}

void KIAService::updateWCInfo()
{
    CURL *curl;
    CURLcode res;
    std::string readBufferWC;

    curl = curl_easy_init();
    if(curl)
    {
        // First set the URL that is about to receive our POST. This URL can
        // just as well be a https:// URL if that is what should receive the
        // data.
        curl_easy_setopt(curl, CURLOPT_URL, "https://ccc.kia.gov.tw/WC/json/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WCInfoWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBufferWC);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);

        // always cleanup
        curl_easy_cleanup(curl);

        updateWCInfoArr(readBufferWC);
    }
}

void KIAService::updateWCInfoArr(std::string content)
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    // std::string pureCont = remove_nonJSON(content.c_str());

    try{
        parsingSuccessful = reader->parse( content, JSONresult);

        // vilslog::printf ("KIAService::updateWCInfoArr() content[%s]\n", content.c_str());

        if (parsingSuccessful)
        {
            if ( JSONresult.isArray() )
            {
                for(unsigned int i=0;i<JSONresult.size();i++)
                {
                    Json::Value tmp;
                    Json::Value item = JSONresult.get((Json::UInt)i, tmp);

                    WCInfoStruct * newInfo = new WCInfoStruct();
                    if ( item.isMember("GID") )
                        newInfo->GID = item["GID"].asString();
                    if ( item.isMember("place") )
                        newInfo->place = item["place"].asString();
                    if ( item.isMember("item") )
                        newInfo->item = item["item"].asString();
                    if ( item.isMember("notetime") )
                        newInfo->notetime = item["notetime"].asString();

                    // vilslog::printf ("KIAService::updateWCInfoArr() newInfo->GID[%s] place[%s] item[%s] notetime[%s]\n",
                    //     newInfo->GID.c_str(), newInfo->place.c_str(), newInfo->item.c_str(), newInfo->notetime.c_str());

                    putToWCInfoList(newInfo);

                    delete newInfo;
                }
            }
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("KIAService::updateWCInfoArr() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;
}

void KIAService::sendNotify()
{
    for (std::list<WCInfoStruct*>::iterator info = mWCInfoList->begin(); info != mWCInfoList->end(); info++)
    {
        if ((*info)->sendNotify == 0)
        {
            //
            // Load from database
            //
            int projectid = 1;
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb != NULL)
            {
                (*info)->sendNotify = baseDb->get_WCInfoSendNotifyStatus((*info)->GID);
            }

            if ((*info)->sendNotify == 0)
            {
                // send notify
                // newInfo->place  = 人員帳號
                // newInfo->item = 待辦事項 -> 事項名稱
                //if ((*info)->GID.compare("7204") == 0)
                //{
                    vilslog::printf ("KIAService::sendNotify() GID[%s] place[%d] sendNotify[%d]\n",
                        (*info)->GID.c_str(), (*info)->place.c_str(), (*info)->sendNotify);

                    UserStruct * user = baseDb->getUser((*info)->place);
                    if (user != NULL)
                    {
                        std::list<std::string> * sendtargetlist = new std::list<std::string>();
                        std::list<std::string> targetlist;
                        std::list<std::string> subgrouptargetlist;
                        std::list<std::string> subgrouplist;
                        std::list<std::string> surveylist;
                        std::list<std::string> deadlinelist;
                        std::string accountid = (*info)->place;
                        std::string userid = std::to_string(user->userid);
                        std::string maingroups = convert_list_to_string(user->maingroups);
                        std::string title = "";
                        std::string context = "";
                        std::string imageuid = "";
                        std::string randomKey = random_string(16);
                        int type = MULTI_PUSHNOTIFY_FUNCTION_204; // 手動群發 -> 工作指示

                        sendtargetlist->push_back(userid);
                        targetlist.push_back(userid);

                        SurveyStruct * survey = baseDb->readSurveyFromTitle((*info)->item);
                        if ( survey != NULL )
                        {
                            title = survey->title;
                            context = survey->description;
                            surveylist.push_back(std::to_string(survey->surveyid));
                            deadlinelist.push_back(currentDateTimeForSQL(-10));
                            delete survey;
                        }

                        NotificationItem * notifactionitem = new NotificationItem(type, projectid, sendtargetlist, targetlist, subgrouplist, surveylist, deadlinelist, accountid, userid, maingroups, title, context, randomKey);
                        notifactionitem->m_formid = (*info)->GID;
                        mNotifiactionQueue.add(notifactionitem);

                        (*info)->sendNotify = 1;
                        putWCInfoToDB((*info));
                    }
                //}
            }
        }
    }
}

void KIAService::putToWCInfoList(WCInfoStruct * newInfo)
{
    bool bFound = false;
    for (std::list<WCInfoStruct*>::iterator info = mWCInfoList->begin(); info != mWCInfoList->end(); info++)
    {
        if ((*info)->GID.compare(newInfo->GID) == 0)
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        int projectid = 1;
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            WCInfoStruct * info = new WCInfoStruct();
            info->GID = newInfo->GID;
            info->place = newInfo->place;
            info->item = newInfo->item;
            info->notetime = newInfo->notetime;
            info->sendNotify = baseDb->get_WCInfoSendNotifyStatus(info->GID);
            info->maingroupid = 1;

            mWCInfoList->push_back(info);

            // vilslog::printf ("KIAService::putToWCInfoList() GID[%s] place[%d] sendNotify[%d]\n",
            //     info->GID.c_str(), info->place.c_str(), info->sendNotify);

            putWCInfoToDB(info);
        }
    }
}

void KIAService::putWCInfoToDB(WCInfoStruct * newInfo)
{
    int projectid = 1;
    int maingroupid = 1;

    WCInfoStruct * info = new WCInfoStruct();
    info->GID = newInfo->GID;
    info->place = newInfo->place;
    info->item = newInfo->item;
    info->notetime = newInfo->notetime;
    info->sendNotify = newInfo->sendNotify;
    info->maingroupid = maingroupid;

    WorkItem * witem = new WorkItem("wcinfo", projectid, newInfo);
    mMySQLJobQueue.add(witem);
    delete info;
}
