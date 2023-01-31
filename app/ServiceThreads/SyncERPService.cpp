/*
 *  SyncERPService.cpp
 *
 *  Created on: 2019/08/15
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

#include "SyncERPService.h"

using namespace std;

SyncERPService::SyncERPService(wqueue<DataItem*> & mainJobQueue, wqueue<DataItem*> & ERPQueue, wqueue<WorkItem*> & MySQLJobQueue, DBManager * dbManager)
    : mMainJobQueue(mainJobQueue), mSyncERPJobQueue(ERPQueue), mMySQLJobQueue(MySQLJobQueue), mDbManager(dbManager)
{
    //mERPServer = "";
}

SyncERPService::~SyncERPService()
{
}

bool SyncERPService::status()
{
    return mInitial;
}

std::string SyncERPService::statusString()
{
    return mStatus;
}

void SyncERPService::updateERPServer(int projectid, const char * server)
{
    //if (projectid == 1)
    //    mERPServer = server;
    // vilslog::printf("SyncERPService::updateERPServer() projectid[%d] server[%s]\n", projectid, server);
}

void * SyncERPService::run()
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
            vilslog::printf("SyncERPService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "SyncERPService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void SyncERPService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long syncTime = 0;
    mPreStartSyncTime = 0;

    mInitial = true;

    mStatus = "SyncERPService()::run() Initial Courses successfully!";
    vilslog::printf ("SyncERPService()::run() Initial Courses successfully!\n");

    //test_parser();
    //test_md5();

    while (1)
    {
        DataItem * item = mSyncERPJobQueue.removetimed(10000);
        if( item != NULL )
        {
            processDataItem(item);
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        syncTime += difft;

        if ( syncTime >= 60000) // one minute
        {
            std::string time = currentTimeForSQL();

            if ( time.compare("00:00") == 0 )
            {
                vilslog::printf ("SyncERPService()::run() time[%s] startSync()\n", time.c_str());

                SyncAllProject();

                // startSyncWithRetry(3);
                //
                // vilslog::printf("SyncERPService::run() mResidentList.size()[%d]\n", (int)mResidentList.size());
                // vilslog::printf("SyncERPService::run() mEmployeeList.size()[%d]\n", (int)mEmployeeList.size());
                //
                // DataItem * work = new DataItem("CheckResidentMoveOut", 1, &mResidentList);
                // mMainJobQueue.add(work);
                // DataItem * work1 = new DataItem("CheckEmployeeMoveOut", 1, &mEmployeeList);
                // mMainJobQueue.add(work1);
            }
            syncTime = 0;
        }

        start1 = end1;
    }
}

int SyncERPService::processDataItem(DataItem * dataItem)
{
    if ( strcmp(dataItem->getType(), "startsync") == 0 )
    {
        // vilslog::printf ("SyncERPService::processDataItem() startsync \n");

        long long curTime = getSystemTime();
        long long difft = curTime - mPreStartSyncTime;
        if (difft >= 60000) //60 seconds
        {
            mPreStartSyncTime = getSystemTime();

            SyncAllProject();

            // startSyncWithRetry(3);
            //
            // vilslog::printf("SyncERPService::processDataItem() mResidentList.size()[%d]\n", (int)mResidentList.size());
            // vilslog::printf("SyncERPService::processDataItem() mEmployeeList.size()[%d]\n", (int)mEmployeeList.size());
            //
            // DataItem * work = new DataItem("CheckResidentMoveOut", 1, &mResidentList);
            // mMainJobQueue.add(work);
            // DataItem * work1 = new DataItem("CheckEmployeeMoveOut", 1, &mEmployeeList);
            // mMainJobQueue.add(work1);

        }
        else
        {
            vilslog::printf ("SyncERPService::processDataItem() startsync Already Sync!!! difft[%lld]\n", difft);
        }
    }

    return 0;
}

bool SyncERPService::SyncAllProject()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        startSyncWithRetry(projectid, 3);

        // vilslog::printf("SyncERPService::SyncAllProject() projectid[%d] mResidentList1.size()[%d]\n", projectid, (int)mResidentList1.size());
        // vilslog::printf("SyncERPService::SyncAllProject() projectid[%d] mResidentList2.size()[%d]\n", projectid, (int)mResidentList2.size());
        // vilslog::printf("SyncERPService::SyncAllProject() projectid[%d] mResidentList3.size()[%d]\n", projectid, (int)mResidentList3.size());
        // vilslog::printf("SyncERPService::SyncAllProject() projectid[%d] mEmployeeList1.size()[%d]\n", projectid, (int)mEmployeeList1.size());
        // vilslog::printf("SyncERPService::SyncAllProject() projectid[%d] mEmployeeList2.size()[%d]\n", projectid, (int)mEmployeeList2.size());
        // vilslog::printf("SyncERPService::SyncAllProject() projectid[%d] mEmployeeList3.size()[%d]\n", projectid, (int)mEmployeeList3.size());

        std::list<std::string> * residentList = &mResidentList1;
        if ( mResidentList2.size() > mResidentList1.size() )
            residentList = &mResidentList2;
        if ( mResidentList3.size() > mResidentList2.size() )
            residentList = &mResidentList3;

        DataItem * work = new DataItem("CheckResidentMoveOut", projectid, residentList);
        mMainJobQueue.add(work);

        std::list<std::string> * employeeList = &mEmployeeList1;
        if ( mEmployeeList2.size() > mEmployeeList1.size() )
            employeeList = &mEmployeeList2;
        if ( mEmployeeList3.size() > mEmployeeList2.size() )
            employeeList = &mEmployeeList3;

        DataItem * work1 = new DataItem("CheckEmployeeMoveOut", projectid, employeeList);
        mMainJobQueue.add(work1);
    }

    prjidList->clear();
    delete prjidList;

    mResidentList1.clear();
    mResidentList2.clear();
    mResidentList3.clear();

    mEmployeeList1.clear();
    mEmployeeList2.clear();
    mEmployeeList3.clear();

    return true;
}

bool SyncERPService::startSyncWithRetry(int projectid, int retry_Count)
{
    mResidentList1.clear();
    mResidentList2.clear();
    mResidentList3.clear();

    mEmployeeList1.clear();
    mEmployeeList2.clear();
    mEmployeeList3.clear();

    for(int i=0;i<3;i++)
    {
        int count = 0;
        bool bSuccess = false;
        while(!bSuccess && count < retry_Count)
        {
            bSuccess = startSync(projectid, 1, i);
            sleep(2);
            count++;
        }
    }

    sleep(1);

    for(int i=0;i<3;i++)
    {
        int count = 0;
        bool bSuccess = false;
        while(!bSuccess && count < retry_Count)
        {
            bSuccess = startSync(projectid, 2, i);
            sleep(2);
            count++;
        }
    }

    return true;
}

bool SyncERPService::startSync(int projectid, int type, int listindex)
{
    bool bSuccess = false;
    // vilslog::printf("SyncERPService::startSync() projectid[%d] type[%d]\n", projectid, type);

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    std::string mERPServer = vilssysConfig->ERPServer;

    if (mERPServer.length() == 0)
    {
        // vilslog::printf("SyncERPService::startSync() mERPServer Not define!!!\n");
        return true;
    }

    // http://47.100.182.173/scapi_ibl/api/searcher?type=1&keyword=%
    // http://mopsfundweb.appspot.com/fetchlastfund?action=hotfund
    // http://vils-iot-datacenter.appspot.com/

    //const char *host = "vils-iot-datacenter.appspot.com"; // 目標 URI
    //const char *command = "";

    // const char *host = "47.100.182.173"; // 目標 URI
    const char *host = mERPServer.c_str();

    //const char *command = "scapi_ibl/api/searcher?type=1&keyword=%"; // 長者資料
    //const char *command = "scapi_ibl/api/searcher?type=2&keyword=%"; // 員工資料
    const char *PORT_NUM = "80"; // HTTP port

    char request[0xfff], response[4096]; // 請求 與 回應訊息
    const char *requestLineFmt = "GET /scapi_ibl/api/searcher?type=%d&keyword=%% HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標

    char buffer[1024];
    int bufferLen = 1024;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, type);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    strcat(request, CRLF);


    // 以 memset 清空 hints 結構
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    if ((gaiStatus = getaddrinfo(host, PORT_NUM, &hints, &result)) != 0)
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return false;
    }


    // 分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    if ( cfd == -1 )
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return false;
    }

    // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        errPrint("Connect");
        return false;
    }


    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    // vilslog::printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) < 0)
    {
        errPrint("Send");
        return false;
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

    std::string http_response_string = "";



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

        // vilslog::printf ("\n-------------------\nresponse:%d\n%s\n------------------\n",
        //    (int)http_response_string.length(), http_response_string.c_str());

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
            if (response[n-4] == '\r' && response[n-3] == '\n' &&response[n-2] == '\r' &&response[n-1] == '\n')
            {
                // vilslog::printf ("Found Http end!!!\n");
                break;
            }
        }

        memset(response, 0, size);
    }

    // vilslog::printf ("Final total[%d] length[%d]\n", total, http_response_string.length());
    // vilslog::printf ("\n-------------------\nresponse:\n%s\n------------------\n", http_response_string.c_str());

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
                if ( JSONresult.isMember("success") )
                {
                     vilslog::printf ("SyncERPService::startSync() type[%d] parsingSuccessful[%d] success[%d]\n",
                        type, parsingSuccessful, JSONresult["success"].asBool());

                     //
                     // update data
                     //
                     if ( JSONresult["success"].asBool() )
                     {
                         bSuccess = update_data(projectid, type, JSONresult, listindex);
                     }
                     else
                     {
                         bSuccess = false;
                     }
                }
                else
                {
                    vilslog::printf ("SyncERPService::startSync() type[%d] can not find \"success\" \n", type);
                    bSuccess = false;
                }
            }
            else
            {
                vilslog::printf ("SyncERPService::startSync() type[%d] parsingSuccessful[%d]\n", type, parsingSuccessful);
                bSuccess = false;
            }
        }
        catch(exception exc)
        {
            vilslog::printf ("SyncERPService::startSync() type[%d] reader->parse() catch exception[%s]\n", type, exc.what());
            bSuccess = false;
        }

        delete reader;
    }

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // if (recv(cfd, response, 0xffff, 0) < 0)
    //    errExit("Receive");

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
    //     vilslog::printf ("SyncERPService::startSync() reader->parse() catch exception...\n");
    // }
    //
    // if (parsingSuccessful)
    // {
    //     if ( JSONresult.isMember("success") )
    //     {
    //          vilslog::printf ("SyncERPService::startSync() success[%s]\n", JSONresult["success"].asCString());
    //     }
    // }
    // else
    // {
    //     vilslog::printf ("SyncERPService::startSync() parsingSuccessful[%d]\n", parsingSuccessful);
    // }
    //
    // delete reader;

    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return bSuccess;
}

std::list<HttpHeader*> * SyncERPService::parse_http_header(const char * response, size_t n)
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

            // vilslog::printf ("SyncERPService::parse_http_header() parameter[%s] value[%s]\n",
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

            // vilslog::printf ("SyncERPService::parse_http_header() parameter[%s] value[%s]\n",
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

            // vilslog::printf ("SyncERPService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        {
            //vilslog::printf ("SyncERPService::parse_http_header() buffer[%s]\n", buffer);
        }
    }
    return httpHeaderlist;
}

int SyncERPService::get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len)
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

void SyncERPService::errExit(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    printf("Error: %s", buff.c_str());
    exit(EXIT_FAILURE);
}

void SyncERPService::errPrint(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    vilslog::printf("Error: %s", buff.c_str());
}

bool SyncERPService::update_data(int projectid, int type, Json::Value JSONresult, int listindex)
{
    Json::Value dataValue(Json::arrayValue);

    if ( JSONresult.isMember("data") )
    {
        dataValue = JSONresult.get("data", dataValue);

        if (dataValue.isArray())
        {
            //vilslog::printf ("SyncERPService::update_data() dataValue.isArray()\n");
        }
        else
        {
            vilslog::printf ("SyncERPService::update_data() dataValue is not Array\n");
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
        if (type == 1)
            update_resident(projectid, data, listindex);
        else
        if (type == 2)
            update_employee(projectid, data, listindex);
    }

    return true;
}

void SyncERPService::update_resident(int projectid, Json::Value JSONresult, int listindex)
{
    //vilslog::printf ("SyncERPService::update_resident()\n");

    UserResidentStruct Resident;

    Resident.id = -1;
    Resident.serno = std::to_string(getJSONInt(JSONresult, "serno"));
    Resident.gender = std::to_string(getJSONInt(JSONresult, "gender"));
    Resident.name = getJSONString(JSONresult, "name");
    Resident.roomNo = getJSONString(JSONresult, "roomNo");
    Resident.residentType = std::to_string(getJSONInt(JSONresult, "residentType"));
    Resident.nurseNo = getJSONString(JSONresult, "nurseNo");
    Resident.datetime = "";

    if ( listindex == 0 )
        mResidentList1.push_back(Resident.serno);
    else
    if ( listindex == 1 )
        mResidentList2.push_back(Resident.serno);
    else
    if ( listindex == 2 )
        mResidentList3.push_back(Resident.serno);

    WorkItem * work = new WorkItem("UpdateResident", projectid, &Resident);
    mMySQLJobQueue.add(work);
}

void SyncERPService::update_employee(int projectid, Json::Value JSONresult, int listindex)
{
    //vilslog::printf ("SyncERPService::update_employee()\n");

    UserEmployeeStruct Employee;

    Employee.id = -1;
    Employee.empid = getJSONString(JSONresult, "empid");
    Employee.cname = getJSONString(JSONresult, "cname");
    Employee.orgid = getJSONString(JSONresult, "orgid");
    Employee.orgName = getJSONString(JSONresult, "orgName");
    Employee.area = getJSONString(JSONresult, "area");
    Employee.jobTitle = getJSONString(JSONresult, "jobTitle");
    Employee.gender = getJSONString(JSONresult, "gender");
    Employee.pwd = getJSONString(JSONresult, "pwd");
    Employee.datetime = "";

    if ( listindex == 0 )
        mEmployeeList1.push_back(Employee.empid);
    else
    if ( listindex == 1 )
        mEmployeeList2.push_back(Employee.empid);
    else
    if ( listindex == 2 )
        mEmployeeList3.push_back(Employee.empid);

    WorkItem * work = new WorkItem("UpdateEmployee", projectid, &Employee);
    mMySQLJobQueue.add(work);
}

std::string SyncERPService::getJSONString(Json::Value JSONresult, const char * key)
{
    std::string value = "";

    if ( JSONresult.isMember(key) && !JSONresult[key].empty())
        value = JSONresult[key].asString();

    return value;
}

int SyncERPService::getJSONInt(Json::Value JSONresult, const char * key)
{
    int value = -1;

    if ( JSONresult.isMember(key) && !JSONresult[key].empty())
        value = JSONresult[key].asInt();

    return value;
}

void SyncERPService::test_parser()
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    vilslog::printf ("\n\n================================\n\nSyncERPService::test_parser()\n");

    const char * json_string = "{\"success\":true,\"message\":\"\",\"data\":[{\"empid\":\"A001\",\"cname\":\"蔡孟偉\",\"orgid\":\"BL01\",\"orgName\":\"營運管理部\",\"area\":null,\"jobTitle\":\"經理\",\"gender\":\"1\",\"pwd\":\"6CD117A7A6205FB8DEB87391FD44EB55\"}\
        ,{\"empid\":\"A002\",\"cname\":\"曾瑪珊\",\"orgid\":\"BL02\",\"orgName\":\"行政管理部\",\"area\":null,\"jobTitle\":\"主任\",\"gender\":\"2\",\"pwd\":\"185536316E6571007DE73633726FF76B\"}\
        ,{\"empid\":\"A003\",\"cname\":\"趙婕希\",\"orgid\":\"BL02\",\"orgName\":\"行政管理部\",\"area\":null,\"jobTitle\":\"人資行政\",\"gender\":\"2\",\"pwd\":\"D146B740854D99BA360930CDA8E9BDC0\"}\
        ,{\"empid\":\"A004\",\"cname\":\"邱昭琪\",\"orgid\":\"BL03\",\"orgName\":\"生活管理部\",\"area\":null,\"jobTitle\":\"主任\",\"gender\":\"2\",\"pwd\":\"86AE13B79560C4F9D12E4A32768ADB4B\"}\
        ,{\"empid\":\"A005\",\"cname\":\"葉怡伶\",\"orgid\":\"BL03\",\"orgName\":\"生活管理部\",\"area\":null,\"jobTitle\":\"社工\",\"gender\":\"2\",\"pwd\":\"3933E31AC5311CFCE104D4F28660A331\"}\
        ,{\"empid\":\"A006\",\"cname\":\"陳德睿\",\"orgid\":\"BL03\",\"orgName\":\"生活管理部\",\"area\":null,\"jobTitle\":\"生活管家\",\"gender\":\"1\",\"pwd\":\"CC21CD4E695C94026249691D864A007D\"}\
        ,{\"empid\":\"A007\",\"cname\":\"楊博丞\",\"orgid\":\"BL03\",\"orgName\":\"生活管理部\",\"area\":null,\"jobTitle\":\"庶務\",\"gender\":\"1\",\"pwd\":\"5C06451625C1A4ED92FD4D06A6B9B17C\"}\
        ,{\"empid\":\"A008\",\"cname\":\"張方華\",\"orgid\":\"BL03\",\"orgName\":\"生活管理部\",\"area\":null,\"jobTitle\":\"護理師\",\"gender\":\"2\",\"pwd\":\"04D83262D4D365E18C1B2BBEDCD9D0BA\"}\
        ,{\"empid\":\"A010\",\"cname\":\"劉淑芳\",\"orgid\":null,\"orgName\":null,\"area\":null,\"jobTitle\":null,\"gender\":\"2\",\"pwd\":\"AD92E8C9B86EC0F9E92EEFD825E6BE47\"}\
        ,{\"empid\":\"Z001\",\"cname\":\"宋勇達\",\"orgid\":\"BL03\",\"orgName\":\"生活管理部\",\"area\":null,\"jobTitle\":\"社工\",\"gender\":\"1\",\"pwd\":\"814AEA787D9E14626E867654D55428DF\"}\
        ,{\"empid\":\"A020\",\"cname\":\"張小姐\",\"orgid\":null,\"orgName\":null,\"area\":null,\"jobTitle\":null,\"gender\":\"2\",\"pwd\":\"39AF5A2983EBEB3678A1F8694795FB77\"}\
        ,{\"empid\":\"A011\",\"cname\":\"彭艾潔\",\"orgid\":\"BL03\",\"orgName\":\"生活管理部\",\"area\":null,\"jobTitle\":\"護理師\",\"gender\":\"2\",\"pwd\":\"F54E21F215D776A39C339C92CF9B026B\"}\
        ]}\r\n";

    try{
        parsingSuccessful = reader->parse( json_string, JSONresult);

        if (parsingSuccessful)
        {
            if ( JSONresult.isMember("success") )
            {
                 vilslog::printf ("SyncERPService::test_parser() success[%d]\n", JSONresult["success"].asBool());

                 update_data(1, 2, JSONresult, 1);

            }
        }
        else
        {
            vilslog::printf ("SyncERPService::test_parser() parsingSuccessful[%d]\n", parsingSuccessful);
        }
    }
    catch(...)
    {
        vilslog::printf ("SyncERPService::test_parser() reader->parse() catch exception...\n");
    }

    vilslog::printf ("SyncERPService::test_parser() end\n\n================================\n\n");


    delete reader;
}

void SyncERPService::test_md5()
{
    unsigned char* key = (unsigned char*) "5kNAKSDPTmkKCueWFQynFtmuycb5r2Y84bVkBzxbVtXkWKrFgTKyaaCYBEMHrDX2";
    unsigned char* data = (unsigned char*) "123456";
    //unsigned char* expected = (unsigned char*) "abcd1d87dca34f334786307d0da4fcbd";
    unsigned char* result;
    // unsigned int result_len = 16;
    unsigned int result_len = 16;
    unsigned int i;
    static char res_hexstring[33];

    // result = HMAC(EVP_sha256(), key, 4, data, 28, NULL, NULL);
    //result = HMAC(EVP_md5(), key, 32, data, 28, NULL, NULL);
    result = HMAC(EVP_md5(), key, 64, data, 6, NULL, NULL);
    for (i = 0; i < result_len; i++)
    {
        sprintf(&(res_hexstring[i * 2]), "%02X", result[i]);
    }

    res_hexstring[32] = '\0';
    vilslog::printf ("\n===========\nSyncERPService::test_parser() res_hexstring[%s]\n===========\n", res_hexstring);

    // if (strcmp((char*) res_hexstring, (char*) expected) == 0)
    // {
    //     printf("Test ok, result length %d\n", result_len);
    // }
    // else
    // {
    //     printf("Got %s instead of %s\n", res_hexstring, expected);
    // }
}
