
/**
  ******************************************************************************
  * @file    FrameRateStrategy.c
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   该文件包含了库的具体实现
  ******************************************************************************
  */
/*inlcude--------------------------------------------------------------------*/
#include "FrameRateStrategy_internal.h"
#include "heap_solution_1.h"
#include "EngineFault.h"	//引擎错误(故障)处理
#include <string.h>

/*定义-----------------------------------------------------------------------*/
//接口兼容设计
#define malloc(size)	c_malloc(size)	
#define free(ptr)			c_free(ptr)


#define FrmRS_NULL			((void*)0)
DeltaTime_t* DeltaTime = FrmRS_NULL;




/*函数实现--------------------------------------------------------------------*/


/**
  * @brief  deltaTime模型初始化
  * @param  DeltaTimeBaseStruct: Δt时基单元
  * @retval 操作成功返回0
	*					参数错误返回-1
	*					内存不足返回-2
  */
int FrmRS_deltaTime_Init(const DeltaTimeBaseStruct_t* DeltaTimeBaseStruct){
	if(DeltaTime != FrmRS_NULL || DeltaTimeBaseStruct == FrmRS_NULL){
			return -1;
	}
	
	//申请空间
	DeltaTime = (DeltaTime_t*)malloc(sizeof(DeltaTime_t));
	if(DeltaTime == FrmRS_NULL){
		return -2;//空间申请失败
	}
	
	DeltaTime->deltaTime = 0;
	DeltaTime->flag = 0;
	for(int i = 0;i < DELTATIME_GROUP_COUNT;i++){
			DeltaTime->deltaTimeGroup[i] = 0;
	}
	DeltaTime->deltaTimeIndex = 0;
	memcpy(&DeltaTime->DeltaTimeStruct,DeltaTimeBaseStruct,sizeof(DeltaTimeBaseStruct_t));
	
	//初始化计数器
	if(DeltaTime->DeltaTimeStruct.TimeUnitConfig == FrmRS_NULL){
		//空指针访问
		void EngineFault_NullPointerGuard_Handle(void);
	}
	DeltaTime->DeltaTimeStruct.TimeUnitConfig();
	
	return 0;
}

/**
  * @brief  时间增量检测触发器，每次触发记录时间增量，并计算并更新deltaTime的值
  * @param  none
  * @retval none
  */
void FrmRS_deltaTime_Trigger(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//空指针访问
	 }
	 if(DeltaTime->flag == 0){
		 DeltaTime->flag = 1;
		 return;
	 }
	 //更新环形缓存区
	 DeltaTime->deltaTimeGroup[DeltaTime->deltaTimeIndex++] = DeltaTime->DeltaTimeStruct.CalculateTimeInterval(DeltaTime->DeltaTimeStruct.PrivateData);
	 if(DeltaTime->deltaTimeIndex >= DELTATIME_GROUP_COUNT){
			DeltaTime->deltaTimeIndex = 0;
	 }
	
	 //计算时间增量
	 double temp = 0;
	 for(int i = 0;i < DELTATIME_GROUP_COUNT;i++){
		 temp += DeltaTime->deltaTimeGroup[i];
	 }
	 temp /= (double)DELTATIME_GROUP_COUNT;
	 DeltaTime->deltaTime = temp;
	 return;
}

/**
  * @brief  获取当前的平均帧率
  * @param  none
  * @retval 帧率
  */
double FrmRS_GetFrmRate(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//空指针访问
	 }
	
	 if(DeltaTime->deltaTime <= 1e-12){
		 return 1.0/1e-12;//设置一个帧率无线大的值。
	 }
	 
	 return 1.0/DeltaTime->deltaTime;
}

/**
  * @brief  获取当前的deltaTime时间
  * @param  none
  * @retval 帧率
  */
double FrmRS_deltaTime_Get(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//空指针访问
	 }
	 return DeltaTime->deltaTime;
}

/**
  * @brief  deltaTime模型空间释放
  * @param  none
  * @retval none
  */

void FrmRS_deltaTime_Free(void){
		if(DeltaTime == FrmRS_NULL)
				return;
		
		//释放私有资源
		if(DeltaTime->DeltaTimeStruct.FreeHandle){
			DeltaTime->DeltaTimeStruct.FreeHandle(DeltaTime->DeltaTimeStruct.PrivateData);
		}
		
		free(DeltaTime);
		DeltaTime = FrmRS_NULL;
		return;
}

