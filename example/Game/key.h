#ifndef __KEY_H__
#define __KEY_H__
#include "stm32f10x.h" 

typedef enum{
	K_Left,//左
	K_Right,//右
	K_Mid,//中间
	K_Up,//上
	K_Down,//下
	K_Idle//空置
}KeyValue_t;


void Key_Init(void);

KeyValue_t Get_Key(void);
#endif

