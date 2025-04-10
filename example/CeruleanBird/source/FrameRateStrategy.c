
/**
  ******************************************************************************
  * @file    FrameRateStrategy.c
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ���ļ������˿�ľ���ʵ��
  ******************************************************************************
  */
/*inlcude--------------------------------------------------------------------*/
#include "FrameRateStrategy_internal.h"
#include "heap_solution_1.h"
#include "EngineFault.h"	//�������(����)����
#include <string.h>

/*����-----------------------------------------------------------------------*/
//�ӿڼ������
#define malloc(size)	c_malloc(size)	
#define free(ptr)			c_free(ptr)


#define FrmRS_NULL			((void*)0)
DeltaTime_t* DeltaTime = FrmRS_NULL;




/*����ʵ��--------------------------------------------------------------------*/


/**
  * @brief  deltaTimeģ�ͳ�ʼ��
  * @param  DeltaTimeBaseStruct: ��tʱ����Ԫ
  * @retval �����ɹ�����0
	*					�������󷵻�-1
	*					�ڴ治�㷵��-2
  */
int FrmRS_deltaTime_Init(const DeltaTimeBaseStruct_t* DeltaTimeBaseStruct){
	if(DeltaTime != FrmRS_NULL || DeltaTimeBaseStruct == FrmRS_NULL){
			return -1;
	}
	
	//����ռ�
	DeltaTime = (DeltaTime_t*)malloc(sizeof(DeltaTime_t));
	if(DeltaTime == FrmRS_NULL){
		return -2;//�ռ�����ʧ��
	}
	
	DeltaTime->deltaTime = 0;
	DeltaTime->flag = 0;
	for(int i = 0;i < DELTATIME_GROUP_COUNT;i++){
			DeltaTime->deltaTimeGroup[i] = 0;
	}
	DeltaTime->deltaTimeIndex = 0;
	memcpy(&DeltaTime->DeltaTimeStruct,DeltaTimeBaseStruct,sizeof(DeltaTimeBaseStruct_t));
	
	//��ʼ��������
	if(DeltaTime->DeltaTimeStruct.TimeUnitConfig == FrmRS_NULL){
		//��ָ�����
		void EngineFault_NullPointerGuard_Handle(void);
	}
	DeltaTime->DeltaTimeStruct.TimeUnitConfig();
	
	return 0;
}

/**
  * @brief  ʱ��������ⴥ������ÿ�δ�����¼ʱ�������������㲢����deltaTime��ֵ
  * @param  none
  * @retval none
  */
void FrmRS_deltaTime_Trigger(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//��ָ�����
	 }
	 if(DeltaTime->flag == 0){
		 DeltaTime->flag = 1;
		 return;
	 }
	 //���»��λ�����
	 DeltaTime->deltaTimeGroup[DeltaTime->deltaTimeIndex++] = DeltaTime->DeltaTimeStruct.CalculateTimeInterval(DeltaTime->DeltaTimeStruct.PrivateData);
	 if(DeltaTime->deltaTimeIndex >= DELTATIME_GROUP_COUNT){
			DeltaTime->deltaTimeIndex = 0;
	 }
	
	 //����ʱ������
	 double temp = 0;
	 for(int i = 0;i < DELTATIME_GROUP_COUNT;i++){
		 temp += DeltaTime->deltaTimeGroup[i];
	 }
	 temp /= (double)DELTATIME_GROUP_COUNT;
	 DeltaTime->deltaTime = temp;
	 return;
}

/**
  * @brief  ��ȡ��ǰ��ƽ��֡��
  * @param  none
  * @retval ֡��
  */
double FrmRS_GetFrmRate(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//��ָ�����
	 }
	
	 if(DeltaTime->deltaTime <= 1e-12){
		 return 1.0/1e-12;//����һ��֡�����ߴ��ֵ��
	 }
	 
	 return 1.0/DeltaTime->deltaTime;
}

/**
  * @brief  ��ȡ��ǰ��deltaTimeʱ��
  * @param  none
  * @retval ֡��
  */
double FrmRS_deltaTime_Get(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//��ָ�����
	 }
	 return DeltaTime->deltaTime;
}

/**
  * @brief  deltaTimeģ�Ϳռ��ͷ�
  * @param  none
  * @retval none
  */

void FrmRS_deltaTime_Free(void){
		if(DeltaTime == FrmRS_NULL)
				return;
		
		//�ͷ�˽����Դ
		if(DeltaTime->DeltaTimeStruct.FreeHandle){
			DeltaTime->DeltaTimeStruct.FreeHandle(DeltaTime->DeltaTimeStruct.PrivateData);
		}
		
		free(DeltaTime);
		DeltaTime = FrmRS_NULL;
		return;
}

