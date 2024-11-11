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
	double moveSpeed; // 移动速度
	
	double rotSpeed;  // 旋转速度 （单位：弧度值n PI）
	
	vector position;    // 相机位置
	
    vector direction;   // 相机方向
	
	double FOV;			//相机视野 （单位：角度值）
	
}Camera_InitTypeDef;




//初始化相机
//int Camera_Init(Camera_Struct* camera, Camera_InitTypeDef* Camera_InitStruct);
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct);





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
