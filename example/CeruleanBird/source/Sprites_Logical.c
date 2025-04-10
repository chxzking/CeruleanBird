/**
  ******************************************************************************
  * @file    Sprites_Animation.c
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    2025-2-7
  * @brief   该文件包含了库精灵逻辑有关的函数实现，例如精灵总线
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
#include <math.h>

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


extern RB_Node* Sprites_Header;	


/*

	  精灵的坐标逻辑
			|
			V
		 世界坐标	
			|
			V
     相对于摄像机坐标（相对坐标）
			|
			V
  摄像机视野坐标系中的坐标
  
	
*/

/****************************************************
	
	
	
									【精灵数据操作】
	
	
	
*****************************************************/





/**********************
	【精灵体总线操作】
***********************/
/**
*	@brief	精灵总线执行触发函数(当前还没有处理静态精灵)
*	@param	
*		@arg	camera	摄像机对象
*	@retval	
*		none
*/
void* Sprites_Exec(Camera_Struct* camera){
	if(camera == C_NULL)	return C_NULL;
	//整理精灵渲染顺序
	SpritesTree_Sort(camera);
	//逐步精灵计算
	SpritesTreeNode_t* spritesNode = SpritesTree->head;
	while(spritesNode != NULL){
		//更新精灵数据
		if(Sprites_SingleUpdate(spritesNode->SpriteTrDescript.sprite, spritesNode->SpriteTrDescript.SpriteFeature) != 0){
			break;//精灵更新失败，跳过此精灵
		}
		
		//渲染此精灵
		Sprites_SingleRender(spritesNode->SpriteTrDescript.sprite ,camera);
		//更新节点
		spritesNode = spritesNode->next;
	}
	//返回引擎共享数据空间
	return camera->Engine_Common_Data.data;
}

/**
*	@brief	单精灵状态更新
*	@param	
*		@arg	camera	摄像机对象
*	@retval	
*		@arg	执行成功返回 0
*		@arg	执行失败返回-1
*/
int Sprites_SingleUpdate(SpritesBase_t* sprite, SpriteFeature_t SpriteFeature){
	if(sprite == NULL){
		return -1;
	}
	if(SpriteFeature == NormalSprites){//普通精灵处理
		return SpritesBT_BusExecute((Sprites_t*)sprite);
	}
	else if(SpriteFeature == StaticSprites){//静态精灵处理
		SpritesAnim_Update(sprite);
	}else{
		//【错误】<枚举错误访问>
		EngineFault_ExceptionEnumParam_Handle();
	}
	return 0;
}



/**
*	@brief	（Bresenham直线算法）墙体阻挡探测
*	@param	
*		@arg	x1	self的横坐标
*		@arg	y1	self的纵坐标
*		@arg	x2	目标的横坐标
*		@arg	y2	目标的纵坐标
*	@retval	
*		@arg	如果不存在墙体阻挡，则返回0
*		@arg	如果执行存在墙体阻挡，则返回-1
*/
int BresenhamLine_WallDetect(int x1, int y1, int x2, int y2) {
	
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        //地图获取出错，出现严重错误，紧急进入错误阻止
		//【错误】<非法访问>
		EngineFault_IllegalAccess_Handle();
    }
	
    int dx = x2 - x1;
    int dy = y2 - y1;

    int x = x1;
    int y = y1;

    int sx = (dx > 0) ? 1 : -1;  // x方向增量
    int sy = (dy > 0) ? 1 : -1;  // y方向增量

    dx = abs(dx);
    dy = abs(dy);

    int err = (dx > dy ? dx : -dy) / 2;  // 初始误差值
    int e2;

    while (1) {
        // 终止条件
        if (x == x2 && y == y2) {
            return 0;
        }
		
		//地图沿数据提取
		if(WorldMap_Extract(WorldMapData,x,y) != MOVABLE_AREA_VALUE){//x代表ROW，y代表地图的COL
			//发现墙壁
			return 1;
		}
		
        e2 = err;

        // 更新误差和位置
        if (e2 > -dx) {
            err -= dy;
            x += sx;
        }
        if (e2 < dy) {
            err += dx;
            y += sy;
        }
    }
}



/**********************
	【普通精灵体操作】
***********************/

/**
*	@brief	实例化一个普通精灵体对象
*	@param	
*		@arg	AnimationManageIndex	精灵模板代号(精灵管理器)
*		@arg	posRow	精灵位置横坐标
*		@arg	posCol	精灵位置纵坐标
*		@arg	defaultAnimationChainIndex 设置默认动画链的序号
*		@arg	moveSpeed 精灵的移动速度
*		@arg  map_value 地图代号
*		@arg  StateHandle 状态机句柄
*	@retval	
*		@arg	创键失败返回 空；
*		@arg	创建成功返回 精灵地址
*/
Sprites_t* Sprites_ObjectCreate(int AnimationManageIndex,double posRow,double posCol,unsigned char defaultAnimationChainIndex,\
	double moveSpeed,int map_value,int BehaviorTreeValue){
	//分配内存
	Sprites_t* sprites = (Sprites_t*)malloc(sizeof(Sprites_t));
	if(sprites == C_NULL)	return C_NULL;
		
	//位置初始化
	sprites->SpritesBase.posRow = posRow;
	sprites->SpritesBase.posCol = posCol;
	sprites->dir.x = 1.0;
	sprites->dir.y = 0.0;	
	sprites->SpritesBase.VerticalOffset = 0.0;
		
	//累积值初始化		
	sprites->SpritesBase.accumulatedTime = 0.0;	
	sprites->SpritesBase.map_value = map_value;	
	sprites->MoveSpeed = moveSpeed;		
		
	//动画树配置
	sprites->SpritesBase.AnimMoudle.value = AnimationManageIndex;
	sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.defaultAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.currentFrmIndex = 0;
	sprites->SpritesBase.AnimMoudle.AnimationType = Anim_Once;//初始化为非循环动画
	
	//行为树配置
	sprites->BehaviorMoudle.value = BehaviorTreeValue;
	sprites->BehaviorMoudle.IT_Enable = 0;//默认关闭中断
	sprites->BehaviorMoudle.bt_context.node.action = NULL;
	sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;
	sprites->BehaviorMoudle.IT_running = 0;//默认设置为当前没有中断运行
	sprites->BehaviorMoudle.bt_context.Previous_IT_index = -1;//随机设置一个负值代表当前执行的占位中断号（中断号不能为负）
	
	//初始化私有数据
	sprites->Private.data = C_NULL;
	sprites->Private.SpritesPrivateFreeHandle = C_NULL;

	//初始墙壁碰撞值设置
	sprites->wall = MOVABLE_AREA_VALUE;
	
	//记录动画链
	
	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,sprites->SpritesBase.AnimMoudle.value);
	if(SpritesManager == C_NULL){
		free(sprites);
		return C_NULL;//目标管理器不存在，返回错误值
	}
	
	//获取目标动画链
	if(sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex >= SpritesManager->AnimationChainCount){
		free(sprites);
		return C_NULL;//动画链索引存在错误
	}

	//动画链首地址存入精灵体作为初始化条件
	if(SpritesManager->SpritesRecordGroup == C_NULL){
		//动画链不存在动画帧
		free(sprites);
		return C_NULL;
	}
	
	//获取首个动画帧
	sprites->SpritesBase.AnimMoudle.ReadyAnimation = SpritesManager->SpritesRecordGroup[defaultAnimationChainIndex].AnimationChainGroup;
	sprites->SpritesBase.AnimMoudle.SpritesManager = SpritesManager;
	
	SpritesManager->SpritesCount++;
	
	
	SpritesTree_Add(sprites);
	
	//返回精灵体
	return sprites;
}



/**
*	@brief	设置精灵的方向
*	@param	
*		@arg	sprites 精灵体
*	@retval	
*		@arg	设置成功返回 0
*		@arg	设置失败返回 -1
*/
int SpritesMov_SetDir(Sprites_t* sprites,vector dir){
	//参数检查
	if(sprites == NULL){
		return -1;
	}
	// 零值检查
	if ((dir.x > -1e-9 && dir.x < 1e-9) && (dir.y > -1e-9 && dir.y < 1e-9)) {
		return -1;//产生了零向量
	}
	//归一化方向向量
	sprites->dir = vector_Normalize(&dir);
	return 0;
}
/**
*	@brief	获取精灵方向
*	@param	
*		@arg	sprites 精灵体
*	@retval	
*		@arg	返回精灵方向向量(空指针访问会导致引擎进入错误拦截状态)
*/
vector SpritesMov_GetDir(Sprites_t* sprites){
	//参数检查
	if(sprites == NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	return sprites->dir;
}
/**
*	@brief	将精灵方向设置为朝向摄像机
*	@param	
*		@arg	sprites 精灵体
*		@arg	camera	摄像机
*	@retval	
*		@arg	设置成功返回 0
*		@arg	设置失败返回 -1
*/
int SpritesMov_SetDirToCamera(Sprites_t* sprites,Camera_Struct* camera){
	//参数检查
	if(sprites == NULL || camera == NULL){
		return -1;
	}
	//计算坐标
	vector dir;
	dir.x = camera->position.x - sprites->SpritesBase.posRow;
	dir.y = camera->position.y - sprites->SpritesBase.posCol;
	
	//验证是否两个坐标重合
	if(( dir.x  > -1e-9 && dir.x < 1e-9) && (dir.y > -1e-9 && dir.y < 1e-9)){
		return -1;//两点重合为设置失败
	}
	//归一化方向向量
	sprites->dir = vector_Normalize(&dir);
	return 0;
}
/**
*	@brief	将精灵方向设置为与摄像机一致
*	@param	
*		@arg	sprites 精灵体
*		@arg	camera	摄像机
*	@retval	
*		@arg	设置成功返回 0
*		@arg	设置失败返回 -1
*/
int SpritesMov_SetDirWithCamera(Sprites_t* sprites,Camera_Struct* camera){
	//参数检查
	if(sprites == NULL || camera == NULL){
		return -1;
	}
	sprites->dir = camera->direction;
	return 0;
}


/**
*	@brief	精灵朝左侧移动(不允许穿过墙壁)
*	@param	
*		@arg	sprites 精灵体
*	@retval	
*		@arg	执行成功返回 0
*		@arg	执行失败返回 -1
*/
int SpritesMov_Left(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // 地图数据获取错误
    }

    // 修正为帧率无关移动速度
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // 计算左移方向的向量
    vector leftDir;
    leftDir.x = -sprite->dir.y; // 左移垂直于当前方向
    leftDir.y = sprite->dir.x;

    // 检查是否可以移动到新位置
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow + leftDir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol + leftDir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow += leftDir.x * correctionSpeed;
        sprite->SpritesBase.posCol += leftDir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}


/**
*	@brief	精灵朝右侧移动(不允许穿过墙壁)
*	@param	
*		@arg	sprites 精灵体
*	@retval	
*		@arg	执行成功返回 0
*		@arg	执行失败返回 -1
*/
int SpritesMov_Right(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // 地图数据获取错误
    }

    // 修正为帧率无关移动速度
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // 计算右移方向的向量
    vector rightDir;
    rightDir.x = sprite->dir.y;  // 右移垂直于当前方向
    rightDir.y = -sprite->dir.x;

    // 检查是否可以移动到新位置
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow + rightDir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol + rightDir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow += rightDir.x * correctionSpeed;
        sprite->SpritesBase.posCol += rightDir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}


/**
*	@brief	精灵朝前方移动(不允许穿过墙壁)
*	@param	
*		@arg	sprites 精灵体
*	@retval	
*		@arg	执行成功返回 0
*		@arg	执行失败返回 -1
*/
int SpritesMov_Forward(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // 地图数据获取错误
    }

    // 修正为帧率无关移动速度
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // 检查是否可以移动到新位置
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow + sprite->dir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol + sprite->dir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow += sprite->dir.x * correctionSpeed;
        sprite->SpritesBase.posCol += sprite->dir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}


/**
*	@brief	精灵朝后方移动(不允许穿过墙壁)
*	@param	
*		@arg	sprites 精灵体
*	@retval	
*		@arg	执行成功返回 0
*		@arg	执行失败返回 -1
*/
int SpritesMov_Backward(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // 地图数据获取错误
    }

    // 修正为帧率无关移动速度
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // 检查是否可以移动到新位置
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow - sprite->dir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol - sprite->dir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow -= sprite->dir.x * correctionSpeed;
        sprite->SpritesBase.posCol -= sprite->dir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}

/**
*	@brief	获取精灵碰撞的墙壁的值
*	@param	
*		@arg	sprites 精灵体
*	@retval	
*		返回精灵碰撞的墙壁
*/
int Sprites_collisionWallValue(Sprites_t* sprite){
	if(sprite == NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	int wall = sprite->wall;
	sprite->wall = MOVABLE_AREA_VALUE;//重置新的值
	return wall;
}

/**
*	@brief	设置精灵与摄像机位置重合位置
*	@param	
*		@arg	sprites 精灵体
*		@arg	camera 摄像机
*	@retval	
*		@arg	执行成功返回 0
*		@arg	执行失败返回 -1
*/
int SpritesMov_SetPosToCamera(Sprites_t* sprites,Camera_Struct* camera){
	if(sprites == NULL || camera == NULL) return -1;
	sprites->SpritesBase.posRow = camera->position.x;
	sprites->SpritesBase.posCol = camera->position.y;
	return 0;
}


/**********************
	【静态精灵体操作】
***********************/

/**
*	@brief	实例化一个静态精灵体对象
*	@param	
*		@arg	AnimationManageIndex	精灵元素代号（精灵管理器索引）
*		@arg	posRow	精灵位置横坐标
*		@arg	posCol	精灵位置纵坐标
*		@arg	defaultAnimationChainIndex 设置默认动画链的序号
*		@arg  	map_value 地图代号
*	@retval	
*		@arg	创键失败返回 空；
*		@arg	创建成功返回 精灵地址
*/
SpritesStatic_t* SpritesStatic_ObjectCreate(int AnimationManageIndex,double posRow,double posCol,unsigned char defaultAnimationChainIndex,int map_value){
	//分配内存
	SpritesStatic_t* sprites = (SpritesStatic_t*)malloc(sizeof(SpritesStatic_t));
	if(sprites == C_NULL)	return C_NULL;
	//初始化
	sprites->SpritesBase.posRow = posRow;
	sprites->SpritesBase.posCol = posCol;	
	sprites->SpritesBase.accumulatedTime = 0.0;
	sprites->SpritesBase.map_value = map_value;	
	sprites->SpritesBase.VerticalOffset = 0.0;
		
	sprites->SpritesBase.AnimMoudle.value = AnimationManageIndex;
	sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.defaultAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.currentFrmIndex = 0;
	sprites->SpritesBase.AnimMoudle.AnimationType = Anim_Once;//初始化为非循环动画

	
	//记录动画链
	
	//获取指定的精灵管理器
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,sprites->SpritesBase.AnimMoudle.value);
	if(SpritesManager == C_NULL){
		free(sprites);
		return C_NULL;//目标管理器不存在，返回错误值
	}
	
	//获取目标动画链
	if(sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex >= SpritesManager->AnimationChainCount){
		free(sprites);
		return C_NULL;//动画链索引存在错误
	}

	//动画链首地址存入精灵体作为初始化条件
	if(SpritesManager->SpritesRecordGroup == C_NULL){
		//动画链不存在动画帧
		free(sprites);
		return C_NULL;
	}
	
	//获取首个动画帧
	sprites->SpritesBase.AnimMoudle.ReadyAnimation = SpritesManager->SpritesRecordGroup[defaultAnimationChainIndex].AnimationChainGroup;
	sprites->SpritesBase.AnimMoudle.SpritesManager = SpritesManager;
	
	SpritesManager->SpritesCount++;
	
	//加入精灵树
	SpritesTree_StaticAdd(sprites);
	
	//返回精灵体
	return sprites;
		
}

/**********************
	【精灵通用操作】
***********************/
/**
*	@brief	获取指定精灵的位置
*	@param	
*		@arg	sprites	精灵对象
*	@ret	
*		@arg	操作成功返回 位置
*		@arg	参数异常进入错误处理态；
*/
vector SpritesData_GetPos(SpritesBase_t* sprites){
		if(sprites == C_NULL){
			//【错误】<空指针访问>
			EngineFault_NullPointerGuard_Handle();
		}
		vector pos;
		pos.x = sprites->posRow;
		pos.y = sprites->posCol;
		return pos;
}
/**
*	@brief	设置精灵的垂直方向的偏移量
*	@param	
*		@arg	sprites	精灵对象
*		@arg	VerticalHeight 精灵垂直偏移量(正数向上偏移，负数向下偏移，单位：像素)
*	@retval	
*		@arg	操作失败返回 -1；
*		@arg	操作成功返回 0
*/
int SpritesData_SetVerticalHeight(SpritesBase_t* sprites,int VerticalHeight){
		if(sprites == C_NULL){
				return -1;
		}
		sprites->VerticalOffset = VerticalHeight;
		return 0;
}
/**
*	@brief	获取精灵的垂直方向的偏移量
*	@param	
*		@arg	sprites	精灵对象
*	@retval	
*		@arg	返回垂直偏移量(正数向上偏移，负数向下偏移，单位：像素)
*		发生异常会陷入错误处理态
*/
int SpritesData_GetVerticalHeight(SpritesBase_t* sprites){
	if(sprites == C_NULL){
				//【错误】<空指针访问>
			EngineFault_NullPointerGuard_Handle();
		}
		return sprites->VerticalOffset;
}

/**
*	@brief		精灵垂直上移
*	@param	
*		@arg	sprites	精灵对象
*		@arg	VerticalHeight 精灵上移的偏移量(必须大于等于0，单位：像素) 
*	@retval	
*		@arg	操作失败返回 -1；
*		@arg	操作成功返回 0
*/
int SpritesData_VerticalHeight_MoveUp(SpritesBase_t* sprites,int VerticalHeight){
		if(sprites == C_NULL || VerticalHeight < 0){
				return -1;
		}
		sprites->VerticalOffset += VerticalHeight;
		return 0;
}
/**
*	@brief		精灵垂直下移
*	@param	
*		@arg	sprites	精灵对象
*		@arg	VerticalHeight 精灵下移的偏移量(必须大于等于0，单位：像素) 
*	@retval	
*		@arg	操作失败返回 -1；
*		@arg	操作成功返回 0
*/
int SpritesData_VerticalHeight_MoveDown(SpritesBase_t* sprites,int VerticalHeight){
		if(sprites == C_NULL || VerticalHeight < 0){
				return -1;
		}
		sprites->VerticalOffset -= VerticalHeight;
		return 0;
}

/**********************
	【私有数据操作】
***********************/
/**
*	@brief	在指定精灵对象中添加私有数据
*	@param	
*		@arg	sprites	普通精灵对象
*		@arg	data	  私有数据
*		@arg	FreeHandle	私有数据处理
*	@retval	
*		@arg	操作失败返回错误值 -1；
*		@arg	操作成功返回 0
*/
int SpritesPriv_Add(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle){
	if(sprites == C_NULL || sprites->Private.data != C_NULL)	return -1;
	sprites->Private.data = data;
	sprites->Private.SpritesPrivateFreeHandle = FreeHandle;
	return 0;
}
/**
*	@brief	替换指定精灵对象中的私有数据
*	@param	
*		@arg	sprites	普通精灵对象
*		@arg	data	  私有数据
*		@arg	FreeHandle	私有数据处理
*	@retval	
*		@arg	操作失败返回错误值 -1；
*		@arg	操作成功返回 0
*/
int SpritesPriv_Replace(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle){
	if(sprites == C_NULL)	return -1;
	SpritesPriv_Reset(sprites);
	sprites->Private.data = data;
	sprites->Private.SpritesPrivateFreeHandle = FreeHandle;
	return 0;
}
/**
*	@brief	重置指定精灵的私有数据(私有数据复位为空，同时调用资源回收句柄)
*	@param	
*		@arg	sprites	普通精灵对象
*	@retval	
*		none
*/
void SpritesPriv_Reset(Sprites_t* sprites){
		if(sprites == C_NULL)	return;
	
		if(sprites->Private.SpritesPrivateFreeHandle != C_NULL){
				sprites->Private.SpritesPrivateFreeHandle(sprites->Private.data);
		}
	
		sprites->Private.data = C_NULL;
		sprites->Private.SpritesPrivateFreeHandle = C_NULL;
	
		return;
}

/**
*	@brief	获取指定精灵对象中的私有数据
*	@param	
*		@arg	sprites	普通精灵对象
*	@retval	
*		@arg	操作失败返回 空；
*		@arg	操作成功返回 数据地址
*/
void* SpritesPriv_Get(Sprites_t* sprites){
	return (sprites == C_NULL ? C_NULL :sprites->Private.data);
}


	







/**************************************************

 渲染

***************************************************/


double CalculateStepDistance(double x1, double y1, double x2, double y2) {
    // 光线方向分量
    double RayX = x2 - x1;
    double RayY = y2 - y1;

    // 步进长度计算
    double stepLengthX = fabs(1 / RayX);
    double stepLengthY = fabs(1 / RayY);
	if(stepLengthX < 1e-9 || stepLengthY < 1e-9){
		return 1e-9;
	}

    double distance = 0.0; // 总步进距离
    double currentX = x1; // 当前X坐标
    double currentY = y1; // 当前Y坐标

    // 模拟光线逐步走向目标点
    while (fabs(currentX - x2) > stepLengthX || fabs(currentY - y2) > stepLengthY) {
        if (fabs(x2 - currentX) > fabs(y2 - currentY)) {
            currentX += stepLengthX * ((x2 > currentX) ? 1 : -1);
        } else {
            currentY += stepLengthY * ((y2 > currentY) ? 1 : -1);
        }
        distance += sqrt(stepLengthX * stepLengthX + stepLengthY * stepLengthY); // 累加步进距离
    }
    return distance;
}


/**
 * 	@brief 对单精灵进行渲染（已经消除墙壁透视问题）
 *	@param	
 *		@arg	sprites	精灵对象
 *		@arg	camera	摄像机对象
 *	@retval	
 *		none
 * 修改说明：
 * 1. 保持原有坐标转换和透视计算公式，确保精灵在相机坐标系中的位置及深度计算不变。
 * 2. 横向偏移计算采用原代码公式，避免修改建议中因额外系数引起的精灵位置偏差。
 * 3. 像素跳跃（pixelSkip）部分改为：当精灵在屏幕上尺寸较大时，不再直接返回，
 *    而是采用较大的跳步值，从而既能进行性能优化，又不影响上层逻辑执行节奏。
 * 4. 裁剪和纹理映射逻辑保持原代码方式，逐列、逐行计算纹理坐标，确保绘制不会产生形变。
 */
void Sprites_SingleRender(SpritesBase_t* sprites, Camera_Struct* camera) {
    // 坐标转换：将精灵坐标转换为相机局部坐标系
    double relativeCoordSys_Row = sprites->posRow - camera->position.x;
    double relativeCoordSys_Col = sprites->posCol - camera->position.y;

    // 使用相机平面与方向向量计算变换系数
    double coefficient = 1.0 / (camera->plane.x * camera->direction.y - camera->direction.x * camera->plane.y);

    // 计算精灵在相机方向上的深度（Dir_component）
    double Dir_component = coefficient * (-camera->plane.y * relativeCoordSys_Row + camera->plane.x * relativeCoordSys_Col);
    if (Dir_component <= 0) {
        // 精灵在相机后方，直接返回
        return;
    }

    // 计算精灵在相机平面（横向）的坐标
    double Plane_component = coefficient * (camera->direction.y * relativeCoordSys_Row - camera->direction.x * relativeCoordSys_Col);

    // 横向偏移采用原代码公式，保证精灵水平位置正确
    int HorizontalOffset = (int)((SCREEN_COLUMN / 2) * (1 + Plane_component / Dir_component));
    // 垂直偏移保持原处理逻辑
    int VerticalOffset = (sprites->VerticalOffset == 0) ? 0 : -(int)(sprites->VerticalOffset / Dir_component);

    // 计算精灵在屏幕上的高度和宽度（透视缩放）
    int spriteHeight = abs((int)(sprites->AnimMoudle.SpritesManager->
                        SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Logic_ROW / Dir_component));
    int spriteWidth  = abs((int)(sprites->AnimMoudle.SpritesManager->
                        SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Column / Dir_component));

    // 计算垂直绘制区域，居中并加上垂直偏移
    int drawStartY = -spriteHeight / 2 + SCREEN_ROW / 2 + VerticalOffset;
    int drawEndY   = spriteHeight / 2 + SCREEN_ROW / 2 + VerticalOffset;

    // 垂直裁剪，同时调整对应纹理坐标（texStartY, texEndY）
    int texStartY = 0;
    int texEndY = sprites->AnimMoudle.SpritesManager->
                  SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Logic_ROW;
    if (drawStartY < 0) {
        texStartY = (-drawStartY) * texEndY / (drawEndY - drawStartY);
        drawStartY = 0;
    }
    if (drawEndY >= SCREEN_ROW) {
        texEndY = texEndY - ((drawEndY - SCREEN_ROW + 1) * texEndY) / (drawEndY - drawStartY);
        drawEndY = SCREEN_ROW - 1;
    }

    // 计算水平绘制区域
    int drawStartX = -spriteWidth / 2 + HorizontalOffset;
    int drawEndX = spriteWidth / 2 + HorizontalOffset;
    int texStartX = 0;
    int texEndX = sprites->AnimMoudle.SpritesManager->
                  SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Column;
    if (drawStartX < 0) {
        texStartX = (-drawStartX) * texEndX / (drawEndX - drawStartX);
        drawStartX = 0;
    }
    if (drawEndX >= SCREEN_COLUMN) {
        texEndX = texEndX - ((drawEndX - SCREEN_COLUMN + 1) * texEndX) / (drawEndX - drawStartX);
        drawEndX = SCREEN_COLUMN - 1;
    }

    // 如果绘制区域无效，则直接退出
    if (drawStartX > drawEndX || drawStartY > drawEndY) {
        return;
    }

    // 计算纹理映射时每列和每行的步长
    double step_col = (texEndX - texStartX) / (double)(drawEndX - drawStartX);
    double step_row = (texEndY - texStartY) / (double)(drawEndY - drawStartY);

    // 当精灵在屏幕上的尺寸较大时，为了提升效率采用跳跃采样
    int pixelSkip = 1;
    if (spriteHeight > SCREEN_ROW || spriteWidth > SCREEN_COLUMN) {
        double differHight = spriteHeight / (double)SCREEN_ROW;
        double differWidth  = spriteWidth / (double)SCREEN_COLUMN;
        if (differHight > 1.5 || differWidth > 1.5) {
            // 原建议直接返回可能导致上层逻辑执行过快，这里改为采用较大跳步值，以保留渲染过程
            pixelSkip = 16;
        }
        else if (differHight > 1.35 || differWidth > 1.35) {
            pixelSkip = 8;
        }
        else if (differHight > 1.2 || differWidth > 1.2) {
            pixelSkip = 4;
        }
        else {
            pixelSkip = 2;
        }
    }

    // 计算精灵中心与相机的欧氏距离，用于逐列深度检测
    double dx = sprites->posRow - camera->position.x;
    double dy = sprites->posCol - camera->position.y;
    double dis = sqrt(dx * dx + dy * dy);

    // 纹理映射和绘制：逐列遍历（使用 pixelSkip 进行采样优化）
    double texX = texStartX;
    for (int col = drawStartX; col <= drawEndX; col += pixelSkip) {
		//越界约束
        if (col < 0 || col >= SCREEN_COLUMN) continue;
           
        // 逐列深度检测：只有当精灵距离小于当前背景距离时才绘制该列
        if (dis < Camera_DeepBuffer_Read(camera, col)) {
            double currTexX = texX;  // 保证每列独立的纹理横坐标
            double texY = texStartY;
            for (int row = drawStartY; row <= drawEndY; row += pixelSkip) {
                int signal = SpritesAnim_ExtractFrmData(sprites, (int)texY, (int)currTexX);
                if (signal == 0)
                    Canvas_PixelOperate(row, col, CANVAS_PIXEL_CLOSE);
                else if (signal == 1)
                    Canvas_PixelOperate(row, col, CANVAS_PIXEL_OPEN);
                texY += step_row * pixelSkip;
            }
        }
        texX += step_col * pixelSkip;
    }
}


/**
*	@brief	休眠一个精灵
*	@param	
*		@arg	Sprite 精灵
*	@retval	
*		@arg	操作成功返回0
*		@arg	操作失败返回-1
*/
int Sprites_Sleep(SpritesBase_t* Sprite){
	if(Sprite == NULL)	return -1;
	
	//提取精灵()
	SpritesTreeNode_t* node = SpritesTree_NodeExtract(Sprite,SpritesTree);
	if(node == C_NULL){
		return -1;//如果提取失败意味着精灵地址并不存在，或者已经休眠
	}
	
	//加入到休眠树中
	node->next = SpritesSleepTree->head;
	SpritesSleepTree->count++;
	SpritesSleepTree->head = node;
	
	return 0;
}


/**
*	@brief	唤醒一个精灵
*	@param	
*		@arg	Sprite 精灵
*	@retval	
*		@arg	操作成功返回0
*		@arg	操作失败返回-1
*/
int Sprites_Awaken(SpritesBase_t* Sprite){
	if(Sprite == NULL)	return -1;
	
	//提取精灵()
	SpritesTreeNode_t* node = SpritesTree_NodeExtract(Sprite,SpritesSleepTree);
	if(node == C_NULL){
		return -1;//如果提取失败意味着精灵地址并不存在，或者已经休眠
	}
	
	//加入到精灵树中
	node->next = SpritesTree->head;
	SpritesTree->count++;
	SpritesTree->head = node;
	return 0;
}


/**
*	@brief	对指定精灵当前的帧进行缩放，返回以缩放比例读取的源帧的像素状态(只允许整数倍放大)
*	@param	
*		@arg	spritesNode	精灵节点
*		@arg	ROW_Pixel	精灵数组的行
*		@arg	row_div	行的缩放比率	
*		@arg	col_div	列的缩放比率
*	@retval	
*		@arg	显示透明值 返回-1；
*		@arg	显示暗色值 返回0；
*		@arg	显示亮色值 返回1；
*/
//int SpritesAnim_AdjustScale(SpritesBase_t* sprites,int ROW_Pixel,int COL_Pixel,int row_div,int col_div){
//	
//	//SpritesAnim_ExtractFrmData:对sprites的二维单色数组图像中指定坐标的像素点进行读取，如果返回1，则控制像素点点亮，如果返回0，则控制像素点灭，如果返回-1，则保持之前的像素状态不做改变
//	int result = SpritesAnim_ExtractFrmData(TO_SPRITES_BASE(sprites),ROW_Pixel,COL_Pixel);

//}

















/**************************************************

精灵树

***************************************************/

SpritesTree_t* SpritesTree = C_NULL;	
SpritesTree_t* SpritesSleepTree = C_NULL;

/**
*	@brief	精灵树初始化
*	@param	
*		none
*	@retval	
*		@arg	操作成功 返回0；
*		@arg	操作失败 返回-1；
*/
int SpritesTree_Init(void){
	if(SpritesTree != C_NULL)	return 0;
	//精灵树初始化
	SpritesTree = (SpritesTree_t*)malloc(sizeof(SpritesTree_t));
	if(SpritesTree == C_NULL){
		return -1;
	}
	SpritesTree->count = 0;
	SpritesTree->head = C_NULL;
	
	//休眠树初始化
	SpritesSleepTree = (SpritesTree_t*)malloc(sizeof(SpritesTree_t));
	if(SpritesSleepTree == C_NULL){
		free(SpritesTree);
		SpritesTree = C_NULL;
		return -1;
	}
	SpritesSleepTree->count = 0;
	SpritesSleepTree->head = C_NULL;
	return 0;
}
/**
*	@brief	精灵树挂载精灵(仅能挂载普通精灵)
*	@param	
*		@arg	sprites 精灵对象
*	@retval	
*		@arg	操作成功 返回0；
*		@arg	操作失败 返回-1；
*/
int SpritesTree_Add(Sprites_t* sprites){
	if(sprites == C_NULL || SpritesTree == C_NULL)	return -1;
	SpritesTreeNode_t* node = (SpritesTreeNode_t*)malloc(sizeof(SpritesTreeNode_t));
	if(node == C_NULL)	return -1;
	node->next = SpritesTree->head;
	node->SpriteTrDescript.sprite = (SpritesBase_t*)sprites;
	node->SpriteTrDescript.SpriteFeature = NormalSprites;
	SpritesTree->count++;
	SpritesTree->head = node;
	return 0;
}
/**
*	@brief	精灵树挂载静态精灵
*	@param	
*		@arg	sprites 精灵对象
*	@retval	
*		@arg	操作成功 返回0；
*		@arg	操作失败 返回-1；
*/
int SpritesTree_StaticAdd(SpritesStatic_t* sprites){
	if(sprites == C_NULL || SpritesTree == C_NULL)	return -1;
	SpritesTreeNode_t* node = (SpritesTreeNode_t*)malloc(sizeof(SpritesTreeNode_t));
	if(node == C_NULL)	return -1;
	node->next = SpritesTree->head;
	node->SpriteTrDescript.sprite = (SpritesBase_t*)sprites;
	node->SpriteTrDescript.SpriteFeature = StaticSprites;
	SpritesTree->count++;
	SpritesTree->head = node;
	return 0;
}

/**
*	@brief	精灵树精灵从远到近排序
*	@param	
*		@arg	camera 摄像机对象
*	@retval	
*		none
*/
void SpritesTree_Sort(Camera_Struct* camera) {
    if (SpritesTree->head == NULL || SpritesTree->head->next == NULL) {
        return;
    }
    int swapped;
    SpritesTreeNode_t* ptr1;
    SpritesTreeNode_t* lptr = NULL;

    // 冒泡排序
    do {
        swapped = 0;
        ptr1 = SpritesTree->head;

        while (ptr1->next != lptr) {
            SpritesBase_t* sprite1 = (SpritesBase_t*)(ptr1->SpriteTrDescript.sprite);
            SpritesBase_t* sprite2 = (SpritesBase_t*)(ptr1->next->SpriteTrDescript.sprite);

            float distance1 = SpritesTree_DistanceFromplayer(sprite1, camera);
            float distance2 = SpritesTree_DistanceFromplayer(sprite2, camera);

            // 修改比较条件，从大到小排序
            if (distance1 < distance2) {
								SpriteTrDescript_t temp = ptr1->SpriteTrDescript;
								ptr1->SpriteTrDescript = ptr1->next->SpriteTrDescript;
								ptr1->next->SpriteTrDescript = temp;
							
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

/**
*	@brief	从精灵树中提取出一个节点
*	@param	
*		@arg	Sprite 精灵
*	@retval	
*		@arg	操作成功返回 节点地址
*		@arg	操作失败返回 空
*/
SpritesTreeNode_t* SpritesTree_NodeExtract(SpritesBase_t* Sprite, SpritesTree_t* Tree) {
    if (Sprite == NULL || Tree == NULL || Tree->head == NULL) {
        return C_NULL; // 检查输入参数是否有效
    }

    SpritesTreeNode_t* current = Tree->head;
    SpritesTreeNode_t* previous = C_NULL;

    // 遍历链表找到目标节点
    while (current != C_NULL) {
        if (current->SpriteTrDescript.sprite == Sprite) {
            // 找到匹配节点
            if (previous == C_NULL) {
                // 如果是头节点
                Tree->head = current->next;
            } else {
                // 如果不是头节点
                previous->next = current->next;
            }

            // 更新链表计数器
            Tree->count--;

            // 将节点与链表脱离
            current->next = C_NULL;
            return current; // 返回找到的节点
        }

        // 继续遍历
        previous = current;
        current = current->next;
    }

    return C_NULL; // 如果未找到匹配节点
}



















