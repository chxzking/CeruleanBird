#ifndef __TEXTUREMAPPER_API_H__
#define __TEXTUREMAPPER_API_H__

/*当前文件为一层塔*/

//纹理数组的类型
/**
*
*	@ref TEXTURE_TYPE
*
*/
typedef enum TEXTURE_TYPE{
	BYTE_TEXTURE,		   //纹理数组元素的值以字节为单位（一整个字节控制一个像素点）
	BIT_TEXTURE,		   //纹理数组元素的值以位为单位（一个位控制一个像素点）
	ERROR_TEXTURE		   //错误纹理
}TEXTURE_TYPE;

//纹理结构表映射结构体
typedef struct TextureMapperDef TextureMapperDef;

/**
*		@note	注意，代表纹理的值必须具有唯一性
*	@brief	在引擎中添加纹理，通过地图元素与纹理数组绑定
*	@param	
*		@arg	value	地图元素的值
*		@arg	Texture_Picture	需要被绑定的纹理数组
*		@arg	Texture_Row	纹理数组的行
*		@arg	Texture_COL纹理数组的列
*		@arg	Texture_Type 纹理数组的类型，有两种可选类型，一种是数组每一个
*				字节代表一个像素点 BYTE_TEXTURE ，另外一种是每一个位代表一个
*				像素点 BIT_TEXTURE。定义在 @ref TEXTURE_TYPE
*	@retval	
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是1，意味着该值已经存在
*		@arg	如果返回结果是-1，意味着内存分配失败或者参数错误
*/
int Texture_Add(int value,unsigned char* Texture_Picture,\
	int Texture_Row_Pixel,int Texture_COL_Pixel,TEXTURE_TYPE Texture_Type);
/**
*	@brief	删除引擎中指定的纹理
*	@param	
*		@arg	value	地图元素的值
*	@retval	none；
*/
void Texture_Delete(int value);
/**
*	@brief	删除引擎中所有的纹理(整颗纹理树被释放)
*	@param	none
*	@retval	none；
*	@note	耗时相对较长
*/
void Texture_Free(void);
/**
*	@brief	修改绑定的纹理数组绑定的值
*	@param	
*		@arg	value	原绑定的值
*		@arg	new_value	新修改的值
*	@retval	修改成功返回0，修改稿失败返回-1
*/
int Texture_ValueModify(int value,int new_value);
/**
*	@brief	修改绑定的纹理的行
*	@param	
*		@arg	value	绑定的值
*		@arg	ROW	纹理的行
*	@retval	修改成功返回0，修改失败返回-1
*/
int Texture_ROWModify(int value,int ROW);
/**
*	@brief	修改绑定的纹理的列
*	@param	
*		@arg	value	绑定的值
*		@arg	COL	纹理的列
*	@retval	修改成功返回0，修改失败返回-1
*/
int Texture_COLModify(int value,int COL);
/**
*	@brief	修改绑定的纹理数组
*	@param	
*		@arg	value	绑定的值
*		@arg	array	新的纹理数组
*	@retval	修改成功返回0，修改失败返回-1
*/
int Texture_ArrayModify(int value,unsigned char* array);
/**
*	@brief	修改绑定的纹理的类型
*	@param	
*		@arg	value	绑定的值
*		@arg	Texture_Type	新的纹理类型，BYTE_TEXTURE
*				和 BIT_TEXTURE可选。定义在 @ref TEXTURE_TYPE
*	@retval	修改成功返回0，修改失败返回-1
*/
int Texture_TypeModify(int value,TEXTURE_TYPE Texture_Type);
/**
*	@brief	获取指定绑定值的纹理
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回纹理结构体指针 @ref TextureMapDef，获
*			取失败返回空
*/
TextureMapperDef* Texture_GetTexture(int value);
/**
*	@brief	获取指定绑定值的纹理数组
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回纹理数组地址，获取失败返回空
*/
unsigned char* Texture_GetArray(int value);
/**
*	@brief	获取指定绑定值的纹理类型
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回纹理数组地址 BYTE_TEXTURE 或 BIT_TEXTURE
*			获取失败返回 ERROR_TEXTURE 。定义在 @ref TEXTURE_TYPE
*/
TEXTURE_TYPE Texture_GetTextureType(int value);
/**
*	@brief	获取指定绑定值的纹理数组的逻辑行
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回纹理数组行 ，获取失败返回-1
*/
int Texture_GetROW(int value);
/**
*	@brief	获取指定绑定值的纹理数组的列
*	@param	
*		@arg	value	绑定的值
*	@retval	获取成功返回纹理数组地址的行 ，获取失败返回-1
*/
int Texture_GetColumn(int value);

#endif
