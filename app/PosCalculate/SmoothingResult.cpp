
/*
 * SmoothingResult.cpp
 *
 *  Created on: 2018/10/11
 *      Author: ikki
 */

#include "SmoothingResult.h"
#include "../common.h"

#include <math.h>

SmoothingResult::SmoothingResult()
{
    // mOffset = 10;
    // mSmoothingStep = 6;
    // mFirstStateOffsetRatio = 0.3f;
    // mFirstStateRunTime = 200;
    // mSecondStateOffsetRatio = 0.8f;
    // mSecondStateRunTime = 600;
}

SmoothingResult::~SmoothingResult()
{
}

bool SmoothingResult::updatResult(int projectid, const char * TagId, double * results)
{
    bool bFound = false;
    for (std::list<SmoothingNodeStruct*>::iterator existed = mTagNodeList.begin(); existed != mTagNodeList.end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->TagId) == 0 && ((*existed)->projectid == projectid) )
        {
            (*existed)->totalposX += (int)results[0];
            (*existed)->totalposY += (int)results[1];
            (*existed)->totalposZ += (int)results[2];
            (*existed)->count++;
            bFound = true;

            //printf("SmoothingResult::updatResult() [%d] Tag_idx[%d] totalpos[%d, %d,%d] count[%d]\n",
            //    (int)getCurrentTimeInMilliSec(), Tag_idx, (*existed)->totalposX, (*existed)->totalposY, (*existed)->totalposZ, (*existed)->count);
            break;
        }
    }

    if (!bFound)
    {
         SmoothingNodeStruct * newNode = new SmoothingNodeStruct();
         memset(newNode->TagId, 0, 32);
         strncpy(newNode->TagId, TagId, 32);
         newNode->projectid = projectid;
         reset_draw_node(newNode, (int)results[0], (int)results[1], (int)results[2]);
         mTagNodeList.push_back(newNode);
    }

    return bFound;
}

int SmoothingResult::setArguments(int projectid, int offset, int SmoothingStep, float FirstStateOffsetRatio, int FirstStateRunTime, float SecondStateOffsetRatio, int SecondStateRunTime)
{
    bool bFound = false;
    for (std::list<SmoothingArgumentsStruct*>::iterator argu = mArgumentList.begin(); argu != mArgumentList.end(); argu++)
    {
        if ( (*argu)->projectid == projectid )
        {
            (*argu)->mOffset = offset;
            (*argu)->mSmoothingStep = SmoothingStep;
            (*argu)->mFirstStateOffsetRatio = FirstStateOffsetRatio;
            (*argu)->mFirstStateRunTime = FirstStateRunTime;
            (*argu)->mSecondStateOffsetRatio = SecondStateOffsetRatio;
            (*argu)->mSecondStateRunTime = SecondStateRunTime;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
         SmoothingArgumentsStruct * newArgu = new SmoothingArgumentsStruct();
         newArgu->projectid = projectid;
         newArgu->mOffset = offset;
         newArgu->mSmoothingStep = SmoothingStep;
         newArgu->mFirstStateOffsetRatio = FirstStateOffsetRatio;
         newArgu->mFirstStateRunTime = FirstStateRunTime;
         newArgu->mSecondStateOffsetRatio = SecondStateOffsetRatio;
         newArgu->mSecondStateRunTime = SecondStateRunTime;
         mArgumentList.push_back(newArgu);
    }

    // mOffset = offset;
    // mSmoothingStep = SmoothingStep;
    // mFirstStateOffsetRatio = FirstStateOffsetRatio;
    // mFirstStateRunTime = FirstStateRunTime;
    // mSecondStateOffsetRatio = SecondStateOffsetRatio;
    // mSecondStateRunTime = SecondStateRunTime;
    return 0;
}

bool SmoothingResult::getResult(int projectid, const char * TagId, int * results)
{
    bool bFound = false;
    for (std::list<SmoothingNodeStruct*>::iterator existed = mTagNodeList.begin(); existed != mTagNodeList.end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->TagId) == 0 && ((*existed)->projectid == projectid))
        {
            results[0] = (*existed)->ShowPosX;
            results[1] = (*existed)->ShowPosY;
            results[2] = (*existed)->ShowPosZ;
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool SmoothingResult::getResult(int projectid, const char * TagId, double * results)
{
    bool bFound = false;
    for (std::list<SmoothingNodeStruct*>::iterator existed = mTagNodeList.begin(); existed != mTagNodeList.end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->TagId) == 0 && ((*existed)->projectid == projectid) )
        {
            results[0] = (*existed)->ShowPosX;
            results[1] = (*existed)->ShowPosY;
            results[2] = (*existed)->ShowPosZ;
            bFound = true;
            break;
        }
    }
    return bFound;
}

int SmoothingResult::updatePosition()
{
    int updateCount = 0;
    //time_t curTime = getCurrentTimeInMilliSec();
    long long curTime = getSystemTime();

    for (std::list<SmoothingNodeStruct*>::iterator existed = mTagNodeList.begin(); existed != mTagNodeList.end(); existed++)
    {
        //
        // check time valid
        //
        //int difft = (int)(curTime - (*existed)->preUpdateTime);
        long long difft = curTime - (*existed)->preUpdateTime;

        //printf("SmoothingResult::updatePosition() index[%d] curTime[%lld] difft[%lld] smoothing_time[%d] smoothing_step_time[%d]\n",
        //                (*existed)->index, curTime, difft, (*existed)->smoothing_time, (*existed)->smoothing_step_time);
        if (difft >= (*existed)->smoothing_step_time)
        {
            updatePosition((*existed));
            (*existed)->preUpdateTime = curTime;
        }

        updateCount++;
    }

    return updateCount;
}

int SmoothingResult::updatePosition(SmoothingNodeStruct * exist_node)
{
    SmoothingArgumentsStruct * Arguments = NULL;
    for (std::list<SmoothingArgumentsStruct*>::iterator argu = mArgumentList.begin(); argu != mArgumentList.end(); argu++)
    {
        if ( (*argu)->projectid == exist_node->projectid )
        {
            Arguments = (*argu);
            break;
        }
    }
    if ( Arguments == NULL )
    {
        vilslog::printf("SmoothingResult::updatePosition() Arguments == NULL");
        return 0;
    }

    int total_distance_X = exist_node->curposX - exist_node->preposX;
    int total_distance_Y = exist_node->curposY - exist_node->preposY;
    int total_distance_Z = exist_node->curposZ - exist_node->preposZ;

    int distance = (int)sqrt(total_distance_X * total_distance_X + total_distance_Y * total_distance_Y + total_distance_Z * total_distance_Z);

	if ( distance >= Arguments->mOffset || exist_node->not_move_count > 10)
    {
        int current_loop = exist_node->draw_loop_index + 1;

        int step_dist_x = total_distance_X / Arguments->mSmoothingStep;
        int step_dist_y = total_distance_Y / Arguments->mSmoothingStep;
        int step_dist_z = total_distance_Z / Arguments->mSmoothingStep;
        int dist_x = step_dist_x * current_loop;
        int dist_y = step_dist_y * current_loop;
        int dist_z = step_dist_z * current_loop;

        exist_node->not_move_count = 0;

        //
        // upate to show
        //
        exist_node->ShowPosX = exist_node->preposX + dist_x;
        exist_node->ShowPosY = exist_node->preposY + dist_y;
        exist_node->ShowPosZ = exist_node->preposZ + dist_z;

        //printf("SmoothingResult::updatePosition() index[%d] ShowPos[%d, %d,%d]\n",
        //                exist_node->index, exist_node->ShowPosX, exist_node->ShowPosY, exist_node->ShowPosZ);

    }
    else
    {
        exist_node->not_move_count++;
    }

    exist_node->draw_loop_index++;

    if (exist_node->draw_loop_index >= Arguments->mSmoothingStep)
    {
        if (exist_node->count > 0)
        {
            //
            // average the values
            //
            int posX = exist_node->totalposX / exist_node->count;
            int posY = exist_node->totalposY / exist_node->count;
            int posZ = exist_node->totalposZ / exist_node->count;

            float r = getCurrent_r(exist_node->move_status, Arguments);
            if ( r == Arguments->mFirstStateOffsetRatio)
            {
                exist_node->smoothing_time = Arguments->mFirstStateRunTime;// ms
            }
            else
            if ( r == Arguments->mSecondStateOffsetRatio)
            {
                exist_node->smoothing_time = Arguments->mSecondStateRunTime;// ms
            }
            else
            {
                exist_node->smoothing_time = Arguments->mSecondStateRunTime;// ms
            }
            exist_node->smoothing_step_time = exist_node->smoothing_time / Arguments->mSmoothingStep;

            //printf("SmoothingResult::updatePosition() index[%d] r[%.02f] smoothing_step_time[%d]\n",
            //                exist_node->index, r, exist_node->smoothing_step_time);

            exist_node->move_moving_X = (int)(r * (float)(posX - exist_node->move_no_block_old_X));
            exist_node->move_no_block_X = exist_node->move_moving_X + exist_node->move_no_block_old_X;
            exist_node->move_moving_old_X = exist_node->move_moving_X;

            exist_node->move_moving_Y = (int)(r * (float)(posY - exist_node->move_no_block_old_Y));
            exist_node->move_no_block_Y = exist_node->move_moving_Y + exist_node->move_no_block_old_Y;
            exist_node->move_moving_old_Y = exist_node->move_moving_Y;

            exist_node->move_status += 1;
            if ( exist_node->move_status > 6 || ((exist_node->move_moving_X * exist_node->move_moving_old_X) < 0))
            {
                exist_node->move_status = 1;
            }

            exist_node->move_no_block_old_X = exist_node->move_no_block_X;
            exist_node->move_no_block_old_Y = exist_node->move_no_block_Y;
            //========================

            exist_node->preposX = exist_node->curposX;
            exist_node->preposY = exist_node->curposY;
            exist_node->preposZ = exist_node->curposZ;
            exist_node->curposX = exist_node->move_no_block_X;
            exist_node->curposY = exist_node->move_no_block_Y;
            exist_node->curposZ = posZ;
            exist_node->preAveragePosX = exist_node->totalposX / exist_node->count;
            exist_node->preAveragePosY = exist_node->totalposY / exist_node->count;
            exist_node->preAveragePosZ = exist_node->totalposZ / exist_node->count;
            exist_node->totalposX = 0;
            exist_node->totalposY = 0;
            exist_node->totalposZ = 0;
            exist_node->count = 0;
            exist_node->draw_loop_index = 0;
        }
        else
        {
            reset_draw_node(exist_node, exist_node->curposX, exist_node->curposY, exist_node->curposZ);
        }
    }

    return 0;
}

void SmoothingResult::reset_draw_node(SmoothingNodeStruct * exist_node, int posX, int posY, int posZ)
{
    SmoothingArgumentsStruct * Arguments = NULL;
    for (std::list<SmoothingArgumentsStruct*>::iterator argu = mArgumentList.begin(); argu != mArgumentList.end(); argu++)
    {
        if ( (*argu)->projectid == exist_node->projectid )
        {
            Arguments = (*argu);
            break;
        }
    }
    if ( Arguments == NULL )
    {
        vilslog::printf("SmoothingResult::reset_draw_node() Arguments == NULL\n");
        return;
    }

    exist_node->count = 0;
    exist_node->totalposX = 0;
    exist_node->totalposY = 0;
    exist_node->totalposZ = 0;
    exist_node->preAveragePosX = posX;
    exist_node->preAveragePosY = posY;
    exist_node->preAveragePosZ = posZ;
    exist_node->preposX = posX;
    exist_node->preposY = posY;
    exist_node->preposZ = posZ;
    exist_node->curposX = posX;
    exist_node->curposY = posY;
    exist_node->curposZ = posZ;
    exist_node->ShowPosX = posX;
    exist_node->ShowPosY = posY;
    exist_node->ShowPosZ = posZ;
    exist_node->draw_loop_index = 0;
    exist_node->not_move_count = 0;
    exist_node->move_no_block_X = posX;
    exist_node->move_no_block_old_X = posX;
    exist_node->move_moving_X = posX;
    exist_node->move_moving_old_X = posX;
    exist_node->move_no_block_Y = posY;
    exist_node->move_no_block_old_Y = posY;
    exist_node->move_moving_Y = posY;
    exist_node->move_moving_old_Y = posY;
    exist_node->move_status = 1;
    exist_node->smoothing_time = Arguments->mSecondStateOffsetRatio;
    exist_node->smoothing_step_time = Arguments->mSecondStateOffsetRatio / Arguments->mSmoothingStep;
    exist_node->preUpdateTime = 0;
}

float SmoothingResult::getCurrent_r(int status, SmoothingArgumentsStruct * Arguments)
{
    float r = 0.1f;
    switch(status)
    {
        case 1:
        case 2:
            //r = 0.15f;
            r = Arguments->mFirstStateOffsetRatio;
        break;
        case 3:
        case 4:
        case 5:
        case 6:
            //r = 0.55f;
            r = Arguments->mSecondStateOffsetRatio;
        break;
    }
    return r;
}
