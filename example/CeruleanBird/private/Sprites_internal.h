/**
  ******************************************************************************
  * @file    Sprites_internal.h
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   该文件包含了精灵库引擎内部的接口
  ******************************************************************************
  */
#ifndef __SPRITES_INTERNAL_H__
#define __SPRITES_INTERNAL_H__

/*inlcude--------------------------------------------------------------------------------*/
#include "Sprites_API.h"
#include "RB_Tree_API.h"

/*定义--------------------------------------------------------------------------------*/
typedef struct SpritesLinkNode_t SpritesLinkNode_t;//动画帧
typedef struct SpritesRecord_t SpritesRecord_t;		 //动画记录表


typedef struct SpriteBehaviorManger_t SpriteBehaviorManger_t;	 //行为管理器
typedef struct EncapsulatedContainer_t EncapsulatedContainer_t;//封装类型容器
typedef struct LeafContainer_t LeafContainer_t;								 //叶子类型容器
typedef struct SpriteITManageNode_t SpriteITManageNode_t;			 //中断表管理节点
typedef struct SpriteITNode_t SpriteITNode_t;									 //中断表节点
/**
*	精灵用户自定义数据结构体
*	@ref SpritesPrivate_t
*
*/
struct SpritesPrivate_t{
	void* data;//用户自定义私有数据
	SpritesPrivateFreeHandle_t SpritesPrivateFreeHandle;//当精灵的被销毁的时候，自动调用此函数回收资源
};


/**
	*	@breif 精灵结构体中动画控制模块的信息定义
	*/
typedef struct SpritesBase_Anim_t{
		int value;																	//精灵代号(动画)
		SpritesAnimationManager_t* SpritesManager;	//对应的精灵动画管理器地址
		AnimChain_t CurrentAnimationChainIndex;			//当前执行的动画链的索引;
		AnimChain_t defaultAnimationChainIndex;			//默认的动画链的索引;（主要用于恢复精灵动画状态）
		SpritesLinkNode_t* ReadyAnimation;					//就绪动画的地址
		unsigned int currentFrmIndex;								//当前所在的精灵帧的序号，主要用于帧跳跃使用（记录位置）
		AnimationType_t AnimationType;							//动画播放类型
}SpritesBase_Anim_t;


/**
	*	@breif 上次执行的节点类型
	*/
typedef enum{
		SBB_IT,//中断类型 
		SBB_BT,//行为树类型
		SBB_NO //空节点（还没有执行任何节点）
}SBB_NodeHandleType_t;
/**
	*	@breif 精灵结构体中行为控制模块的信息定义
	*/
typedef struct SpritesBase_Behavior_t{
		int value;																//管理节点编号(包含了行为树和中断表)
		unsigned char	IT_running;												//中断执行标志(正在执行为1，没有执行为0)				
		unsigned char 	IT_Enable;												//中断使能标志(使能为1，失能为0)
		//上下文
		struct{
				SITindex_t Previous_IT_index;					//上一帧执行的中断号（必须是连续的中断执行，如果上一帧为中断，下一帧为普通，那么就会立即被置为-1）
				SBB_NodeHandleType_t SBB_NodeHandleType;		//上次执行的节点类型
				union{
						struct SpriteBTNode_t *action;							//行为节点
						struct SpriteITNode_t *ITnode;							//中断节点
				}node;
		}bt_context;

}SpritesBase_Behavior_t;
/**
	*	@breif 基础精灵结构体
	*/
struct SpritesBase_t{
	/*空间位置信息*/
	double posRow;															//精灵行位置
	double posCol;															//精灵列位置
	int VerticalOffset;														//精灵的垂直方向的偏移量(正数向上偏移，负数向下偏移，单位：像素)
	/*帧率协调*/
	float accumulatedTime;													//累计时间
	/*模块*/
	SpritesBase_Anim_t AnimMoudle;											//动画模块控制
};

/**
	*	@breif 静态精灵结构体，此类精灵没有任何交互能力，可以用作建筑装饰品，它允许被动切换动画
	*/
struct SpritesStatic_t{
		SpritesBase_t SpritesBase;									//基础数据
};

/**
	*	@breif 精灵结构体
	*/
struct Sprites_t{
	/*精灵核心数据*/
	SpritesBase_t SpritesBase;										//基础数据
	/*精灵特征数据*/
	double MoveSpeed;												//精灵移动速度
	vector	dir;													//方向
	double rotate;													//旋转角速度
	/*精灵私有数据*/
	SpritesPrivate_t Private;										//精灵私有数据（具备数据与资源回收）
	/*模块*/
	SpritesBase_Behavior_t	BehaviorMoudle;							//行为树控制模块
	/*墙壁检查*/
	int wall;														//默认为空地值，碰到墙壁后会保留最新碰到的墙壁值，读取后被重置
};



/**
*	@breif	精灵特征类型
*
*/
typedef enum{
	NormalSprites,		//普通精灵
	StaticSprites		//静态精灵
}SpriteFeature_t;




/******************************

	精灵中断表

*******************************/

#define SIT_NODE_MAX_COUNT		5				//动态数组策略最多存储5个
#define SIT_ARR								0				//向量表动态数组策略
#define	SIT_RBTREE							1				//向量表红黑树策略

/**
	*@ref SpriteITManageNode
	*中断表管理节点
	*/
struct SpriteITManageNode_t{
		SITindex_t ITcount;	//中断数量
		unsigned char	Storage_strategy;//中断表存储策略 0为数组存储，1为红黑树存储
		SIT_MapIndexHandle_t SIT_MapIndexHandle;//中断查找函数
		union{
				struct SpriteITNode_t* head;//动态数组策略
				RB_Node* Sprites_IT_ManagerTree;//红黑树存储策略
		}table;
};

//中断表节点
/**
	*@ref SpriteITNode
	*中断表节点
	*/
struct SpriteITNode_t{
		SITindex_t value;								//中断编号
		unsigned char priority;							//中断优先级
		SpriteITType_t SpriteITType;					//中断取代类型
		SBT_ExecuteHandle_t ExecuteHandle;				//中断处理函数
};




/******************************

	精灵行为树

*******************************/

/**
	*@ref childType
	*容器用于存储孩子容器的策略，基本限度内使用静态数组，超过基本限度将采用动态数组策略
	*/
#define BT_STATIC										0								//静态数组
#define BT_DYNAMIC									1								//动态数组
/**
	*@ref EncapsulatedContainer_t
	*封装型容器，它允许封装任意类型的容器，包括自身类型的容器，但不允许封装逻辑函数
	*/
struct EncapsulatedContainer_t{
	
		int childCount;																			/*!<直系成员数量		*/
		
		int maxChildren;																		/*!<最大孩子数量		*/
		
		unsigned char childType;																/*!<孩子存储的策略	*/
	
		union {
			struct SpriteBTNode_t* fixedChildren[SpriteBTNode_MAX_COUNT]; 						/*!<静态数组			*/
			struct SpriteBTNode_t** dynamicChildren;  											/*!<动态数组，超过限度后改用动态数组		*/
		} children;
	
};

/**
	*@ref LeafContainer_t
	*叶子型容器，它不允许封装其他容器（即不可存在子节点），只允许封装逻辑函数
	*/
struct LeafContainer_t{
		SpriteActionType_t SpriteActionType;								/*!<精灵帧类型 @ref SpriteActionType_t	*/
		SBT_ExecuteHandle_t SBT_ExecuteHandle; 								/*!<执行函数		*/
		/*非完备帧特有数据*/
		SpriteITType_t SpriteITType;										/*!<取代类型		*/
		unsigned char priority;												/*!<优先级			*/
};

/**
*	精灵行为树节点结构体
*	@ref SpriteBTNode_t
*/
struct SpriteBTNode_t{
	SpriteBTType_t BTNodeType;														/*!<容器类型			*/
	//容器特性
	union{
			struct EncapsulatedContainer_t* EncapsulatedContainer;		/*!<封装型容器【选择器容器】【顺序流容器】	*/
			struct LeafContainer_t* LeafContainer;										/*!<叶子型容器【条件容器】【执行容器】	*/
	}ContainerType;
	
};


/**
*	精灵行为管理结构
*	@ref SpriteBehaviorManger_t
*/
struct SpriteBehaviorManger_t{
		struct SpriteBTNode_t* root;										//行为树根节点
		struct SpriteITManageNode_t* SpriteITManager;		//中断管理器									
};



//单个精灵行为树执行总线
int SpritesBT_BusExecute(Sprites_t* Sprites); 
//内部函数
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

	精灵动画树

*******************************/

//用于挂载在红黑树上的（每个精灵最多有256个动画链）
/**
*	精灵管理器
*	@ref SpritesManager_t
*
*/
struct SpritesAnimationManager_t{
	int value;															//精灵标号
	AnimChain_t PreallocationChainCount;		//预分配动画链组数量
	AnimChain_t AnimationChainCount;				//动画链组数量
	SpritesCount_t SpritesCount;						//使用此动画的精灵数量
	SpritesRecord_t* SpritesRecordGroup;		//精灵动画链信息查询表
};

/*
	【动画帧的性质】{
		1、同一个动画链的动画帧的数组长度很宽度必须一样
		2、同一个动画链的动画帧的类型必须相同，比如不能
			在同一个出现动画链同时出现bit型和byte型的帧
		3、动画帧的序号不能重复
	}
*/
/**
*	精灵动画链记录器
*	@ref SpritesRecord_t
*
*/
struct SpritesRecord_t{
	int Row;																	//精灵数组的实际行
	int Logic_ROW;														//精灵数组的逻辑行
	int Column;																//精灵数组的实际（逻辑）列。（列中实际和逻辑是相等的）
	AnimFrm_t frame_count;										//动画帧的数量
	float	frame_duration;											//单帧播放时间
	float	Anim_duration;											//完整动画的播放时长
	SPRITES_TYPE SpritesType;									//精灵数据类型（bit型还是byte型）
	SpritesLinkNode_t* AnimationChainGroup;		//动画链组，通过下标索引查找动画链（存储的是动画链头）
};

//精灵链表节点定义（bit/byte型动画链）
/**
*	精灵动画帧
*	@ref SpritesLinkNode_t
*
*/
struct SpritesLinkNode_t{
	char SerialNumber;									//动画帧的序号
	const unsigned char* SpritesData;		//精灵数据
	const unsigned char* SpritesShadow;	//精灵影子，用于判断当前位是透明还是灭
	SpritesRecord_t* record;						//反向查询记录表信息，用于获取长宽类型信息
	SpritesLinkNode_t* next;						//下一个动画链
};	



//提取指定精灵的指定坐标的信息
int SpritesAnim_ExtractFrmData(SpritesBase_t* sprites,int ROW_Pixel,int COL_Pixel);
//对指定精灵进行更新(该函数之前已经完成了所有逻辑的处理)
//void SpritesAnim_Update(SpritesBase_t* sprites);

//单精灵渲染
void Sprites_SingleRender(SpritesBase_t* sprites ,Camera_Struct* camera);


//返回精灵与玩家的距离
#define SpritesTree_DistanceFromplayer(SpritesBase,camera) ((float)((SpritesBase->posRow - camera->position.x)*(SpritesBase->posRow - camera->position.x) + \
																														(SpritesBase->posCol - camera->position.y)*(SpritesBase->posCol - camera->position.y)))   



////////////////////////////////////////////////////////////////////////////


/******************************

	全局精灵控制（精灵树）

*******************************/


/**
*	@breif	精灵树节点的精灵体描述定义
*
*/
typedef struct{
	SpritesBase_t* sprite;
	SpriteFeature_t SpriteFeature;
}SpriteTrDescript_t;

/**
*	@breif	精灵树节点
*
*/
typedef struct SpritesTreeNode_t{
	SpriteTrDescript_t SpriteTrDescript;
	struct SpritesTreeNode_t* next;
}SpritesTreeNode_t;
/**
*	@breif	精灵树定义
*
*/	
struct SpritesTree_t{
	int count;
	struct SpritesTreeNode_t* head;
};

extern SpritesTree_t* SpritesTree;
extern SpritesTree_t* SpritesSleepTree;
//全局精灵控制初始化
int SpritesTree_Init(void);
int SpritesTree_Add(Sprites_t* sprites);
int SpritesTree_StaticAdd(SpritesStatic_t* sprites);
void SpritesTree_Sort(Camera_Struct* camera);

SpritesTreeNode_t* SpritesTree_NodeExtract(SpritesBase_t* Sprite, SpritesTree_t* Tree); 
#endif
