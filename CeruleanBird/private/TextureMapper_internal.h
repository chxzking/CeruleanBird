/*当前文件为二层塔*/
#ifndef __TEXTUREMAPPER_INTERNAL_H__
#define __TEXTUREMAPPER_INTERNAL_H__
#include "TextureMapper_API.h"
/**
*
*	@ref TextureMapDef
*
*/
struct TextureMapperDef{
	unsigned char* texture_array;	//该纹理的数组地址（注意，数组只能为一维数组）
	int Row;						//该纹理数组的行（纹理数组物理层次的行）
	int Logic_ROW;					//该纹理的逻辑行，在使用位图的纹理数组时，它是位图的行(仅在使用位图情况下有效)
	int Column;						//该纹理数组的列
	TEXTURE_TYPE texture_type;		//该纹理数组成员的类型
};

/**
*	@brief	显示指定位置的元素
*	@param	
*		@arg	texture	 纹理资源
*		@arg	ROW_Pixel	行（以像素为单位）
*		@arg	COL_Pixel	列（以像素为单位）
*	@retval	
*		@arg	获取失败返回-1
*		@arg	获取亮信号返回非0正数
*		@arg	获取灭信号返回0
*/
int Texture_Extract(TextureMapperDef* texture,int ROW_Pixel,int COL_Pixel);
#endif
