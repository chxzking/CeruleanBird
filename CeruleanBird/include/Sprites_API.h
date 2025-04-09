/**
  ******************************************************************************
  * @file    Sprites_API.h
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   该文件包含了精灵库对外的接口
  ******************************************************************************
  */
#ifndef __SPRITES_API_H__
#define __SPRITES_API_H__

/*inlcude--------------------------------------------------------------------------------*/

#include "vector.h"
#include "Camera_API.h"
/*
		精灵动画管理模型
									【动画链设计结构】
										
										精灵管理器（红黑树节点）
											|
											V
										精灵记录表（也叫动画链）（动态数组，可通过索引定位动画链，记录表存储量挂载在上面的动画帧组的信息）
										|        |	      |
										V		 V		  V			V
									  动画帧     动画帧   动画帧	
									（动画帧组)(动画帧组)(动画帧组）
												
			
*/

/*
【精灵状态】
	【状态类型】
		非活跃态：
			1、隐藏状态：精灵完全被引擎忽略， 除非被外界主动唤醒，否则将无法被引擎执行
			
			2、死亡状态：精灵死亡，此时停止执行逻辑，执行当前的结束动画（火球爆炸）或者立即结束（弓箭）(最后陷入隐藏状态)
			
		活跃态：
			1、空转状态：精灵的执行逻辑处于停止，继续播放当前动画。（可以用于对短帧率逻辑和长帧率动画不匹配时进行逻辑填充，以保证动画正常进行）
			
			2、静止状态：精灵的执行逻辑处于停止，并且动画也处于停止状态，此时精灵会保持上一次状态的动画帧静止在地图中。
			
			3、移动状态：精灵执行移动逻辑，播放动画。
						|
						+--->自由移动：例如：敌方npc没有发现玩家时的随机运动
						|
						+--->定向移动：例如：敌方NPC朝着玩家移动
			
			4、攻击状态：精灵执行攻击逻辑，播放动画
			
			5、受击状态：精灵执行首击逻辑，播放动画
		
			n、自定义状态：待定
*/


/*定义--------------------------------------------------------------------------------*/

/************************************************

		【可自定义配置】

*************************************************/
typedef unsigned char AnimChain_t;//动画链索引的数据类型（此数据类型设置了动画链组的最大数量上限） 
typedef unsigned short SpritesCount_t;//精灵总数的数据类型（此数据类型记录了一个精灵类型对应创建的精灵实例数量）
typedef	unsigned int AnimFrm_t;//动画链中挂载的动画帧数量

/************************************************
		【声明】
*************************************************/

typedef enum SPRITES_TYPE SPRITES_TYPE;		//精灵数据类型（bit型/字节型）
typedef struct SpritesAnimationManager_t SpritesAnimationManager_t;//精灵动画管理器（每一个精灵类型对应一个动画管理器）

typedef struct SpritesPrivate_t SpritesPrivate_t;//精灵私有数据结构体
typedef void (*SpritesPrivateFreeHandle_t)(void* pdata);//精灵私有数据的处理回调函数定义。

typedef struct SpritesBase_t SpritesBase_t;						//精灵基础单元
typedef struct SpritesStatic_t SpritesStatic_t;					//静态精灵
typedef struct Sprites_t Sprites_t;								//精灵对象（普通精灵）

typedef struct SpritesTree_t SpritesTree_t;//精灵树，用于记录了所有精灵的实体(待定对象)


typedef enum SpriteBTStatus_t SpriteBTStatus_t;//行为树节点类型 
typedef struct SpriteBTNode_t SpriteBTNode_t;//行为树节点
typedef int SITindex_t;//中断数量的类型（由于红黑树设计的遗留问题，只能为int）

typedef SpriteBTStatus_t (*SBT_ExecuteHandle_t)(Sprites_t* Sprites,void* privateData);/*!<行为树节点执行函数
																							@param Sprites 精灵
																							@param privateData 精灵中的私有数据域
																							@retval 执行状态
																						*/
typedef SITindex_t (*SIT_MapIndexHandle_t)(Sprites_t* Sprites,void* privateData);/*!<中断查找函数自定义接口,
																							@param Sprites 精灵
																							@param privateData 精灵中的私有数据域
																							@retval 返回值是将要调用的中断索引编号，查找失败则返回任意负值
																						*/ 


#define SpriteBTNode_MAX_COUNT 5						//行为树容器中初始化最大容量（超过此容量将切换为动态数组策略）

/**
	*	@breif 精灵行为树BehaviorTree节点类型
	*	
	*	@note	节点解释：
	*					【条件容器节点】它的主要作用是实现条件判断，也类似于if，根据此节点挂载的条件函数处理结果，返回对应的布尔值
	*					【选择器容器节点】它的主要作用是选择执行挂载于此节点的行为组，直到执行完成一次返回结果为真的节点。例如
	*												[Selector]
	*												 ├── [Condition] 目标是否在远程攻击范围？ → [Action] 远程攻击
	*												 ├── [Condition] 目标是否在近战范围？ → [Action] 近战攻击
	*												 ├── [Action] 靠近目标
	*												如果执行了【目标是否在远程攻击范围】的返回结果为假，那么需要执行【目标是否在近战范围】，
	*												直到返回结果为真后，立即退出选择器。当然如果【目标是否在远程攻击范围】为真，那么后面的
	*												所有节点均不执行。
	*					【顺序流容器节点】它的主要作用是依次执行所有挂载至行为组的节点，直到遇到了首个返回结果为假的节点后立即退出。
	*												[Sequence]
	*												 ├── [Condition] 目标是否在远程攻击范围？ → [Action] 远程攻击
	*												 ├── [Condition] 目标是否在近战范围？ → [Action] 近战攻击
	*												 ├── [Action] 靠近目标
	*												如果执行了【目标是否在远程攻击范围】的返回结果为真，那么需要执行【目标是否在近战范围】，
	*												直到返回结果为假后，立即退出顺序流。当然如果【目标是否在远程攻击范围】为假，那么后面的
	*												所有节点均不执行。
	*					【动作容器节点】精灵的响应动作。
	*
	*					其中，【顺序流】与【选择器】封装容器，允许包含任意类型的容器，但是没有自己的执行函数。
	*								【条件容器】与【执行容器】属于伪容器，他们不允许包含容器，只允许存在执行函数，此外【条件容器】的执行函数
	*								 只能是判断作用的函数。
	*					
	*						大纲示例：
	*					[Root] 
	*					 ├── (选择) [是否看到玩家？]
	*					 │   ├── (顺序) 远程攻击逻辑
	*					 │   │   ├── (条件) 玩家距离适中？
	*					 │   │   ├── (执行) 远程射击
	*					 │   ├── (顺序) 近战应对逻辑
	*					 │   │   ├── (条件) 玩家距离太近？
	*					 │   │   ├── (选择) 先尝试撤退
	*					 │   │   │   ├── (执行) 撤退
	*					 │   │   │   ├── (执行) 近战攻击
	*
	*
	*/
typedef enum{
	SBT_Condition,								/*!<条件容器节点			*/
	SBT_Action, 								/*!<动作容器节点			*/
	SBT_Selector,								/*!<选择器容器节点		*/
	SBT_Sequence 								/*!<顺序流容器	节点	*/
}SpriteBTType_t;




//瞬时逻辑和持续逻辑
//瞬时逻辑状态
enum SpriteBTStatus_t {
    SBT_FAILURE,  // 行为失败
    SBT_SUCCESS,  // 行为成功
    SBT_RUNNING   // 行为仍在执行中
};



/**
*	中断取代类型
*	@ref SpriteITType_t
*
*/
typedef enum SpriteITType_t{
		SIT_AllowSubstitution,		//当前执行的节点允许被高优先级中断取代执行
		SIT_ProhibitSubstitution	//当前执行节点禁止被高优先级中断取代执行
}SpriteITType_t;

/**
*	动作帧类型
*	@ref SpriteActionType_t
*
*/
typedef enum SpriteActionType_t{
		SAT_STEADY,												/*!<完备属性，指一个运行周期中每帧的逻辑可以等分的帧，例如走路逻辑	.
																					完备属性具有以下特点：
																									1、返回值只存在SBT_SUCCESS或者SBT_FAILURE，如果为SBT_RUNNING则会被
																										 视为存在SBT_SUCCESS
																									2、可以被任意中断进行取代。不具备优先级属性
																			*/
	
		SAT_PHASED												/*!<非完备属性，指一个运行周期中逻辑不能等分的帧，例如射击，一帧为逻辑，其余为动画。	
																					非完备属性具有以下特点：
																									1、返回值具有 @ref SpriteBTStatus_t中所有的类型
																									2、具有自定义取代类型设计 @ref SpriteITType_t，
																											如果允许取代，则会被更高优先级的中断代替
																											如果不允许取代，则会无法被任何优先级的中断取代其优先级会被忽略。
															
																			*/
}SpriteActionType_t;










/**
*	精灵数组类型
*	@ref SPRITES_TYPE
*
*/
enum SPRITES_TYPE{
		BYTE_SPRITES,		   /*!<精灵数组元素的值以字节为单位（一整个字节控制一个像素点）*/
	
		BIT_SPRITES,		   /*!<精灵数组元素的值以位为单位（一个位控制一个像素点）*/
	
		ERROR_SPRITES		   /*!<错误精灵值*/
};

/**
*	 @breif 动画类型
*
*/
typedef enum {
    Anim_Loop,		// 循环动画
    Anim_Once,		// 单次播放动画
	Anim_Error		// 错误动画参数
} AnimationType_t;



//精灵驱动总线函数
void* Sprites_Exec(Camera_Struct* camera);








/*动画接口-------------------------------------------------------------------*/
//将不同类型的精灵转换为基础精灵
#define TO_SPRITES_BASE(ptr) ((SpritesBase_t*)(ptr))  

//管理器操作
int SpritesAnim_CreateManager(int value);
int SpritesAnim_DelManager(int value);
int SpritesAnim_GetManagerIndex(SpritesBase_t* sprites);

//动画链操作
int SpritesAnim_CreateNewChain(int value,int ROW_Pixel,int COL_Pixel,SPRITES_TYPE SpritesType,float Anim_duration);
int SpritesAnim_SwitchChain(SpritesBase_t* sprites,AnimChain_t NewtAnimationChainIndex);
int SpritesAnim_SwitchDefaultChain(SpritesBase_t* sprites);
int SpritesAnim_SetDefaultChain(SpritesBase_t* sprites,AnimChain_t newDefaultChainIdx);
int SpritesAnim_GetFrmCountOfChain(SpritesBase_t* sprites,AnimChain_t ChainIdx);
AnimChain_t SpritesAnim_GetChainIndex(SpritesBase_t* sprites);

//动画帧操作
void SpritesAnim_Update(SpritesBase_t* sprites);
int SpritesAnim_InsertNewFrm(int AnimationManageIndex,int AnimationChainIndex,int AnimationFrameSeq,const unsigned char* SpritesData,const unsigned char* SpritesShadow);
int SpritesAnim_SwitchFirstFrm(SpritesBase_t* sprites);
int SpritesAnim_SetAnimType(SpritesBase_t* sprites,AnimationType_t AnimationType);
AnimationType_t SpritesAnim_GetAnimType(SpritesBase_t* sprites);
int SpritesAnim_IsEndFrm(SpritesBase_t* sprites);


/*行为接口-------------------------------------------------------------------*/
int SpritesBT_Init(int value);

//行为树操作
SpriteBTNode_t* SpritesBT_SelectorCreate(void);
SpriteBTNode_t* SpritesBT_Sequence(void);
SpriteBTNode_t* SpritesBT_Condition(SBT_ExecuteHandle_t SBT_ExecuteHandle);
SpriteBTNode_t* SpritesBT_Action(SBT_ExecuteHandle_t SBT_ExecuteHandle);
int SpritesBT_InsertBehavior(SpriteBTNode_t* parent, SpriteBTNode_t* child);
int SpritesBT_MountToRoot(int value, SpriteBTNode_t* node);

//动作节点属性操作
int SpritesBT_Action_ToPhased(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority);
int SpritesBT_Action_ToSteady(SpriteBTNode_t* node);
int SpritesBT_Action_ModPriority(SpriteBTNode_t* node,unsigned char priority);
int SpritesBT_Action_ModITType(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority);



//中断表操作
int SpritesIT_Register(int value,SIT_MapIndexHandle_t SIT_MapIndexHandle);
int SpritesIT_Add(int value ,SITindex_t IT_index,unsigned char priority,SpriteITType_t SpriteITType,SBT_ExecuteHandle_t SBT_ExecuteHandle);
int SpritesIT_Trigger(Sprites_t* Sprites);//中断需要手动清楚，否则会持续进入中断（存疑）。
int SpritesIT_TriggerClear(Sprites_t* Sprites);
int SpritesIT_Enable(Sprites_t* Sprites);
int SpritesIT_Disable(Sprites_t* Sprites);


/*普通精灵接口--------------------------------------------------------------------*/
//普通精灵接口
Sprites_t* Sprites_ObjectCreate(int value,double posRow,double posCol,unsigned char defaultAnimationChainIndex,double moveSpeed,int map_value,int BehaviorTreeValue);

//位置操作
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

/*静态精灵接口--------------------------------------------------------------------*/


/*精灵通用操作接口----------------------------------------------------------------*/
int SpritesData_SetVerticalHeight(SpritesBase_t* sprites,int VerticalHeight);
int SpritesData_GetVerticalHeight(SpritesBase_t* sprites);
vector SpritesData_GetPos(SpritesBase_t* sprites);
int SpritesData_VerticalHeight_MoveUp(SpritesBase_t* sprites,int VerticalHeight);
int SpritesData_VerticalHeight_MoveDown(SpritesBase_t* sprites,int VerticalHeight);

int Sprites_Sleep(SpritesBase_t* Sprite);
int Sprites_Awaken(SpritesBase_t* Sprite);
/*私有数据操作接口----------------------------------------------------------------*/
int SpritesPriv_Add(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);
int SpritesPriv_Replace(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);
void SpritesPriv_Reset(Sprites_t* sprites);
void* SpritesPriv_Get(Sprites_t* sprites);










/*


【操作当前精灵的运行逻辑】
	【获取指定逻辑存储的函数】
	【替换指定逻辑中的函数】
		
【私有数据操作】
	【私有数据添加】
	1、基础添加私有数据
		int SpritesPriv_Add(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);//只有私有数据为空的时候允许添加，否则返回失败值
	2、用新的私有数据替换原有的私有数据
		int SpritesPriv_Replace(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle);//如果原来没有私有数据，就直接添加，如果有则先调用资源回收清理源数据
		
	【私有数据清除】
	void SpritesPriv_Del(Sprites_t* sprites);
	
	获取私有数据
	void* SpritesPriv_Get(Sprites_t* sprites);
	
*/

#endif
