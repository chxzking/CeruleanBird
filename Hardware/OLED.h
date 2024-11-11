#ifndef __OLED_H
#define __OLED_H
#include "stm32f10x.h"  
#include <stdio.h>
//像素点打开或者关闭
typedef enum BOLT{
	CLOSE,
	OPEN
}BOLT;

/******硬件操作*******/
void OLED_Init(void);//初始化
void OLED_Clear(void);//清除屏幕显示
void Buffer_Output(void);//打印缓存区内容
/******缓存区操作******/

//操控位置像素点(横坐标（0-63），纵坐标（0-127）)
int Pixel_Operate(unsigned char __ROW,unsigned char __Columns,BOLT Bolt);
//给指定显存赋值(横坐标（0-7），纵坐标（0-127）)
int Display_register(unsigned char __ROW,unsigned char __Columns,unsigned char Register_value);
//刷新缓存区
int Buffer_Refresh(void);

//原子操作，将缓存区的值输出到屏幕上
void print(void);
//加载图片，参数：（图片位置，图片的数据，图片的长宽）
int Picture_Loading(signed char __ROW,signed char __Columns,unsigned char* picture,unsigned char picture_length,unsigned char picture_width);
//矩形
void Rectangle(signed char ROW,signed char Columns,unsigned char length, unsigned char width);

//字模读取
int Font_Read(unsigned char ROW,unsigned char Columns,char serial_numbers);
//字符串打印
void Hj_String(signed char __ROW,signed char __Columns,char* str);
//整数打印
void Hj_Int(signed char __ROW,signed char __Columns,int num);
//缓存区接口
unsigned char* GetBufferAPI(void);
#endif
