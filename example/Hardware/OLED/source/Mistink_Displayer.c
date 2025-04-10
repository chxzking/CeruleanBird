/**
  ******************************************************************************
  * @file    Mistink_Displayer.c
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   这个文件实现了显示器硬件和显示器实例有关的操作
  ******************************************************************************
  */
	
	
/* Includes ------------------------------------------------------------------*/	
#include "Mistink_Com.h"

//动态内存库
#include "heap_solution_1.h"

//为了兼容第三方动态内存库做的接口
#define malloc(size)			c_malloc(size)		
#define free(ptr)					c_free(ptr)		

#define Mistink_NULL 			((void*)0)

/* 函数实现 ------------------------------------------------------------------*/	
/**
  * @brief  实例化一个显示器对象
  * @param  Pixel_Row: 定义显示器行的像素个数
  * @param  Pixel_Col: 定义显示器列的像素个数
	* @param  MistinkPlatformDrivers: 为实例添加驱动组，它的定义可以在此处查看 @ref MistinkPltfrmDrv_t
  * @retval 实例化成功返回对象地址，实例化失败返回空地址
  */
MistinkDisplayer_t* MistinkDisplayer_Create(Buffer_t Pixel_Row,Buffer_t Pixel_Col,const MistinkPltfrmDrv_t* MistinkPlatformDrivers){
		memory_pool_init();
		//计算逻辑实际数组的行
		Buffer_t R_Row = Pixel_Row / 8;
		if(Pixel_Row % 8 != 0){
				R_Row++;
		}
		
		//创建显示器实例
		MistinkDisplayer_t* MistinkDisplayer = (MistinkDisplayer_t*)malloc(sizeof(MistinkDisplayer_t) + sizeof(unsigned char) * R_Row * Pixel_Col); 
		if(MistinkDisplayer == Mistink_NULL){//创建失败
				return MistinkDisplayer;
		}
		//初始化
		MistinkDisplayer->Pixel_Row = Pixel_Row;
		MistinkDisplayer->Pixel_Col = Pixel_Col;
		MistinkDisplayer->real_Row = R_Row;
		//硬件驱动写入
		memcpy(&MistinkDisplayer->MistinkPlatformDrivers,MistinkPlatformDrivers,sizeof(MistinkPltfrmDrv_t));
		//缓冲区定位
		MistinkDisplayer->buffer = ((unsigned char*)MistinkDisplayer) + sizeof(MistinkDisplayer_t);
		
		//硬件初始化
		(MistinkDisplayer->MistinkPlatformDrivers.__Init)();
		return MistinkDisplayer;
}

/**
  * @brief  销毁一个显示器对象
  * @param  displayer: 显示器对象
  * @retval none
  */
void MistinkDisplayer_Free(MistinkDisplayer_t* displayer){
		free(displayer);
		return;
}

/**
  * @brief  清除显示器显示的内容
  * @param  displayer: 显示器对象
  * @retval none
  */
void MistinkDisplayer_Clear(MistinkDisplayer_t* displayer){
	(displayer->MistinkPlatformDrivers.__Clear)();
	return;
}
/**
  * @brief  将缓存区的数据输出到屏幕上
  * @param  displayer: 显示器对象
  * @retval none
  */
void MistinkDisplayer_BufferOutput(MistinkDisplayer_t* displayer){
	(displayer->MistinkPlatformDrivers.__BufferOutput)(displayer->buffer,displayer->real_Row,displayer->Pixel_Col);
	return;
}
