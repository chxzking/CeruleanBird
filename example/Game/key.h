#ifndef __KEY_H__
#define __KEY_H__
#include "stm32f10x.h" 

typedef enum{
	K_Left,//��
	K_Right,//��
	K_Mid,//�м�
	K_Up,//��
	K_Down,//��
	K_Idle//����
}KeyValue_t;


void Key_Init(void);

KeyValue_t Get_Key(void);
#endif

