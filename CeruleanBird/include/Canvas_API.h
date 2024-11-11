#ifndef __CANVAS_H_API__
#define __CANVAS_H_API__
/*��ǰ�ļ�Ϊһ����*/
#include "engine_config.h"
#include <stdio.h>

//���ص�ö��
typedef enum CANVAS_PIXEL_STATUS{
	CANVAS_PIXEL_CLOSE = 0,			//�ر����ص�
	CANVAS_PIXEL_OPEN			//�������ص�
}CANVAS_PIXEL_STATUS;

/**
*	@brief	���ڳ�ʼ���������Ӷ�����ȡָ����С�Ŀռ�
*	@param	�޲�
*	@retval	�ռ�����ɹ�������0�����ʧ�ܷ���-1��
*/
int Canvas_Init(void);
/**
*	@brief	�ڻ����Ͻ������ص㼶��Ĳ���
*	@param	Pixel_ROW --> ��������
*			Pixel_COL --> ��������
*			OperateNum -->	���ص��״̬
*	@retval	none��
*/
void Canvas_PixelOperate(int Pixel_ROW,int Pixel_COL,CANVAS_PIXEL_STATUS OperateNum);
/**
*	@brief	����ˢ��,��֮ǰ�Ļ�����Ϣȫ�����
*	@param	none
*	@retval	none
*/
void Canvas_Refresh(void);
/**
*		@note	ע�⣺�ض�����뷢���ڻ�����ʼ��֮ǰ
*	@brief	���û���ģʽΪ�ض���ģʽ
*	@param	NewCanvasBase -->	�ض���Ļ���
*	@retval	����ڳ�ʼ��֮����ã��᷵�ش���ֵ-1�����ִ�гɹ���᷵��0
*/
int Canvas_SetCanvasModeToRedirect(unsigned char* NewCanvasBuffer);
/**
*		@note	ע�⣬���ʹ�����ض�����ԣ��ض�����Ƕ�̬���棬����������ȥ�ͷ��������
*	@brief	��������ռ��ͷ�
*	@param	none
*	@retval	none
*/
void Canvas_Free(void);
#endif
