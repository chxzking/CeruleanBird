#include "CeruleanBird.h"

//�����ʼ��
int CeruleanBird_Init(Camera_Struct** camera, Camera_InitTypeDef* Camera_InitStruct){
	//������ʼ��
	if(Canvas_Init() == -1) return -1;//������ʼ��ʧ��
	//�������ʼ��
	//if(Camera_Init(camera,Camera_InitStruct) == -1) return -1;//�������ʼ��ʧ��
	*camera = Camera_Init(Camera_InitStruct);
	if(camera == 0)	return -1;
	
	return 0;//��ʼ�����
}
