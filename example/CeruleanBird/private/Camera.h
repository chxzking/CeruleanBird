/*��ǰ�ļ�Ϊ������*/
#ifndef __Camera_H__
#define __Camera_H__
#include "Camera_internal.h"
#include "RB_Tree_API.h"
#include "FrameRateStrategy_API.h"

#define M_PI 3.1415926535
//����Ⱦ��
int FloorCeil_Render(Camera_Struct* camera);
/*�ذ�--------------------------------------------------------------*/
extern RB_Node* FloorTreeManager;

//void Floor_DefaultRender(void);
void Floor_DefaultRender(Camera_Struct* camera);
FC_Node_t* Floor_GetTexture(int index);//��ȡ������Ӧ�ĵذ���ͼ��Ϣ
void Floor_TextureRender(Camera_Struct* camera);


/*�컨��-----------------------------------------------------------*/
extern RB_Node* CeliTreeManager;
void Celi_DefaultRender(Camera_Struct* camera);
FC_Node_t* Celi_GetTexture(int index);//��ȡ������Ӧ���컨����ͼ��Ϣ
void Ceil_TextureRender(Camera_Struct* camera);


/*������Ⱦ����Ļ�ռ���ȾScreen space rendering��--------------------*/
extern RB_Node* SSR_CoreTree;

int SSRAnim_FrmExtract(SSR_Anim_t* anim,SSR_Frm_t* frm,int ROW_Pixel,int COL_Pixel);
void SSRAnim_Render(Camera_Struct* camera);

int FC_TextureExtract(FC_Node_t *FC_Node,int ROW_Pixel,int COL_Pixel);
#endif

