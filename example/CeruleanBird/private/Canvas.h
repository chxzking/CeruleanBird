/*��ǰ�ļ�Ϊ������*/
#ifndef __CANVAS_H__
#define __CANVAS_H__
#include "Canvas_API.h"

//������������������һά������ά�߼���
int Canvas_Base(int ROW, int COLUMN, ScreenType data);
//��ȡ������һ����Ԫ�ĵ�ַ
ScreenType* Canvas_GetUnit(int ROW, int COLUMN);

//����ģʽ
#define DEFAULT_MODE		0		//Ĭ��ģʽ
#define REDIRECT_MODE		1		//�ض���ģʽ


//���û���ӳ�䷽ʽ

#endif
