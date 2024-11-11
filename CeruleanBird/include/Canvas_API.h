#ifndef __CANVAS_H_API__
#define __CANVAS_H_API__
/*当前文件为一层塔*/
#include "engine_config.h"
#include <stdio.h>

//像素点枚举
typedef enum CANVAS_PIXEL_STATUS{
	CANVAS_PIXEL_CLOSE = 0,			//关闭像素点
	CANVAS_PIXEL_OPEN			//开启像素点
}CANVAS_PIXEL_STATUS;

/**
*	@brief	用于初始化画布，从堆区获取指定大小的空间
*	@param	无参
*	@retval	空间申请成功，返回0，如果失败返回-1；
*/
int Canvas_Init(void);
/**
*	@brief	在画布上进行像素点级别的操作
*	@param	Pixel_ROW --> 画布的行
*			Pixel_COL --> 画布的列
*			OperateNum -->	像素点的状态
*	@retval	none；
*/
void Canvas_PixelOperate(int Pixel_ROW,int Pixel_COL,CANVAS_PIXEL_STATUS OperateNum);
/**
*	@brief	画布刷新,将之前的画布信息全部清楚
*	@param	none
*	@retval	none
*/
void Canvas_Refresh(void);
/**
*		@note	注意：重定向必须发生在画布初始化之前
*	@brief	设置画布模式为重定向模式
*	@param	NewCanvasBase -->	重定向的缓存
*	@retval	如果在初始化之后调用，会返回错误值-1，如果执行成功则会返回0
*/
int Canvas_SetCanvasModeToRedirect(unsigned char* NewCanvasBuffer);
/**
*		@note	注意，如果使用了重定向策略，重定向的是动态缓存，函数并不会去释放这个缓存
*	@brief	画布缓存空间释放
*	@param	none
*	@retval	none
*/
void Canvas_Free(void);
#endif
