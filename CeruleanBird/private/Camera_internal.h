/*当前文件为二层塔*/
#ifndef __CAMERA_INTERNAL_H__
#define __CAMERA_INTERNAL_H__
#include "Camera_API.h"
//摄像机结构体定义
typedef struct Camera_Struct{
	//属性1
    double moveSpeed; 	// 移动速度
    double rotSpeed;  	// 旋转速度 每次旋转nPI的值
	//属性2
    vector position;    // 相机位置
    vector direction;   // 相机方向
	vector plane;		// 相机法平面
}Camera_Struct;

//渲染
void Camera_Render(Camera_Struct* camera);

#endif

