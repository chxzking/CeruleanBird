/**
  ******************************************************************************
  * @file    Sprites_BehaviorTree.c
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    2025-3-31
  * @brief   该文件包含了库精灵行为树有关的函数实现，例如普通行为树，中断表
  ******************************************************************************
  */
#include "Sprites_internal.h"
#include "engine_config.h"
#include "bit_operate.h"
#include "Camera_internal.h"//摄像机内部接口
#include "WorldMap_internal.h"//世界地图内部接口
#include "Canvas.h"		//画布内部接口
#include "EngineFault.h"//引擎错误处理
#include "FrameRateStrategy_internal.h"//帧率优化

#include <string.h>
#include <stdlib.h>

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_0          //启用标准库堆解决方案
#include <malloc.h>
#define C_NULL NULL
#endif	//启用标准库堆解决方案

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //启用引擎动态内存解决方案
#include "heap_solution_1.h"

//宏定义统一函数接口
#define malloc(size) c_malloc(size)
#define realloc(ptr,size) c_realloc(ptr,size)
#define free(ptr)	c_free(ptr)
#endif	//启用引擎动态内存解决方案



/**
	1、建立一个统一的红黑树用于管理所有的行为树

**/

/**
	*@breif 定义行为树的管理树，即红黑树，每个节点管理一种类型的行为树
	*/
static RB_Node* Sprites_BT_ManagerTree = C_NULL;	




/******************************

	精灵行为树

*******************************/

/**
*		@note	这个函数本质上是在行为红黑树上申请一个行为管理器
*	@brief	创建一棵行为树
*	@param
*		@arg	value	精灵行为树编号，必须要大于0
*	@retval
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是1，意味着该值已经存在
*		@arg	如果返回结果是-1，意味着内存分配失败或者参数错误
*		@arg	如果返回结果是-2，意味着行为树初始化失败
*/
int SpritesBT_Init(int value) {
	if (value <= 0)	return -1;
	//存在性检测
	void* temp = search(Sprites_BT_ManagerTree, value);
	if (temp != NULL)	return 1;

	//创建行为管理器
	SpriteBehaviorManger_t* SpriteBehaviorManger = malloc(sizeof(SpriteBehaviorManger_t));
	if (SpriteBehaviorManger == NULL) {
		return -1;
	}
	SpriteBehaviorManger->root = NULL;
	SpriteBehaviorManger->SpriteITManager = NULL;
	//加入红黑树中
	int result = rbInsert(&Sprites_BT_ManagerTree, value, SpriteBehaviorManger);
	if (result != 0) {
		free(SpriteBehaviorManger);
	}
	return result;
}





/**
*	@brief	创建并初始化封装类型容器节点
*	@param
*		none
*	@retval
*		@arg	如果执行成功，返回节点
*		@arg  如果空间申请失败，返回空
*/

SpriteBTNode_t* Sprites_BT_ECNodeCreate(void) {
	//空间申请
	SpriteBTNode_t* node = (SpriteBTNode_t*)malloc(sizeof(SpriteBTNode_t) + sizeof(EncapsulatedContainer_t));
	if (node == NULL) {
		return NULL;
	}
	//获取EC空间的探针
	EncapsulatedContainer_t* ECprobe = (EncapsulatedContainer_t*)(((char*)node) + sizeof(SpriteBTNode_t));
	node->ContainerType.EncapsulatedContainer = ECprobe;
	ECprobe->childCount = 0;
	ECprobe->maxChildren = SpriteBTNode_MAX_COUNT;
	ECprobe->childType = BT_STATIC;
	for (int i = 0; i < SpriteBTNode_MAX_COUNT; i++) {
		ECprobe->children.fixedChildren[i] = NULL;
	}
	return node;
}

/**
*	@brief	创建并初始化叶子类型容器节点
*	@param
*		none
*	@retval
*		@arg	如果执行成功，返回节点
*		@arg  如果空间申请失败，返回空
*/

SpriteBTNode_t* Sprites_BT_LCNodeCreate(void) {
	//空间申请
	SpriteBTNode_t* node = (SpriteBTNode_t*)malloc(sizeof(SpriteBTNode_t) + sizeof(LeafContainer_t));
	if (node == NULL) {
		return NULL;
	}
	LeafContainer_t* LCprobe = (LeafContainer_t*)(((char*)node) + sizeof(SpriteBTNode_t));
	node->ContainerType.LeafContainer = LCprobe;
	LCprobe->SBT_ExecuteHandle = NULL;
	return node;
}

/**
*	@brief	选择器容器创建
*	@param
*		none
*	@retval
*		@arg	如果执行成功，返回容器的地址（这个地址不能进行任何操作，仅能作为对一个容器的唯一表示）
*		@arg  如果执行失败，返回空
*/
SpriteBTNode_t* SpritesBT_SelectorCreate(void) {
	SpriteBTNode_t* node = Sprites_BT_ECNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//修改节点类型
	node->BTNodeType = SBT_Selector;
	return node;
}


/**
*	@brief	顺序流容器创建
*	@param
*		none
*	@retval
*		@arg	如果执行成功，返回容器的地址（这个地址不能进行任何操作，仅能作为对一个容器的唯一表示）
*		@arg  如果执行失败，返回空
*/
SpriteBTNode_t* SpritesBT_Sequence(void) {
	SpriteBTNode_t* node = Sprites_BT_ECNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//修改节点类型
	node->BTNodeType = SBT_Sequence;
	return node;
}

/**
*	@brief	条件流节点创建
*	@param
*		@arg  SBT_ExecuteHandle 逻辑判断函数
*	@retval
*		@arg	如果执行成功，返回容器的地址（这个地址不能进行任何操作，仅能作为对一个容器的唯一表示）
*		@arg  如果执行失败，返回空
*/
SpriteBTNode_t* SpritesBT_Condition(SBT_ExecuteHandle_t SBT_ExecuteHandle) {
	//参数检测
	if (SBT_ExecuteHandle == NULL)	return NULL;

	SpriteBTNode_t* node = Sprites_BT_LCNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//修改节点类型
	node->BTNodeType = SBT_Condition;
	node->ContainerType.LeafContainer->SBT_ExecuteHandle = SBT_ExecuteHandle;
	return node;
}

/**
*	@brief	执行流节点创建（默认创建完备帧）
*	@param
*		@arg  SBT_ExecuteHandle 逻辑执行函数
*	@retval
*		@arg	如果执行成功，返回容器的地址（这个地址不能进行任何操作，仅能作为对一个容器的唯一表示）
*		@arg  如果执行失败，返回空
*/
SpriteBTNode_t* SpritesBT_Action(SBT_ExecuteHandle_t SBT_ExecuteHandle) {
	//参数检测
	if (SBT_ExecuteHandle == NULL)	return NULL;

	SpriteBTNode_t* node = Sprites_BT_LCNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//修改节点类型
	node->BTNodeType = SBT_Action;
	node->ContainerType.LeafContainer->SBT_ExecuteHandle = SBT_ExecuteHandle;
	//完备帧初始化
	node->ContainerType.LeafContainer->SpriteActionType = SAT_STEADY;
	return node;
}

/**
*	@brief	将一个完备属性的动作节点转化为非完备属性
*	@param
*		@arg  node 动作节点地址
*		@arg	SpriteITType 取代类型
*		@arg	priority 优先级（如果取代类型设置为SIT_ProhibitSubstitution，则此参数被忽略）
*	@retval
*		@arg	如果执行成功，返回0
*		@arg  如果执行失败，返回-1
*/
int SpritesBT_Action_ToPhased(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority){
	//参数检查
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType == SAT_PHASED)	return -1;//如果输入节点是非完备帧也返回错误

	//参数设置
	if(SpriteITType == SIT_AllowSubstitution){//允许取代
			node->ContainerType.LeafContainer->SpriteITType = SIT_AllowSubstitution;
			node->ContainerType.LeafContainer->priority = priority;
	}
	else if(SpriteITType == SIT_ProhibitSubstitution){//不允许取代
			node->ContainerType.LeafContainer->SpriteITType = SIT_ProhibitSubstitution;
	}
	else{
		//非法输入
		return -1;
	}
	node->ContainerType.LeafContainer->SpriteActionType = SAT_PHASED;
	return 0;
}
/**
*	@brief	将一个非完备属性的动作节点转化为完备属性
*	@param
*		@arg  node 动作节点地址
*	@retval
*		@arg	如果执行成功，返回0
*		@arg  如果执行失败，返回-1
*/
int SpritesBT_Action_ToSteady(SpriteBTNode_t* node){
	//参数检查
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType == SAT_STEADY)	return -1;//如果输入节点是完备帧也返回错误

	node->ContainerType.LeafContainer->SpriteActionType = SAT_STEADY;
	return 0;
}

/**
*	@brief	修改一个非完备属性的优先级
*	@param
*		@arg  node 动作节点地址
*		@arg	priority 新优先级
*	@retval
*		@arg	如果执行成功，返回0
*		@arg  如果执行失败，返回-1
*/
int SpritesBT_Action_ModPriority(SpriteBTNode_t* node,unsigned char priority){
	//参数检查
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType != SAT_PHASED)	return -1;//如果输入节点不是非完备帧返回错误
	if(node->ContainerType.LeafContainer->SpriteITType != SIT_AllowSubstitution)	return -1;//如果输入节点不为可取代类型，返回错误
	
	node->ContainerType.LeafContainer->priority = priority;
	return 0;
}

/**
*	@brief	设置一个非完备属性的节点的取代类型
*	@param
*		@arg  node 动作节点地址
*		@arg	SpriteITType 取代类型
*		@arg	priority 新优先级(如果为不可取代类型，则优先级参数被忽略)
*	@retval
*		@arg	如果执行成功，返回0
*		@arg  如果执行失败，返回-1
*/
int SpritesBT_Action_ModITType(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority){
	//参数检查
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType != SAT_PHASED)	return -1;//如果输入节点不是非完备帧返回错误

	
	if(SpriteITType == SIT_AllowSubstitution){//允许取代
			node->ContainerType.LeafContainer->SpriteITType = SIT_AllowSubstitution;
			node->ContainerType.LeafContainer->priority = priority;
	}
	else if(SpriteITType == SIT_ProhibitSubstitution){//不允许取代
			node->ContainerType.LeafContainer->SpriteITType = SIT_ProhibitSubstitution;
	}
	else{
		//非法输入
		return -1;
	}
	return 0;
}

/**
*	@brief	行为节点之间进行装载操作
*	@param
*		@arg	parent	被挂载的对象
*		@arg	child 	需要挂载的目标
*	@retval
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是-1，意味着执行失败
*/
int SpritesBT_InsertBehavior(SpriteBTNode_t* parent, SpriteBTNode_t* child) {
	//参数合法性检测
	if (parent == NULL || child == NULL || parent->BTNodeType == SBT_Condition || parent->BTNodeType == SBT_Action) {
		return -1;
	}

	/*
	1、孩子节点数量大于等于max，意味着需要分配动态数组，或者扩容当前动态数组
			1.1如果max等于数组静态数组上限，意味着需要转化为动态数组

			1.2如果max不等于数组上限，意味着当前是动态数组，需要扩容

	2、如果孩子节点小于max，意味着只需要添加到尾部即可。
	*/

	//节点等于最大限度，意味着新加节点会发生溢出
	if (parent->ContainerType.EncapsulatedContainer->childCount == parent->ContainerType.EncapsulatedContainer->maxChildren) {

		//发生首次溢出
		if (parent->ContainerType.EncapsulatedContainer->childType == BT_STATIC) {
			struct SpriteBTNode_t** childgroup = (struct SpriteBTNode_t**)malloc(sizeof(struct SpriteBTNode_t*) * SpriteBTNode_MAX_COUNT * 2);
			if (childgroup == NULL) {
				return -1;//空间分配失败
			}
			parent->ContainerType.EncapsulatedContainer->maxChildren = SpriteBTNode_MAX_COUNT * 2;
			//数据转移
			for (int i = 0; i < SpriteBTNode_MAX_COUNT; i++) {
				childgroup[i] = parent->ContainerType.EncapsulatedContainer->children.fixedChildren[i];
			}
			//插入新的孩子数据
			childgroup[SpriteBTNode_MAX_COUNT] = child;
			parent->ContainerType.EncapsulatedContainer->childCount++;
			//将当前数组切换为动态数组
			parent->ContainerType.EncapsulatedContainer->children.dynamicChildren = childgroup;
			parent->ContainerType.EncapsulatedContainer->childType = BT_DYNAMIC;
			return 0;

		}
		//非首次溢出
		else {
			struct SpriteBTNode_t** childgroup = (struct SpriteBTNode_t**)realloc(parent->ContainerType.EncapsulatedContainer->children.dynamicChildren, sizeof(struct SpriteBTNode_t*) * parent->ContainerType.EncapsulatedContainer->maxChildren * 2);
			if (childgroup == NULL) {
				return -1;//扩容失败
			}
			//如果新空间不是在原空间扩容
			if (childgroup != parent->ContainerType.EncapsulatedContainer->children.dynamicChildren) {
				//数据迁移
				for (int i = 0; i < parent->ContainerType.EncapsulatedContainer->maxChildren; i++) {
					childgroup[i] = parent->ContainerType.EncapsulatedContainer->children.dynamicChildren[i];
				}
				//释放原空间
				free(parent->ContainerType.EncapsulatedContainer->children.dynamicChildren);
				//替换原空间
				parent->ContainerType.EncapsulatedContainer->children.dynamicChildren = childgroup;
			}
			parent->ContainerType.EncapsulatedContainer->maxChildren *= 2;
			//插入新数据
			childgroup[parent->ContainerType.EncapsulatedContainer->childCount] = child;
			parent->ContainerType.EncapsulatedContainer->childCount++;
			return 0;
		}
	}
	//在限度之内
	else {

		//静态数组策略下
		if (parent->ContainerType.EncapsulatedContainer->childType == BT_STATIC) {
			parent->ContainerType.EncapsulatedContainer->children.fixedChildren[parent->ContainerType.EncapsulatedContainer->childCount] = child;
		}
		//动态数组策略下
		else {
			parent->ContainerType.EncapsulatedContainer->children.dynamicChildren[parent->ContainerType.EncapsulatedContainer->childCount] = child;
		}
		parent->ContainerType.EncapsulatedContainer->childCount++;
		return 0;
	}
}

/**
*	@brief	子节点访问
*	@param
*		@arg	parent	父亲节点
*		@arg	child 	index子节点索引
*	@retval
*		@arg	如果执行成功，返回子节点的地址
*		@arg	如果执行失败，返回空
*/
SpriteBTNode_t* Sprites_BT_GetChild(SpriteBTNode_t* parent, int index) {
	//参数验证
	if (parent == NULL || index < 0) {
		return NULL;
	}
	if (index >= parent->ContainerType.EncapsulatedContainer->childCount) {
		return NULL;
	}
	//数据提取
	//静态数组策略
	if (parent->ContainerType.EncapsulatedContainer->childType == BT_STATIC) {
		return parent->ContainerType.EncapsulatedContainer->children.fixedChildren[index];
	}
	//动态数组
	else {
		return parent->ContainerType.EncapsulatedContainer->children.dynamicChildren[index];
	}
}



/**
*	@brief	将指定节点挂载到行为树的根节点上
*	@param
*		@arg	value		根节点的描述值
*		@arg	node 	需要挂载的目标
*	@retval
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是-1，意味着执行失败
*/

int SpritesBT_MountToRoot(int value, SpriteBTNode_t* node) {
	if (value <= 0 || node == NULL || Sprites_BT_ManagerTree == NULL) {
		return -1;
	}

	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//不存在这棵行为树
	}

	SpriteBehaviorManger->root = node;
	return 0;
}



//执行一棵树

/**
*	@brief	执行一棵普通行为树
*	@param
*		@arg	root		根节点
*		@arg	Sprites 行为树的精灵本体（其中包含了精灵的信息，以及精灵的私有数据空间）
*	@retval
*
*		执行结果
*/
SpriteBTStatus_t ExecuteBehaviorTree(SpriteBTNode_t* root, Sprites_t* Sprites) {
	SpriteBTStatus_t result;
	switch (root->BTNodeType) {
	case SBT_Condition://条件流节点处理
		result = root->ContainerType.LeafContainer->SBT_ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
		return	(result == SBT_FAILURE) ? SBT_FAILURE : SBT_SUCCESS;

	case SBT_Action://执行节点的处理
		result = root->ContainerType.LeafContainer->SBT_ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
		if(root->ContainerType.LeafContainer->SpriteActionType == SAT_STEADY){//纠正完备属性的返回值
			return (result == SBT_FAILURE) ? SBT_FAILURE : SBT_SUCCESS;
		}
		
		//非完备属性区
		if (result == SBT_RUNNING) {
			//出现执行中状态，立即爆粗当前的执行状态并退出
			Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_BT;//设置当前执行的为节点类型
			Sprites->BehaviorMoudle.bt_context.node.action = root;
		}
		return result;//非完备属性直接返回
			
	case SBT_Selector://选择器容器处理
		for (int i = 0; i < root->ContainerType.EncapsulatedContainer->childCount; i++) {
			//获取孩子
			struct SpriteBTNode_t* node = Sprites_BT_GetChild(root, i);
			//执行子节点
			result = ExecuteBehaviorTree(node, Sprites);
			if (result == SBT_RUNNING || result == SBT_SUCCESS) {//出现了运行时或者执行成功，立即退出当前行为树遍历
				return result;
			}
		}
		return SBT_FAILURE;
	case SBT_Sequence:
		for (int i = 0; i < root->ContainerType.EncapsulatedContainer->childCount; i++) {
			//获取孩子
			struct SpriteBTNode_t* node = Sprites_BT_GetChild(root, i);
			result = ExecuteBehaviorTree(node, Sprites);
			if (result == SBT_RUNNING || result == SBT_FAILURE) {//出现了运行时或者执行失败，立即退出当前行为树遍历
				return result;
			}
		}
		return SBT_SUCCESS;
	}

	return SBT_FAILURE;

}


//总执行函数
/**
*	@brief	行为树执行总线
*	@param
*		@arg	Sprites 行为树的精灵本体（其中包含了精灵的信息，以及精灵的私有数据空间）
*	@retval
*		@arg	如果返回的结果是0，意味着执行成功
*		@arg	如果返回结果是-1，意味着执行失败
*/
int SpritesBT_BusExecute(Sprites_t* Sprites) {
	if (Sprites_BT_ManagerTree == NULL) {
		return -1;
	}
	int value = Sprites->BehaviorMoudle.value;
	if(value <= 0){
		return -1;
	}
	//找出指定行为树
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//不存在这棵行为树
	}

	if (SpriteBehaviorManger->root == NULL) {
		return 0;//根节点为空不需要执行，直接返回
	}
	
	//检查是否产生中断
	if(Sprites->BehaviorMoudle.IT_Enable == 1){//执行中断表
		//执行中断
		
		int result = SpritesIT_Exec(Sprites,SpriteBehaviorManger);
		switch(result){
			case 0:
				return 0;
			case 1:
				goto BT_SAT_EXEC_FLAG;//跳转普通行为树执行或非完备帧验证持续执行
			default:
				return -1;
		}
	}
	
BT_SAT_EXEC_FLAG:;//完备与非完备执行标记
	if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType != SBB_NO){//如果上次执行的节点不为空节点
		SpritesPhased_Exec(Sprites);
		return 0;
	}
	
	//执行行为树
	ExecuteBehaviorTree(SpriteBehaviorManger->root, Sprites);
		
	
	return 0;
}

/******************************

	精灵中断表

*******************************/

/**
*	@brief	在指定行为树中创建中断表
*	@param
*		@arg	value		行为树的描述值
*		@arg    SIT_MapIndexHandle 自定义中断匹配函数
*	@retval
*		@arg	如果返回的结果是0，意味着创建成功
*		@arg	如果返回结果是-1，意味着创建失败
*/

int SpritesIT_Register(int value,SIT_MapIndexHandle_t SIT_MapIndexHandle){
	if(value <= 0 || SIT_MapIndexHandle == NULL)	return -1;
	//找出指定行为树
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//不存在这棵行为树
	}
	if(SpriteBehaviorManger->SpriteITManager != NULL){
		return -1;//当前行为树已经挂载了中断表
	}
	
	//创建中断管理节点
	struct SpriteITManageNode_t* SpriteITManager = (struct SpriteITManageNode_t*)malloc(sizeof(struct SpriteITManageNode_t));
	if(SpriteITManager == NULL){
		return -1;
	}
	SpriteITManager->ITcount = 0;
	SpriteITManager->Storage_strategy = SIT_ARR;
	SpriteITManager->table.head = (struct SpriteITNode_t*)malloc(sizeof(struct SpriteITNode_t) * SIT_NODE_MAX_COUNT);
	if(SpriteITManager->table.head == NULL){
		free(SpriteITManager);
		return -1;
	}
	for(int i = 0;i<SIT_NODE_MAX_COUNT;i++){
		SpriteITManager->table.head[i].ExecuteHandle = NULL;
	}
	SpriteITManager->SIT_MapIndexHandle = SIT_MapIndexHandle;
	
	//加载
	SpriteBehaviorManger->SpriteITManager = SpriteITManager;
	return 0;
}

/**
*	@brief	创建红黑树策略的中断表
*	@param
*		@arg	SpriteBehaviorManger		精灵行为管理器
*		@arg	arr							备份的数组策略数据
*	@retval
*		@arg	创建成功返回0
*		@arg	创建失败返回-1
*/
int SpritesIT_RBStorageStrategyCreate(SpriteBehaviorManger_t* SpriteBehaviorManger,SpriteITNode_t* arr){
	SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree = NULL;
	int i;
	for(i = 0;i<SIT_NODE_MAX_COUNT;i++){
		if(arr[i].ExecuteHandle != NULL){//数据迁移
			SpriteITNode_t* node = (SpriteITNode_t*)malloc(sizeof(SpriteITNode_t));
			if(node == NULL){
				//内存分配失败
				break;
			}
			node->value = i;
			node->priority = arr[i].priority;
			node->SpriteITType = arr[i].SpriteITType;
			node->ExecuteHandle = arr[i].ExecuteHandle;
			int result =  rbInsert(&SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, i,node);
			if(result != 0){
				free(node);
				break;
			}
		}
	}
	if(i != SIT_NODE_MAX_COUNT){
		//出现了红黑树添加失败的情况,进行资源回收
		i--;//当前指向的索引红黑树没有添加，需要回退一个
		while(i >= 0){
			//释放之前添加的节点
			if(arr[i].ExecuteHandle != NULL){
				rbDelete(&SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree,i);
			}
			
			i--;
		}
		return -1;
	}	
	return 0;
}

/**
*	@brief	在指定中断表中创建中断处理节点
*	@param
*		@arg	value			  行为树的描述值
*		@arg	IT_index		  中断索引编号（中断号不能小于0）
*		@arg	priority		  中断优先级
*		@arg	SpriteITType	  中断取代类型
*		@arg    SBT_ExecuteHandle 中断处理函数
*	@retval
*		@arg	如果返回的结果是0，意味着创建成功
*		@arg	如果返回结果是-1，意味着创建失败
*/
int SpritesIT_Add(int value ,SITindex_t IT_index,unsigned char priority,SpriteITType_t SpriteITType,SBT_ExecuteHandle_t SBT_ExecuteHandle){
	//参数检查
	if(value <= 0 || IT_index < 0 || SBT_ExecuteHandle == NULL){
		return -1;
	}
	//行为树查找
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//不存在这棵行为树
	}
	if(SpriteBehaviorManger->SpriteITManager == NULL){
		return -1;//没有注册中断表
	}
	
strategy_rejudgment_flag:;	//分配策略判断节点
	//策略分配
	if(SpriteBehaviorManger->SpriteITManager->Storage_strategy == SIT_ARR){//当前采用的是数组策略
		//索引验证
		
		//索引超过了数组界限，应该跳转红黑树策略 或者 数组策略满载应重新采取红黑树策略
		if(IT_index >= SIT_NODE_MAX_COUNT || SpriteBehaviorManger->SpriteITManager->ITcount >= SIT_NODE_MAX_COUNT){
			//临时备份之前的数组策略
			struct SpriteITNode_t* temp = SpriteBehaviorManger->SpriteITManager->table.head;
			
			//创建红黑树
			int result = SpritesIT_RBStorageStrategyCreate(SpriteBehaviorManger,temp);
			if(result != 0){
				SpriteBehaviorManger->SpriteITManager->table.head = temp;//恢复数组策略的值
				return -1;//红黑树创建失败
			}
			free(temp);//释放原来的内存
			SpriteBehaviorManger->SpriteITManager->Storage_strategy = SIT_RBTREE;
			goto strategy_rejudgment_flag;//重新判断策略分配
		}
		
		//判断是否已经存在该节点
		if(SpriteBehaviorManger->SpriteITManager->table.head[IT_index].ExecuteHandle != NULL){
			return -1;
		}
		
		//数组策略下添加新数据
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].value = value;
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].priority = priority;
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].SpriteITType = SpriteITType;
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].ExecuteHandle = SBT_ExecuteHandle;
		//中断节点总数加一
		SpriteBehaviorManger->SpriteITManager->ITcount++;
		return 0;
		
	}
	else{//当前策略采用的是红黑树策略
		
		//检查是否已经存在了该节点
		void* temp = search(SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, value);
		if(temp != NULL){
			return -1;
		}
		
		SpriteITNode_t* node = (SpriteITNode_t*)malloc(sizeof(SpriteITNode_t));
		if(node == NULL){
			//内存分配失败
			return -1;
		}
		node->value = IT_index;
		node->priority = priority;
		node->SpriteITType = SpriteITType;
		node->ExecuteHandle = SBT_ExecuteHandle;
		
		int result =  rbInsert(&SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, value,node);
		if(result != 0){
			free(node);
			return -1;
		}
		//中断节点总数加一
		SpriteBehaviorManger->SpriteITManager->ITcount++;
		return 0;
	}
}


/**
*	@brief	开启指定精灵的行为树的中断使能
*	@param
*		@arg	Sprites			  精灵描述
*	@retval
*		@arg	激活成功返回0
*		@arg	激活失败返回-1
*/
int SpritesIT_Enable(Sprites_t* Sprites){
	if(Sprites == NULL){
		return -1;
	}
	//找出指定行为树
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, Sprites->BehaviorMoudle.value);
	if (SpriteBehaviorManger == NULL || SpriteBehaviorManger->SpriteITManager == NULL || SpriteBehaviorManger->SpriteITManager->SIT_MapIndexHandle == NULL) {
		return -1;//不存在这棵行为树 或者 中断表并未创建 或者 中断表不合法 
	}
	Sprites->BehaviorMoudle.IT_Enable = 1;
	return 0;
}
/**
*	@brief	指定精灵的行为树的中断失能
*	@param
*		@arg	Sprites			  精灵描述
*	@retval
*		@arg	激活成功返回0
*		@arg	激活失败返回-1
*/
int SpritesIT_Disable(Sprites_t* Sprites){
	if(Sprites == NULL){
		return -1;
	}
	Sprites->BehaviorMoudle.IT_Enable = 0;
	Sprites->BehaviorMoudle.IT_running = 0;
	//如果上下文是中断，则清理上下文
	if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType == SBB_IT){
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;
		Sprites->BehaviorMoudle.bt_context.node.ITnode = NULL;
	}
	return 0;
}

/**
*	@brief	获取中断处理函数
*	@param
*		@arg	IT_index			 中断号
*		@arg	SpriteBehaviorManger 行为管理节点
*	@retval
*		@arg	成功返回中断节点
*		@arg	失败返回空
*/
SpriteITNode_t* SpritesIT_GetITNode(SpriteBehaviorManger_t* SpriteBehaviorManger,SITindex_t IT_index){
	//数组存储类型
	if(SpriteBehaviorManger->SpriteITManager->Storage_strategy == SIT_ARR){
		return &SpriteBehaviorManger->SpriteITManager->table.head[IT_index];
	}
	else if(SpriteBehaviorManger->SpriteITManager->Storage_strategy == SIT_RBTREE){
		SpriteITNode_t* node = (SpriteITNode_t*)search(SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, IT_index);
		return node;
	}
	else{
		return NULL;
	}
}


/**
*	@brief	中断执行函数
*	@param
*		@arg	Sprites			 精灵
*		@arg	SpriteBehaviorManger 行为管理节点
*	@retval
*		@arg	执行完成，返回0
*		@arg	跳转到行为树普通执行或非完备帧继续执行，返回1
*/
int SpritesIT_Exec(Sprites_t* Sprites,SpriteBehaviorManger_t* SpriteBehaviorManger){
	//获取中断号
	int IT_index = SpriteBehaviorManger->SpriteITManager->SIT_MapIndexHandle(Sprites,SpritesPriv_Get(Sprites));
	if(IT_index < 0){
		//查找失败
		return 1;
	}
	
//	//清除中断标志
//	Sprites->BehaviorMoudle.IT_flag = 0;
	/*检查特殊情况：
		如果上一帧是中断且在运行，如果和本次将执行的中断号（适用于多帧中断反复被相同中断触发），
		则直接跳过下面的判断，提高效率。
		
		1、只有中断才会触发这个情况。
		2、只有两次执行相同的中断
		3、中断必须为多帧，（因为单帧在执行后IT_running会被置假）
	*/
	if(Sprites->BehaviorMoudle.IT_running == 1){
		//激活重复执行
		if(Sprites->BehaviorMoudle.bt_context.Previous_IT_index == IT_index){//与之前的执行一致
			return 1;
		}
	}
	
	
	
	//获取中断节点
	SpriteITNode_t* ITnode = SpritesIT_GetITNode(SpriteBehaviorManger,IT_index);
	if(ITnode == NULL){
		//中断节点查找失败
		return 1;
	}
	
	//检查上次执行的节点
	if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType == SBB_IT){//如果上次执行的是中断类型的节点需要特殊处理
		//检查上次执行的节点是否可以被取代
		if(ITnode->SpriteITType == SIT_ProhibitSubstitution){//禁止被取代
			//跳转继续执行，当前的中断会被忽略
			//goto SAT_EXEC_FLAG;
			return 1;
		}
		else if(ITnode->SpriteITType == SIT_AllowSubstitution){//允许被取代
			//对比两者的优先级
			if(ITnode->priority <= Sprites->BehaviorMoudle.bt_context.node.ITnode->priority){
				//中断节点比之前的中断优先级相同或者更低，则继续上次执行
				//goto SAT_EXEC_FLAG;
				Sprites->BehaviorMoudle.bt_context.Previous_IT_index = IT_index;//保存当前执行的中断
				return 1;
			}
		}
		else{
			//【错误】<枚举错误访问>
			EngineFault_ExceptionEnumParam_Handle();
		}
	}
	else if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType == SBB_BT){//如果上次执行的是行为树类型的节点需要特殊处理
		//检查上次执行的行为树节点是否为非完备帧
		if(Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->SpriteActionType == SAT_PHASED){
			//检查该非完备帧是否可以被中断取代
			if(Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->SpriteITType == SIT_ProhibitSubstitution){
				//禁止被取代
				//goto SAT_EXEC_FLAG;//跳转继续执行，当前的中断会被忽略
				return 1;
			}
			else if(Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->SpriteITType == SIT_AllowSubstitution){
				//允许被取代
				//对比两者的优先级
				if(ITnode->priority <= Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->priority){
					//中断节点比之前的行为树节点优先级相同或者更低，则继续上次执行
					//goto SAT_EXEC_FLAG;	
					return 1;
				}
			}
			else{
				//【错误】<枚举错误访问>
				EngineFault_ExceptionEnumParam_Handle();
			}
		}
	}
	//如果上次执行的是空类型节点，无需任何处理，直接执行中断。
	
	
	
	
	//执行（预执行一次，可以过滤单帧逻辑的中断，或者多帧的执行一次逻辑）
	SpriteBTStatus_t result = ITnode->ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
	if(result == SBT_FAILURE || result == SBT_SUCCESS){
		//用户执行返回失败或者完成都意味着当前中断不再需要继续执行
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;//正在执行的节点类型置空
		Sprites->BehaviorMoudle.bt_context.node.action = NULL;
		Sprites->BehaviorMoudle.IT_running = 0;//清除为中断执行态
	}
	else if(result == SBT_RUNNING){
		//用户执行返回正在执行的结果，意味着需要给记录当前执行的节点信息，为下次继续执行做准备
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_IT;//记录当前执行的是中断节点
		Sprites->BehaviorMoudle.bt_context.node.ITnode = ITnode;//记录正在执行的中断节点
		Sprites->BehaviorMoudle.IT_running = 1;//设置为中断执行态
		Sprites->BehaviorMoudle.bt_context.Previous_IT_index = IT_index;//（触发了新中断执行）保存当前执行的中断
	}
	else{
		//【错误】<枚举错误访问>
		EngineFault_ExceptionEnumParam_Handle();
	}
	return 0;
}
/**
*	@brief	非完备帧继续执行函数
*	@param
*		@arg	Sprites			 精灵
*	@retval
*		none
*/
void SpritesPhased_Exec(Sprites_t* Sprites){
	//判断节点类型
	SpriteBTStatus_t result;
	SpriteITNode_t *ITnode = NULL;
	SpriteBTNode_t* BTnode = NULL;
	switch(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType){
		case SBB_IT:;//上次执行的节点是中断类型
			//执行
			ITnode = Sprites->BehaviorMoudle.bt_context.node.ITnode;
			result = ITnode->ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
			
			//结果处理
		
			if(result == SBT_RUNNING){
				//用户执行返回正在执行的结果，意味着需要给记录当前执行的节点信息，为下次继续执行做准备
				Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_IT;//记录当前执行的是中断节点
				Sprites->BehaviorMoudle.bt_context.node.ITnode = ITnode;//记录正在执行的中断节点
				
				return;
			}

			break;
		case SBB_BT:;//上次执行的节点是行为树类型
			//执行
			BTnode = Sprites->BehaviorMoudle.bt_context.node.action;
			result = BTnode->ContainerType.LeafContainer->SBT_ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
			
			//结果处理
			if(result == SBT_RUNNING){
				//用户执行返回正在执行的结果，意味着需要给记录当前执行的节点信息，为下次继续执行做准备
				Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_BT;//记录当前执行的是中断节点
				Sprites->BehaviorMoudle.bt_context.node.action = BTnode;//记录正在执行的中断节点
				
				return;
			}
			break;
		default:;
			//【错误】<枚举错误访问>
			EngineFault_ExceptionEnumParam_Handle();
	}
	
	if(result == SBT_FAILURE || result == SBT_SUCCESS){
		//用户执行返回失败或者完成都意味着当前中断不在需要继续执行
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;//正在执行的节点类型置空
		Sprites->BehaviorMoudle.bt_context.node.action = NULL;
		
		/*如果之前执行的是普通节点，那么中断运行标志就是0，如果之前执行的是中断，现在中断完成，需要置0*/
		Sprites->BehaviorMoudle.IT_running = 0;//重置运行标志
	}

	else{
		//【错误】<枚举错误访问>
		EngineFault_ExceptionEnumParam_Handle();
	}
}
