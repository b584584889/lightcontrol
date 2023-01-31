/*
 *  CheckEmployeeActionService.cpp
 *
 *  Created on: 2019/10/15
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>

#include "CheckEmployeeActionService.h"

#include "../common.h"
using namespace std;

CheckEmployeeActionService::CheckEmployeeActionService(wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{

}

bool CheckEmployeeActionService::status()
{
    return mInitial;
}

std::string CheckEmployeeActionService::statusString()
{
    return mStatus;
}

void CheckEmployeeActionService::updateProject()
{
    mUpdateProject = true;
}

void * CheckEmployeeActionService::run()
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
            vilslog::printf("CheckEmployeeActionService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "CheckEmployeeActionService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CheckEmployeeActionService::runloop()
{
    mInitial = false;

    mStatus = "CheckEmployeeActionService()::run() Initial successfully!";
    vilslog::printf ("CheckEmployeeActionService()::run() Initial successfully!\n");

    mInitial = true;

    waitloop();
}

void CheckEmployeeActionService::waitloop()
{
    long long start1 = getSystemTime();
    long long checkEmployeeActionTime = 50000;
    long long dumpTime = 0;
    long long updatePrjIdTime = 0;

    mPrjidList = mDbManager->getAllProjectConfig();

    while ( 1 )
    {
        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        //printf("wait_loop() time[%lld ms]\n", difft);

        checkEmployeeActionTime += difft;
        dumpTime += difft;
        updatePrjIdTime += difft;

        if (checkEmployeeActionTime >= 60000) // 60 seconds
        {
            //vilslog::printf ("CheckEmployeeActionService()::waitloop() checkEmployeeAction start...\n");
            // long long checkEmployee_start = getSystemTime();

            checkEmployeeAction();

            // long long checkDeviceType_start = getSystemTime();

            // long long check_end = getSystemTime();
            // long long checkEmployee_difft = check_end - checkEmployee_start;

            // checkDeviceTypeAction(5); // 5:醫師 6:護理師
            //
            // check_end = getSystemTime();
            // long long checkDeviceType_difft = check_end - checkDeviceType_start;
            // vilslog::printf ("CheckEmployeeActionService()::waitloop() time checkEmployee_difft[%lld] checkDeviceType_difft[%lld]\n",
            //     checkEmployee_difft, checkDeviceType_difft);

            //vilslog::printf ("CheckEmployeeActionService()::waitloop() checkEmployeeAction Finished!\n");
            checkEmployeeActionTime = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        if (dumpTime >= 10000) // 10 seconds
        {
            dumpTime = 0;
        }
        start1 = end1;
        sleep(10);
    }
}

void CheckEmployeeActionService::checkDeviceTypeAction(int type)
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        checkDeviceTypeAction(projectid, type);
    }
}

void CheckEmployeeActionService::checkDeviceTypeAction(int projectid, int type)
{
    std::string current_datetime = currentDateTimeForSQL();
    std::string one_day_ago = currentDateTimeForSQL(1);

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return;

    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
    {
        if ( (*existed)->tafid == type )
        {
            std::list<AreaInOutEventStruct*> * nodeAreaInOutList = baseDb->getNodeAreaInOutList((*existed)->nodeid, one_day_ago.c_str(), current_datetime.c_str(), "DESC");

            vilslog::printf ("CheckEmployeeActionService::checkDeviceTypeAction() nodeid[%s] nodeAreaInOutList->size()[%d]\n",
                (*existed)->nodeid, (int)nodeAreaInOutList->size());

            std::list<Pair_AreaInOutEventStruct*> * pair_inout_events = pair_inout_event(nodeAreaInOutList);

            vilslog::printf ("CheckEmployeeActionService::checkDeviceTypeAction() nodeid[%s] pair_inout_events->size()[%d]\n",
                (*existed)->nodeid, (int)pair_inout_events->size());

            //
            // output
            //
            output_inout_pairevent(projectid, pair_inout_events);

            clear_Pair_AreaInOutEventStruct_list(pair_inout_events);
            delete pair_inout_events;

            clear_AreaInOutEventStruct_list(nodeAreaInOutList);
            delete nodeAreaInOutList;
        }
    }
}

std::list<Pair_AreaInOutEventStruct*> * CheckEmployeeActionService::pair_inout_event(std::list<AreaInOutEventStruct*> * nodeAreaInOutList)
{
    std::list<Pair_AreaInOutEventStruct*> * pair_inout_events = new std::list<Pair_AreaInOutEventStruct*>();

    for (std::list<AreaInOutEventStruct*>::iterator event = nodeAreaInOutList->begin(); event != nodeAreaInOutList->end(); event++)
    {
        // vilslog::printf ("CheckEmployeeActionService::checkDeviceTypeAction() AreaID[%s] AreaName[%s] action[%d] DataTime[%s]\n",
        //     (*event)->AreaID.c_str(), (*event)->AreaName.c_str(), (*event)->action, (*event)->DataTime.c_str());

        if ( (*event)->action == 2 ) // "out" action
        {
            // find "in" action
            bool bFound = false;
            for (std::list<AreaInOutEventStruct *>::iterator check_event = std::next(event, 1); check_event != nodeAreaInOutList->end();)
            {
                if ( (*event)->AreaID.compare((*check_event)->AreaID) == 0)
                {
                    if ( (*check_event)->action == 1)
                    {
                        Pair_AreaInOutEventStruct * pairEvent = new Pair_AreaInOutEventStruct();
                        pairEvent->type = "pair";
                        pairEvent->event1 = copyAreaInOutEventStruct((*check_event));
                        pairEvent->event2 = copyAreaInOutEventStruct((*event));
                        pair_inout_events->push_back(pairEvent);

                        // remove from list
                        AreaInOutEventStruct * old_event = (*check_event);
                        check_event = nodeAreaInOutList->erase(check_event);
                        delete old_event;

                        bFound = true;
                        break;
                    }
                    else
                    {
                        // not found
                        break;
                    }
                }
                else
                {
                    check_event++;
                }
            }

            if (!bFound)
            {
                //vilslog::printf("CheckEmployeeActionService::pair_event() output single firedeventid[%d]\n", (*event)->firedeventid);
                Pair_AreaInOutEventStruct * singleEvent = new Pair_AreaInOutEventStruct();
                singleEvent->type = "single";
                singleEvent->event1 = copyAreaInOutEventStruct((*event));
                singleEvent->event2 = NULL;
                pair_inout_events->push_back(singleEvent);
            }
        }
        else
        {
            Pair_AreaInOutEventStruct * singleEvent = new Pair_AreaInOutEventStruct();
            singleEvent->type = "single";
            singleEvent->event1 = copyAreaInOutEventStruct((*event));
            singleEvent->event2 = NULL;
            pair_inout_events->push_back(singleEvent);
            continue;
        }
    }

    return pair_inout_events;
}

void CheckEmployeeActionService::output_inout_pairevent(int projectid, std::list<Pair_AreaInOutEventStruct*> * pair_inout_events)
{
    for (std::list<Pair_AreaInOutEventStruct *>::iterator event = pair_inout_events->begin(); event != pair_inout_events->end(); event++)
    {
        if ( (*event)->type.compare("single") == 0 )
        {
            create_inout_single_event(projectid, (*event)->event1);
        }
        else
        if ( (*event)->type.compare("pair") == 0 )
        {
            create_inout_pair_event(projectid, (*event)->event1, (*event)->event2);
        }
    }
}

void CheckEmployeeActionService::create_inout_single_event(int projectid, AreaInOutEventStruct * event)
{
    vilslog::printf ("CheckEmployeeActionService::create_inout_single_event() AreaID[%s] AreaName[%s] action[%d] DataTime[%s]\n",
        event->AreaID.c_str(), event->AreaName.c_str(), event->action, event->DataTime.c_str());
}

void CheckEmployeeActionService::create_inout_pair_event(int projectid, AreaInOutEventStruct * event_in, AreaInOutEventStruct * event_out)
{
    // 123
    time_t t_in = StringDateTimeTotime_t(event_in->DataTime);
    time_t t_out = StringDateTimeTotime_t(event_out->DataTime);
    time_t t_diff = t_out - t_in;
    std::string total_time = time_tToTimerString(t_diff);

    vilslog::printf ("CheckEmployeeActionService::create_inout_pair_event() AreaID[%s] AreaName[%s] action[%d] DataTime[%s] action[%d] DataTime[%s] total_time[%s]\n",
        event_in->AreaID.c_str(), event_in->AreaName.c_str(), event_in->action, event_in->DataTime.c_str(),
        event_out->action, event_out->DataTime.c_str(), total_time.c_str());

}

void CheckEmployeeActionService::checkEmployeeAction()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        checkEmployeeAction(projectid);
    }
}

void CheckEmployeeActionService::checkEmployeeAction(int projectid)
{
    std::string current_datetime = currentDateTimeForSQL();
    std::string thirty_day_ago = currentDateTimeForSQL(30);
    std::string one_day_ago = currentDateTimeForSQL(1);

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return;

    //std::list<FiredEventStruct*> * firedEventList = baseDb->read_fired_events_DayRange("9999", thirty_day_ago.c_str(), current_datetime.c_str(), "ASC");
    std::list<FiredEventStruct*> * firedEventList = baseDb->read_fired_events_DayRange("9999", one_day_ago.c_str(), current_datetime.c_str(), "ASC");
    //vilslog::printf ("CheckEmployeeActionService::checkEmployeeAction() firedEventList->size()[%d]\n", (int)firedEventList->size());

    std::list<FiredEventStruct*> * merge_events = merge_same_event(firedEventList);
    //vilslog::printf ("CheckEmployeeActionService::checkEmployeeAction() merge_events->size()[%d]\n", (int)merge_events->size());

    std::list<Pair_FiredEventStruct*> * pair_events = pair_event(merge_events);
    //vilslog::printf ("CheckEmployeeActionService::checkEmployeeAction() pair_events->size()[%d]\n", (int)pair_events->size());

    std::list<Pair_FiredEventStruct*> * same_tagaction_events = merge_same_tagaction_event(pair_events);

    output_pairevent(projectid, same_tagaction_events);

    clear_FiredEventStruct_list(firedEventList);
    delete firedEventList;

    clear_FiredEventStruct_list(merge_events);
    delete merge_events;

    clear_Pair_FiredEventStruct_list(pair_events);
    delete pair_events;

    clear_Pair_FiredEventStruct_list(same_tagaction_events);
    delete same_tagaction_events;
}

std::list<FiredEventStruct*> * CheckEmployeeActionService::merge_same_event(std::list<FiredEventStruct*> * firedEventList)
{
    std::list<FiredEventStruct*> * merge_events = new std::list<FiredEventStruct*>();

    for (std::list<FiredEventStruct *>::iterator event = firedEventList->begin(); event != firedEventList->end(); event++)
    {
        Json::Value JSONresult;
        bool parsingSuccessful = parseJSON((*event)->eventjson.c_str(), JSONresult);
        if (!parsingSuccessful)
        {
            continue;
        }

        std::string eventid = "";
        if ( JSONresult.isMember("eventid") )
            eventid = JSONresult["eventid"].asString();
        else
        {
            // checkpointgroup event
            //vilslog::printf("CheckEmployeeActionService::merge_same_event() output firedeventid[%d]\n", (*event)->firedeventid);

            FiredEventStruct * newevent = copyFiredEventStruct((*event));
            merge_events->push_back(newevent);
            continue;
        }

        if ( eventid.compare("9999") == 0 )
        {
            std::string cur_event_nodeid = JSONresult["nodeid"].asString();
            std::string cur_event_action = JSONresult["action"].asString();
            std::string cur_event_locatorid = JSONresult["locatorid"].asString();
            std::string cur_event_datetime = (*event)->datetime;

            for (std::list<FiredEventStruct *>::iterator check_event = std::next(event, 1); check_event != firedEventList->end();)
            {
                Json::Value check_JSONresult;
                bool parsingSuccessful = parseJSON((*check_event)->eventjson.c_str(), check_JSONresult);
                if (!parsingSuccessful)
                {
                    check_event++;
                    continue;
                }

                std::string eventid = "";
                if ( check_JSONresult.isMember("eventid") )
                {
                    eventid = check_JSONresult["eventid"].asString();
                    if ( eventid.compare("9999") != 0 )
                    {
                        check_event++;
                        continue;
                    }
                }
                else
                {
                    check_event++;
                    continue;
                }

                // check same event
                std::string check_event_nodeid = check_JSONresult["nodeid"].asString();
                std::string check_event_action = check_JSONresult["action"].asString();
                std::string check_event_locatorid = check_JSONresult["locatorid"].asString();
                std::string check_event_datetime = (*check_event)->datetime;

                if ( (cur_event_nodeid.compare(check_event_nodeid) == 0) &&
                     (cur_event_action.compare(check_event_action) == 0) &&
                     (cur_event_locatorid.compare(check_event_locatorid) == 0) )
                {
                    time_t cur_datetime = StringDateTimeTotime_t(cur_event_datetime);
                    time_t check_datetime = StringDateTimeTotime_t(check_event_datetime);
                    int diffSeconds = (int)(check_datetime - cur_datetime);

                    int timeout = 60; // one minites
                    if ( check_event_action.compare("Cancel") == 0 )
                       timeout = 90;

                    if (diffSeconds < timeout)
                    {
                        // keep as same event
                        (*event)->firstdatetime = check_event_datetime;

                        // remove from list
                        FiredEventStruct * old_event = (*check_event);
                        check_event = firedEventList->erase(check_event);

                        // vilslog::printf("CheckEmployeeActionService::merge_same_event() remove firedeventid[%d] action[%s]\n",
                        //     old_event->firedeventid, check_event_action.c_str());
                        delete old_event;
                    }
                    else
                    {
                        // keep different event, break this loop.
                        break;
                    }
                }
                else
                if ( (cur_event_action.compare("Cancel") == 0) &&
                     (cur_event_action.compare(check_event_action) == 0) &&
                     (cur_event_locatorid.compare(check_event_locatorid) == 0) )
                {
                    // same locator same action
                    // keep same event
                    // remove from list
                    FiredEventStruct * old_event = (*check_event);
                    check_event = firedEventList->erase(check_event);
                    delete old_event;
                }
                else
                if ( (cur_event_action.compare(check_event_action) != 0) &&
                     (cur_event_locatorid.compare(check_event_locatorid) == 0) )
                {
                    // same locator but different action
                    // keep different event
                    break;
                }
                else
                {
                    check_event++;
                }
            }
            //vilslog::printf("CheckEmployeeActionService::merge_same_event() output firedeventid[%d]\n", (*event)->firedeventid);

            FiredEventStruct * newevent = copyFiredEventStruct((*event));
            merge_events->push_back(newevent);
        }
        else
        {
            //vilslog::printf("CheckEmployeeActionService::merge_same_event() output firedeventid[%d]\n", (*event)->firedeventid);

            FiredEventStruct * newevent = copyFiredEventStruct((*event));
            merge_events->push_back(newevent);
        }
    }

    return merge_events;
}

std::list<Pair_FiredEventStruct*> * CheckEmployeeActionService::pair_event(std::list<FiredEventStruct*> * merge_events)
{
    std::list<Pair_FiredEventStruct*> * pair_events = new std::list<Pair_FiredEventStruct*>();
    for (std::list<FiredEventStruct *>::iterator event = merge_events->begin(); event != merge_events->end(); event++)
    {
        // vilslog::printf("CheckEmployeeActionService::pair_event() firedeventid[%d] eventjson[%s]\n",
        //     (*event)->firedeventid, (*event)->eventjson.c_str());

        Json::Value JSONresult;
        bool parsingSuccessful = parseJSON((*event)->eventjson.c_str(), JSONresult);
        if (!parsingSuccessful)
        {
            continue;
        }

        std::string eventid = "";
        if ( JSONresult.isMember("eventid") )
            eventid = JSONresult["eventid"].asString();
        else
        {
            // checkpointgroup event
            //vilslog::printf("CheckEmployeeActionService::pair_event() output single firedeventid[%d]\n", (*event)->firedeventid);

            Pair_FiredEventStruct * singleEvent = new Pair_FiredEventStruct();
            singleEvent->type = "single";
            singleEvent->event1 = copyFiredEventStruct((*event));
            singleEvent->event2 = NULL;
            singleEvent->events = NULL;
            pair_events->push_back(singleEvent);
            continue;
        }

        if ( eventid.compare("9999") == 0 )
        {
            std::string cur_event_nodeid = JSONresult["nodeid"].asString();
            std::string cur_event_action = JSONresult["action"].asString();
            std::string cur_event_locatorid = JSONresult["locatorid"].asString();
            std::string cur_event_datetime = (*event)->datetime;
            std::string cur_event_firstdatetime = (*event)->firstdatetime;

            //if ( cur_event_action.compare("Cancel") == 0 )
            if ( cur_event_action.compare("Alert") == 0 )
            {
                // find alert
                bool bFound = false;
                for (std::list<FiredEventStruct *>::iterator check_event = std::next(event, 1); check_event != merge_events->end();check_event++)
                {
                    Json::Value check_JSONresult;
                    bool parsingSuccessful = parseJSON((*check_event)->eventjson.c_str(), check_JSONresult);
                    if (!parsingSuccessful)
                    {
                        continue;
                    }

                    std::string eventid = "";
                    if ( check_JSONresult.isMember("eventid") )
                    {
                        eventid = check_JSONresult["eventid"].asString();
                        if ( eventid.compare("9999") != 0 )
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }

                    // check same event
                    std::string check_event_nodeid = check_JSONresult["nodeid"].asString();
                    std::string check_event_action = check_JSONresult["action"].asString();
                    std::string check_event_locatorid = check_JSONresult["locatorid"].asString();
                    std::string check_event_datetime = (*check_event)->datetime;

                    // if ( (check_event_action.compare("Alert") == 0)  &&
                    //      (check_event_locatorid.compare(cur_event_locatorid) == 0) )
                    if ( (check_event_action.compare("Cancel") == 0)  &&
                         (check_event_locatorid.compare(cur_event_locatorid) == 0) )
                    {
                        //vilslog::printf("CheckEmployeeActionService::pair_event() output pair firedeventid[%d]\n", (*event)->firedeventid);

                        time_t cur_datetime = StringDateTimeTotime_t(cur_event_datetime);
                        time_t check_datetime = StringDateTimeTotime_t(check_event_datetime);
                        // int diffSeconds = (int)(check_datetime - cur_datetime);
                        int diffSeconds = (int)(cur_datetime - check_datetime);

                        if (diffSeconds < 600)
                        {
                            Pair_FiredEventStruct * pairEvent = new Pair_FiredEventStruct();
                            pairEvent->type = "pair";
                            // pairEvent->event1 = copyFiredEventStruct((*event));
                            // pairEvent->event2 = copyFiredEventStruct((*check_event));
                            pairEvent->event1 = copyFiredEventStruct((*check_event));
                            pairEvent->event2 = copyFiredEventStruct((*event));
                            pairEvent->events = NULL;
                            pair_events->push_back(pairEvent);

                            // remove from list
                            FiredEventStruct * old_event = (*check_event);
                            check_event = merge_events->erase(check_event);

                            // vilslog::printf("CheckEmployeeActionService::pair_event() remove firedeventid[%d] action[%s]\n",
                            //     old_event->firedeventid, check_event_action.c_str());

                            delete old_event;
                            bFound = true;
                        }
                        break;
                    }
                    else
                    if ( (check_event_action.compare("Alert") == 0) &&
                         (check_event_locatorid.compare(cur_event_locatorid) == 0) )
                    {
                        // Same locator but different tag
                        // should stop find cancel on this locator
                        break;
                    }
                }

                if (!bFound)
                {
                    //vilslog::printf("CheckEmployeeActionService::pair_event() output single firedeventid[%d]\n", (*event)->firedeventid);

                    Pair_FiredEventStruct * singleEvent = new Pair_FiredEventStruct();
                    singleEvent->type = "single";
                    singleEvent->event1 = copyFiredEventStruct((*event));
                    singleEvent->event2 = NULL;
                    singleEvent->events = NULL;
                    pair_events->push_back(singleEvent);
                }
            }
            else
            {
                //vilslog::printf("CheckEmployeeActionService::pair_event() output single firedeventid[%d]\n", (*event)->firedeventid);

                Pair_FiredEventStruct * singleEvent = new Pair_FiredEventStruct();
                singleEvent->type = "single";
                singleEvent->event1 = copyFiredEventStruct((*event));
                singleEvent->event2 = NULL;
                singleEvent->events = NULL;
                pair_events->push_back(singleEvent);
            }
        }
        else
        {
            //vilslog::printf("CheckEmployeeActionService::pair_event() output single firedeventid[%d]\n", (*event)->firedeventid);

            Pair_FiredEventStruct * singleEvent = new Pair_FiredEventStruct();
            singleEvent->type = "single";
            singleEvent->event1 = copyFiredEventStruct((*event));
            singleEvent->event2 = NULL;
            singleEvent->events = NULL;
            pair_events->push_back(singleEvent);
        }
    }
    return pair_events;
}

std::list<Pair_FiredEventStruct*> * CheckEmployeeActionService::merge_same_tagaction_event(std::list<Pair_FiredEventStruct*> * pair_events)
{
    std::list<Pair_FiredEventStruct*> * same_tagaction_events = new std::list<Pair_FiredEventStruct*>();
    for (std::list<Pair_FiredEventStruct *>::iterator event = pair_events->begin(); event != pair_events->end(); event++)
    {
        if ( (*event)->type.compare("single") == 0 )
        {
            Json::Value JSONresult;
            bool parsingSuccessful = parseJSON((*event)->event1->eventjson.c_str(), JSONresult);
            if (!parsingSuccessful)
            {
                continue;
            }

            std::string cur_event_nodeid = JSONresult["nodeid"].asString();
            std::string cur_event_action = JSONresult["action"].asString();
            std::string cur_event_datetime = (*event)->event1->datetime;

            for (std::list<Pair_FiredEventStruct *>::iterator check_event = std::next(event, 1); check_event != pair_events->end();check_event++)
            {
                if ( (*check_event)->type.compare("single") == 0 )
                {
                    Json::Value check_JSONresult;
                    bool parsingSuccessful = parseJSON((*check_event)->event1->eventjson.c_str(), check_JSONresult);
                    if (!parsingSuccessful)
                    {
                        continue;
                    }

                    std::string eventid = "";
                    if ( check_JSONresult.isMember("eventid") )
                    {
                        eventid = check_JSONresult["eventid"].asString();
                        if ( eventid.compare("9999") != 0 )
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }

                    std::string check_event_nodeid = check_JSONresult["nodeid"].asString();
                    std::string check_event_action = check_JSONresult["action"].asString();
                    std::string check_event_datetime = (*check_event)->event1->datetime;


                    time_t cur_datetime = StringDateTimeTotime_t(cur_event_datetime);
                    time_t check_datetime = StringDateTimeTotime_t(check_event_datetime);
                    // int diffSeconds = (int)(check_datetime - cur_datetime);
                    int diffSeconds = (int)(cur_datetime - check_datetime);

                    if (diffSeconds > 60)
                    {
                        break;
                    }

                    // found same node
                    if ( (cur_event_nodeid.compare(check_event_nodeid) == 0) )
                    {
                        (*event)->type = "sametag";
                        if ((*event)->events == NULL)
                        {
                            (*event)->events = new std::list<FiredEventStruct*>();
                        }

                        FiredEventStruct * new_event = copyFiredEventStruct((*check_event)->event1);
                        (*event)->events->push_back(new_event);

                        // remove from list
                        Pair_FiredEventStruct * old_event = (*check_event);
                        check_event = pair_events->erase(check_event);
                        clear_Pair_FiredEventStruct(old_event);
                        delete old_event;
                    }
                }
            }

            Pair_FiredEventStruct * pairevent = copyPair_FiredEventStruct((*event));
            same_tagaction_events->push_back(pairevent);
        }
        else
        {
            Pair_FiredEventStruct * pairevent = copyPair_FiredEventStruct((*event));
            same_tagaction_events->push_back(pairevent);
        }
    }
    return same_tagaction_events;
}

void CheckEmployeeActionService::output_pairevent(int projectid, std::list<Pair_FiredEventStruct*> * pair_events)
{
    for (std::list<Pair_FiredEventStruct *>::iterator event = pair_events->begin(); event != pair_events->end(); event++)
    {
        if ( (*event)->type.compare("single") == 0 )
        {
            if ( (*event)->event1->empid.length() != 0 )
                create_single_event(projectid, (*event)->event1);
        }
        else
        if ( (*event)->type.compare("pair") == 0 )
        {
            if ( (*event)->event1->empid.length() != 0 || (*event)->event2->empid.length() != 0 )
                create_pair_event(projectid, (*event)->event2, (*event)->event1);
        }
        else
        if ( (*event)->type.compare("sametag") == 0 )
        {
            if ( (*event)->event1->empid.length() != 0 )
            {
                create_sametag_event(projectid, (*event)->event1, (*event)->events);
            }
        }
    }
}

void CheckEmployeeActionService::create_single_event(int projectid, FiredEventStruct * event_Alert)
{
    //vilslog::printf("CheckEmployeeActionService::create_single_event()\n");
    Json::Value JSONresult_Alert;

    Json::Reader * reader = new Json::Reader();
    reader->parse( event_Alert->eventjson.c_str(), JSONresult_Alert);
    delete reader;

    std::string action = JSONresult_Alert["action"].asString();
    std::string nodeid_Alert = JSONresult_Alert["nodeid"].asString();
    std::string locatorid_Alert = JSONresult_Alert["locatorid"].asString();

    std::string empid = event_Alert->empid;
    std::string comment = event_Alert->comment;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return;

    UserEmployeeStruct * employee = baseDb->getEmployee(empid);
    if (employee == NULL)
        return;

    // vilslog::printf("empid[%s] cname[%s] orgName[%s] jobTitle[%s] comment[%s]\n",
    //     employee->empid.c_str(), employee->cname.c_str(), employee->orgName.c_str(), employee->jobTitle.c_str(), comment.c_str());

    NodeStruct * node = baseDb->getNodeStruct(nodeid_Alert);
    if (node == NULL)
    {
        delete employee;
        return;
    }
    std::string serno = node->resident_serno;

    UserResidentStruct * resident = baseDb->getResident(serno.c_str());
    if (resident == NULL)
    {
        delete employee;
        delete node;
        return;
    }
    std::string reponsetime = event_Alert->timeend; // remove same date
    std::string eventDate = event_Alert->timestart.substr(0, 11); // '2019-08-22 '
    string::size_type position = event_Alert->timeend.find(eventDate);
    if (position != std::string::npos)
    {
        reponsetime = event_Alert->timeend.substr(11, 8);
    }

    EmployeeEventReportStruct * report = new EmployeeEventReportStruct();
    report->eventid_alert = event_Alert->firedeventid;
    report->eventid_cancel = -1;
    report->emp_empid = empid;
    report->emp_cname = employee->cname;
    report->emp_orgName = employee->orgName;
    report->emp_jobTitle = employee->jobTitle;
    report->comment = comment;
    report->action = action;
    report->nodeid = nodeid_Alert;
    report->locatorid = locatorid_Alert;
    report->resident_serno = resident->serno;
    if ( resident->name.length() == 0)
        report->resident_name = node->nodename;
    else
        report->resident_name = resident->name;
    report->resident_roomNo = resident->roomNo;
    report->eventtime = event_Alert->timestart;
    report->reponsetime = reponsetime;

    WorkItem * witem = new WorkItem("EmployeeEventReport", projectid, report);
    mMySQLJobQueue.add(witem);

    delete employee;
    delete node;
    delete resident;
}


void CheckEmployeeActionService::create_pair_event(int projectid, FiredEventStruct * event_Alert, FiredEventStruct * event_Cancel)
{
    //vilslog::printf("CheckEmployeeActionService::create_pair_event()\n");

    Json::Value JSONresult_Alert;
    Json::Value JSONresult_Cancel;

    Json::Reader * reader = new Json::Reader();
    reader->parse( event_Alert->eventjson.c_str(), JSONresult_Alert);
    reader->parse( event_Cancel->eventjson.c_str(), JSONresult_Cancel);
    delete reader;

    std::string action = JSONresult_Alert["action"].asString();
    std::string nodeid_Alert = JSONresult_Alert["nodeid"].asString();
    std::string locatorid_Alert = JSONresult_Alert["locatorid"].asString();
    std::string nodeid_Cancel = JSONresult_Cancel["nodeid"].asString();
    std::string locatorid_Cancel = JSONresult_Cancel["locatorid"].asString();


    std::string nodeid = nodeid_Alert;
    if ( nodeid_Cancel.compare(nodeid_Alert) == 0 )
    {
        nodeid = nodeid_Alert;
    }
    if ( locatorid_Cancel.compare(nodeid_Alert) == 0)
    {
        nodeid = nodeid_Cancel;
    }

    // vilslog::printf("nodeid_Alert[%s] locatorid_Alert[%s] nodeid_Cancel[%s] locatorid_Cancel[%s] CurType[%d]\n",
    //     nodeid_Alert.c_str(), locatorid_Alert.c_str(), nodeid_Cancel.c_str(), locatorid_Cancel.c_str(), CurType);

    std::string empid = event_Cancel->empid;
    if ( empid.length() == 0 )
        empid = event_Alert->empid;

    std::string comment = event_Cancel->comment;
    if ( comment.length() == 0 )
        comment = event_Alert->comment;


    // vilslog::printf("nodeid[%s] action[%s] locatorid[%s]\n",
    //     nodeid.c_str(), action.c_str(), locatorid_Alert.c_str());

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return;

    UserEmployeeStruct * employee = baseDb->getEmployee(empid);
    if (employee == NULL)
        return;

    // vilslog::printf("empid[%s] cname[%s] orgName[%s] jobTitle[%s] comment[%s]\n",
    //     employee->empid.c_str(), employee->cname.c_str(), employee->orgName.c_str(), employee->jobTitle.c_str(), comment.c_str());

    NodeStruct * node = baseDb->getNodeStruct(nodeid);
    if (node == NULL)
    {
        delete employee;
        return;
    }
    std::string serno = node->resident_serno;

    UserResidentStruct * resident = baseDb->getResident(serno.c_str());
    if (resident == NULL)
    {
        delete employee;
        delete node;
        return;
    }

    // vilslog::printf("serno[%s] name[%s] roomNo[%s] residentType[%s] gender[%s]\n",
    //     resident->serno.c_str(), resident->name.c_str(), resident->roomNo.c_str(), resident->residentType.c_str(), resident->gender.c_str());

    std::string reponsetime = event_Cancel->timestart; // remove same date
    std::string eventDate = event_Alert->timestart.substr(0, 11); // '2019-08-22 '
    string::size_type position = event_Cancel->timestart.find(eventDate);
    if (position != std::string::npos)
    {
        reponsetime = event_Cancel->timestart.substr(11, 8);
    }
    EmployeeEventReportStruct * report = new EmployeeEventReportStruct();
    report->eventid_alert = event_Alert->firedeventid;
    report->eventid_cancel = event_Cancel->firedeventid;
    report->emp_empid = empid;
    report->emp_cname = employee->cname;
    report->emp_orgName = employee->orgName;
    report->emp_jobTitle = employee->jobTitle;
    report->comment = comment;
    report->action = action;
    report->nodeid = nodeid;
    report->locatorid = locatorid_Alert;
    report->resident_serno = resident->serno;
    if ( resident->name.length() == 0)
        report->resident_name = node->nodename;
    else
        report->resident_name = resident->name;
    report->resident_roomNo = resident->roomNo;
    report->eventtime = event_Alert->timestart;
    report->reponsetime = reponsetime;

    WorkItem * witem = new WorkItem("EmployeeEventReport", projectid, report);
    mMySQLJobQueue.add(witem);

    delete employee;
    delete node;
    delete resident;
}

void CheckEmployeeActionService::create_sametag_event(int projectid, FiredEventStruct * event, std::list<FiredEventStruct*> * firedEventList)
{
    //vilslog::printf("CheckEmployeeActionService::create_sametag_event() \n");

    // Json::Value JSONresult;
    //
    // Json::Reader * reader = new Json::Reader();
    // reader->parse( event->eventjson.c_str(), JSONresult);
    // delete reader;
    //
    // std::string action = JSONresult["action"].asString();
    // std::string nodeid = JSONresult["nodeid"].asString();
    // std::string locatorid = JSONresult["locatorid"].asString();
    //
    // MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    // if (baseDb == NULL)
    //     return;

}

Pair_FiredEventStruct * CheckEmployeeActionService::copyPair_FiredEventStruct(Pair_FiredEventStruct * srcEvent)
{
    Pair_FiredEventStruct * dstEvent = new Pair_FiredEventStruct();
    dstEvent->type = srcEvent->type;
    dstEvent->event1 = NULL;
    dstEvent->event2 = NULL;
    dstEvent->events = NULL;

    if (srcEvent->event1 != NULL )
        dstEvent->event1 = copyFiredEventStruct(srcEvent->event1);
    if (srcEvent->event2 != NULL )
        dstEvent->event2 = copyFiredEventStruct(srcEvent->event2);

    if (srcEvent->events != NULL )
    {
        dstEvent->events = new std::list<FiredEventStruct*>();
        for (std::list<FiredEventStruct*>::iterator i = srcEvent->events->begin(); i != srcEvent->events->end(); i++)
        {
            FiredEventStruct * new_event = copyFiredEventStruct((*i));
            dstEvent->events->push_back(new_event);
        }
    }

    return dstEvent;
}
