#ifndef __COMMON_H__
#define __COMMON_H__



#include "CeruleanBird.h"//引擎
#include "animation.h"//动画库
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "map.h"

#include "Mistink.h"

#include "M_stm32f103c8t6.h"

extern MistinkDisplayer_t* displayer;
extern int LongS_survivalCount;//场上远程敌人数量
extern int LongC_survivalCount;//场上近程敌人数量

Sprites_t* GetEnemy(vector Bullent_pos);//获取被碰撞到的敌人
extern int enemyCore;


#endif

