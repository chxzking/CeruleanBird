#ifndef __ENGINE_CONFIG_H__
#define __ENGINE_CONFIG_H__
#include <stdint.h>

/****************************
*	��Ļ�ֱ�������
*	SCREEN_ROW	-->	��Ļ����
*	SCREEN_COLUMN	-->	��Ļ����
*	ScreenType	-->	���ڴ�����Ļ����������
*****************************/
#define SCREEN_ROW 64								//��Ļ����
#define SCREEN_COLUMN 128							//��Ļ����
typedef unsigned char ScreenType;					//���� ����"��Ļ����"����		




/****************************
*	���ڴ�ģʽѡ��
*	MEMORY_MODE_1	-->	��Ļ����
*	MEMORY_MODE_2	-->	��Ļ����
*	ScreenType	-->	���ڴ�����Ļ����������
*****************************/
#define MEMORY_MODE_0		0						//�ٷ��⶯̬�ڴ�
#define MEMORY_MODE_1		1						//���涯̬�ڴ�
//����
#define HEAP_MEMORY_CONFIG		MEMORY_MODE_1		/*���ö�̬�ڴ棨Ĭ��ѡ��ģʽ2��*/

//[������ڴ�����]�����ʹ���ڴ淽��MEMORY_MODE_0�����������ûᱻ���ԡ�
#define	HEAP_POOL_SIZE		10240					//���ö��ڴ��С����λ���ֽڣ�



#define MOVABLE_AREA_VALUE			0									//���õ�ͼ�пյ�ֵ����ͼ�����ҽ���һ��ȷ���Ŀյ�ֵ��������������ƶ��ĵ�ͼֵ��Ҳ���ǿյص�ֵ���ɱ��޸ģ����ǲ����飬���Է���˽���ļ��У�
typedef double DeepBuffer_t;											//��Ȼ���������
#define POSITIVE_INFINITY		(*(DeepBuffer_t*)&(uint64_t){0x7FF0000000000000})	//��Ȼ�����������

#endif
