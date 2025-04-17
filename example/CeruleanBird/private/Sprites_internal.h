/**
  ******************************************************************************
  * @file    Sprites_internal.h
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ���ļ������˾���������ڲ��Ľӿ�
  ******************************************************************************
  */
#ifndef __SPRITES_INTERNAL_H__
#define __SPRITES_INTERNAL_H__

/*inlcude--------------------------------------------------------------------------------*/
#include "Sprites_API.h"
#include "RB_Tree_API.h"

/*����--------------------------------------------------------------------------------*/
typedef struct SpritesLinkNode_t SpritesLinkNode_t;//����֡
typedef struct SpritesRecord_t SpritesRecord_t;		 //������¼��


typedef struct SpriteBehaviorManger_t SpriteBehaviorManger_t;	 //��Ϊ������
typedef struct EncapsulatedContainer_t EncapsulatedContainer_t;//��װ��������
typedef struct LeafContainer_t LeafContainer_t;								 //Ҷ����������
typedef struct SpriteITManageNode_t SpriteITManageNode_t;			 //�жϱ����ڵ�
typedef struct SpriteITNode_t SpriteITNode_t;									 //�жϱ�ڵ�
/**
*	�����û��Զ������ݽṹ��
*	@ref SpritesPrivate_t
*
*/
struct SpritesPrivate_t{
	void* data;//�û��Զ���˽������
	SpritesPrivateFreeHandle_t SpritesPrivateFreeHandle;//������ı����ٵ�ʱ���Զ����ô˺���������Դ
};


/**
	*	@breif ����ṹ���ж�������ģ�����Ϣ����
	*/
typedef struct SpritesBase_Anim_t{
		int value;																	//�������(����)
		SpritesAnimationManager_t* SpritesManager;	//��Ӧ�ľ��鶯����������ַ
		AnimChain_t CurrentAnimationChainIndex;			//��ǰִ�еĶ�����������;
		AnimChain_t defaultAnimationChainIndex;			//Ĭ�ϵĶ�����������;����Ҫ���ڻָ����鶯��״̬��
		SpritesLinkNode_t* ReadyAnimation;					//���������ĵ�ַ
		unsigned int currentFrmIndex;								//��ǰ���ڵľ���֡����ţ���Ҫ����֡��Ծʹ�ã���¼λ�ã�
		AnimationType_t AnimationType;							//������������
}SpritesBase_Anim_t;


/**
	*	@breif �ϴ�ִ�еĽڵ�����
	*/
typedef enum{
		SBB_IT,//�ж����� 
		SBB_BT,//��Ϊ������
		SBB_NO //�սڵ㣨��û��ִ���κνڵ㣩
}SBB_NodeHandleType_t;
/**
	*	@breif ����ṹ������Ϊ����ģ�����Ϣ����
	*/
typedef struct SpritesBase_Behavior_t{
		int value;																//����ڵ���(��������Ϊ�����жϱ�)
		unsigned char	IT_running;												//�ж�ִ�б�־(����ִ��Ϊ1��û��ִ��Ϊ0)				
		unsigned char 	IT_Enable;												//�ж�ʹ�ܱ�־(ʹ��Ϊ1��ʧ��Ϊ0)
		//������
		struct{
				SITindex_t Previous_IT_index;					//��һִ֡�е��жϺţ��������������ж�ִ�У������һ֡Ϊ�жϣ���һ֡Ϊ��ͨ����ô�ͻ���������Ϊ-1��
				SBB_NodeHandleType_t SBB_NodeHandleType;		//�ϴ�ִ�еĽڵ�����
				union{
						struct SpriteBTNode_t *action;							//��Ϊ�ڵ�
						struct SpriteITNode_t *ITnode;							//�жϽڵ�
				}node;
		}bt_context;

}SpritesBase_Behavior_t;
/**
	*	@breif ��������ṹ��
	*/
struct SpritesBase_t{
	/*�ռ�λ����Ϣ*/
	double posRow;															//������λ��
	double posCol;															//������λ��
	int VerticalOffset;														//����Ĵ�ֱ�����ƫ����(��������ƫ�ƣ���������ƫ�ƣ���λ������)
	/*֡��Э��*/
	float accumulatedTime;													//�ۼ�ʱ��
	/*ģ��*/
	SpritesBase_Anim_t AnimMoudle;											//����ģ�����
};

/**
	*	@breif ��̬����ṹ�壬���ྫ��û���κν���������������������װ��Ʒ�����������л�����
	*/
struct SpritesStatic_t{
		SpritesBase_t SpritesBase;									//��������
};

/**
	*	@breif ����ṹ��
	*/
struct Sprites_t{
	/*�����������*/
	SpritesBase_t SpritesBase;										//��������
	/*������������*/
	double MoveSpeed;												//�����ƶ��ٶ�
	vector	dir;													//����
	double rotate;													//��ת���ٶ�
	/*����˽������*/
	SpritesPrivate_t Private;										//����˽�����ݣ��߱���������Դ���գ�
	/*ģ��*/
	SpritesBase_Behavior_t	BehaviorMoudle;							//��Ϊ������ģ��
	/*ǽ�ڼ��*/
	int wall;														//Ĭ��Ϊ�յ�ֵ������ǽ�ں�ᱣ������������ǽ��ֵ����ȡ������
};



/**
*	@breif	������������
*
*/
typedef enum{
	NormalSprites,		//��ͨ����
	StaticSprites		//��̬����
}SpriteFeature_t;




/******************************

	�����жϱ�

*******************************/

#define SIT_NODE_MAX_COUNT		5				//��̬����������洢5��
#define SIT_ARR								0				//������̬�������
#define	SIT_RBTREE							1				//��������������

/**
	*@ref SpriteITManageNode
	*�жϱ����ڵ�
	*/
struct SpriteITManageNode_t{
		SITindex_t ITcount;	//�ж�����
		unsigned char	Storage_strategy;//�жϱ�洢���� 0Ϊ����洢��1Ϊ������洢
		SIT_MapIndexHandle_t SIT_MapIndexHandle;//�жϲ��Һ���
		union{
				struct SpriteITNode_t* head;//��̬�������
				RB_Node* Sprites_IT_ManagerTree;//������洢����
		}table;
};

//�жϱ�ڵ�
/**
	*@ref SpriteITNode
	*�жϱ�ڵ�
	*/
struct SpriteITNode_t{
		SITindex_t value;								//�жϱ��
		unsigned char priority;							//�ж����ȼ�
		SpriteITType_t SpriteITType;					//�ж�ȡ������
		SBT_ExecuteHandle_t ExecuteHandle;				//�жϴ�����
};




/******************************

	������Ϊ��

*******************************/

/**
	*@ref childType
	*�������ڴ洢���������Ĳ��ԣ������޶���ʹ�þ�̬���飬���������޶Ƚ����ö�̬�������
	*/
#define BT_STATIC										0								//��̬����
#define BT_DYNAMIC									1								//��̬����
/**
	*@ref EncapsulatedContainer_t
	*��װ���������������װ�������͵������������������͵����������������װ�߼�����
	*/
struct EncapsulatedContainer_t{
	
		int childCount;																			/*!<ֱϵ��Ա����		*/
		
		int maxChildren;																		/*!<���������		*/
		
		unsigned char childType;																/*!<���Ӵ洢�Ĳ���	*/
	
		union {
			struct SpriteBTNode_t* fixedChildren[SpriteBTNode_MAX_COUNT]; 						/*!<��̬����			*/
			struct SpriteBTNode_t** dynamicChildren;  											/*!<��̬���飬�����޶Ⱥ���ö�̬����		*/
		} children;
	
};

/**
	*@ref LeafContainer_t
	*Ҷ�������������������װ���������������ɴ����ӽڵ㣩��ֻ�����װ�߼�����
	*/
struct LeafContainer_t{
		SpriteActionType_t SpriteActionType;								/*!<����֡���� @ref SpriteActionType_t	*/
		SBT_ExecuteHandle_t SBT_ExecuteHandle; 								/*!<ִ�к���		*/
		/*���걸֡��������*/
		SpriteITType_t SpriteITType;										/*!<ȡ������		*/
		unsigned char priority;												/*!<���ȼ�			*/
};

/**
*	������Ϊ���ڵ�ṹ��
*	@ref SpriteBTNode_t
*/
struct SpriteBTNode_t{
	SpriteBTType_t BTNodeType;														/*!<��������			*/
	//��������
	union{
			struct EncapsulatedContainer_t* EncapsulatedContainer;		/*!<��װ��������ѡ������������˳����������	*/
			struct LeafContainer_t* LeafContainer;										/*!<Ҷ����������������������ִ��������	*/
	}ContainerType;
	
};


/**
*	������Ϊ����ṹ
*	@ref SpriteBehaviorManger_t
*/
struct SpriteBehaviorManger_t{
		struct SpriteBTNode_t* root;										//��Ϊ�����ڵ�
		struct SpriteITManageNode_t* SpriteITManager;		//�жϹ�����									
};



//����������Ϊ��ִ������
int SpritesBT_BusExecute(Sprites_t* Sprites); 
//�ڲ�����
SpriteBTStatus_t ExecuteBehaviorTree(SpriteBTNode_t* root, Sprites_t* Sprites);
SpriteBTNode_t* Sprites_BT_ECNodeCreate(void);
SpriteBTNode_t* Sprites_BT_LCNodeCreate(void);
SpriteBTNode_t* Sprites_BT_GetChild(SpriteBTNode_t* parent, int index);
int SpritesIT_RBStorageStrategyCreate(SpriteBehaviorManger_t* SpriteBehaviorManger,SpriteITNode_t* arr);
SpriteITNode_t* SpritesIT_GetITNode(SpriteBehaviorManger_t* SpriteBehaviorManger,SITindex_t IT_index);
int SpritesIT_Exec(Sprites_t* Sprites,SpriteBehaviorManger_t* SpriteBehaviorManger);
void SpritesPhased_Exec(Sprites_t* Sprites);

int Sprites_SingleUpdate(SpritesBase_t* sprite, SpriteFeature_t SpriteFeature);
int BresenhamLine_WallDetect(int x1, int y1, int x2, int y2);

/******************************

	���鶯����

*******************************/

//���ڹ����ں�����ϵģ�ÿ�����������256����������
/**
*	���������
*	@ref SpritesManager_t
*
*/
struct SpritesAnimationManager_t{
	int value;															//������
	AnimChain_t PreallocationChainCount;		//Ԥ���䶯����������
	AnimChain_t AnimationChainCount;				//������������
	SpritesCount_t SpritesCount;						//ʹ�ô˶����ľ�������
	SpritesRecord_t* SpritesRecordGroup;		//���鶯������Ϣ��ѯ��
};

/*
	������֡�����ʡ�{
		1��ͬһ���������Ķ���֡�����鳤�Ⱥܿ�ȱ���һ��
		2��ͬһ���������Ķ���֡�����ͱ�����ͬ�����粻��
			��ͬһ�����ֶ�����ͬʱ����bit�ͺ�byte�͵�֡
		3������֡����Ų����ظ�
	}
*/
/**
*	���鶯������¼��
*	@ref SpritesRecord_t
*
*/
struct SpritesRecord_t{
	int Row;																	//���������ʵ����
	int Logic_ROW;														//����������߼���
	int Column;																//���������ʵ�ʣ��߼����С�������ʵ�ʺ��߼�����ȵģ�
	AnimFrm_t frame_count;										//����֡������
	float	frame_duration;											//��֡����ʱ��
	float	Anim_duration;											//���������Ĳ���ʱ��
	SPRITES_TYPE SpritesType;									//�����������ͣ�bit�ͻ���byte�ͣ�
	SpritesLinkNode_t* AnimationChainGroup;		//�������飬ͨ���±��������Ҷ��������洢���Ƕ�����ͷ��
};

//��������ڵ㶨�壨bit/byte�Ͷ�������
/**
*	���鶯��֡
*	@ref SpritesLinkNode_t
*
*/
struct SpritesLinkNode_t{
	char SerialNumber;									//����֡�����
	const unsigned char* SpritesData;		//��������
	const unsigned char* SpritesShadow;	//����Ӱ�ӣ������жϵ�ǰλ��͸��������
	SpritesRecord_t* record;						//�����ѯ��¼����Ϣ�����ڻ�ȡ����������Ϣ
	SpritesLinkNode_t* next;						//��һ��������
};	



//��ȡָ�������ָ���������Ϣ
int SpritesAnim_ExtractFrmData(SpritesBase_t* sprites,int ROW_Pixel,int COL_Pixel);
//��ָ��������и���(�ú���֮ǰ�Ѿ�����������߼��Ĵ���)
//void SpritesAnim_Update(SpritesBase_t* sprites);

//��������Ⱦ
void Sprites_SingleRender(SpritesBase_t* sprites ,Camera_Struct* camera);


//���ؾ�������ҵľ���
#define SpritesTree_DistanceFromplayer(SpritesBase,camera) ((float)((SpritesBase->posRow - camera->position.x)*(SpritesBase->posRow - camera->position.x) + \
																														(SpritesBase->posCol - camera->position.y)*(SpritesBase->posCol - camera->position.y)))   



////////////////////////////////////////////////////////////////////////////


/******************************

	ȫ�־�����ƣ���������

*******************************/


/**
*	@breif	�������ڵ�ľ�������������
*
*/
typedef struct{
	SpritesBase_t* sprite;
	SpriteFeature_t SpriteFeature;
}SpriteTrDescript_t;

/**
*	@breif	�������ڵ�
*
*/
typedef struct SpritesTreeNode_t{
	SpriteTrDescript_t SpriteTrDescript;
	struct SpritesTreeNode_t* next;
}SpritesTreeNode_t;
/**
*	@breif	����������
*
*/	
struct SpritesTree_t{
	int count;
	struct SpritesTreeNode_t* head;
};

extern SpritesTree_t* SpritesTree;
extern SpritesTree_t* SpritesSleepTree;
//ȫ�־�����Ƴ�ʼ��
int SpritesTree_Init(void);
int SpritesTree_Add(Sprites_t* sprites);
int SpritesTree_StaticAdd(SpritesStatic_t* sprites);
void SpritesTree_Sort(Camera_Struct* camera);

SpritesTreeNode_t* SpritesTree_NodeExtract(SpritesBase_t* Sprite, SpritesTree_t* Tree); 
#endif
