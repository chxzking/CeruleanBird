#include "TextureMapper_internal.h"


#include "RB_Tree_API.h"
#include "bit_operate.h"
#include "engine_config.h"

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


RB_Node* TextureMapper_Header = C_NULL;

/****************
	【纹理添加】
*****************/

//添加纹理
int Texture_Add(int value,unsigned char* Texture_Picture,\
	int Texture_Row_Pixel,int Texture_COL_Pixel,TEXTURE_TYPE Texture_Type){
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //启用引擎动态内存解决方案
		memory_pool_init();
	#endif	//启用引擎动态内存解决方案
	
	//数据校验
	if(Texture_Picture == C_NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	//内存分配
	TextureMapperDef* node = (TextureMapperDef*)malloc(sizeof(TextureMapperDef));
	if(node == C_NULL)	return -1;
	node->texture_array = Texture_Picture;
	node->texture_type = Texture_Type;
	node->Column = Texture_COL_Pixel;
	node->Logic_ROW = Texture_Row_Pixel;
	if(Texture_Type == BIT_TEXTURE){
		node->Row = bitToByte(Texture_Row_Pixel);
	}else{
		node->Row = Texture_Row_Pixel;
	}
	//添加进入树中
	int result = rbInsert(&TextureMapper_Header, value,(void*)node);
	if(result != 0){
		//添加出现问题
		free(node);
		return result;
	}
	return 0;
}

/****************
	【纹理查找】
*****************/

//纹理查找
TextureMapperDef* Texture_GetTexture(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	return result;
}
unsigned char* Texture_GetArray(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return C_NULL;
	}
	return result->texture_array;
}
TEXTURE_TYPE Texture_GetTextureType(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return ERROR_TEXTURE;
	}
	return result->texture_type;
}
int Texture_GetROW(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Logic_ROW;
}
int Texture_GetColumn(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Column;
}
/****************
  【纹理参数修改】
*****************/
int Texture_ValueModify(int value,int new_value){
	//查找目标值是否存在
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL)	return -1;//查找失败
	
	//查找新值是否已经存在
	TextureMapperDef* new_result = (TextureMapperDef*)search(TextureMapper_Header, new_value);
	if(new_result != C_NULL)	return -1;//发现存在，则不可更新
	
	//创建新节点
	int temp = Texture_Add(new_value,new_result->texture_array,new_result->Logic_ROW ,new_result->Column,new_result->texture_type);
	if(temp != 0)	return -1;//新节点创建失败
	
	//删除原节点
	Texture_Delete(value);
	return 0;
}

int Texture_ROWModify(int value,int ROW){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Logic_ROW = ROW;
	if(result->texture_type == BIT_TEXTURE){
		result->Row = bitToByte(ROW);
	}else{
		result->Row = ROW;
	}
	return 0;
}
int Texture_COLModify(int value,int COL){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Column= COL;
	return 0;
}
int Texture_ArrayModify(int value,unsigned char* array){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	if(array == C_NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	result->texture_array = array;
	return 0;
}
int Texture_TypeModify(int value,TEXTURE_TYPE Texture_Type){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->texture_type = Texture_Type;
	return 0;
}

/****************
	【纹理删除】
*****************/

//单个纹理删除
void Texture_Delete(int value){
	if(TextureMapper_Header == C_NULL)	return;
	rbDelete(&TextureMapper_Header, value);
}

//释放所有纹理
void Texture_Free(void){
	RB_Tree_Free(&TextureMapper_Header);
}

/****************
	【纹理信息提取】
*****************/
//显示指定位置的元素是
int Texture_Extract(TextureMapperDef* texture,int ROW_Pixel,int COL_Pixel){
	//检查纹理
	if(texture == C_NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	if((ROW_Pixel < 0) || (ROW_Pixel >= texture->Logic_ROW))	return -1;
	if((COL_Pixel < 0) || (COL_Pixel >= texture->Column))	return -1;
	//点位模式
	if(texture->texture_type == BIT_TEXTURE){
		int byte = ROW_Pixel/8;
		int offset = ROW_Pixel%8;
//		unsigned char texture_Unit = texture->texture_array[byte*texture->Column + COL_Pixel];
//		return Bit_Read_Within_Byte(&texture_Unit,7 - offset);
		//这里使用从低位往高位读取，是因为取模软件是屏幕高位为字节低位，比如10000000，显示出来的是1在最下面
		//这该死的取模软件在这个地方导致我的图片倒转，我查找了几个小时的代码问题，最终发现是取模软件问题，气死了。
		return (texture->texture_array[byte*texture->Column + COL_Pixel] & (0x01<<offset)) ? 1:0;
	}
	//字节模式
	else if(texture->texture_type == BYTE_TEXTURE){
		return texture->texture_array[ROW_Pixel*texture->Column + COL_Pixel];
	}
	return -1;
}
