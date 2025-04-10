#ifndef __FRAME_RATE_STRATEGY_INTERNAL_H__
#define __FRAME_RATE_STRATEGY_INTERNAL_H__
/**
  ******************************************************************************
  * @file    FrameRateStrategy_internal.h
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   该文件包含了库对内部的接口
  ******************************************************************************
  */
/*inlcude--------------------------------------------------------------------*/
#include "FrameRateStrategy_API.h"


/*定义--------------------------------------------------------------------*/

#define DELTATIME_GROUP_COUNT		5



/** 
  * @brief  deltatime的核心结构体
  */
typedef struct DeltaTime_t{
	double	deltaTimeGroup[DELTATIME_GROUP_COUNT];				/*!<时间增量缓存区，它虽然是一个数组，但是在逻辑中配合索引
																														抽象为环形缓存区*/
	
	double 	deltaTime;																		/*!<时间增量	*/
	
	int deltaTimeIndex;																		/*!<指向最新的数据的索引*/
	
	unsigned char flag;																		/*!<首次启动的标志，用于过滤第一次获取的时间增量，以降低对
																														整体的误差*/
	
	DeltaTimeBaseStruct_t DeltaTimeStruct;								/*!< delta时间基础配置结构体，定义在 @ref DeltaTimeBaseStruct_t	*/
	
}DeltaTime_t;



extern DeltaTime_t* DeltaTime;//声明Δt的存在

#endif


