#ifndef __Camera_API_H__
#define __Camera_API_H__

#include "vector.h"
//#include <stdio.h>
#include "engine_config.h"

/*当前文件为一层塔*/

//摄像机结构体声明
typedef struct Camera_Struct Camera_Struct;



//摄像机初始化结构体
typedef struct Camera_InitTypeDef{
	double moveSpeed; 	// 移动速度
	
	double rotSpeed;  	// 旋转速度 （单位：弧度值n PI）
	
	vector position;    // 相机位置
	
	vector direction;   // 相机方向
	
	double FOV;			//相机视野 （单位：角度值）
	
}Camera_InitTypeDef;




/*地板操作接口--------------------------------------------------------------------------------------*/
int Floor_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray);//默认开启默认渲染模式
int Floor_SwitchDefaultReaderMode(Camera_Struct* camera);
int Floor_SwitchCustomReaderMode(Camera_Struct* camera);
int Floor_TextureAdd(int index,int Row,int Col,const unsigned char* texture);


/*天花板操作接口--------------------------------------------------------------------------------------*/
int Celi_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray);//默认开启默认渲染模式(即不渲染)
int Celi_SwitchDefaultReaderMode(Camera_Struct* camera);
int Celi_SwitchCustomReaderMode(Camera_Struct* camera);
int Celi_TextureAdd(int index,int Row,int Col,const unsigned char* texture);



/*顶层操作接口--------------------------------------------------------------------------------------*/
int SSRAnim_ManagerCreate(int value,int Row,int Col,double time);
int SSRAnim_FrmAdd(int value, int seq, const unsigned char* Frm,const unsigned char* shadow);
int SSRAnim_RenderEnable(Camera_Struct* camera);
int SSRAnim_RenderDisable(Camera_Struct* camera);
int SSRAnim_SetPos(Camera_Struct* camera,int ROW_Pixel,int COL_Pixel);
int SSR_SetCameraExecAnim(Camera_Struct* camera,int value);//设置value为顶层动画
int SSRAnim_ResetSingle(Camera_Struct* camera);
int SSRAnim_Update(Camera_Struct* camera);//更新下一帧
void SSRAnim_Exec(Camera_Struct* camera);//顶层渲染总线


/*摄像机操作接口--------------------------------------------------------------------------------------*/

//初始化相机
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct);

void Camera_Render(Camera_Struct* camera);
vector Camera_GetPos(Camera_Struct* camera);


/*******************
	【镜头移动】
********************/
//镜头左移
void Camera_LeftMove(Camera_Struct* camera);
//镜头右移
void Camera_RightMove(Camera_Struct* camera);
//镜头前移
void Camera_ForwardMove(Camera_Struct* camera);
//镜头后移
void Camera_BackMove(Camera_Struct* camera);
/*******************
	【镜头旋转】
********************/
//镜头左旋
void Camera_LeftPan(Camera_Struct* camera);
//镜头右旋
void Camera_RightPan(Camera_Struct* camera);
//视角左偏转
void Camera_LeftTilt(Camera_Struct* camera);
//视角右偏转
void Camera_RightTilt(Camera_Struct* camera);
#endif
