
#include "Logger.h"
#include <iostream>
#include <string>
#include <time.h>
#include <ctime>
#include <sys/time.h>
#include <time.h>

#include "../config.h"

const string CLogger::m_sFileDir = LOG_DIRECTORY;
const string CLogger::m_sFileName = "Log.txt";
CLogger* CLogger::m_pThis = NULL;
ofstream CLogger::m_Logfile;
CLogger::CLogger()
{

}

CLogger* CLogger::GetLogger()
{
    if (m_pThis == NULL)
    {
        m_pThis = new CLogger();
        m_Logfile.open(m_sFileName.c_str(), ios::out | ios::app);
    }
    return m_pThis;
}

void CLogger::startdumpCSV(const char * prefix)
{
    if (m_pThis != NULL)
    {
        m_Logfile.close();
        delete m_pThis;
    }
    char sFileName[80];
    m_pThis = new CLogger();
    snprintf(sFileName, 80, "%s/Log_%s_%s.csv", m_sFileDir.c_str(), CurrentDateTime().c_str(), prefix);
    m_Logfile.open(sFileName, ios::out | ios::app);
}

void CLogger::stopdumpCSV()
{
    if (m_pThis != NULL)
    {
        m_Logfile.close();
        delete m_pThis;
    }
    m_pThis = new CLogger();
    m_Logfile.open(m_sFileName.c_str(), ios::out | ios::app);
}

void CLogger::Log(const char * format, ...)
{
    char sMessage[65536];
    //int nLength = 0;
    va_list args;
    va_start(args, format);
    //  Return the number of characters in the string referenced the list of arguments.
    // _vscprintf doesn't count terminating '\0' (that's why +1)
    //nLength = vsnprintf(format, args) + 1;
    //sMessage = new char[nLength];
    //vsprintf_r(sMessage, nLength, format, args);
    //vsprintf(sMessage, format, args);

    vsnprintf(sMessage, 65536, format, args);

    m_Logfile << CurrentDateTimeInMS() << "";
    m_Logfile << sMessage << "\n";
    va_end(args);

    //delete [] sMessage;
}

void CLogger::LogWOTimeStamp(const char * format, ...)
{
    char sMessage[65536];
    //int nLength = 0;
    va_list args;
    va_start(args, format);
    vsnprintf(sMessage, 65536, format, args);

    m_Logfile << sMessage << "\n";
    va_end(args);
}

void CLogger::Log(const string& sMessage)
{
    m_Logfile <<  CurrentDateTimeInMS() << ":\t";
    m_Logfile << sMessage << "\n";
}

CLogger& CLogger::operator<<(const string& sMessage)
{
    m_Logfile << "\n" << CurrentDateTimeInMS() << ":\t";
    m_Logfile << sMessage << "\n";
    return *this;
}

std::string CLogger::CurrentDateTime()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;

    //strftime(buf, 30, "%Y-%m-%d_%T.", localtime(&curtime));
    strftime(buf, 30, "%Y-%m-%d_%H-%M-%S", localtime(&curtime));

    return buf;

    // time_t     now = time(NULL);
    // struct tm  tstruct;
    // char       buf[80];
    // localtime_r(&now, &tstruct);
    // strftime(buf, sizeof(buf), "%Y-%m-%d_%X", &tstruct);
    // return buf;
}

std::string CLogger::CurrentDateTimeInMS()
{
    char buf[30];
    char bufstring[80];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;

    strftime(buf, 30, "%Y/%m/%d %T.", localtime(&curtime));
    snprintf(bufstring, 80, "%s%.3f", buf, (float)(tv.tv_usec / 1000));

    return bufstring;
}
