#ifndef __CERULEANBIRD_H__
#define __CERULEANBIRD_H__

//暂时定引擎名为 CeruleanBird 青鸟

//导入配置
#include "engine_config.h"
#include "Camera_API.h"
#include "Canvas_API.h"

//引擎初始化
//int CeruleanBird_Init(Camera_Struct* camera, Camera_InitTypeDef* Camera_InitStruct);
int CeruleanBird_Init(Camera_Struct** camera, Camera_InitTypeDef* Camera_InitStruct);
#endif
