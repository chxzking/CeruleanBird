
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
 * @brief deltaTime 模型初始化
 *
 * 该函数用于初始化 delta 时间模块，为其分配内存、配置必要的计时资源，并确保后续时间测量功能的正常运行。
 * 
 * 详细说明：
 * - 该函数会检测是否已经初始化过（检查全局变量 DeltaTime 是否为空）。
 * - 如果模块尚未初始化，则动态分配 DeltaTime 结构体，并清空相关成员变量。
 * - 调用 `TimeUnitConfig()` 接口函数以完成计时器的底层配置。
 *
 * @param  DeltaTimeBaseStruct 一个指向 DeltaTimeBaseStruct_t 类型的指针，
 *                             其定义了底层计时器的配置和时间间隔计算接口。
 *
 * @retval  0      成功初始化。
 * @retval -1      输入参数错误，可能是 DeltaTime 已经初始化，或者传入的指针为空。
 * @retval -2      内存不足，DeltaTime 动态分配失败。
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
 * @brief 时间增量检测触发器
 *
 * 该函数用于记录两次触发之间的时间增量，并将增量存入环形缓冲区（deltaTimeGroup 数组）。
 * 同时，它会更新 deltaTime 的值为最近若干次时间增量的平均值。
 *
 * 详细说明：
 * - 该函数需要周期性调用（如每帧调用一次），以记录时间间隔。
 * - 当首次调用时，内部的 flag 标志会确保忽略第一次的采样结果，避免初始时间差计算产生误差。
 * - 随后，时间增量会存入环形缓冲区，并根据缓冲区中的历史值计算 deltaTime（平均时间增量）。
 *
 * @param  无。
 * @retval 无。
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
 * @brief 获取当前帧率（Frame Rate）
 *
 * 通过当前时间增量 deltaTime 计算并返回帧率，单位为帧/秒（FPS）。
 *
 * 详细说明：
 * - 帧率计算公式为 1.0 / deltaTime。
 * - 如果 deltaTime 的值非常小（小于 1e-12），函数会返回一个极大的帧率值（1.0 / 1e-12）以避免错误计算。
 * - 此函数通常用于实时系统中监控运行效率，或者控制循环的帧率。
 *
 * @param  无。
 * @return double 当前帧率（单位为帧/秒）。
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
 * @brief 获取当前的平均时间增量（Delta Time）
 *
 * 该函数用于返回经过环形缓冲区平滑处理后的时间增量，
 * 此时间增量是调用函数 FrmRS_deltaTime_Trigger() 时计算并更新得到的。
 * 
 * 详细说明：
 * - 在运行过程中，调用方会定时触发 FrmRS_deltaTime_Trigger() 来采集时间间隔，
 *   并将多次采样结果存入一个环形缓冲区（deltaTimeGroup 数组）。
 * - 每次触发后，会对环形缓冲区中的所有时间间隔进行求和并取平均，
 *   这个平均值即为当前的 deltaTime，单位为秒。此值通常用于计算帧率（通过 1.0/deltaTime）
 *   或者作为实时系统中的时间步长，确保逻辑处理与真实时间同步。
 * - 在调用该函数前，需要确保已正确调用 FrmRS_deltaTime_Init() 完成初始化工作，
 *   否则全局变量 DeltaTime 会为空（NULL）。
 * - 为了防止空指针访问，在函数开始处首先检查 DeltaTime 是否为空，
 *   若为空则调用错误处理函数 EngineFault_NullPointerGuard_Handle() 进行保护。
 *
 * @return double 返回当前最新计算出的平均时间增量，单位为秒。
 *                例如，若系统以约 60 FPS 运行，则 deltaTime 的值通常接近 1/60（约 0.01667）秒。
 */
double FrmRS_deltaTime_Get(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//空指针访问
	 }
	 return DeltaTime->deltaTime;
}

/**
 * @brief deltaTime 模块的资源释放
 *
 * 释放 DeltaTime 模块所占用的内存和资源，避免内存泄漏。
 *
 * 详细说明：
 * - 如果 DeltaTime 存在且其 `FreeHandle` 接口有效，则调用该接口释放私有数据（PrivateData）。
 * - 释放动态分配的 DeltaTime 内存，并将全局指针 DeltaTime 置为 NULL。
 * - 如果 DeltaTime 已经为空（未初始化或已释放），函数将直接返回。
 *
 * @param  无。
 * @retval 无。
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

