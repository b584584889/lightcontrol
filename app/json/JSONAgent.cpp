/*
 * JSONAgent.cpp
 *
 *  Created on: 2017/07/05
 *      Author: ikki
 */


#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "JSONAgent.h"

JSONAgent::JSONAgent()
{
    mReceiveBufLen = 0;
    mReader = new Json::Reader();
}

JSONAgent::~JSONAgent()
{
    delete mReader;
}

int JSONAgent::put(const char * content, int length)
{
    if (mReceiveBufLen + length < JSON_BUF_MAX)
    {
        memcpy(mReceiveBuffer + mReceiveBufLen, content, length);
        mReceiveBufLen += length;
    }
    return mReceiveBufLen;
}

bool JSONAgent::get(Json::Value & JSONresult)
{
    int length = 0;
    int start_idx = 0;
    bool parsingSuccessful = false;

    do {
        memset(line, 0, JSON_LINE_MAX);

        length = get_line(line, start_idx);

        if (length > 0 && line[0] == '{')
        {
     		line[length] = '\0';
            //printf ("JSONAgent::get() length[%d] line[%s]\n", length, line);

            try{

                //================
                //Json::Reader * reader = new Json::Reader();
                //parsingSuccessful = reader->parse( &line[0], JSONresult);
                //delete reader;
                //================
                parsingSuccessful = mReader->parse( &line[0], JSONresult);
                //================
                //printf ("JSONAgent::get() parsingSuccessful[%d]\n", parsingSuccessful);

                //printf ("JSONAgent::get() length[%d] line[%s]\n", length, line);
            }
            catch(...)
            {
                vilslog::printf ("JSONAgent::get() catch exception...\n");
            }
            start_idx += length;
            break;
        }
        else
        if (length > 0)
        {
            //continue find.
            //printf ("JSONAgent::get() length[%d] length > 0\n", length);
        }
        else
        if (length == 0)
        {
            //printf ("JSONAgent::get() length[%d] length == 0\n", length);
            parsingSuccessful = false;
            break;
        }

        start_idx += length;
    } while (length > 0);

    // store rest data in buffer
    if (start_idx >= mReceiveBufLen)
    {
        mReceiveBufLen = 0;
    }
    else
    {
        mReceiveBufLen = mReceiveBufLen - start_idx;
        for (int i = 0; i < mReceiveBufLen; i++)
        {
            mReceiveBuffer[i] = mReceiveBuffer[start_idx + i];
        }
    }

    //printf ("JSONAgent::get() mReceiveBufLen[%d]\n", mReceiveBufLen);

    return parsingSuccessful;
}

int JSONAgent::getBufferLen()
{
    return mReceiveBufLen;
}

int JSONAgent::get_line(char * linebuffer, int start_idx)
{
    int length = 0;
    bool bFound = false;

    //printf ("MessageParser::get_line() start_idx[%d] mReceiveBufLen[%d]\n", start_idx, mReceiveBufLen);

    // find 0x10 0x13
    for (int i = start_idx; i < mReceiveBufLen; i++)
    {
        if (length < JSON_LINE_MAX)
        {
            linebuffer[length] = mReceiveBuffer[i];
            length++;
        }

        // detect '{'
        // if ( mReceiveBuffer[i] == '{' && i > 0 )
        // {
        //     //Found JSON head, stop this line.
        //     length--;
        //     bFound = true;
        //     break;
        // }
        // else
        if (mReceiveBuffer[i] == 0x0A || mReceiveBuffer[i] == 0x0D)
        {
            if (i + 1 <= mReceiveBufLen - 1 && mReceiveBuffer[i + 1] == 0x0A)
            {
                if (length < JSON_LINE_MAX)
                {
                    linebuffer[length] = mReceiveBuffer[i + 1];
                    length++;
                }
                bFound = true;
                break;
            }
        }
    }

    //printf ("MessageParser::get_line() bFound[%d] length[%d]\n", bFound, length);

    if (!bFound)
    {
        length = 0;
    }

    return length;
}
