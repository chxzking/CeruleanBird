#ifndef __PISTOL_H__
#define __PISTOL_H__

/*��ǹ��*/
#include "Common.h"

typedef struct bulletHead_t bulletHead_t;
typedef struct bulletStatus_t bulletStatus_t;
typedef struct bullet_t bullet_t;
typedef struct bulletPrivate_t bulletPrivate_t;

struct bulletHead_t{
	//�µ��뷨
	int MaxCount;//���������ӵ����������
	
	//����
	int usableCount;//��ʹ�õ��ӵ���������
	bullet_t* usableLink;//��ʹ�õ���
	
	//��������
	int runningCount;//�������е��ӵ�����
	bullet_t* runningLink;//�������е���
	
};


struct bullet_t{
	vector pos;//����λ��
	Sprites_t* sprite;//�ӵ����鱾��
	struct bullet_t* next;//��һ���ڵ�
	struct bullet_t* previous;//��һ���ڵ�
};

//�ӵ���˽������
struct bulletPrivate_t{
	struct bullet_t* bullet;//�ӵ�����
	Camera_Struct* camera;//�����
	int isShow;//�Ƿ���ʾ
};



int pistol_Init(int count,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double Speed);

int pistol_Fire(Camera_Struct* camera);
#endif 

