#ifndef __COMMON_H__
#define __COMMON_H__



#include "CeruleanBird.h"//����
#include "animation.h"//������
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "map.h"

#include "Mistink.h"

#include "M_stm32f103c8t6.h"

extern MistinkDisplayer_t* displayer;
extern int LongS_survivalCount;//����Զ�̵�������
extern int LongC_survivalCount;//���Ͻ��̵�������

Sprites_t* GetEnemy(vector Bullent_pos);//��ȡ����ײ���ĵ���
extern int enemyCore;


#endif

