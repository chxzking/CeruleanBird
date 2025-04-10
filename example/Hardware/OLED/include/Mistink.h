#ifndef __MISTINK_H__
#define __MISTINK_H__
/**
  ******************************************************************************
  * @file    Mistink.h
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   库描述：此图形库名为“水墨晕染”，专有名词为Mistink ,即雾一般散开的墨水。
	*									 此图形库旨在实现基于显示器实例为核心的多屏操作，分离了驱动与上位
	*									 渲染，具有高移植性与自定义特性，此外支持同时操作多个不同协议不同
	*									 大小的屏幕。
	*					 文件描述：此文件中包含了整个库外部的API
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "Mistink_Config.h"


/*定义-----------------------------------------------------------------*/
/** 
  * @brief  显示器对象的声明
  */
typedef struct MistinkDisplayer_t MistinkDisplayer_t;

/** 
  * @brief  硬件驱动接口
  */
typedef struct MistinkPlatformDrivers_t{
	
	void (*__Init)(void);							 										/*!< 硬件初始化函数接口*/
	
	void (*__BufferOutput)(const unsigned char* buffer,\
				Buffer_t arrRow, Buffer_t arrCol);              /*!< 数据从内存缓冲区刷新到屏幕的接口*/
	
	void (*__Clear)(void);																/*!< 清除屏幕上的显示内容的接口*/
	
}MistinkPltfrmDrv_t;

/** 
  * @brief  像素点状态描述，打开或者关闭
  */
typedef enum PixelStatus_t{
	PixelClose,
	PixelOpen
}PixelStatus_t;



/*显示器操作-----------------------------------------------------------------*/
//创建一个显示器实例
MistinkDisplayer_t* MistinkDisplayer_Create(Buffer_t Pixel_Row,Buffer_t Pixel_Col,const MistinkPltfrmDrv_t* MistinkPlatformDrivers);
//销毁指定显示器实例
void MistinkDisplayer_Free(MistinkDisplayer_t* displayer);
//清除指定显示器屏幕显示
void MistinkDisplayer_Clear(MistinkDisplayer_t* displayer);
//将指定缓存区数据刷新到屏幕中
void MistinkDisplayer_BufferOutput(MistinkDisplayer_t* displayer);



/*像素点操作------------------------------------------------------------------*/
int MistinkPixel_Operate(MistinkDisplayer_t* displayer,Buffer_t __ROW,Buffer_t __Columns,PixelStatus_t status);
int MistinkPixel_RegisterOP(MistinkDisplayer_t* displayer,Buffer_t __ROW,Buffer_t __Columns,unsigned char Register_value);//以寄存器为单位操作像素点





/*缓冲区操作--------------------------------------------------------------------*/

//重置缓存区
void MistinkOP_BufferReset(MistinkDisplayer_t* displayer);
//返回缓冲区
unsigned char* MistinkOP_GetBufferAPI(MistinkDisplayer_t* displayer);

//加载图片，参数：（图片位置，图片的数据，图片的长宽）
int M_PictureLoad(MistinkDisplayer_t* displayer,Pos_t __ROW,Pos_t __Columns,const unsigned char* picture,PictureSize_t picture_length,PictureSize_t picture_width);
//字符串打印
void M_String(MistinkDisplayer_t* displayer, Pos_t __ROW,Pos_t __Columns,char* str);
//整数打印
void M_Int(MistinkDisplayer_t* displayer, Pos_t __ROW,Pos_t __Columns,int num);




/*图形与算法--------------------------------------------------------------------*/






#endif
