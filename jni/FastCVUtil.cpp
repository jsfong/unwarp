/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Utility.
 *
 */

#include <time.h>
#include "FastCVUtil.h"
#define LOG_TAG    "FastCVDemoJNI"

FastCVUtil util;

JNIEXPORT void JNICALL Java_com_hiptest_displayjpeg_FastCVUtil_init
(
   JNIEnv* env,
   jobject obj
)
{
   char sVersion[32];  
   
   fcvSetOperationMode( (fcvOperationMode) FASTCV_OP_PERFORMANCE );

   fcvGetVersion(sVersion, 32);
   DPRINTF( "Using FastCV version %s \n", sVersion );

   return;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL Java_com_hiptest_displayjpeg_FastCVUtil_getFastCVProcessTime
(
   JNIEnv*  env,
   jobject  obj
)
{
   return (jfloat) util.getProcessTime();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

float FastCVUtil::getProcessTime(){
	return timeFilteredMs;
}

void FastCVUtil::setProcessTime(float f){
	timeFilteredMs = f;
}



//------------------------------------------------------------------------------
/// @brief Returns current time in microseconds
///
/// @return Time in microseconds
//------------------------------------------------------------------------------
uint64_t FastCVUtil::getTimeMicroSeconds()
{
   struct timespec t;
   t.tv_sec = t.tv_nsec = 0;

   clock_gettime(CLOCK_MONOTONIC, &t);
   return (uint64_t)t.tv_sec * 1000000ULL + t.tv_nsec / 1000L;
}
