#ifndef __Camera_API_H__
#define __Camera_API_H__

#include "vector.h"
//#include <stdio.h>
#include "engine_config.h"

/*��ǰ�ļ�Ϊһ����*/

//������ṹ������
typedef struct Camera_Struct Camera_Struct;



//�������ʼ���ṹ��
typedef struct Camera_InitTypeDef{
	double moveSpeed; // �ƶ��ٶ�
	
	double rotSpeed;  // ��ת�ٶ� ����λ������ֵn PI��
	
	vector position;    // ���λ��
	
    vector direction;   // �������
	
	double FOV;			//�����Ұ ����λ���Ƕ�ֵ��
	
}Camera_InitTypeDef;




//��ʼ�����
//int Camera_Init(Camera_Struct* camera, Camera_InitTypeDef* Camera_InitStruct);
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct);





/*******************
	����ͷ�ƶ���
********************/
//��ͷ����
void Camera_LeftMove(Camera_Struct* camera);
//��ͷ����
void Camera_RightMove(Camera_Struct* camera);
//��ͷǰ��
void Camera_ForwardMove(Camera_Struct* camera);
//��ͷ����
void Camera_BackMove(Camera_Struct* camera);
/*******************
	����ͷ��ת��
********************/
//��ͷ����
void Camera_LeftPan(Camera_Struct* camera);
//��ͷ����
void Camera_RightPan(Camera_Struct* camera);
//�ӽ���ƫת
void Camera_LeftTilt(Camera_Struct* camera);
//�ӽ���ƫת
void Camera_RightTilt(Camera_Struct* camera);
#endif
