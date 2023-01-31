
/*
 * SmoothingResult.h
 *
 *  Created on: 2018/10/11
 *      Author: ikki
 */


#ifndef SMOOTHING_RESULT_H
#define SMOOTHING_RESULT_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

//#define SMOOTHING_TIME 400 // ms
//#define SMOOTHING_STEP 6

typedef struct
{
    char TagId[32];
    int projectid;
    int count;
    int totalposX;
    int totalposY;
    int totalposZ;
    int preAveragePosX;
    int preAveragePosY;
    int preAveragePosZ;
    int preposX;
    int preposY;
    int preposZ;
    int curposX;
    int curposY;
    int curposZ;
    int ShowPosX;
    int ShowPosY;
    int ShowPosZ;
    int draw_loop_index;
    int not_move_count;
    int move_no_block_X;
    int move_no_block_old_X;
    int move_moving_X;
    int move_moving_old_X;
    int move_no_block_Y;
    int move_no_block_old_Y;
    int move_moving_Y;
    int move_moving_old_Y;
    int move_status;
    int smoothing_time;
    int smoothing_step_time; // smoothing_step_time = smoothing_time / SMOOTHING_STEP
    long long preUpdateTime;
} SmoothingNodeStruct;

typedef struct
{
    int projectid;
    int mOffset;
    int mSmoothingStep;
    float mFirstStateOffsetRatio;
    int mFirstStateRunTime;
    float mSecondStateOffsetRatio;
    int mSecondStateRunTime;
} SmoothingArgumentsStruct;

class SmoothingResult
{
public:
	SmoothingResult();
	~SmoothingResult();
    bool updatResult(int projectid, const char * TagId, double * results);
    bool getResult(int projectid, const char * TagId, int * results);
    bool getResult(int projectid, const char * TagId, double * results);
    int updatePosition();
    int updatePosition(SmoothingNodeStruct * exist_node);
    int setArguments(int projectid, int offset, int SmoothingStep, float FirstStateOffsetRatio, int FirstStateRunTime, float SecondStateOffsetRatio, int SecondStateRunTime);
private:

    // int mOffset;
    // int mSmoothingStep;
    // float mFirstStateOffsetRatio;
    // int mFirstStateRunTime;
    // float mSecondStateOffsetRatio;
    // int mSecondStateRunTime;

    std::list<SmoothingNodeStruct*> mTagNodeList; // node_type = 1
    std::list<SmoothingArgumentsStruct*> mArgumentList;
    void reset_draw_node(SmoothingNodeStruct * exist_node, int posX, int posY, int posZ);
    float getCurrent_r(int status, SmoothingArgumentsStruct * Arguments);
};

#endif
