/*当前文件为二层塔*/
#ifndef __CANVAS_H__
#define __CANVAS_H__
#include "Canvas_API.h"

//画布基础函数，用于一维画布二维逻辑化
int Canvas_Base(int ROW, int COLUMN, ScreenType data);
//获取画布的一个单元的地址
ScreenType* Canvas_GetUnit(int ROW, int COLUMN);

//画布模式
#define DEFAULT_MODE		0		//默认模式
#define REDIRECT_MODE		1		//重定向模式


//设置画布映射方式

#endif
