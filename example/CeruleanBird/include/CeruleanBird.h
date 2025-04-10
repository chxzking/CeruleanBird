#ifndef __CERULEANBIRD_H__
#define __CERULEANBIRD_H__


//导入配置
#include "engine_config.h"//配置接口
#include "Camera_API.h"//摄像机接口
#include "Canvas_API.h"//画布接口
#include "FrameRateStrategy_API.h"//帧率策略接口
#include "TextureMapper_API.h"//纹理映射接口
#include "WorldMap_API.h"//地图接口
#include "Sprites_API.h"//精灵接口
#include "heap_solution_1.h"//动态内存接口
#include "EngineFault.h"	//引擎错误紧急阻隔
#include "vector.h"//向量库

//引擎渲染（这个函数是一个可选项，它将引擎的渲染总线进行了二次封装以方便统一渲染）
void* CeruleanBird_EngineRender(Camera_Struct* camera);
#endif
