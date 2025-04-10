/**
  ******************************************************************************
  * @file    Sprites_Animation.c
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   该文件包含了库精灵动画有关的函数实现
  ******************************************************************************
  */

#include "Sprites_internal.h"
#include "engine_config.h"
#include "RB_Tree_API.h"
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


//定义红黑树根指针
RB_Node* Sprites_Header = C_NULL;	

//引擎内精灵总数
int Sprites_Count = 0;

/*
注意事项
【动画链切换】
		会重置当前存储的当前帧
		重置累计时间
		不会改变动画类型

*/


/**********************
	【精灵管理器操作】
***********************/
/**
*		@note	这个函数是一个精灵操作的基础，不管后续进行什么操作，必须要先进行该函数创建一个精灵管理器
*	@brief	创建一个类型的精灵管理器
*	@param	
*		@arg	value	精灵代号
*	@retval	
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是1，意味着该值已经存在
*		@arg	如果返回结果是-1，意味着内存分配失败或者参数错误
*		@arg	如果返回结果是-2，意味着精灵树初始化失败
*/
int SpritesAnim_CreateManager(int value){	
	if(SpritesTree_Init()){//初始化精灵树
		return -2;
	}
	//创建节点
	SpritesAnimationManager_t* node = (SpritesAnimationManager_t*)malloc(sizeof(SpritesAnimationManager_t));
	if(node == C_NULL)	return -1;			//内存申请失败
	node->value = value;								//精灵类型的代号
	node->PreallocationChainCount = 0;	//预分配的动画链组的数量
	node->AnimationChainCount = 0;			//精灵动画链组数量
	node->SpritesCount = 0;							//设置初始使用此动画的精灵数量为0
	node->SpritesRecordGroup = C_NULL;	//精灵动画链组
	
	//插入红黑树
	int result = rbInsert(&Sprites_Header,value,(void*)node);
	if(result != 0){
		//插入失败
		free(node);//释放空间
		return result;//返回错误值
	}
	//插入成功
	return 0;
}
/**
*		@note	
*	@brief	删除一个类型的精灵管理器
*	@param	
*		@arg	value	精灵代号
*	@retval	
*		@arg	如果返回的结果是0，操作成功
*		@arg	如果返回结果是-1，操作失败
*/
int SpritesAnim_DelManager(int value){
		/*
	
			待定。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
	
		*/
	
		return -1;
}
/**
*		@note	
*	@brief	获取指定精灵的管理器代号
*	@param	
*		@arg	sprites	精灵对象
*	@retval	
*		@arg	如果返回的结果精灵代号，如果参数出现错误，则会激活错误处理
*/
int SpritesAnim_GetManagerIndex(SpritesBase_t* sprites){
		if(sprites == C_NULL){
			//【错误】<空指针访问>
			EngineFault_NullPointerGuard_Handle();
		}
		return sprites->AnimMoudle.value;	
}

/**********************
	【精灵动画链操作】
***********************/
/**
*		
*	@brief	在指定动画类型下创建一个新的动画链组
*	@param	
*		@arg	value	精灵代号
*		@arg	ROW_Pixel	精灵数组的行
*		@arg	COL_Pixel	精灵数组的列
*		@arg	SpritesType	精灵的类型	两个可选项BYTE_SPRITES和BIT_SPRITES，定义于 @ref SPRITES_TYPE
*		@arg  	Anim_duration 此动画链播放完成所消耗的时间 （单位：秒）
*	@retval	
*		@arg	创键失败返回错误值 -1；
*		@arg	创建成功返回该动画链的索引值（动画链描述符）
*/
int SpritesAnim_CreateNewChain(int value,int ROW_Pixel,int COL_Pixel,SPRITES_TYPE SpritesType,float Anim_duration){
	//查找指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,value);
	if(SpritesManager == C_NULL)	return -1;//目标管理器不存在，返回错误值
	
	//如果预分配数量是0
	if(SpritesManager->PreallocationChainCount == 0){
		/*
			创建一个记录表,原因如下：
			1、考虑到有些是静态动画，不存在动画链，如果
				预分配两个及以上的空间会导致空间浪费
		*/
		SpritesRecord_t* record = (SpritesRecord_t*)malloc(sizeof(SpritesRecord_t) * 1);//先预分配一个记录表
		if(record == C_NULL) 	return -1;//空间申请失败
		//更新管理器
		SpritesManager->PreallocationChainCount = 1;
		SpritesManager->AnimationChainCount = 1;
		SpritesManager->SpritesRecordGroup = record;
		//初始化当前记录表
		record->Logic_ROW = ROW_Pixel;
		record->Column = COL_Pixel;
		record->SpritesType = SpritesType;
		record->AnimationChainGroup = C_NULL;
		record->frame_count = 0;
		record->Anim_duration = Anim_duration;
		record->frame_duration = 0.0;
		//实际行计算
		if(SpritesType == BIT_SPRITES){
			//如果bit类型
			record->Row = bitToByte(ROW_Pixel);
		}else if(SpritesType == BYTE_SPRITES){
			//如果是byte类型
			record->Row = ROW_Pixel;//直接保存
		}
		//创建完成，返回动画链组索引
		return 0;
	}
	//当前记录表需要存储多组动画
	else{	
		//检查当前创建的表是否已经被写满
		
		//已经被写满
		if(SpritesManager->PreallocationChainCount == SpritesManager->AnimationChainCount){
			//获取当前的内存大小
			int size = SpritesManager->PreallocationChainCount;
			//扩容两个单位长度
			size+=2;
			//临时存储原地址
			SpritesRecord_t* InitialPtr = SpritesManager->SpritesRecordGroup;
			//扩容动态数组
			SpritesRecord_t* temp = (SpritesRecord_t*)realloc(SpritesManager->SpritesRecordGroup,size*sizeof(SpritesRecord_t));
			if(temp == C_NULL)	return -1;//内存扩容失败
			//内存扩容完成
			SpritesManager->SpritesRecordGroup = temp;
			SpritesManager->PreallocationChainCount = size;
			
			//检查地址是否发生了变化。
			if(InitialPtr != temp){
				//如果新的记录表地址发生了变化，则需要修正之前所有的record定位
				for(int i = 0;i<SpritesManager->AnimationChainCount;i++){
					SpritesLinkNode_t* head = SpritesManager->SpritesRecordGroup[i].AnimationChainGroup;
					SpritesLinkNode_t* current = head;
					do{
						current->record = &(SpritesManager->SpritesRecordGroup[i]);
						current = current->next;
					}while(current != head);
				}
			}
			
		}
		//没有被写满,进行动画组添加
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Logic_ROW = ROW_Pixel;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Column = COL_Pixel;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].SpritesType = SpritesType;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].AnimationChainGroup = C_NULL;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].frame_count = 0;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Anim_duration = Anim_duration;
		//实际行计算
		if(SpritesType == BIT_SPRITES){
			//如果bit类型
			SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Row = bitToByte(ROW_Pixel);
		}else if(SpritesType == BYTE_SPRITES){
			//如果是byte类型
			SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Row = ROW_Pixel;//直接保存
		}
		
		//更新当前动画组的数量
		SpritesManager->AnimationChainCount++;
		//返回新添加的组的编号
		return SpritesManager->AnimationChainCount - 1;
	}
}



/**
*		
*	@brief	将指定精灵当前的动画链切换新的动画链,（动画帧索引记录值会被重置为帧首,累计时间会被清除，切换不会改变动画类型《循环或者非循环》）
*	@param	
*		@arg	sprites	精灵对象
*		@arg	NewtAnimationChainIndex	新动画链索引
*	@retval	
*		@arg	操作失败返回错误值 -1；
*		@arg	操作成功返回 0
*/
int SpritesAnim_SwitchChain(SpritesBase_t* sprites,AnimChain_t NewtAnimationChainIndex){
	if(sprites == C_NULL)	return -1;//非法参数
	
	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//目标管理器不存在，返回错误
	
	//验证记录表
	if(NewtAnimationChainIndex >= SpritesManager->AnimationChainCount){
		return -1;//记录表非法访问
	}
	
	//验证完成，修改动画链
	SpritesLinkNode_t* temp = SpritesManager->SpritesRecordGroup[NewtAnimationChainIndex].AnimationChainGroup;
	if(temp == C_NULL)	return -1;//当前子动画链中没有添加动画
	
	sprites->AnimMoudle.ReadyAnimation = temp;
	sprites->AnimMoudle.CurrentAnimationChainIndex = NewtAnimationChainIndex;
	sprites->AnimMoudle.currentFrmIndex = 0;
	sprites->accumulatedTime = 0.0;//累计时间清除
	return 0;
}
/**
*		
*	@brief	将指定精灵当前的动画链切换为默认的动画链，（动画帧索引记录值会被重置为帧首）
*	@param	
*		@arg	sprites	精灵对象
*	@retval	
*		@arg	操作失败返回错误值 -1；
*		@arg	操作成功返回 0
*/
int SpritesAnim_SwitchDefaultChain(SpritesBase_t* sprites){
	if(sprites == C_NULL)	return -1;
	return SpritesAnim_SwitchChain(sprites,sprites->AnimMoudle.defaultAnimationChainIndex);
}
/**
*		
*	@brief	设置一个精灵的指定动画链为默认动画链,但是当前动画链不会切换，依旧保持在当前动画链上
*	@param	
*		@arg	sprites	精灵对象
*		@arg  newDefaultChainIdx 新的默认动画链
*	@retval	
*		@arg	操作失败返回错误值 -1；
*		@arg	操作成功返回 0
*/
int SpritesAnim_SetDefaultChain(SpritesBase_t* sprites,AnimChain_t newDefaultChainIdx){
	if(sprites == C_NULL)	return -1;
	
	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//目标管理器不存在，返回错误
	
	//验证记录表
	if(newDefaultChainIdx >= SpritesManager->AnimationChainCount){
		return -1;//记录表非法访问
	}
	
	sprites->AnimMoudle.defaultAnimationChainIndex = newDefaultChainIdx;
	return 0;
}
/**
*		
*	@brief	获取一个精灵指定动画链的帧的数量
*	@param	
*		@arg	sprites	精灵对象
*		@arg  ChainIdx 动画链索引
*	@retval	
*		@arg	操作失败返回错误值 -1；
*		@arg	操作成功返回 动画链的帧的数量
*/
int SpritesAnim_GetFrmCountOfChain(SpritesBase_t* sprites,AnimChain_t ChainIdx){
	if(sprites == C_NULL)	return -1;
	
	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//目标管理器不存在，返回错误
	
	//验证记录表
	if(ChainIdx >= SpritesManager->AnimationChainCount){
		return -1;//记录表非法访问
	}
	
	return SpritesManager->SpritesRecordGroup[ChainIdx].frame_count;
}
/**
*		
*	@brief	获取指定精灵当前所在的动画链的索引
*	@param	
*		@arg	sprites	精灵对象
*	@retval	
*		@arg	操作失败，进入异常处理并阻塞；
*		@arg	操作成功返回 索引
*/
AnimChain_t SpritesAnim_GetChainIndex(SpritesBase_t* sprites){
		if(sprites == C_NULL)	{
				//【错误】<空指针访问>
				EngineFault_NullPointerGuard_Handle();
		}
		return sprites->AnimMoudle.CurrentAnimationChainIndex;
}
/**********************
	【精灵动画帧操作】
***********************/
/**
*		
*	@brief	为指定动画链添加一个动画帧
*	@param	
*		@arg	AnimationManageIndex	动画管理器索引
*		@arg	AnimationChainIndex	精灵动画链的索引
*		@arg	AnimationFrameSeq	动画帧的序号， @note 注意，引擎会根据序号修正动画帧的播放顺序，且不能出现相同的序号
*		@arg	SpritesData	精灵数据数组
*		@arg	SpritesShadow	精灵影子数组 @note 该数组用于描述 SpritesData 成员的有效部分，如果精灵使用BYTE_SPRITES，则此参数会被忽略
*	@retval	
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是-1，意味着内存分配失败或者参数错误
*/
int SpritesAnim_InsertNewFrm(int AnimationManageIndex,int AnimationChainIndex,int AnimationFrameSeq,\
	const unsigned char* SpritesData,const unsigned char* SpritesShadow){
	//查找指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,AnimationManageIndex);
	if(SpritesManager == C_NULL)	return -1;//目标管理器不存在，返回错误值
	
	//验证是否存在对应的动画记录表索引(由于记录表记录的是数量，参数是索引，索引要比数量-1)
	if(AnimationChainIndex >= SpritesManager->AnimationChainCount){
		return -1;
	}
	
	//创建动画帧
	SpritesLinkNode_t* node = (SpritesLinkNode_t*)malloc(sizeof(SpritesLinkNode_t));
	if(node == C_NULL)	return -1;//内存申请失败
	//初始化节点
	node->record = &(SpritesManager->SpritesRecordGroup[AnimationChainIndex]);
	node->next = C_NULL;
	node->SerialNumber = AnimationFrameSeq;
	node->SpritesData = SpritesData;
	node->SpritesShadow = SpritesShadow;
	//插入进入动画链中
	if(SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup == C_NULL){
		//如果之前动画链中不存在动画帧，那么直接插入
		node->next = node;
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup = node;
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count++;//动画帧数量自增
		//更新单帧播放时长
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_duration = SpritesManager->SpritesRecordGroup[AnimationChainIndex].Anim_duration / \
																																						 SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count;
		return 0;
	}else{
		//如果存在动画帧，则按照顺序插入
		//定义一个探针用于遍历
		SpritesLinkNode_t* current = SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup;
		SpritesLinkNode_t* head = current;
		SpritesLinkNode_t* prev = C_NULL;//慢指针
		
		// 遍历链表找到插入位置 
		do {
			// 检查是否存在重复的SerialNumber
			if (current->SerialNumber == node->SerialNumber) {
				// 处理SerialNumber重复的情况 
				free(node);//释放节点
				return -1;//返回错误
			}
			
			// 找到插入位置
			if (current->SerialNumber > node->SerialNumber) {
				break;
			}
			
			prev = current;
			current = current->next;
		}while (current != head);
		
		
		// 插入新节点
		node->next = current;
		if (prev != C_NULL){
			//慢指针不等于空，意味着结果不是首指针
			prev->next = node;//慢指针作为上一个指针连接node节点
		}else{
			// 处理头部插入的情况 
			SpritesLinkNode_t* tail = head; 
			while (tail->next != head) { 
				tail = tail->next; 
			} 
			tail->next = node; 
			node->next = head;
			//设置新的头作为动画链的首帧
			SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup = node;
		}
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count++;//动画帧数量自增
		//更新单帧播放时长
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_duration = SpritesManager->SpritesRecordGroup[AnimationChainIndex].Anim_duration / \
																																						 SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count;
		return 0;
	}
}

/**
*		
*	@brief	指定精灵的动画更新
*	@param	
* @arg	sprites	精灵对象
*	@retval	
*		none
*/
void SpritesAnim_Update(SpritesBase_t* sprites){
	if(sprites == C_NULL || sprites->AnimMoudle.ReadyAnimation == C_NULL)	return;
	
	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return ;//目标管理器不存在，返回错误
	
	//更新累计阈值
	sprites->accumulatedTime += (float)(DeltaTime->deltaTime);
	
	//判断是否超过了更新阈值
	while(sprites->accumulatedTime >= SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_duration){
		//降低阈值
		sprites->accumulatedTime -= SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_duration;
		//判断动画类型是循环动画还是非循环动画
		if(sprites->AnimMoudle.AnimationType == Anim_Once){
			//非循环动画
			
			//如果当前动画帧不是最后一帧
			if(sprites->AnimMoudle.currentFrmIndex < SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_count){
				sprites->AnimMoudle.ReadyAnimation = sprites->AnimMoudle.ReadyAnimation->next;
				sprites->AnimMoudle.currentFrmIndex++;
			}
			
		}else{
			//循环动画
			//更新为下一帧
			sprites->AnimMoudle.ReadyAnimation = sprites->AnimMoudle.ReadyAnimation->next;
		}
	}
}
/**
*		
*	@brief	指定精灵的动画切换为当前动画链的第一帧
*	@param	
* @arg	sprites	精灵对象
*	@retval	
*		@arg	操作失败返回 -1；
*		@arg	操作成功返回 0
*/
int SpritesAnim_SwitchFirstFrm(SpritesBase_t* sprites){
	//参数检测
	if(sprites == C_NULL || sprites->AnimMoudle.ReadyAnimation == C_NULL)	return -1;
	
	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//目标管理器不存在，返回错误
	
	sprites->AnimMoudle.ReadyAnimation = SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].AnimationChainGroup;
	sprites->AnimMoudle.currentFrmIndex = 0;
	return 0;
}
/**
*		
*	@brief	精灵信息提取
*	@param	
*		@arg	spritesNode	精灵节点
*		@arg	ROW_Pixel	精灵数组的行
*		@arg	COL_Pixel	精灵数组的列
*	@retval	
*		@arg	显示透明值 返回-1；
*		@arg	显示暗色值 返回0；
*		@arg	显示亮色值 返回1；
*/
int SpritesAnim_ExtractFrmData(SpritesBase_t* sprites,int ROW_Pixel,int COL_Pixel){
	//参数合法性检查
	if(sprites == C_NULL)	return -1;
	if(ROW_Pixel < 0 || ROW_Pixel >= sprites->AnimMoudle.ReadyAnimation->record->Logic_ROW)	return -1;
	if(COL_Pixel < 0 || COL_Pixel >= sprites->AnimMoudle.ReadyAnimation->record->Column)	return -1;	
	
	//如果数据是字节型
	if(sprites->AnimMoudle.ReadyAnimation->record->SpritesType == BYTE_SPRITES){
		return sprites->AnimMoudle.ReadyAnimation->SpritesData[ROW_Pixel* sprites->AnimMoudle.ReadyAnimation->record->Column + COL_Pixel];
	}
	//如果数据是bit型
	else if(sprites->AnimMoudle.ReadyAnimation->record->SpritesType == BIT_SPRITES){
		int byte = ROW_Pixel/8;
		int offset = ROW_Pixel%8;
		//检查影子是否为1
		int data_shadow = (sprites->AnimMoudle.ReadyAnimation->SpritesShadow[byte * sprites->AnimMoudle.ReadyAnimation->record->Column + COL_Pixel] & (0x01<<offset)) ? 1:0;
		if(data_shadow == 0){
			//为0显示透明
			return -1;
		}
		//为1显示数组图案
		return (sprites->AnimMoudle.ReadyAnimation->SpritesData[byte * sprites->AnimMoudle.ReadyAnimation->record->Column + COL_Pixel] & (0x01<<offset)) ? 1:0;
	}
	return -1;
}

/**
*		
*	@brief	设置精灵的动画类型
*	@param	
*		@arg	sprites	精灵对象
*		@arg	AnimationType	动画类型定义在 @ref AnimationType_t
*	@retval	
*		操作成功返回 0
*		操作成功返回 -1
*/
int SpritesAnim_SetAnimType(SpritesBase_t* sprites,AnimationType_t AnimationType){
	if(sprites == C_NULL)	return -1;
	sprites->AnimMoudle.AnimationType = AnimationType;
	return 0;
}
/**
*		
*	@brief	获取精灵的动画类型
*	@param	
*		@arg	sprites	精灵对象
*	@retval	
*		操作成功返回 类型，定义在 @ref AnimationType_t
*		操作成功返回 -1
*/
AnimationType_t SpritesAnim_GetAnimType(SpritesBase_t* sprites){
	if(sprites == C_NULL){
		//【错误】<枚举错误访问>
		EngineFault_ExceptionEnumParam_Handle();
		return Anim_Error;
	}
	return sprites->AnimMoudle.AnimationType;
}
/**
*		
*	@brief	获取精灵动画是否到了动画链的尾帧，注意只有非循环动画才能计算，否则返回错误值(函数待完善，因为非循环动画到了最后一帧，无法判定到底执行了没有)
*	@param	
*		@arg	sprites	精灵对象
*	@retval	
*		到达尾帧返回 1
*		没有到达尾帧返回 0
*		操作失败返回 -1
*/
int SpritesAnim_IsEndFrm(SpritesBase_t* sprites){
	if(sprites == C_NULL || sprites->AnimMoudle.AnimationType == Anim_Loop)	return -1;

	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//目标管理器不存在，返回错误
	
	if(sprites->AnimMoudle.currentFrmIndex == (SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_count - 1)){
		return 1;
	}else{
		return 0;
	}
}



