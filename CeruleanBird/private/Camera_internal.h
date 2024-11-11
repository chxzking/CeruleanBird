/*��ǰ�ļ�Ϊ������*/
#ifndef __CAMERA_INTERNAL_H__
#define __CAMERA_INTERNAL_H__
#include "Camera_API.h"
//������ṹ�嶨��
typedef struct Camera_Struct{
	//����1
    double moveSpeed; 	// �ƶ��ٶ�
    double rotSpeed;  	// ��ת�ٶ� ÿ����תnPI��ֵ
	//����2
    vector position;    // ���λ��
    vector direction;   // �������
	vector plane;		// �����ƽ��
}Camera_Struct;

//��Ⱦ
void Camera_Render(Camera_Struct* camera);

#endif

