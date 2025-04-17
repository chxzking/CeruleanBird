#include "Camera.h"

#include <math.h>

#include "TextureMapper_internal.h"//纹理内部接口

#include "Canvas_API.h"			//画布接口

#include "WorldMap_internal.h"	//地图内部接口

#include "FrameRateStrategy_internal.h"//Δt模型修正帧率无关

#include "EngineFault.h"//引擎错误处理

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



/**
*	@brief	初始化相机（测试通过）
*	@param
*		@arg	Camera_InitStruct		相机初始化结构体
*	@retval
*		@arg	初始化成功返回相机地址
*		@arg	初始化失败返回空
*/
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct){
	//检查输入
	if(Camera_InitStruct == C_NULL) return C_NULL;
	if(Camera_InitStruct->FOV <= 0 || Camera_InitStruct->FOV >= 180) return C_NULL;//限制摄像头的视角在0度到180度之间，不包括0和180度
	 
	Camera_Struct* camera = (Camera_Struct*)malloc(sizeof(Camera_Struct) + sizeof(DeepBuffer_t)*SCREEN_COLUMN);
	if(camera == C_NULL)	return C_NULL;
	//用户定义属性赋值
	//有限处理简易的赋值
	camera->moveSpeed = Camera_InitStruct->moveSpeed;
	camera->rotSpeed = Camera_InitStruct->rotSpeed * 3.0;//将移动速度乘以PI值
	camera->position = Camera_InitStruct->position;
	camera->direction = vector_Normalize(&Camera_InitStruct->direction);//归一化方向向量
	
	//私有数据初始化
	camera->Camera_PrivateData.data = C_NULL;
	camera->Camera_PrivateData.__FreeHandle = C_NULL;
	camera->Engine_Common_Data.data = C_NULL;
	camera->Engine_Common_Data.__FreeHandle = C_NULL;
	
	//修正深度缓冲区
	camera->DeepBuffer = (DeepBuffer_t*)((char*)camera + sizeof(Camera_Struct));
	//深度初始设置为无穷大
	for(int i = 0;i < SCREEN_COLUMN;i++){
		camera->DeepBuffer[i] = POSITIVE_INFINITY;
	}
	
	//属性5集初始化
	camera->Ceil.Col = 0;
	camera->Ceil.Row = 0;
	camera->Ceil.renderType = 0;
	camera->Ceil.ceilDescriptArr = NULL;
	camera->Ceil.Ceil_RenderHandle = Celi_DefaultRender;
	
	camera->Floor.Row = 0;
	camera->Floor.Col = 0;
	camera->Floor.renderType = 0;
	camera->Floor.floorDescriptArr = NULL;
	camera->Floor.Floor_RenderHandle = Floor_DefaultRender;
	
	camera->SSR_AnimData.index = 0;
	camera->SSR_AnimData.AnimType = 0;
	camera->SSR_AnimData.ReadyFrm = NULL;
	camera->SSR_AnimData.Anim = NULL;
	camera->SSR_AnimData.SSRAnim_Render = NULL;
	
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



/**
*	@brief	获取相机位置
*	@param
*		@arg	camera		相机
*	@retval
*		返回相机坐标，错误会进入引擎紧急阻隔
*/
vector Camera_GetPos(Camera_Struct* camera){
	if(camera == NULL){
		//【错误】<空指针访问>
	EngineFault_NullPointerGuard_Handle();
	}
	return camera->position;
}



/**
*	@brief	单帧摄像机视角基底模型渲染
*	@param
*		@arg	camera		相机
*	@retval
*		none
*/
void Camera_Render(Camera_Struct* camera){
	
	//天花板与地面渲染
	camera->Floor.Floor_RenderHandle(camera);
	camera->Ceil.Ceil_RenderHandle(camera);
	
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
		int UnitAttribute = MOVABLE_AREA_VALUE;
		//定义单元格被光线击中的方向
		char UnitDir = UNIT_PLANE_VERTICAL;
		//单列光线扫描【障碍识别正常，扫描终点都停留在了预期的位置】
		while(UnitAttribute == MOVABLE_AREA_VALUE){
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
		
		//深度缓冲区更新
		Camera_DeepBuffer_Write(camera,x,RealDistance);
		
		
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

/**
*	@brief	镜头向左侧旋转
*	@param
*		@arg	camera		相机
*	@retval
*		none
*/
void Camera_LeftPan(Camera_Struct* camera) {
    //保存当前方向向量的分量：
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
		//修正为帧率无关旋转速度
		double correctionSpeed = camera->rotSpeed * DeltaTime->deltaTime; 
    //利用旋转矩阵计算更新方向向量：
    camera->direction.x = dirX * cos(correctionSpeed) - dirY * sin(correctionSpeed);
    camera->direction.y = oldDirX * sin(correctionSpeed) + dirY * cos(correctionSpeed);
    //保存当前平面法向量的 x 分量：
    double oldPlaneX = camera->plane.x;
    //更新平面法向量：
    camera->plane.x = camera->plane.x * cos(correctionSpeed) - camera->plane.y * sin(correctionSpeed);
    camera->plane.y = oldPlaneX * sin(correctionSpeed) + camera->plane.y * cos(correctionSpeed);
}

/**
*	@brief	镜头向右侧旋转
*	@param
*		@arg	camera		相机
*	@retval
*		none
*/
void Camera_RightPan(Camera_Struct* camera){
		//保存当前方向向量的分量：
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
		//修正为帧率无关旋转速度
		double correctionSpeed = camera->rotSpeed * DeltaTime->deltaTime; 
    //利用旋转矩阵计算更新方向向量：
    camera->direction.x = dirX * cos(-correctionSpeed) - dirY * sin(-correctionSpeed);
    camera->direction.y = oldDirX * sin(-correctionSpeed) + dirY * cos(-correctionSpeed);
    //保存当前平面法向量的 x 分量：
    double oldPlaneX = camera->plane.x;
    //更新平面法向量：
    camera->plane.x = camera->plane.x * cos(-correctionSpeed) - camera->plane.y * sin(-correctionSpeed);
    camera->plane.y = oldPlaneX * sin(-correctionSpeed) + camera->plane.y * cos(-correctionSpeed);
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

/**
*	@brief	相机向左侧移动
*	@param
*		@arg	camera		相机
*	@retval
*		none
*/
void Camera_LeftMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	//修正为帧率无关移动速度
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime; 
	
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->plane.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
		camera->position.x -= camera->plane.x * correctionSpeed;
	}
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->plane.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
		camera->position.y -= camera->plane.y * correctionSpeed;
	}
   
  
}

/**
*	@brief	相机向右侧移动
*	@param
*		@arg	camera		相机
*	@retval
*		none
*/
void Camera_RightMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	//修正为帧率无关移动速度
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime; 
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->plane.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
		camera->position.x += camera->plane.x * correctionSpeed;
	}
	
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->plane.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
        camera->position.y += camera->plane.y * correctionSpeed;
	}
}
/**
*	@brief	相机向前方移动
*	@param
*		@arg	camera		相机
*	@retval
*		none
*/
void Camera_ForwardMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	//修正为帧率无关移动速度
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime;
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->direction.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
        camera->position.x += camera->direction.x * correctionSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->direction.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
        camera->position.y += camera->direction.y * correctionSpeed;
	}
}

/**
*	@brief	相机向后方移动
*	@param
*		@arg	camera		相机
*	@retval
*		none
*/
void Camera_BackMove(Camera_Struct* camera){
	//获取当前地图属性
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//地图数据获取错误
	}
	//修正为帧率无关移动速度
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime;
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->direction.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
        camera->position.x -= camera->direction.x * correctionSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->direction.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
        camera->position.y -= camera->direction.y * correctionSpeed;
	}
}









/**
*	@brief	深度缓冲区写入
*	@param
*		@arg	camera		相机
*		@arg	index		索引
*		@arg	value		写入的值
*	@retval
*		none
*	@note
*		失败属于紧急错误，会导致进入错误阻止状态
*/
void Camera_DeepBuffer_Write(Camera_Struct* camera,int index,DeepBuffer_t value){
	if(camera == NULL || index < 0 || index >= SCREEN_COLUMN){
		//【错误】<非法访问>
		EngineFault_IllegalAccess_Handle();
	}
	camera->DeepBuffer[index] = value;
	return;
}

/**
*	@brief	深度缓冲区读取
*	@param
*		@arg	camera		相机
*		@arg	index		索引
*	@retval
*		@arg	成功则读取对应的值
*		@arg	失败属于紧急错误，会导致进入错误阻止状态
*/
DeepBuffer_t Camera_DeepBuffer_Read(Camera_Struct* camera,int index){
	if(camera == NULL || index < 0 || index >= SCREEN_COLUMN){
		//【错误】<非法访问>
		EngineFault_IllegalAccess_Handle();
	}
	return camera->DeepBuffer[index];
}



/*******************
	【私有域】
********************/
/**
*	@brief	给摄像机添加私有变量
*	@param
*		@arg	camera		相机
*		@arg	data		数据(不能为空)
*		@arg	__FreeHandle 数据处理
*	@retval
*		@arg	添加成功返回0
*		@arg	添加失败返回-1
*/
int CameraPriv_Add(Camera_Struct* camera,void* data,void (*__FreeHandle)(void* ptr)){
	if(camera == NULL || data == NULL){
		return -1;
	}
	camera->Camera_PrivateData.data = data;
	camera->Camera_PrivateData.__FreeHandle = __FreeHandle;
	return 0;
}
/**
*	@brief	替换原有摄像机的私有变量
*	@param
*		@arg	camera		相机
*		@arg	data		数据(不能为空)
*		@arg	__FreeHandle 数据处理
*	@retval
*		@arg	添加成功返回0
*		@arg	添加失败返回-1
*/
int CameraPriv_Replace(Camera_Struct* camera,void* data,void (*__FreeHandle)(void* ptr)){
	if(camera == NULL || data == NULL){
		return -1;
	}
	if(camera->Camera_PrivateData.data == NULL){
		//如果原来没有添加私有变量
		camera->Camera_PrivateData.data = data;
		camera->Camera_PrivateData.__FreeHandle = __FreeHandle;
		return 0;
	}
	//存在数据则先释放数据
	if(camera->Camera_PrivateData.__FreeHandle != NULL){
		camera->Camera_PrivateData.__FreeHandle(camera->Camera_PrivateData.data);
	}
	camera->Camera_PrivateData.data = data;
	camera->Camera_PrivateData.__FreeHandle = __FreeHandle;
	return 0;
}
/**
*	@brief	重置原有的私有数据
*	@param
*		@arg	camera		相机
*	@retval
*		@arg	添加成功返回0
*		@arg	添加失败返回-1
*/
int CameraPriv_Reset(Camera_Struct* camera){
	if(camera == NULL){
		return -1;
	}
	if(camera->Camera_PrivateData.__FreeHandle != NULL){
		camera->Camera_PrivateData.__FreeHandle(camera->Camera_PrivateData.data);
	}
	camera->Camera_PrivateData.data = NULL;
	camera->Camera_PrivateData.__FreeHandle = NULL;
	return 0;
}
/**
*	@brief	重置原有的私有数据
*	@param
*		@arg	camera		相机
*	@retval
*		返回私有数据
*/
void* CameraPriv_Get(Camera_Struct* camera){
	if(camera == NULL){
		return NULL;
	}
	return camera->Camera_PrivateData.data;
}

/****************************************************
	
				【背景渲染与顶层渲染】
	
*****************************************************/


/**********************
	【背景渲染】
***********************/
/**
*	@brief		添加一张指定类型的贴图
*	@param
*		@arg  	Row	贴图的行
*		@arg  	Col	贴图的列
*		@arg  	texture	贴图纹理数组
*		@arg  	FC_TreeManager	贴图管理树
*	@retval
*		@arg	成功则0
*		@arg	失败返回-1
*/

#define FC_TextureAdd(Row,Col,texture,FC_TreeManager)	do{\
	if(index <= 0 || Row <= 0 || Col <= 0 || texture == NULL){\
		return -1;\
	}\
	/*查找是否存在了贴图*/\
	void* temp = search(FC_TreeManager, index);\
	if(temp != NULL){\
		/*存在此索引*/\
		return -1;\
	}\
	/*创建节点*/\
	FC_Node_t* node = (FC_Node_t*)malloc(sizeof(FC_Node_t));\
	if(node == NULL){\
		/*空间分配失败*/\
		return -1;\
	}\
	node->ROW_Logic = Row;\
	if(Row % 8 == 0){\
		node->ROW = Row / 8;\
	}else{\
		node->ROW = Row / 8 + 1;\
	}\
	node->COL = Col;\
	node->FC_texture = texture;\
	int result =  rbInsert(&FC_TreeManager, index,node);\
	if(result != 0){\
		free(node);\
		return -1;\
	}\
	return 0;\
}while(0)

/**
*	@brief		获取指定类型的贴图的节点
*	@param
*		@arg  	index	索引节点
*		@arg  	FC_TreeManager 贴图管理树
*	@retval
*		@arg	成功则节点地址
*		@arg	失败返回空
*/
#define FC_GetTexture(index,FC_TreeManager)	do{\
	if(index <= 0){\
		return NULL;\
	}\
	FC_Node_t* node = (FC_Node_t*)search(FC_TreeManager, index);\
	return node;\
}while(0)

/**
*	@brief		获取指定类型的贴图的纹理提取
*	@param
*		@arg	FC_Node		贴图节点描述
*		@arg  	ROW_Pixel	像素级别行坐标
*		@arg  	COL_Pixel	像素级别列坐标
*	@retval
*		@arg	亮返回1
*		@arg	灭返回0
*		@arg	函数错误返回-1
*/
int FC_TextureExtract(FC_Node_t *FC_Node,int ROW_Pixel,int COL_Pixel){
	if(FC_Node == NULL || ROW_Pixel < 0 || COL_Pixel < 0) return -1;
	if(ROW_Pixel >= FC_Node->ROW_Logic || COL_Pixel >= FC_Node->COL) return -1;
	
	int byte = ROW_Pixel/8;
	int offset = ROW_Pixel%8;
	
	return (FC_Node->FC_texture[byte * FC_Node->COL + COL_Pixel] & (0x01<<offset)) ? 1:0;
}
/*渲染实现---------------------------------------------------------------------------------*/

/**
 * @brief  地板纹理渲染
 * @param  
 *		@arg	camera: 指向摄像机结构体
 * @retval
 *		none
 */
void Floor_TextureRender(Camera_Struct* camera) {
	int repeatFactor = 2;
    // 计算视角左右边界射线（用于地板采样）
    double rayDirRow0 = camera->direction.x - camera->plane.x;
    double rayDirCol0 = camera->direction.y - camera->plane.y;
    double rayDirRow1 = camera->direction.x + camera->plane.x;
    double rayDirCol1 = camera->direction.y + camera->plane.y;
    
    // 只对屏幕下半部分进行地面渲染（中线以上通常为天花板）
    for (int scrRow = SCREEN_ROW / 2 + 1; scrRow < SCREEN_ROW; scrRow++) {
        int p = scrRow - SCREEN_ROW / 2;
        double posZ = 0.5 * SCREEN_ROW;  // 模拟距离（根据屏幕高度设置）
        double rowDistance = posZ / p;   // 透视投影距离
        
        // 计算当前行每个像素对应在世界中地面采样的位置差值
        double floorStepRow = rowDistance * (rayDirRow1 - rayDirRow0) / SCREEN_COLUMN;
        double floorStepCol = rowDistance * (rayDirCol1 - rayDirCol0) / SCREEN_COLUMN;
        
        // 初始采样位置：当前行最左侧对应的地面坐标
        double floorRowPos = camera->position.x + rowDistance * rayDirRow0;
        double floorColPos = camera->position.y + rowDistance * rayDirCol0;
        
        for (int scrCol = 0; scrCol < SCREEN_COLUMN; scrCol++) {
            // 将采样位置转换为整数部分（地面单元标识）和小数部分（局部坐标）
            int worldTileRow = (int)floorRowPos;
            int worldTileCol = (int)floorColPos;
            
            // 将世界坐标映射到地面描述数组中（处理负值以及越界情况）
            int mapIdxRow = ((worldTileRow % camera->Floor.Row) + camera->Floor.Row) % camera->Floor.Row;
            int mapIdxCol = ((worldTileCol % camera->Floor.Col) + camera->Floor.Col) % camera->Floor.Col;
            int floorDescIndex = mapIdxRow * camera->Floor.Col + mapIdxCol;
            
            // 获取该地面单元应使用的纹理编号
            unsigned char tileIndexFloor = camera->Floor.floorDescriptArr[floorDescIndex];
            
            // 计算该单元内的局部位置（范围 0～1）
            double fracRow = floorRowPos - worldTileRow;
            double fracCol = floorColPos - worldTileCol;
            
            // 关键：将局部坐标乘以重复因子，再取其小数部分，
            // 使得每个 1×1 单位内重复显示纹理 repeatFactor 次
            fracRow = fracRow * repeatFactor - floor(fracRow * repeatFactor);
            fracCol = fracCol * repeatFactor - floor(fracCol * repeatFactor);
            
            // 根据纹理编号获取对应的纹理节点
            FC_Node_t* floorTexNode = Floor_GetTexture(tileIndexFloor);
            if (floorTexNode == NULL)
                floorTexNode = Floor_GetTexture(0);
            
            // 根据局部坐标与纹理尺寸计算要采样的具体纹理坐标
            int textureRow = (int)(fracRow * floorTexNode->ROW_Logic) % floorTexNode->ROW_Logic;
            int textureCol = (int)(fracCol * floorTexNode->COL) % floorTexNode->COL;
            
            // 提取当前纹理像素（注意内部使用了位打包格式读取）
            int pixelVal = FC_TextureExtract(floorTexNode, textureRow, textureCol);
            if (pixelVal < 0)
                pixelVal = 0;
            CANVAS_PIXEL_STATUS pixelStatus = (pixelVal > 0) ? CANVAS_PIXEL_OPEN : CANVAS_PIXEL_CLOSE;
            
            // 绘制当前屏幕像素
            Canvas_PixelOperate(scrRow, scrCol, pixelStatus);
            
            // 更新采样位置（向右移动一个采样步长）
            floorRowPos += floorStepRow;
            floorColPos += floorStepCol;
        }
    }
}


/**
 * @brief  天花板纹理渲染
 * @param  
 *		@arg	camera: 指向摄像机结构体
 * @retval
 *		none
 */
void Ceil_TextureRender(Camera_Struct* camera) {
    double rayDirRow0 = camera->direction.x - camera->plane.x;
    double rayDirCol0 = camera->direction.y - camera->plane.y;
    double rayDirRow1 = camera->direction.x + camera->plane.x;
    double rayDirCol1 = camera->direction.y + camera->plane.y;
	
    for (int scrRow = SCREEN_ROW / 2 + 1; scrRow < SCREEN_ROW; scrRow++) {
        int p = scrRow - SCREEN_ROW / 2;
        double posZ = 0.5 * SCREEN_ROW;
        double rowDistance = posZ / p;
        double floorStepRow = rowDistance * (rayDirRow1 - rayDirRow0) / SCREEN_COLUMN;
        double floorStepCol = rowDistance * (rayDirCol1 - rayDirCol0) / SCREEN_COLUMN;
        double floorRowPos = camera->position.x + rowDistance * rayDirRow0;
        double floorColPos = camera->position.y + rowDistance * rayDirCol0;
        int ceilScreenRow = SCREEN_ROW - scrRow - 1;

        for (int scrCol = 0; scrCol < SCREEN_COLUMN; scrCol++) {
            int worldTileRowC = (int)floorRowPos;
            int worldTileColC = (int)floorColPos;
            int mapIdxRowC = ((worldTileRowC % camera->Ceil.Row) + camera->Ceil.Row) % camera->Ceil.Row;
            int mapIdxColC = ((worldTileColC % camera->Ceil.Col) + camera->Ceil.Col) % camera->Ceil.Col;
            int indexCeil = mapIdxRowC * camera->Ceil.Col + mapIdxColC;
            unsigned char tileIndexCeil = camera->Ceil.ceilDescriptArr[indexCeil];

            double fracRowC = floorRowPos - worldTileRowC;
            double fracColC = floorColPos - worldTileColC;

            FC_Node_t* ceilTexNode = Celi_GetTexture(tileIndexCeil);
            if (ceilTexNode == NULL)
                ceilTexNode = Celi_GetTexture(0);

            int ceilTexRows = ceilTexNode->ROW;
            int ceilTexCols = ceilTexNode->COL;
            int texRowC = (int)(fracRowC * ceilTexRows) % ceilTexRows;
            int texColC = (int)(fracColC * ceilTexCols) % ceilTexCols;
            unsigned char ceilPixelVal = ceilTexNode->FC_texture[ texRowC * ceilTexCols + texColC ];
            CANVAS_PIXEL_STATUS ceilPixelStatus = (ceilPixelVal > 0) ? CANVAS_PIXEL_OPEN : CANVAS_PIXEL_CLOSE;

            Canvas_PixelOperate(ceilScreenRow, scrCol, ceilPixelStatus);

            floorRowPos += floorStepRow;
            floorColPos += floorStepCol;
        }
    }
}



/**
*	@brief	渲染默认地板图形
*	@param
*		camera	形式值，并不使用为了兼容API设计
*	@retval
*		none
*/
void Floor_DefaultRender(Camera_Struct* camera){
	if(camera == NULL)	return;
	int RowStart = SCREEN_ROW/2;
	for(int i = RowStart;i<SCREEN_ROW;i++){
		for(int j = 0;j<SCREEN_COLUMN;j++){
			if(j%2 == 0){
				if(i%2 == 0){//双数行打印
					Canvas_PixelOperate(i,j,CANVAS_PIXEL_OPEN);
				}
			}
			//单数列
			else{
				if(i%2 != 0){//单数行打印
					Canvas_PixelOperate(i,j,CANVAS_PIXEL_OPEN);	
				}	
			}
		}
	}
	return;
}	

/*地板操作-------------------------------------------------------*/

RB_Node* FloorTreeManager = NULL;
RB_Node* CeliTreeManager = NULL;
/**
*	@brief		创建自定义地板渲染，
*	@param
*		@arg	camera		相机
*		@arg	Row			渲染数组的行
*		@arg	Col			渲染数组的列
*		@arg	RenderArray	渲染数组，渲染数组中的每个成员代表了预渲染的贴图索引
*	@retval
*		@arg	创建成功返回0
*		@arg	创建失败返回-1
*/
int Floor_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray){
	if(camera == NULL || RenderArray == NULL){
		return -1;
	}
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return -1;//地图数据获取错误
	}
	if(WorldMapData->Row != Row && WorldMapData->Column != Col){
		return -1;
	}
	camera->Floor.Row = Row;
	camera->Floor.Col = Col;
	camera->Floor.floorDescriptArr = RenderArray;
	return 0;
}
/**
*	@brief		地面渲染模式切换为默认渲染，
*	@param
*		@arg	camera		相机
*	@retval
*		@arg	执行成功返回 0
*		@arg	执行失败返回-1
*/
int Floor_SwitchDefaultReaderMode(Camera_Struct* camera){
	if(camera == NULL)	return -1;
	camera->Floor.renderType = 0;
	return 0;	
}
/**
*	@brief		地面渲染模式切换为自定义渲染，
*	@param
*		@arg	camera		相机
*	@retval
*		@arg	执行成功返回 0
*		@arg	执行失败返回-1
*/
int Floor_SwitchCustomReaderMode(Camera_Struct* camera){
	if(camera == NULL || camera->Floor.floorDescriptArr == NULL)	return -1;
	camera->Floor.renderType = 1;
	camera->Floor.Floor_RenderHandle = Floor_TextureRender;
	return 0;
}


/**
*	@brief		添加一张地图的贴图
*	@param
*		@arg	index		索引(必须大于等于1)
*	@retval
*		@arg	成功则读取对应的值
*		@arg	失败属于紧急错误，会导致进入错误阻止状态
*/
int Floor_TextureAdd(int index,int Row,int Col,const unsigned char* texture){
	FC_TextureAdd(Row,Col,texture,FloorTreeManager);//宏定义函数
}



/**
*	@brief		提取索引对应的地板贴图信息
*	@param
*		@arg	index		索引(必须大于等于1)
*	@retval
*		@arg	成功则读取对应的值
*		@arg	失败属于紧急错误，会导致进入错误阻止状态
*/
FC_Node_t* Floor_GetTexture(int index){
	FC_GetTexture(index,FloorTreeManager);
}



/*天花板操作-------------------------------------------------------*/
/**
*	@brief		添加一张天花板的贴图
*	@param
*		@arg	index		索引(必须大于等于1)
*	@retval
*		@arg	成功则读取对应的值
*		@arg	失败属于紧急错误，会导致进入错误阻止状态
*/
int Celi_TextureAdd(int index,int Row,int Col,const unsigned char* texture){
	FC_TextureAdd(Row,Col,texture,CeliTreeManager);
}


/**
*	@brief		创建自定义天花板渲染，
*	@param
*		@arg	camera		相机
*		@arg	Row			渲染数组的行
*		@arg	Col			渲染数组的列
*		@arg	RenderArray	渲染数组，渲染数组中的每个成员代表了预渲染的贴图索引
*	@retval
*		@arg	创建成功返回0
*		@arg	创建失败返回-1
*/
int Celi_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray){
	if(camera == NULL || RenderArray == NULL){
		return -1;
	}
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return -1;//地图数据获取错误
	}
	if(WorldMapData->Row != Row && WorldMapData->Column != Col){
		return -1;
	}
	camera->Ceil.Row = Row;
	camera->Ceil.Col = Col;
	camera->Ceil.ceilDescriptArr = RenderArray;
	return 0;
}
/**
*	@brief		天花板渲染模式切换为默认渲染，
*	@param
*		@arg	camera		相机
*	@retval
*		@arg	执行成功返回 0
*		@arg	执行失败返回-1
*/
int Celi_SwitchDefaultReaderMode(Camera_Struct* camera){
	if(camera == NULL)	return -1;
	camera->Ceil.renderType = 0;
	return 0;	
}
/**
*	@brief		天花板渲染模式切换为自定义渲染，
*	@param
*		@arg	camera		相机
*	@retval
*		@arg	执行成功返回 0
*		@arg	执行失败返回-1
*/
int Celi_SwitchCustomReaderMode(Camera_Struct* camera){
	if(camera == NULL || camera->Ceil.ceilDescriptArr == NULL)	return -1;
	camera->Ceil.renderType = 1;
	camera->Ceil.Ceil_RenderHandle = Ceil_TextureRender; 
	return 0;
}


/**
*	@brief		提取索引对应的天花板贴图信息
*	@param
*		@arg	index		索引(必须大于等于1)
*	@retval
*		@arg	成功则读取对应的值
*		@arg	失败属于紧急错误，会导致进入错误阻止状态
*/
FC_Node_t* Celi_GetTexture(int index){
	FC_GetTexture(index,CeliTreeManager);
}



/**
*	@brief	渲染默认天花图形(默认渲染为空)
*	@param
*		camera	形式值，并不使用为了兼容API设计
*	@retval
*		执行成功返回0
*		执行失败返回 -1
*/
void Celi_DefaultRender(Camera_Struct* camera){
	if(camera == NULL)	return;
	return ;
}	















/**********************
	【顶层渲染】
***********************/

RB_Node* SSR_CoreTree = NULL;

/**
*	@brief	创建一个动画组管理器
*	@param
*		@arg	value	动画组索引值（大于0）
*		@arg	Row		动画数组的行
*		@arg	Col		动画数组的列
*		@arg	time	动画一个周期的播放时间
*	@retval
*		执行成功返回0
*		执行失败返回 -1
*/

int SSRAnim_ManagerCreate(int value,int Row_Pixel,int Col_Pixel,double time){
	if(value <= 0 || Row_Pixel <= 0 || Col_Pixel <= 0)	return -1;
	//验证是否已经注册
	void* temp = search(SSR_CoreTree,value);
	if(temp != NULL){
		return -1;
	}
	//创建节点
	SSR_Anim_t* manager = (SSR_Anim_t*)malloc(sizeof(SSR_Anim_t));
	if(manager == NULL){
		return -1;
	}
	if(Row_Pixel % 8 == 0){
		manager->Row = Row_Pixel / 8;
	}else{
		manager->Row = ( Row_Pixel / 8 ) + 1;
	}
	manager->ROW_Logic = Row_Pixel;
	manager->Col = Col_Pixel;
	manager->FirstFrm = NULL;
	manager->TailFrm = NULL;
	manager->time = time;
	manager->count = 0;
	manager->frame_duration = time;
	//插入树中
	int result = rbInsert(&SSR_CoreTree,value,manager);
	if(result != 0){
		free(manager);
		return -1;
	}
	return 0;
}

/**
*	@brief	在指定动画组下添加动画帧
*	@param
*		@arg	value	动画组引值（大于0）
*		@arg	seq		动画帧的序号(大于等于0)
*		@arg	Frm		动画帧数组
*		@arg	shadow	动画帧影子描述数组
*	@retval
*		执行成功返回0
*		执行失败返回 -1
*/
int SSRAnim_FrmAdd(int value, int seq, const unsigned char* Frm,const unsigned char* shadow) {
    if (value <= 0 || seq < 0 || Frm == NULL)
        return -1;

    // 获取管理器
    SSR_Anim_t* manager = (SSR_Anim_t*)search(SSR_CoreTree, value);
    if (manager == NULL)
        return -1;

    // 创建新节点
    SSR_Frm_t* frm = (SSR_Frm_t*)malloc(sizeof(SSR_Frm_t));
    if (frm == NULL)
        return -1;

    frm->frmArr = Frm;
    frm->value = seq;
    frm->next = NULL;
	frm->shadowFrmArr = shadow;
	
    // 如果链表为空，构成单节点循环链表
    if (manager->count == 0) {
        manager->FirstFrm = frm;
        frm->next = frm;  // 指向自身
        manager->TailFrm = frm;
        manager->count = 1;
		manager->frame_duration = manager->time / manager->count;//更新单帧时间
        return 0;
    }

    SSR_Frm_t* head = manager->FirstFrm;
    SSR_Frm_t* tail = manager->TailFrm;

    // 特殊情况：新节点序号小于头节点
    if (frm->value < head->value) {
        // 检查尾节点是否与新节点重复
        if (tail->value == frm->value) {
            free(frm); // 释放内存，防止内存泄漏
            return -1; // 序号冲突
        }
        frm->next = head;
        tail->next = frm;           // 更新尾节点的循环指向
        manager->FirstFrm = frm;    // 更新头节点指针
        manager->count++;
		manager->frame_duration = manager->time / manager->count;//更新单帧时间
        return 0;
    }

    // 从头节点开始，寻找插入点
    SSR_Frm_t* current = head;
    while (current->next != head && current->next->value < frm->value) {
        current = current->next;
    }

    // 检查是否与下一个节点的序号重复
    if (current->next->value == frm->value) {
        free(frm); // 释放内存，防止内存泄漏
        return -1; // 序号冲突
    }

    // 插入节点
    frm->next = current->next;
    current->next = frm;

    // 如果插入点是尾节点，则更新 TailFrm 指针
    if (current == tail) {
        manager->TailFrm = frm;
    }

    manager->count++;
	manager->frame_duration = manager->time / manager->count;//更新单帧时间
    return 0;
}

/**
*	@brief	将指定动画设置为当前引擎的执行动画(默认为非循环动画)
*	@param
*		@arg	camera	摄像机
*		@arg	value	动画组引值（大于0）	
*	@retval
*		执行成功返回0
*		执行失败返回 -1
*/
int SSR_SetCameraExecAnim(Camera_Struct* camera,int value){
	if(camera == NULL ||value <= 0)	return -1;
	
	// 获取管理器
    SSR_Anim_t* manager = (SSR_Anim_t*)search(SSR_CoreTree, value);
    if (manager == NULL || manager->count == 0){
        return -1;	
	}
	
	camera->SSR_AnimData.index = value;
	camera->SSR_AnimData.Anim = manager;
	camera->SSR_AnimData.ReadyFrm = manager->FirstFrm;
	camera->SSR_AnimData.accumulatedTime = 0;
	camera->SSR_AnimData.currentFrmIndex = 0;
	camera->SSR_AnimData.AnimType = 0;//非循环
	return 0;
}

/**
*	@brief	顶层动画更新下一帧
*	@param
*		@arg	camera	摄像机
*	@retval
*	情况一：动画属于循环类型
*		输出恒为0
*
*	情况二：动画属于单次执行类型
*		没有完成一个周期返回 0
*		如果已经完成了一个周期返回 -1
*/
int SSRAnim_Update(Camera_Struct* camera){
	if(camera == NULL || camera->SSR_AnimData.ReadyFrm == NULL){
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	camera->SSR_AnimData.accumulatedTime += (DeltaTime->deltaTime);
	//判断是否超过了更新阈值
	while(camera->SSR_AnimData.accumulatedTime >= camera->SSR_AnimData.Anim->frame_duration){
		//降低阈值
		camera->SSR_AnimData.accumulatedTime -= camera->SSR_AnimData.Anim->frame_duration;
		//判断动画类型是循环动画还是非循环动画
		if(camera->SSR_AnimData.AnimType == 0){
			//非循环动画
			
			//如果当前动画帧不是最后一帧
			if(camera->SSR_AnimData.currentFrmIndex < camera->SSR_AnimData.Anim->count){
				camera->SSR_AnimData.ReadyFrm = camera->SSR_AnimData.ReadyFrm->next;
				camera->SSR_AnimData.currentFrmIndex++;
			}else{
				return -1;
			}
			
		}else{
			//循环动画
			//更新为下一帧
			camera->SSR_AnimData.ReadyFrm = camera->SSR_AnimData.ReadyFrm->next;
		}
	}
	
	return 0;
}

/**
*	@brief	重置非循环类动画帧执行位置
*	@param
*		@arg	camera	摄像机
*	@retval
*		执行成功返回0
*		执行失败返回 -1		
*/
int SSRAnim_ResetSingle(Camera_Struct* camera){
	if(camera == NULL || camera->SSR_AnimData.AnimType == 1 || camera->SSR_AnimData.Anim == NULL){
		return -1;
	}
	camera->SSR_AnimData.ReadyFrm = camera->SSR_AnimData.Anim->FirstFrm;
	camera->SSR_AnimData.currentFrmIndex = 0;
	return 0;
}

/**
*	@brief	提取帧的信息
*	@param
*		@arg	anim	帧对应的动画管理器动画
*		@arg	frm		帧
*		@arg	ROW_Pixel	读取的像素的行
*		@arg	COL_Pixel	读取的像素的列
*	@retval
*		返回1代表亮
*		返回0代表灭
*		返回-1代表透明
*/
int SSRAnim_FrmExtract(SSR_Anim_t* anim,SSR_Frm_t* frm,int ROW_Pixel,int COL_Pixel) {
    if(anim == NULL || frm == NULL || ROW_Pixel < 0 || COL_Pixel < 0){
		return -1;
	}
	//
	if(ROW_Pixel >= anim->ROW_Logic || COL_Pixel >= anim->Col){
		return -1;
	}
	
	int byte = ROW_Pixel/8;
	int offset = ROW_Pixel%8;
	int data_shadow = (frm->shadowFrmArr[byte * anim->Col + COL_Pixel] & (0x01<<offset)) ? 1:0;
	if(data_shadow == 0){
			//为0显示透明
			return -1;
	}
	return (frm->frmArr[byte * anim->Col + COL_Pixel] & (0x01<<offset)) ? 1:0;
}


/**
*	@brief	顶层渲染使能
*	@param
*		@arg	camera	摄像机
*	@retval
*		执行成功返回0
*		执行失败返回 -1
*/
int SSRAnim_RenderEnable(Camera_Struct* camera){
	if(camera == NULL){
		return -1; 
	}
	camera->SSR_AnimData.SSRAnim_Render = SSRAnim_Render;
	return 0;
}

/**
*	@brief	设置图层显示位置
*	@param
*		@arg	camera	摄像机
*		@arg	ROW_Pixel	读取的像素的行
*		@arg	COL_Pixel	读取的像素的列
*	@retval
*		执行成功返回0
*		执行失败返回 -1
*/
int SSRAnim_SetPos(Camera_Struct* camera,int ROW_Pixel,int COL_Pixel){
	if(camera == NULL){
		return -1; 
	}
	camera->SSR_AnimData.ROW_Pixel = ROW_Pixel;
	camera->SSR_AnimData.COL_Pixel = COL_Pixel;
	return 0;
}
/**
*	@brief	顶层渲染失能
*	@param
*		@arg	camera	摄像机
*	@retval
*		执行成功返回0
*		执行失败返回 -1
*/
int SSRAnim_RenderDisable(Camera_Struct* camera){
	if(camera == NULL){
		return -1; 
	}
	camera->SSR_AnimData.SSRAnim_Render = NULL;
	return 0;
}


/**
*	@brief	顶层渲染函数
*	@param
*		@arg	camera	摄像机
*	@retval
*		none
*/
void SSRAnim_Render(Camera_Struct* camera) {
    if (camera == NULL) return;
    
    int img_row = camera->SSR_AnimData.Anim->ROW_Logic;
    int img_col = camera->SSR_AnimData.Anim->Col;
    int pos_row = camera->SSR_AnimData.ROW_Pixel;
    int pos_col = camera->SSR_AnimData.COL_Pixel;
    
    // 计算行方向的起始和结束索引
    int i_start = (pos_row < 0) ? -pos_row : 0;
    int i_end = img_row;
    if(pos_row + img_row > SCREEN_ROW) {
        i_end = SCREEN_ROW - pos_row;
    }
    
    // 计算列方向的起始和结束索引
    int j_start = (pos_col < 0) ? -pos_col : 0;
    int j_end = img_col;
    if(pos_col + img_col > SCREEN_COLUMN) {
        j_end = SCREEN_COLUMN - pos_col;
    }
    
    // 如果完全超出屏幕，则不渲染
    if(i_start >= i_end || j_start >= j_end) {
        return;
    }
    
    // 仅渲染屏幕内可见的部分
    for (int i = i_start; i < i_end; i++) {
        for (int j = j_start; j < j_end; j++) {
            int result = SSRAnim_FrmExtract(camera->SSR_AnimData.Anim, camera->SSR_AnimData.ReadyFrm, i, j);
            // 将图像的逻辑坐标加上偏移映射到屏幕坐标
            int screen_i = i + pos_row;
            int screen_j = j + pos_col;
            if (result == 0) {
                Canvas_PixelOperate(screen_i, screen_j, CANVAS_PIXEL_CLOSE);
            } else if (result == 1) {
                Canvas_PixelOperate(screen_i, screen_j, CANVAS_PIXEL_OPEN);
            }
        }
    }
}

/**
*	@brief	顶层渲染总线
*	@param
*		@arg	camera	摄像机
*	@retval
*		none
*/
void SSRAnim_Exec(Camera_Struct* camera){
	if(camera == NULL || camera->SSR_AnimData.SSRAnim_Render == NULL)	return;
	camera->SSR_AnimData.SSRAnim_Render(camera);
	return;
}
