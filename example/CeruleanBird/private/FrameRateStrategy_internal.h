#ifndef __FRAME_RATE_STRATEGY_INTERNAL_H__
#define __FRAME_RATE_STRATEGY_INTERNAL_H__
/**
  ******************************************************************************
  * @file    FrameRateStrategy_internal.h
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ���ļ������˿���ڲ��Ľӿ�
  ******************************************************************************
  */
/*inlcude--------------------------------------------------------------------*/
#include "FrameRateStrategy_API.h"


/*����--------------------------------------------------------------------*/

#define DELTATIME_GROUP_COUNT		5



/** 
  * @brief  deltatime�ĺ��Ľṹ��
  */
typedef struct DeltaTime_t{
	double	deltaTimeGroup[DELTATIME_GROUP_COUNT];				/*!<ʱ������������������Ȼ��һ�����飬�������߼����������
																														����Ϊ���λ�����*/
	
	double 	deltaTime;																		/*!<ʱ������	*/
	
	int deltaTimeIndex;																		/*!<ָ�����µ����ݵ�����*/
	
	unsigned char flag;																		/*!<�״������ı�־�����ڹ��˵�һ�λ�ȡ��ʱ���������Խ��Ͷ�
																														��������*/
	
	DeltaTimeBaseStruct_t DeltaTimeStruct;								/*!< deltaʱ��������ýṹ�壬������ @ref DeltaTimeBaseStruct_t	*/
	
}DeltaTime_t;



extern DeltaTime_t* DeltaTime;//������t�Ĵ���

#endif


