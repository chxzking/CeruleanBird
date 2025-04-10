#ifndef __FRAME_RATE_STRATEGY_API_H__
#define __FRAME_RATE_STRATEGY_API_H__
/**
  ******************************************************************************
  * @file    FrameRateStrategy_API.h
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   该文件包含了库对外的接口，库的功能是提供一种帧率策略，以解决玩家等速度
	*						与帧率相关的问题，本库的解决方案是使用时间增量预测帧率，通过预测帧划分
	*						单帧速度。
  ******************************************************************************
  */

/**
 *	@brief delta时基配置结构体
*/
typedef struct DeltaTimeBaseStruct_t{
		void (*TimeUnitConfig)();																/*!<	计数器配置接口，该函数用于对计数器进行配置	*/
	
				
		double (*CalculateTimeInterval)(void* private_data);		/*!<	时间间隔计算接口，该函数用于计算两次调用此函
																																	数之间的时间间隔，返回此时间，注意返回的结果
																																	应该是一个以秒为单位的浮点数结果*/
	
		void* PrivateData;																			/*!<	私有数据 */
	
		void (*FreeHandle)(void*);															/*!<	当私有数据被清除时对私有数据的数据回收接口 */
	
}DeltaTimeBaseStruct_t;	



/*函数接口-----------------------------------------------------------------------*/

int FrmRS_deltaTime_Init(const DeltaTimeBaseStruct_t* DeltaTimeBaseStruct);
void FrmRS_deltaTime_Trigger(void);
double FrmRS_deltaTime_Get(void);
double FrmRS_GetFrmRate(void);
void FrmRS_deltaTime_Free(void);

#endif
