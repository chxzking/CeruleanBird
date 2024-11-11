#ifndef __ENGINE_CONFIG_H__
#define __ENGINE_CONFIG_H__


/****************************
*	屏幕分辨率设置
*	SCREEN_ROW	-->	屏幕的行
*	SCREEN_COLUMN	-->	屏幕的列
*	ScreenType	-->	用于代表屏幕的数据类型
*****************************/
#define SCREEN_ROW 64								//屏幕的行
#define SCREEN_COLUMN 128							//屏幕的列
typedef unsigned char ScreenType;					//定义 配置"屏幕变量"类型		




/****************************
*	堆内存模式选择
*	MEMORY_MODE_1	-->	屏幕的行
*	MEMORY_MODE_2	-->	屏幕的列
*	ScreenType	-->	用于代表屏幕的数据类型
*****************************/
#define MEMORY_MODE_0		0						//官方库动态内存
#define MEMORY_MODE_1		1						//引擎动态内存
//配置
#define HEAP_MEMORY_CONFIG		MEMORY_MODE_1		/*配置动态内存（默认选择模式2）*/

//[引擎堆内存配置]，如果使用内存方案MEMORY_MODE_0，则以下配置会被忽略。
#define	HEAP_POOL_SIZE		10240					//配置堆内存大小（单位：字节）







#endif
