
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
 * @brief deltaTime ģ�ͳ�ʼ��
 *
 * �ú������ڳ�ʼ�� delta ʱ��ģ�飬Ϊ������ڴ桢���ñ�Ҫ�ļ�ʱ��Դ����ȷ������ʱ��������ܵ��������С�
 * 
 * ��ϸ˵����
 * - �ú��������Ƿ��Ѿ���ʼ���������ȫ�ֱ��� DeltaTime �Ƿ�Ϊ�գ���
 * - ���ģ����δ��ʼ������̬���� DeltaTime �ṹ�壬�������س�Ա������
 * - ���� `TimeUnitConfig()` �ӿں�������ɼ�ʱ���ĵײ����á�
 *
 * @param  DeltaTimeBaseStruct һ��ָ�� DeltaTimeBaseStruct_t ���͵�ָ�룬
 *                             �䶨���˵ײ��ʱ�������ú�ʱ��������ӿڡ�
 *
 * @retval  0      �ɹ���ʼ����
 * @retval -1      ����������󣬿����� DeltaTime �Ѿ���ʼ�������ߴ����ָ��Ϊ�ա�
 * @retval -2      �ڴ治�㣬DeltaTime ��̬����ʧ�ܡ�
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
 * @brief ʱ��������ⴥ����
 *
 * �ú������ڼ�¼���δ���֮���ʱ�������������������뻷�λ�������deltaTimeGroup ���飩��
 * ͬʱ��������� deltaTime ��ֵΪ������ɴ�ʱ��������ƽ��ֵ��
 *
 * ��ϸ˵����
 * - �ú�����Ҫ�����Ե��ã���ÿ֡����һ�Σ����Լ�¼ʱ������
 * - ���״ε���ʱ���ڲ��� flag ��־��ȷ�����Ե�һ�εĲ�������������ʼʱ�����������
 * - ���ʱ����������뻷�λ������������ݻ������е���ʷֵ���� deltaTime��ƽ��ʱ����������
 *
 * @param  �ޡ�
 * @retval �ޡ�
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
 * @brief ��ȡ��ǰ֡�ʣ�Frame Rate��
 *
 * ͨ����ǰʱ������ deltaTime ���㲢����֡�ʣ���λΪ֡/�루FPS����
 *
 * ��ϸ˵����
 * - ֡�ʼ��㹫ʽΪ 1.0 / deltaTime��
 * - ��� deltaTime ��ֵ�ǳ�С��С�� 1e-12���������᷵��һ�������֡��ֵ��1.0 / 1e-12���Ա��������㡣
 * - �˺���ͨ������ʵʱϵͳ�м������Ч�ʣ����߿���ѭ����֡�ʡ�
 *
 * @param  �ޡ�
 * @return double ��ǰ֡�ʣ���λΪ֡/�룩��
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
 * @brief ��ȡ��ǰ��ƽ��ʱ��������Delta Time��
 *
 * �ú������ڷ��ؾ������λ�����ƽ��������ʱ��������
 * ��ʱ�������ǵ��ú��� FrmRS_deltaTime_Trigger() ʱ���㲢���µõ��ġ�
 * 
 * ��ϸ˵����
 * - �����й����У����÷��ᶨʱ���� FrmRS_deltaTime_Trigger() ���ɼ�ʱ������
 *   ������β����������һ�����λ�������deltaTimeGroup ���飩��
 * - ÿ�δ����󣬻�Ի��λ������е�����ʱ����������Ͳ�ȡƽ����
 *   ���ƽ��ֵ��Ϊ��ǰ�� deltaTime����λΪ�롣��ֵͨ�����ڼ���֡�ʣ�ͨ�� 1.0/deltaTime��
 *   ������Ϊʵʱϵͳ�е�ʱ�䲽����ȷ���߼���������ʵʱ��ͬ����
 * - �ڵ��øú���ǰ����Ҫȷ������ȷ���� FrmRS_deltaTime_Init() ��ɳ�ʼ��������
 *   ����ȫ�ֱ��� DeltaTime ��Ϊ�գ�NULL����
 * - Ϊ�˷�ֹ��ָ����ʣ��ں�����ʼ�����ȼ�� DeltaTime �Ƿ�Ϊ�գ�
 *   ��Ϊ������ô������� EngineFault_NullPointerGuard_Handle() ���б�����
 *
 * @return double ���ص�ǰ���¼������ƽ��ʱ����������λΪ�롣
 *                ���磬��ϵͳ��Լ 60 FPS ���У��� deltaTime ��ֵͨ���ӽ� 1/60��Լ 0.01667���롣
 */
double FrmRS_deltaTime_Get(void){
	 if(DeltaTime == FrmRS_NULL){
			EngineFault_NullPointerGuard_Handle();//��ָ�����
	 }
	 return DeltaTime->deltaTime;
}

/**
 * @brief deltaTime ģ�����Դ�ͷ�
 *
 * �ͷ� DeltaTime ģ����ռ�õ��ڴ����Դ�������ڴ�й©��
 *
 * ��ϸ˵����
 * - ��� DeltaTime �������� `FreeHandle` �ӿ���Ч������øýӿ��ͷ�˽�����ݣ�PrivateData����
 * - �ͷŶ�̬����� DeltaTime �ڴ棬����ȫ��ָ�� DeltaTime ��Ϊ NULL��
 * - ��� DeltaTime �Ѿ�Ϊ�գ�δ��ʼ�������ͷţ���������ֱ�ӷ��ء�
 *
 * @param  �ޡ�
 * @retval �ޡ�
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

