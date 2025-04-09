/**
  ******************************************************************************
  * @file    Sprites_API.h
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ���ļ������˾�������Ľӿ�
  ******************************************************************************
  */
#ifndef __SPRITES_API_H__
#define __SPRITES_API_H__

/*inlcude--------------------------------------------------------------------------------*/

#include "vector.h"
#include "Camera_API.h"
/*
		���鶯������ģ��
									����������ƽṹ��
										
										�����������������ڵ㣩
											|
											V
										�����¼��Ҳ�ж�����������̬���飬��ͨ��������λ����������¼��洢������������Ķ���֡�����Ϣ��
										|        |	      |
										V		 V		  V			V
									  ����֡     ����֡   ����֡	
									������֡��)(����֡��)(����֡�飩
												
			
*/

/*
������״̬��
	��״̬���͡�
		�ǻ�Ծ̬��
			1������״̬��������ȫ��������ԣ� ���Ǳ�����������ѣ������޷�������ִ��
			
			2������״̬��������������ʱִֹͣ���߼���ִ�е�ǰ�Ľ�������������ը����������������������(�����������״̬)
			
		��Ծ̬��
			1����ת״̬�������ִ���߼�����ֹͣ���������ŵ�ǰ���������������ڶԶ�֡���߼��ͳ�֡�ʶ�����ƥ��ʱ�����߼���䣬�Ա�֤�����������У�
			
			2����ֹ״̬�������ִ���߼�����ֹͣ�����Ҷ���Ҳ����ֹͣ״̬����ʱ����ᱣ����һ��״̬�Ķ���֡��ֹ�ڵ�ͼ�С�
			
			3���ƶ�״̬������ִ���ƶ��߼������Ŷ�����
						|
						+--->�����ƶ������磺�з�npcû�з������ʱ������˶�
						|
						+--->�����ƶ������磺�з�NPC��������ƶ�
			
			4������״̬������ִ�й����߼������Ŷ���
			
			5���ܻ�״̬������ִ���׻��߼������Ŷ���
		
			n���Զ���״̬������
*/


/*����--------------------------------------------------------------------------------*/

/************************************************

		�����Զ������á�

*************************************************/
typedef unsigned char AnimChain_t;//�������������������ͣ����������������˶������������������ޣ� 
typedef unsigned short SpritesCount_t;//�����������������ͣ����������ͼ�¼��һ���������Ͷ�Ӧ�����ľ���ʵ��������
typedef	unsigned int AnimFrm_t;//�������й��صĶ���֡����

/************************************************
		��������
*************************************************/

typedef enum SPRITES_TYPE SPRITES_TYPE;		//�����������ͣ�bit��/�ֽ��ͣ�
typedef struct SpritesAnimationManager_t SpritesAnimationManager_t;//���鶯����������ÿһ���������Ͷ�Ӧһ��������������

typedef struct SpritesPrivate_t SpritesPrivate_t;//����˽�����ݽṹ��
typedef void (*SpritesPrivateFreeHandle_t)(void* pdata);//����˽�����ݵĴ���ص��������塣

typedef struct SpritesBase_t SpritesBase_t;						//���������Ԫ
typedef struct SpritesStatic_t SpritesStatic_t;					//��̬����
typedef struct Sprites_t Sprites_t;								//���������ͨ���飩

typedef struct SpritesTree_t SpritesTree_t;//�����������ڼ�¼�����о����ʵ��(��������)


typedef enum SpriteBTStatus_t SpriteBTStatus_t;//��Ϊ���ڵ����� 
typedef struct SpriteBTNode_t SpriteBTNode_t;//��Ϊ���ڵ�
typedef int SITindex_t;//�ж����������ͣ����ں������Ƶ��������⣬ֻ��Ϊint��

typedef SpriteBTStatus_t (*SBT_ExecuteHandle_t)(Sprites_t* Sprites,void* privateData);/*!<��Ϊ���ڵ�ִ�к���
																							@param Sprites ����
																							@param privateData �����е�˽��������
																							@retval ִ��״̬
																						*/
typedef SITindex_t (*SIT_MapIndexHandle_t)(Sprites_t* Sprites,void* privateData);/*!<�жϲ��Һ����Զ���ӿ�,
																							@param Sprites ����
																							@param privateData �����е�˽��������
																							@retval ����ֵ�ǽ�Ҫ���õ��ж�������ţ�����ʧ���򷵻����⸺ֵ
																						*/ 


#define SpriteBTNode_MAX_COUNT 5						//��Ϊ�������г�ʼ������������������������л�Ϊ��̬������ԣ�

/**
	*	@breif ������Ϊ��BehaviorTree�ڵ�����
	*	
	*	@note	�ڵ���ͣ�
	*					�����������ڵ㡿������Ҫ������ʵ�������жϣ�Ҳ������if�����ݴ˽ڵ���ص��������������������ض�Ӧ�Ĳ���ֵ
	*					��ѡ���������ڵ㡿������Ҫ������ѡ��ִ�й����ڴ˽ڵ����Ϊ�飬ֱ��ִ�����һ�η��ؽ��Ϊ��Ľڵ㡣����
	*												[Selector]
	*												 ������ [Condition] Ŀ���Ƿ���Զ�̹�����Χ�� �� [Action] Զ�̹���
	*												 ������ [Condition] Ŀ���Ƿ��ڽ�ս��Χ�� �� [Action] ��ս����
	*												 ������ [Action] ����Ŀ��
	*												���ִ���ˡ�Ŀ���Ƿ���Զ�̹�����Χ���ķ��ؽ��Ϊ�٣���ô��Ҫִ�С�Ŀ���Ƿ��ڽ�ս��Χ����
	*												ֱ�����ؽ��Ϊ��������˳�ѡ��������Ȼ�����Ŀ���Ƿ���Զ�̹�����Χ��Ϊ�棬��ô�����
	*												���нڵ����ִ�С�
	*					��˳���������ڵ㡿������Ҫ����������ִ�����й�������Ϊ��Ľڵ㣬ֱ���������׸����ؽ��Ϊ�ٵĽڵ�������˳���
	*												[Sequence]
	*												 ������ [Condition] Ŀ���Ƿ���Զ�̹�����Χ�� �� [Action] Զ�̹���
	*												 ������ [Condition] Ŀ���Ƿ��ڽ�ս��Χ�� �� [Action] ��ս����
	*												 ������ [Action] ����Ŀ��
	*												���ִ���ˡ�Ŀ���Ƿ���Զ�̹�����Χ���ķ��ؽ��Ϊ�棬��ô��Ҫִ�С�Ŀ���Ƿ��ڽ�ս��Χ����
	*												ֱ�����ؽ��Ϊ�ٺ������˳�˳��������Ȼ�����Ŀ���Ƿ���Զ�̹�����Χ��Ϊ�٣���ô�����
	*												���нڵ����ִ�С�
	*					�����������ڵ㡿�������Ӧ������
	*
	*					���У���˳�������롾ѡ��������װ��������������������͵�����������û���Լ���ִ�к�����
	*								�������������롾ִ������������α���������ǲ��������������ֻ�������ִ�к��������⡾������������ִ�к���
	*								 ֻ�����ж����õĺ�����
	*					
	*						���ʾ����
	*					[Root] 
	*					 ������ (ѡ��) [�Ƿ񿴵���ң�]
	*					 ��   ������ (˳��) Զ�̹����߼�
	*					 ��   ��   ������ (����) ��Ҿ������У�
	*					 ��   ��   ������ (ִ��) Զ�����
	*					 ��   ������ (˳��) ��սӦ���߼�
	*					 ��   ��   ������ (����) ��Ҿ���̫����
	*					 ��   ��   ������ (ѡ��) �ȳ��Գ���
	*					 ��   ��   ��   ������ (ִ��) ����
	*					 ��   ��   ��   ������ (ִ��) ��ս����
	*
	*
	*/
typedef enum{
	SBT_Condition,								/*!<���������ڵ�			*/
	SBT_Action, 								/*!<���������ڵ�			*/
	SBT_Selector,								/*!<ѡ���������ڵ�		*/
	SBT_Sequence 								/*!<˳��������	�ڵ�	*/
}SpriteBTType_t;




//˲ʱ�߼��ͳ����߼�
//˲ʱ�߼�״̬
enum SpriteBTStatus_t {
    SBT_FAILURE,  // ��Ϊʧ��
    SBT_SUCCESS,  // ��Ϊ�ɹ�
    SBT_RUNNING   // ��Ϊ����ִ����
};



/**
*	�ж�ȡ������
*	@ref SpriteITType_t
*
*/
typedef enum SpriteITType_t{
		SIT_AllowSubstitution,		//��ǰִ�еĽڵ����������ȼ��ж�ȡ��ִ��
		SIT_ProhibitSubstitution	//��ǰִ�нڵ��ֹ�������ȼ��ж�ȡ��ִ��
}SpriteITType_t;

/**
*	����֡����
*	@ref SpriteActionType_t
*
*/
typedef enum SpriteActionType_t{
		SAT_STEADY,												/*!<�걸���ԣ�ָһ������������ÿ֡���߼����Եȷֵ�֡��������·�߼�	.
																					�걸���Ծ��������ص㣺
																									1������ֵֻ����SBT_SUCCESS����SBT_FAILURE�����ΪSBT_RUNNING��ᱻ
																										 ��Ϊ����SBT_SUCCESS
																									2�����Ա������жϽ���ȡ�������߱����ȼ�����
																			*/
	
		SAT_PHASED												/*!<���걸���ԣ�ָһ�������������߼����ܵȷֵ�֡�����������һ֡Ϊ�߼�������Ϊ������	
																					���걸���Ծ��������ص㣺
																									1������ֵ���� @ref SpriteBTStatus_t�����е�����
																									2�������Զ���ȡ��������� @ref SpriteITType_t��
																											�������ȡ������ᱻ�������ȼ����жϴ���
																											���������ȡ��������޷����κ����ȼ����ж�ȡ�������ȼ��ᱻ���ԡ�
															
																			*/
}SpriteActionType_t;










/**
*	������������
*	@ref SPRITES_TYPE
*
*/
enum SPRITES_TYPE{
		BYTE_SPRITES,		   /*!<��������Ԫ�ص�ֵ���ֽ�Ϊ��λ��һ�����ֽڿ���һ�����ص㣩*/
	
		BIT_SPRITES,		   /*!<��������Ԫ�ص�ֵ��λΪ��λ��һ��λ����һ�����ص㣩*/
	
		ERROR_SPRITES		   /*!<������ֵ*/
};

/**
*	 @breif ��������
*
*/
typedef enum {
    Anim_Loop,		// ѭ������
    Anim_Once,		// ���β��Ŷ���
	Anim_Error		// ���󶯻�����
} AnimationType_t;



//�����������ߺ���
void* Sprites_Exec(Camera_Struct* camera);








/*�����ӿ�-------------------------------------------------------------------*/
//����ͬ���͵ľ���ת��Ϊ��������
#define TO_SPRITES_BASE(ptr) ((SpritesBase_t*)(ptr))  

//����������
int SpritesAnim_CreateManager(int value);
int SpritesAnim_DelManager(int value);
int SpritesAnim_GetManagerIndex(SpritesBase_t* sprites);

//����������
int SpritesAnim_CreateNewChain(int value,int ROW_Pixel,int COL_Pixel,SPRITES_TYPE SpritesType,float Anim_duration);
int SpritesAnim_SwitchChain(SpritesBase_t* sprites,AnimChain_t NewtAnimationChainIndex);
int SpritesAnim_SwitchDefaultChain(SpritesBase_t* sprites);
int SpritesAnim_SetDefaultChain(SpritesBase_t* sprites,AnimChain_t newDefaultChainIdx);
int SpritesAnim_GetFrmCountOfChain(SpritesBase_t* sprites,AnimChain_t ChainIdx);
AnimChain_t SpritesAnim_GetChainIndex(SpritesBase_t* sprites);

//����֡����
void SpritesAnim_Update(SpritesBase_t* sprites);
int SpritesAnim_InsertNewFrm(int AnimationManageIndex,int AnimationChainIndex,int AnimationFrameSeq,const unsigned char* SpritesData,const unsigned char* SpritesShadow);
int SpritesAnim_SwitchFirstFrm(SpritesBase_t* sprites);
int SpritesAnim_SetAnimType(SpritesBase_t* sprites,AnimationType_t AnimationType);
AnimationType_t SpritesAnim_GetAnimType(SpritesBase_t* sprites);
int SpritesAnim_IsEndFrm(SpritesBase_t* sprites);


/*��Ϊ�ӿ�-------------------------------------------------------------------*/
int SpritesBT_Init(int value);

//��Ϊ������
SpriteBTNode_t* SpritesBT_SelectorCreate(void);
SpriteBTNode_t* SpritesBT_Sequence(void);
SpriteBTNode_t* SpritesBT_Condition(SBT_ExecuteHandle_t SBT_ExecuteHandle);
SpriteBTNode_t* SpritesBT_Action(SBT_ExecuteHandle_t SBT_ExecuteHandle);
int SpritesBT_InsertBehavior(SpriteBTNode_t* parent, SpriteBTNode_t* child);
int SpritesBT_MountToRoot(int value, SpriteBTNode_t* node);

//�����ڵ����Բ���
int SpritesBT_Action_ToPhased(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority);
int SpritesBT_Action_ToSteady(SpriteBTNode_t* node);
int SpritesBT_Action_ModPriority(SpriteBTNode_t* node,unsigned char priority);
int SpritesBT_Action_ModITType(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority);



//�жϱ����
int SpritesIT_Register(int value,SIT_MapIndexHandle_t SIT_MapIndexHandle);
int SpritesIT_Add(int value ,SITindex_t IT_index,unsigned char priority,SpriteITType_t SpriteITType,SBT_ExecuteHandle_t SBT_ExecuteHandle);
int SpritesIT_Trigger(Sprites_t* Sprites);//�ж���Ҫ�ֶ�������������������жϣ����ɣ���
int SpritesIT_TriggerClear(Sprites_t* Sprites);
int SpritesIT_Enable(Sprites_t* Sprites);
int SpritesIT_Disable(Sprites_t* Sprites);


/*��ͨ����ӿ�--------------------------------------------------------------------*/
//��ͨ����ӿ�
Sprites_t* Sprites_ObjectCreate(int value,double posRow,double posCol,unsigned char defaultAnimationChainIndex,double moveSpeed,int map_value,int BehaviorTreeValue);

//λ�ò���
int SpritesMov_SetDir(Sprites_t* sprites,vector dir);
vector SpritesMov_GetDir(Sprites_t* sprites);
int SpritesMov_Left(Sprites_t* sprite);
int SpritesMov_Right(Sprites_t* sprite);
int SpritesMov_Forward(Sprites_t* sprite);
int SpritesMov_Backward(Sprites_t* sprite);
int SpritesMov_SetDirToCamera(Sprites_t* sprites,Camera_Struct* camera);
int SpritesMov_SetDirWithCamera(Sprites_t* sprites,Camera_Struct* camera);
int SpritesMov_SetPosToCamera(Sprites_t* sprites,Camera_Struct* camera);
int Sprites_collisionWallValue(Sprites_t* sprite);

/*��̬����ӿ�--------------------------------------------------------------------*/


/*����ͨ�ò����ӿ�----------------------------------------------------------------*/
int SpritesData_SetVerticalHeight(SpritesBase_t* sprites,int VerticalHeight);
int SpritesData_GetVerticalHeight(SpritesBase_t* sprites);
vector SpritesData_GetPos(SpritesBase_t* sprites);
int SpritesData_VerticalHeight_MoveUp(SpritesBase_t* sprites,int VerticalHeight);
int SpritesData_VerticalHeight_MoveDown(SpritesBase_t* sprites,int VerticalHeight);

int Sprites_Sleep(SpritesBase_t* Sprite);
int Sprites_Awaken(SpritesBase_t* Sprite);
/*˽�����ݲ����ӿ�----------------------------------------------------------------*/
int SpritesPriv_Add(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);
int SpritesPriv_Replace(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);
void SpritesPriv_Reset(Sprites_t* sprites);
void* SpritesPriv_Get(Sprites_t* sprites);










/*


��������ǰ����������߼���
	����ȡָ���߼��洢�ĺ�����
	���滻ָ���߼��еĺ�����
		
��˽�����ݲ�����
	��˽��������ӡ�
	1���������˽������
		int SpritesPriv_Add(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);//ֻ��˽������Ϊ�յ�ʱ��������ӣ����򷵻�ʧ��ֵ
	2�����µ�˽�������滻ԭ�е�˽������
		int SpritesPriv_Replace(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);//���ԭ��û��˽�����ݣ���ֱ����ӣ���������ȵ�����Դ��������Դ����
		
	��˽�����������
	void SpritesPriv_Del(Sprites_t* sprites);
	
	��ȡ˽������
	void* SpritesPriv_Get(Sprites_t* sprites);
	
*/

#endif
