#include "CeruleanBird.h"

//引擎初始化
int CeruleanBird_Init(Camera_Struct** camera, Camera_InitTypeDef* Camera_InitStruct){
	//画布初始化
	if(Canvas_Init() == -1) return -1;//画布初始化失败
	//摄像机初始化
	//if(Camera_Init(camera,Camera_InitStruct) == -1) return -1;//摄像机初始化失败
	*camera = Camera_Init(Camera_InitStruct);
	if(camera == 0)	return -1;
	
	return 0;//初始化完成
}
