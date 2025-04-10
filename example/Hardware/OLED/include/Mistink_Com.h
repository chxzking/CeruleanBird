#ifndef __MISTINK_COM_H__
#define __MISTINK_COM_H__
/**
  ******************************************************************************
  * @file    Mistink_Com.h
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   这个文件中是库的内部公共头文件，包含了库内部封装的信息
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/	
#include "Mistink.h"
#include "Mistink_Font.h"
//标准库
#include <math.h>
#include <string.h>
#include <stdio.h>


/* 定义 ----------------------------------------------------------------------*/

/** 
  * @brief  整个是对显示器设备描述的结构体，包含了显示器信息、硬件驱动接口、内存缓存区 
  */
struct MistinkDisplayer_t{

	Buffer_t Pixel_Row;													/*!< 显示器的逻辑行数（以像素个数为单位）*/
	
	Buffer_t Pixel_Col;													/*!< 显示器的逻辑列数（以像素个数为单位）*/
	
	Buffer_t real_Row;													/*!< 显示器缓存的实际行数，即缓存数组的行数（以字节为单位）*/
	
	MistinkPltfrmDrv_t MistinkPlatformDrivers;	/*!< 硬件驱动，它的成员定义在 @ref MistinkPltfrmDrv_t */
	
	unsigned char* buffer;											/*!< 显示器的缓存区地址，它指向了屏幕缓存区，出于cache命中率考虑，
																										缓存区跟随结构体一起申请，并以不可见形式隐藏在结构体尾部，
																										buffer指针将指向此空间，物理结构如下：
																										+---------------------------+
																										|		MistinkDisplayer_t			|
																										+---------------------------+
																										|				buffer ptr					| --+
																										+---------------------------+		|
																										|				buffer mem					|	<-+
																										+---------------------------+
	*/
};



/*内部API-----------------------------------------------------------------*/

//字模读取
int Font_Read(unsigned char ROW,unsigned char Columns,char serial_numbers);
//修改指定字节的位
int Bit_Modification(unsigned char* __format,unsigned char __serial__number,PixelStatus_t __binary_number);
//参数（数据指针,序号），这个函数聚焦于某一个字节，用于输出指定字节中的指定位的值，如果函数执行失败会返回-1
signed char Bit_loading(const unsigned char* __format,unsigned char __serial__number);


#endif
