/*当前文件为一层塔*/
#ifndef __WORLDMAP_API_H__
#define __WORLDMAP_API_H__

/*
	【地图数组模型】（还没有决定，不过目前是低位在上）
	0
   0+----------------------------->(Y)
	|
	|
	|					第4行为0，第5行为0，第6行为0，第7行为0
	|						  |
	|			高位在上   0xF0
	|						| 
	|					第0行为1，第1行为1，第2行为1，第3行为1
	|
	|
	|
	|
	|
	v
   (X)
*/

/*游戏地图的数据只能使用byte型*/

/****************************

	【目前的新想法】
	1、地图结构体完全隐藏，不暴露任何信息给用户层面，
		而地图的数据查询通过 [地图描述符] 访问 
	
*******************************/

typedef struct WorldMapDef WorldMapDef;
/**
*		@note	注意，代表地图的值必须具有唯一性
*	@brief	在引擎中添加地图，通过地图元素与地图数组绑定
*	@param	
*		@arg	value	地图元素的值
*		@arg	WorldMap_Data	需要被绑定的地图数组
*		@arg	WorldMap_Row	地图数组的行
*		@arg	WorldMap_COL 地图数组的列
*	@retval	
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是1，意味着该值已经存在
*		@arg	如果返回结果是-1，意味着内存分配失败或者参数错误
*/
int WorldMap_Add(int value,unsigned char* WorldMap_Data,int WorldMap_Row,int WorldMap_COL);

/*****************************
		【删除操作】
******************************/

/**
*	@brief	删除引擎中指定的地图
*	@param	
*		@arg	value	地图元素的值
*	@retval	none；
*/
void WorldMap_Delete(int value);
/**
*	@brief	删除引擎中所有的地图(整颗地图树被释放)
*	@param	none
*	@retval	none；
*	@note	耗时相对较长
*/
void WorldMap_Free(void);
/*****************************
		【修改操作】
******************************/
/**
*	@brief	修改绑定的地图数组绑定的值
*	@param	
*		@arg	value	原绑定的值
*		@arg	new_value	新修改的值
*	@retval	修改成功返回0，修改稿失败返回-1
*/
int WorldMap_ValueModify(int value,int new_value);
/**
*	@brief	修改绑定的地图的行
*	@param	
*		@arg	value	绑定的值
*		@arg	ROW	地图的行
*	@retval	修改成功返回0，修改失败返回-1
*/
int WorldMap_ROWModify(int value,int ROW);
/**
*	@brief	修改绑定的地图的列
*	@param	
*		@arg	value	绑定的值
*		@arg	COL	地图的列
*	@retval	修改成功返回0，修改失败返回-1
*/
int WorldMap_COLModify(int value,int COL);
/**
*	@brief	修改绑定的地图数组
*	@param	
*		@arg	value	绑定的值
*		@arg	array	新的地图数组
*	@retval	修改成功返回0，修改失败返回-1
*/
int WorldMap_ArrayModify(int value,unsigned char* array);
/*****************************
		【查询操作】
******************************/	
/**
*	@brief	获取指定绑定值的地图数组
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图数组地址，获取失败返回空
*/
unsigned char* WorldMap_GetArray(int value);
/**
*	@brief	获取指定绑定值的地图数组的逻辑行
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图数组行 ，获取失败返回-1
*/
int WorldMap_GetROW(int value);
/**
*	@brief	获取指定绑定值的地图数组的列
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图数组地址的行 ，获取失败返回-1
*/
int WorldMap_GetColumn(int value);
/**
*	@brief	设置当前的地图层
*	@param	
*		@arg	MapPos	选中的指定地图
*	@retval	none
*/
void WorldMap_SetCurrentMapPos(int MapPos);
/**
*	@brief	获取当前地图层的位置
*	@param	none
*	@retval	地图位置
*/
int WorldMap_GetCurrentMapPos(void);

#endif
