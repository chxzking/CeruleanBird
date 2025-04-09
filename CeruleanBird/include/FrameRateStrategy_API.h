#ifndef __FRAME_RATE_STRATEGY_API_H__
#define __FRAME_RATE_STRATEGY_API_H__
/**
  ******************************************************************************
  * @file    FrameRateStrategy_API.h
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ���ļ������˿����Ľӿڣ���Ĺ������ṩһ��֡�ʲ��ԣ��Խ����ҵ��ٶ�
	*						��֡����ص����⣬����Ľ��������ʹ��ʱ������Ԥ��֡�ʣ�ͨ��Ԥ��֡����
	*						��֡�ٶȡ�
  ******************************************************************************
  */

/**
 *	@brief deltaʱ�����ýṹ��
*/
typedef struct DeltaTimeBaseStruct_t{
		void (*TimeUnitConfig)();																/*!<	���������ýӿڣ��ú������ڶԼ�������������	*/
	
				
		double (*CalculateTimeInterval)(void* private_data);		/*!<	ʱ��������ӿڣ��ú������ڼ������ε��ô˺�
																																	��֮���ʱ���������ش�ʱ�䣬ע�ⷵ�صĽ��
																																	Ӧ����һ������Ϊ��λ�ĸ��������*/
	
		void* PrivateData;																			/*!<	˽������ */
	
		void (*FreeHandle)(void*);															/*!<	��˽�����ݱ����ʱ��˽�����ݵ����ݻ��սӿ� */
	
}DeltaTimeBaseStruct_t;	



/*�����ӿ�-----------------------------------------------------------------------*/

int FrmRS_deltaTime_Init(const DeltaTimeBaseStruct_t* DeltaTimeBaseStruct);
void FrmRS_deltaTime_Trigger(void);
double FrmRS_deltaTime_Get(void);
double FrmRS_GetFrmRate(void);
void FrmRS_deltaTime_Free(void);

#endif
