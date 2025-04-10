/*当前文件为三层塔*/
#ifndef __Camera_H__
#define __Camera_H__
#include "Camera_internal.h"
#include "RB_Tree_API.h"
#include "FrameRateStrategy_API.h"

#define M_PI 3.1415926535
//总渲染线
int FloorCeil_Render(Camera_Struct* camera);
/*地板--------------------------------------------------------------*/
extern RB_Node* FloorTreeManager;

//void Floor_DefaultRender(void);
void Floor_DefaultRender(Camera_Struct* camera);
FC_Node_t* Floor_GetTexture(int index);//提取索引对应的地板贴图信息
void Floor_TextureRender(Camera_Struct* camera);


/*天花板-----------------------------------------------------------*/
extern RB_Node* CeliTreeManager;
void Celi_DefaultRender(Camera_Struct* camera);
FC_Node_t* Celi_GetTexture(int index);//提取索引对应的天花板贴图信息
void Ceil_TextureRender(Camera_Struct* camera);


/*顶层渲染（屏幕空间渲染Screen space rendering）--------------------*/
extern RB_Node* SSR_CoreTree;

int SSRAnim_FrmExtract(SSR_Anim_t* anim,SSR_Frm_t* frm,int ROW_Pixel,int COL_Pixel);
void SSRAnim_Render(Camera_Struct* camera);

int FC_TextureExtract(FC_Node_t *FC_Node,int ROW_Pixel,int COL_Pixel);
#endif

