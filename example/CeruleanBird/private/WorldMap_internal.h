/*当前文件为二层塔*/
#ifndef __WORLDMAP_INTERNAL_H__
#define __WORLDMAP_INTERNAL_H__
#include "WorldMap_API.h"
/**
*
*	@ref TextureMapDef
*
*/
struct WorldMapDef{
	unsigned char* worldmap_array;	//该地图的数组地址（注意，数组只能为一维数组）
	int Row;						//该地图数组的行
	int Column;						//该地图数组的列
};

/**
*	@brief	获取指定绑定值的地图
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回地图结构体指针 @ref WorldMapDef，获
*			取失败返回空
*/
WorldMapDef* WorldMap_GetWorldMap(int value);
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
int WorldMap_Extract(WorldMapDef* worldmap,int ROW,int COL);
#endif
