#include "engine_config.h"
/**************************************

		内存模型解决方案

***************************************/
#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //条件编译判断

#ifndef _HEAP_SOLUTION_1_H_
#define _HEAP_SOLUTION_1_H_

#define C_NULL						( ( void* ) 0 )			//定义一个空指针地址
//代码

/**
*	@brief	初始化内存池
*	@param	none
*	@retval	none
*/
void memory_pool_init(void);
/**
*		@note	该函数调用前，必须已经执行过memory_pool_init()初始化，否则会导致未定义错误
*	@brief	请求获取指定大小内存空间
*	@param	size -->	预分配空间大小
*	@retval	操作成功返回空间首地址，否则返回空；
*/
void* c_malloc(unsigned int size);
/**
*		@note	该函数调用前，必须已经执行过memory_pool_init()初始化，否则会导致未定义错误
*	@brief	请求获取重新指定大小内存空间
*		@note	注意只能扩张空间不能缩小空间
*	@param	size -->	预分配空间大小
*			ptr	-->		原空间的值
*	@retval	操作成功返回新空间首地址，否则返回空；
*		@note	注意保存原空间的地址，以防空间分配失败丢失原空间地址造成内存泄露
*/
void* c_realloc(void* ptr,unsigned int size);
/**
*	@brief	释放分配空间
*	@param	ptr	-->	空间首地址
*	@retval	none
*/
void c_free(void* ptr);


#endif	//_HEAP_SOLUTION_1_H_

#endif	//HEAP_SCHEME == HEAP_SOLUTION_1


