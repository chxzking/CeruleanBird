#include "RB_Tree_API.h"
#include "bit_operate.h"
#include "engine_config.h"
#include "WorldMap_internal.h"

#include "EngineFault.h"//引擎错误处理

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


RB_Node* WorldMap_Header = C_NULL;	//定义头

int WorldMap_Pos = 0;	//当前所在的地图在哪个地图上

//地图数据添加
int WorldMap_Add(int value,unsigned char* WorldMap_Data,int WorldMap_Row,int WorldMap_COL){
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //启用引擎动态内存解决方案
		memory_pool_init();
	#endif	//启用引擎动态内存解决方案
		
	//数据校验
	if(WorldMap_Data == C_NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	//内存分配
	WorldMapDef* node = (WorldMapDef*)malloc(sizeof(WorldMapDef));
	if(node == C_NULL)	return -1;
	node->worldmap_array = WorldMap_Data;
	node->Column = WorldMap_COL;
	node->Row = WorldMap_Row;

	//添加进入树中
	int result = rbInsert(&WorldMap_Header, value,(void*)node);
	if(result != 0){
		//添加出现问题
		free(node);
		return result;
	}
	//更新当前的地图定位
	WorldMap_Pos = value;
	return 0;	
}

/**
*	@brief	删除引擎中指定的地图
*	@param	
*		@arg	value	地图元素的值
*	@retval	none；
*/
void WorldMap_Delete(int value){
	if(WorldMap_Header == C_NULL)	return;
	rbDelete(&WorldMap_Header, value);
}
/**
*	@brief	删除引擎中所有的地图(整颗地图树被释放)
*	@param	none
*	@retval	none；
*	@note	耗时相对较长
*/
void WorldMap_Free(void){
	RB_Tree_Free(&WorldMap_Header);
}
/**
*	@brief	修改绑定的地图数组绑定的值
*	@param	
*		@arg	value	原绑定的值
*		@arg	new_value	新修改的值
*	@retval	修改成功返回0，修改稿失败返回-1
*/
int WorldMap_ValueModify(int value,int new_value){
	//查找目标值是否存在
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL)	return -1;//查找失败
	
	//查找新值是否已经存在
	WorldMapDef* new_result = (WorldMapDef*)search(WorldMap_Header, new_value);
	if(new_result != C_NULL)	return -1;//发现存在，则不可更新
	
	//创建新节点
	int temp = WorldMap_Add(new_value,new_result->worldmap_array,new_result->Row ,new_result->Column);
	if(temp != 0)	return -1;//新节点创建失败
	
	//删除原节点
	WorldMap_Delete(value);
	return 0;
}
/**
*	@brief	修改绑定的地图的行
*	@param	
*		@arg	value	绑定的值
*		@arg	ROW	地图的行
*	@retval	修改成功返回0，修改失败返回-1
*/
int WorldMap_ROWModify(int value,int ROW){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Row = ROW;
	return 0;
}
/**
*	@brief	修改绑定的地图的列
*	@param	
*		@arg	value	绑定的值
*		@arg	COL	地图的列
*	@retval	修改成功返回0，修改失败返回-1
*/
int WorldMap_COLModify(int value,int COL){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Column= COL;
	return 0;
}
/**
*	@brief	修改绑定的地图数组
*	@param	
*		@arg	value	绑定的值
*		@arg	array	新的地图数组
*	@retval	修改成功返回0，修改失败返回-1
*/
int WorldMap_ArrayModify(int value,unsigned char* array){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->worldmap_array = array;
	return 0;
}
/**
*	@brief	获取指定绑定值的地图
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图结构体指针 @ref WorldMapDef，获
*			取失败返回空
*/
WorldMapDef* WorldMap_GetWorldMap(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	return result;
}
/**
*	@brief	获取指定绑定值的地图数组
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图数组地址，获取失败返回空
*/
unsigned char* WorldMap_GetArray(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return C_NULL;
	}
	return result->worldmap_array;
}
/**
*	@brief	获取指定绑定值的地图数组的逻辑行
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图数组行 ，获取失败返回-1
*/
int WorldMap_GetROW(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Row;
}
/**
*	@brief	获取指定绑定值的地图数组的列
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图数组地址的行 ，获取失败返回-1
*/
int WorldMap_GetColumn(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Column;
}
/**
*	@brief	显示地图指定位置的元素
*	@param	
*		@arg	worldmap	 地图资源
*		@arg	ROW			行
*		@arg	COL			列
*	@retval	
*		@arg	异常操作，跳转引擎异常函数
*		@arg	获取成功返回地图的值
*/
int WorldMap_Extract(WorldMapDef* worldmap,int ROW,int COL){
	//检查纹理
	if(worldmap == C_NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	if((ROW >= worldmap->Row) || (COL >= worldmap->Column)){
		//【错误】<越界访问>
		EngineFault_IllegalAccess_Handle();
	}
	return worldmap->worldmap_array[ROW * worldmap->Column + COL ];
}

//设置当前的地图层
void WorldMap_SetCurrentMapPos(int MapPos){
	WorldMap_Pos = MapPos;
}
//获取当前地图层的位置
int WorldMap_GetCurrentMapPos(void){
	return WorldMap_Pos;
}




