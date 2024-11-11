#include "Canvas.h"

#include "bit_operate.h"

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

/********************************
对于画布的数组目前有两种策略
策略1：使用引擎自带的画布，渲染完成
	   后将数据复制到外部缓存中。
策略2：允许开发者使用重定位画布的方
	   式，将数据直接渲染到外部缓存
*********************************/
int Canvas_Mode = DEFAULT_MODE;		//默认使用<默认模式>
//画布缓存区指针
ScreenType* Screen_Canvas = C_NULL;
/**
*	@brief	用于初始化画布，从堆区获取指定大小的空间
*	@param	无参
*	@retval	空间申请成功，返回0，如果失败返回-1；
*/
int Canvas_Init(void){
	//从堆区获取画布空间
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //启用引擎动态内存解决方案
		//此方案需要调用初始化函数初始化堆区
		memory_pool_init();
	#endif
	//默认模式
	if(Canvas_Mode == DEFAULT_MODE){
		Screen_Canvas = (ScreenType*)malloc( (SCREEN_ROW/8) * SCREEN_COLUMN * sizeof(ScreenType));
		if(Screen_Canvas == NULL) return -1;
		//画布值置0
		for(int i = 0; i < (SCREEN_ROW/8) * SCREEN_COLUMN;i++){
			Screen_Canvas[i] = 0;
		}
		return 0;
	}
	//重定向模式
	return 0;//重定向的会在重定向的时候初始化
}
/**
*	@brief	画布基础函数，用于一维画布二维逻辑化
*	@param	ROW --> 画布的行
*			COLUMN --> 画布的列
*			data	-->	写入的值。
*	@note 注意ROW与COLUMN均遵循,数组访问规则，例如(1,2)，实际访问的是第2行第3个元素。
*	@retval	写入画布成功，返回0，如果失败返回-1会出现溢出检查；
*/
int Canvas_Base(int ROW, int COLUMN, ScreenType data) {
	//校验行溢出
	if (ROW >= SCREEN_ROW || ROW < 0)	return -1;
	//校验列溢出
	if (COLUMN >= SCREEN_COLUMN || COLUMN < 0)	return -1;
	//二维化写入数据
	Screen_Canvas[ROW * SCREEN_COLUMN + COLUMN] = data;
	//返回成功标志
	return 0;
}
/**
*	@brief	获取画布一个指定位置元素的地址
*	@param	ROW --> 画布的行
*			COLUMN --> 画布的列
*	@note 注意ROW与COLUMN均遵循,数组访问规则，例如(1,2)，实际访问的是第2行第3个元素。
*	@retval	写入画布成功，返回地址，如果失败返回空；
*/
ScreenType* Canvas_GetUnit(int ROW, int COLUMN){
	//校验行溢出
	if (ROW >= SCREEN_ROW || ROW < 0)	return NULL;
	//校验列溢出
	if (COLUMN >= SCREEN_COLUMN || COLUMN < 0)	return NULL;
	//返回
	return &Screen_Canvas[ROW * SCREEN_COLUMN + COLUMN];
}
/**
*	@brief	在画布上进行像素点级别的操作
*	@param	Pixel_ROW --> 画布的行
*			Pixel_COL --> 画布的列
*			OperateNum -->	像素点的状态
*	@retval	none；
*/
void Canvas_PixelOperate(int Pixel_ROW,int Pixel_COL,CANVAS_PIXEL_STATUS OperateNum){
	//检测像素点的行方向溢出
	if(Pixel_ROW < 0 || Pixel_ROW >= SCREEN_ROW)	return;
	//检测像素点的列方向溢出
	if(Pixel_COL < 0 || Pixel_COL >= SCREEN_COLUMN)		return;
	
	//像素点定位
	int PosByteX = Pixel_ROW/8;//获取真实的行
	int PosbitX =  Pixel_ROW%8;//定位到位的位置
	//获取目标字节的地址
	ScreenType* temp = Canvas_GetUnit(PosByteX, Pixel_COL);
	//操作该字节为灭
	if(OperateNum == CANVAS_PIXEL_CLOSE){
		Bit_Modify_Within_Byte(temp, 7-PosbitX, 0);
	}
	//操作该字节为亮
	else if(OperateNum == CANVAS_PIXEL_OPEN){
		Bit_Modify_Within_Byte(temp, 7-PosbitX, 1);
	}
	return;
}
/**
*	@brief	画布刷新,将之前的画布信息全部清楚
*	@param	none
*	@retval	none
*/
void Canvas_Refresh(void){
	for(int i = 0;i<(SCREEN_ROW/8) * SCREEN_COLUMN;i++){
		Screen_Canvas[i] = 0;
	}
}
/**
*		@note	注意：重定向必须发生在画布初始化之前
*	@brief	设置画布模式为重定向模式
*	@param	NewCanvasBase -->	重定向的缓存
*	@retval	如果在初始化之后调用，会返回错误值-1，如果执行成功则会返回0
*/
int Canvas_SetCanvasModeToRedirect(unsigned char* NewCanvasBuffer){
	//检测画布是否已经初始化
	if(Screen_Canvas != C_NULL){
		return -1;//如果已经初始化过了，那么就返回错误
	}
	Canvas_Mode = REDIRECT_MODE;
	Screen_Canvas = NewCanvasBuffer;
	return 0;
}
/**
*		@note	注意，如果使用了重定向策略，重定向的是动态缓存，函数并不会去释放这个缓存
*	@brief	画布缓存空间释放
*	@param	none
*	@retval	none
*/
void Canvas_Free(void){
	//默认策略
	if(Canvas_Mode == DEFAULT_MODE){
		free(Screen_Canvas);
		Screen_Canvas = C_NULL;
		return;
	}
	//重定向策略
	//重置策略
	Canvas_Mode = DEFAULT_MODE;
	return;
}
