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
	double moveSpeed; 	// �ƶ��ٶ�
	
	double rotSpeed;  	// ��ת�ٶ� ����λ������ֵn PI��
	
	vector position;    // ���λ��
	
	vector direction;   // �������
	
	double FOV;			//�����Ұ ����λ���Ƕ�ֵ��
	
}Camera_InitTypeDef;




/*�ذ�����ӿ�--------------------------------------------------------------------------------------*/
int Floor_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray);//Ĭ�Ͽ���Ĭ����Ⱦģʽ
int Floor_SwitchDefaultReaderMode(Camera_Struct* camera);
int Floor_SwitchCustomReaderMode(Camera_Struct* camera);
int Floor_TextureAdd(int index,int Row,int Col,const unsigned char* texture);


/*�컨������ӿ�--------------------------------------------------------------------------------------*/
int Celi_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray);//Ĭ�Ͽ���Ĭ����Ⱦģʽ(������Ⱦ)
int Celi_SwitchDefaultReaderMode(Camera_Struct* camera);
int Celi_SwitchCustomReaderMode(Camera_Struct* camera);
int Celi_TextureAdd(int index,int Row,int Col,const unsigned char* texture);



/*��������ӿ�--------------------------------------------------------------------------------------*/
int SSRAnim_ManagerCreate(int value,int Row,int Col,double time);
int SSRAnim_FrmAdd(int value, int seq, const unsigned char* Frm,const unsigned char* shadow);
int SSRAnim_RenderEnable(Camera_Struct* camera);
int SSRAnim_RenderDisable(Camera_Struct* camera);
int SSRAnim_SetPos(Camera_Struct* camera,int ROW_Pixel,int COL_Pixel);
int SSR_SetCameraExecAnim(Camera_Struct* camera,int value);//����valueΪ���㶯��
int SSRAnim_ResetSingle(Camera_Struct* camera);
int SSRAnim_Update(Camera_Struct* camera);//������һ֡
void SSRAnim_Exec(Camera_Struct* camera);//������Ⱦ����


/*����������ӿ�--------------------------------------------------------------------------------------*/

//��ʼ�����
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct);

void Camera_Render(Camera_Struct* camera);
vector Camera_GetPos(Camera_Struct* camera);


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
