#include "Camera.h"

#include <math.h>

#include "TextureMapper_internal.h"//纹理内部接口

#include "Canvas_API.h"			//画布接口

#include "WorldMap_internal.h"	//地图内部接口


#if HEAP_MEMORY_CONFIG == MEMORY_MODE_0          //启用标准库堆解决方案
#include <malloc.h>

#define C_NULL NULL

#endif	//启用标准库堆解决方案

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //启用引擎动态内存解决方案
#include "heap_solution_1.h"

//宏定义统一函数接口
#define malloc(size) c_malloc(size)
#define realloc(ptr,size) c_realloc(ptr,size)
#define free(ptr)	c_free(ptr)
#endif	//启用引擎动态内存解决方案

#define UNIT_PLANE_VERTICAL			0								//单元格的纵向面（上下面）
#define	UNIT_PLANE_HORIZONTAL		1								//单元格的水平面（左右面）



//初始化相机（测试通过）
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct){
	//检查输入
	if(Camera_InitStruct == C_NULL) return C_NULL;
	if(Camera_InitStruct->FOV <= 0 || Camera_InitStruct->FOV >= 180) return C_NULL;//限制摄像头的视角在0度到180度之间，不包括0和180度
	 
	Camera_Struct* camera = (Camera_Struct*)malloc(sizeof(Camera_Struct));
	if(camera == C_NULL)	return C_NULL;
	//用户定义属性赋值
	//有限处理简易的赋值
	camera->moveSpeed = Camera_InitStruct->moveSpeed;
	camera->rotSpeed = Camera_InitStruct->rotSpeed;
	camera->position = Camera_InitStruct->position;
	camera->direction = vector_Normalize(&Camera_InitStruct->direction);//归一化方向向量
	
	//处理FOV转化为平面法向量
	double FOV_Radians = Camera_InitStruct->FOV * (M_PI / 180.0);// 将FOV从度数转换为弧度
	//【以下函数通过精度验证，精度至少在0.0001层次】
	double planeX = camera->direction.y * tan(FOV_Radians / 2);
	double planeY = -camera->direction.x * tan(FOV_Radians / 2);
	camera->plane = vector_Create(planeX,planeY);
	//处理后的法平面向量的长度只有既定长度的一半，即保持FOV需要10单位长度，但是法平面长度为5，
	//并且法平面方向为方向向量西侧延伸
	//上述处理方法可以在计算的时候，将向量反向计算另外一半视角。
	return camera;
}


//





//单帧摄像机视角基底模型渲染
void Camera_Render(Camera_Struct* camera){
	//获取地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	
	//逐列渲染
	for(int x = 0; x < SCREEN_COLUMN; x++){
		//画面映射到摄像机视角,即为[-1,1]的偏移量【范围映射正常，测试范围处于-1到1之间，但映射不完全为线性，有的存在1的步进有的存在2的步进，推测可能是误差导致】
		double VisualAngleOffset =  x * 2 /(double)(SCREEN_COLUMN - 1) - 1;//
		
		
		//光线修正【修正似乎存在问题，按照66度视野的推测光线分量应该不会大于1，但是存在大于1的情况】
		double RayX = camera->plane.x * VisualAngleOffset + camera->direction.x;//x方向的光线分量
		double RayY = camera->plane.y * VisualAngleOffset + camera->direction.y;//y方向的光纤分量
		

		
		//设置X方向步进长度
		double stepLengthX = fabs(1 / RayX);
		//设置Y方向步进长度
		double stepLengthY = fabs(1 / RayY);
		//步进长度过小则跳过本次渲染
		if(stepLengthX < 1e-9 || stepLengthY < 1e-9){
			continue;	//将极小值1e-9看作0的界限，小于这个界限则跳过绘制
		}

		
		//设置x侧基准距离
		double standardDistanceX;
		//设置y侧基准距离
		double standardDistanceY; 

		//定义X轴的步进方向
		int stepDirX;
		//定义Y轴的步进方向
		int stepDirY;
		
		//光线DDA运算后坐落的单元格
		int UnitX = (int)camera->position.x;
		int UnitY = (int)camera->position.y;
		
		//X轴方向设置初始值和步进
		if(RayX >= 0){
			//光线正向延伸，步进方向为正
			stepDirX = 1;
			//通过相似三角形设置首个当前单元中X侧基准距离
			standardDistanceX = (UnitX + 1 - camera->position.x) * stepLengthX;
		}else {
			//光线负向延伸，步进方向为负
			stepDirX = -1;
			//通过相似三角形设置首个当前单元中X侧基准距离
			standardDistanceX = (camera->position.x - UnitX) * stepLengthX;
		}
		
		//Y轴方向设置初始值和步进
		if(RayY >= 0){
			//光线正向延伸，步进方向为正
			stepDirY = 1;
			//通过相似三角形设置首个当前单元中Y侧基准距离
			standardDistanceY = (UnitY + 1 - camera->position.y) * stepLengthY;	
		}else {
			//光线负向延伸，步进方向为负
			stepDirY = -1;
			//通过相似三角形设置首个当前单元中Y侧基准距离
			standardDistanceY = (camera->position.y - UnitY) * stepLengthY;
		}
		
		//定义单元格属性
		int UnitAttribute = 0;
		//定义单元格被光线击中的方向
		char UnitDir = UNIT_PLANE_VERTICAL;
		//单列光线扫描【障碍识别正常，扫描终点都停留在了预期的位置】
		while(UnitAttribute == 0){
			//检查并选择基准距离小的一侧
			if(standardDistanceX < standardDistanceY){
				standardDistanceX += stepLengthX;
				UnitX += stepDirX;//设置方格行方向的落点
				//光线从X侧击中单元格，单元格被击中方向设置竖直方向
				UnitDir = UNIT_PLANE_VERTICAL;
			}else{
				standardDistanceY += stepLengthY;
				UnitY += stepDirY;//设置方格列方向的落点落点
				//光线从Y侧击中障碍，单元格被击中方向设置为水平方向
				UnitDir = UNIT_PLANE_HORIZONTAL;
			}
			//光线击中障碍后返回障碍的属性
			UnitAttribute = WorldMap_Extract(WorldMapData,UnitX,UnitY);
							
		}
		
		
		
		//光线探测会射入墙内，所以需要回退一次修正距离。
		//【似乎存在异常，总体上距离按照视野越近，距离越短，但是存在个别情况又小数级别的增长，可能是误差导致】
		double RealDistance; 
		if (UnitDir == UNIT_PLANE_VERTICAL) RealDistance = (standardDistanceX - stepLengthX);
        else RealDistance = (standardDistanceY - stepLengthY);
		
		
		//获取建筑纹理资源(纹理通过检测，能正确读取到纹理数组的数据)
		TextureMapperDef* texture = Texture_GetTexture(UnitAttribute);
		
		/*纹理映射*/
		//被光线击中的点的绝对y坐标
		double wallColpos;
		if(UnitDir == UNIT_PLANE_HORIZONTAL){//如果光线击中方向是水平方向 
			wallColpos = camera->position.x + RayX * RealDistance;
		}else{//如果光线击中方向是垂直方向 
			wallColpos = camera->position.y + RayY * RealDistance;
		}
		//【出现巨大异常，wallColpos的值并不变化，理论上应该是线性变化】
		//【日志2：目前修复异常，此前的异常是由于光线射入的方向为垂直（或水平），但是以水平（垂直）的方式进行了计算】
		//转化为被光线击中的点的相对y坐标	
		wallColpos -= (int)wallColpos;
		//【出现巨大异常，纹理y坐标的映射值并不变化，理论上应该是线性变化】
		//【日志2：目前已修复异常，产生原因，wallColpos的值存在问题。】
		//纹理y轴映射
		int Texture_Y;
		//光线从单元格垂直方向射入
		if(UnitDir == UNIT_PLANE_VERTICAL){
			//如果X是从正方向射入
			if(RayX < 0){
				Texture_Y = (texture->Column - 1) * wallColpos;
			}else{
				Texture_Y = (texture->Column - 1) * (1 - wallColpos);
			}
		}
		//光线从单元格水平方向射入
		else{
			if(RayY >= 0){
				Texture_Y = (texture->Column - 1) * wallColpos;
			}else{
				Texture_Y = (texture->Column - 1) * (1 - wallColpos);
			}
		}
		
		
		
		
		/**
		* 注意：以下的渲染均以但列中心点为基准，如果中心点出现上下移动会导致图像上下移动
		*
		**/
		//渲染中心点
		int RenderCenter = SCREEN_ROW / 2 - 1;
		
		//获取墙的高度
		int WallHight = SCREEN_ROW	/ RealDistance;
		if(WallHight > SCREEN_ROW) WallHight = SCREEN_ROW - 1;
		 
		//三维化
		int LineHalf = WallHight/2;
		
		
		//x轴映射（单列渲染物体）
		int RenderHight = LineHalf*2;
		double TextureX_Step = ((double)texture->Logic_ROW)/RenderHight;//获取ROW的映射步进长度
		double Texture_X = 0;
		int Render_top = RenderCenter - LineHalf;
		for(int i = 0;i<RenderHight;i++){
			int signal = Texture_Extract(texture,(int)Texture_X,Texture_Y);
			if(signal > 0){
				//准备不同方向的墙的色差
				if(UnitDir == UNIT_PLANE_VERTICAL){//如果光线垂直射入
					Canvas_PixelOperate(i+Render_top,x,CANVAS_PIXEL_OPEN);
				}else{//如果光线水平射入
					if(x%2 == 0){
						Canvas_PixelOperate(i+Render_top,x,CANVAS_PIXEL_OPEN);
					}
					else {
						if(i%2 == 0){
							Canvas_PixelOperate(i+Render_top,x,CANVAS_PIXEL_OPEN);
						}else{
							Canvas_PixelOperate(i+Render_top,x,CANVAS_PIXEL_CLOSE);
						}
					}
				}
			}else{
				//按照纹理关闭像素点
				Canvas_PixelOperate(i+Render_top,x,CANVAS_PIXEL_CLOSE);
			}
			Texture_X += TextureX_Step;
		}
	
		
		
		
		
		
	}
}
/*******************
	【镜头旋转】
********************/
//镜头左旋
void Camera_LeftPan(Camera_Struct* camera) {
    //保存当前方向向量的分量：
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
    //利用旋转矩阵计算更新方向向量：
    camera->direction.x = dirX * cos(camera->rotSpeed) - dirY * sin(camera->rotSpeed);
    camera->direction.y = oldDirX * sin(camera->rotSpeed) + dirY * cos(camera->rotSpeed);
    //保存当前平面法向量的 x 分量：
    double oldPlaneX = camera->plane.x;
    //更新平面法向量：
    camera->plane.x = camera->plane.x * cos(camera->rotSpeed) - camera->plane.y * sin(camera->rotSpeed);
    camera->plane.y = oldPlaneX * sin(camera->rotSpeed) + camera->plane.y * cos(camera->rotSpeed);
}
//镜头右旋
void Camera_RightPan(Camera_Struct* camera){
	 //保存当前方向向量的分量：
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
    //利用旋转矩阵计算更新方向向量：
    camera->direction.x = dirX * cos(-camera->rotSpeed) - dirY * sin(-camera->rotSpeed);
    camera->direction.y = oldDirX * sin(-camera->rotSpeed) + dirY * cos(-camera->rotSpeed);
    //保存当前平面法向量的 x 分量：
    double oldPlaneX = camera->plane.x;
    //更新平面法向量：
    camera->plane.x = camera->plane.x * cos(-camera->rotSpeed) - camera->plane.y * sin(-camera->rotSpeed);
    camera->plane.y = oldPlaneX * sin(-camera->rotSpeed) + camera->plane.y * cos(-camera->rotSpeed);
}
//视角左偏转
void Camera_LeftTilt(Camera_Struct* camera){
	
}
//视角右偏转
void Camera_RightTilt(Camera_Struct* camera){
	
}
/*******************
	【镜头移动】
********************/
//镜头左移
void Camera_LeftMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	
	
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->plane.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
		camera->position.x -= camera->plane.x * camera->moveSpeed;
	}
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->plane.y * camera->moveSpeed)) == 0){
		camera->position.y -= camera->plane.y * camera->moveSpeed;
	}
   
  
}
//镜头右移
void Camera_RightMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}

	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->plane.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
		camera->position.x += camera->plane.x * camera->moveSpeed;
	}
	
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->plane.y * camera->moveSpeed)) == 0){
        camera->position.y += camera->plane.y * camera->moveSpeed;
	}
}
//镜头前移
void Camera_ForwardMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	//WorldMap_Extract(WorldMapData,int ROW_Pixel,int COL_Pixel)
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->direction.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
        camera->position.x += camera->direction.x * camera->moveSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->direction.y * camera->moveSpeed)) == 0){
        camera->position.y += camera->direction.y * camera->moveSpeed;
	}
}
//镜头后移
void Camera_BackMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->direction.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
        camera->position.x -= camera->direction.x * camera->moveSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->direction.y * camera->moveSpeed)) == 0){
        camera->position.y -= camera->direction.y * camera->moveSpeed;
	}
}
