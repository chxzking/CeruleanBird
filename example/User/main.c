#include "stm32f10x.h"                  // Device header
#include "Mistink.h"

#include "M_stm32f103c8t6.h"
#include <math.h>

#include "CeruleanBird.h"

#include "animation.h"

MistinkDisplayer_t* displayer = NULL;//显示器实例


//测试地图
unsigned char TestMap2[] =
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1,
  1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1,
  1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

const unsigned char floormap[] =
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

const unsigned char floordata_24x24[]={
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0x03,0x03,0xF3,0xF3,
	0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF0,0xF0,0xF3,0xF3,0xF3,
	0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0xF3,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
	0x00,0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
	0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xF0,0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xF
};

const unsigned char jian_16x16[]={
	0x00,0x00,0x00,0xE0,0x60,0xA0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x36,0x7C,0x7C,0xDE,0x06,0x1C,0x08,0x00
};
const unsigned char jian_8x8[]={
	0x00,0x00,0x1C,0x3E,0x3E,0x1C,0x00,0x00
};

void map_print_dn(int row,int col){
	for(int i = 0;i<24/row;i++){
		for(int j = 0;j<24/col;j++){
			int temp = *(TestMap2 + (i*row)*24 + (j*col));
			if(temp){
				MistinkPixel_Operate(displayer,i,j ,PixelOpen);
			}else{
				MistinkPixel_Operate(displayer,i,j ,PixelClose);
			}
		}
	}
}


void frm(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10000-1;// 
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;//7200 0000//0.1ms计数一次100 10ms 10/1000 n*0.1/1000
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_Cmd(TIM3,ENABLE);
}

double func2(void*data){
	double currentTime = TIM_GetCounter(TIM3);
	currentTime /= 10000.0;
	TIM_SetCounter(TIM3, 0);
	return currentTime;
}


void func3(unsigned char *p){
			for(int i = 0;i<5;i++){
					for(int j = 0;j<36;j++){				
						MistinkPixel_RegisterOP(displayer,i,j,p[i*36+j]);
					}
			}
	
}
/*************************************************

	【精灵设计】

**************************************************/

Sprites_t* Spritestemp;//坤坤
Sprites_t* Spritedun;//盾牌

int iskunsleep = 0;
int isdunsleep = 0;

typedef struct{//私有数据成员设计
	Camera_Struct* camera;//摄像机
	int x;
	int y;
	int z;
}privdata;
void SpritesPrivateFreeHandle(void* pdata){
	c_free(pdata);
}



/***********
	【第一个盾牌】
************/

SpriteBTStatus_t SBT_ExecuteHandle(Sprites_t* Sprites,void* privateData){
	privdata* data = (privdata*)privateData;
	int area = 10;
	if(data->y==0){//正向移动
		if(data->x <= area){
			data->x++;
		}else{
			data->x = area;
			data->y = 1;
		}
//		SpritesMov_Forward(Sprites);
//		if(data->z == 0){
			SpritesMov_Left(Sprites);
//		}
	}else{
		if(data->x >= -area){
			data->x--;
		}else{
			data->x = -area;
			data->y = 0;
		}
//		SpritesMov_Backward(Sprites);
//		SpritesMov_Right(Sprites);
	}

	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}
//中断自定义映射
SITindex_t SIT_MapIndexHandle(Sprites_t* Sprites,void* privateData){
	privdata* data = (privdata*)privateData;
	vector pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
	if(pos.y < 10){
		return 0;
	}
	return -1;
}
//中断处理
SpriteBTStatus_t SBT_ExecuteHandle_IT(Sprites_t* Sprites,void* privateData){
	privdata* data = (privdata*)privateData;
	vector pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
	if(pos.y>12){
//		data->z = 0;
		//恢复到18,结束中断
		return SBT_SUCCESS;
	}
	SpritesMov_Right(Sprites);
	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	//向右移动
	return SBT_RUNNING;
}
int BT_func(int a){
	int result = 0;
	int value = a;
	//创建行为树
	result |= SpritesBT_Init(value);
	
	//创建一个动作
	SpriteBTNode_t* node = SpritesBT_Action(SBT_ExecuteHandle);
	if(node == NULL){
		return -1;
	}
	//绑定到根节点
	result |= SpritesBT_MountToRoot(value, node);
	if(result != 0){
		return -1;
	}
	
	
	//注册中断表
	result |= SpritesIT_Register(value,SIT_MapIndexHandle);
	result |= SpritesIT_Add(value ,0,10,SIT_AllowSubstitution,SBT_ExecuteHandle_IT);
	if(result != 0){
		return -1;
	}
	return value;
}



Sprites_t* function(int num){
	int value = BT_func(num);
	if(value <= 0){
		while(1);
	}
	Sprites_t* node = Sprites_ObjectCreate(1,18,12,0,1,1,value);
	vector pos;
	pos.x = 1.0;
	pos.y = 0.0;
	int temp = SpritesMov_SetDir(node,pos);
	if(temp != 0){
		while(1);
	}
	//创建私有数据
	privdata *data = c_malloc(sizeof(privdata));
	data->x = 0;
	data->y = 0;
	data->z = 0;
	SpritesPriv_Add(node,data,SpritesPrivateFreeHandle);
	

	return node;
}
/***********
	【第二个盾牌】
************/

SpriteBTStatus_t SBT_ExecuteHandle2(Sprites_t* Sprites,void* privateData){
	privdata* data = (privdata*)privateData;
	int area = 10;
	if(data->y==0){//正向移动
		if(data->x <= area){
			data->x++;
		}else{
			data->x = area;
			data->y = 1;
		}
//		SpritesMov_Forward(Sprites);
		SpritesMov_Left(Sprites);
	}else{
		if(data->x >= -area){
			data->x--;
		}else{
			data->x = -area;
			data->y = 0;
		}
//		SpritesMov_Backward(Sprites);
		SpritesMov_Right(Sprites);
	}
	if(data->z++ % 200 == 0){
		Sprites_Awaken(TO_SPRITES_BASE(Spritestemp));
		Sprites_Awaken(TO_SPRITES_BASE(Spritedun));
		iskunsleep = 0;
		isdunsleep = 0;
	}

	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}

int BT_func2(int a){
	int result = 0;
	int value = a;
	//创建行为树
	result |= SpritesBT_Init(value);
	
	//创建一个动作
	SpriteBTNode_t* node = SpritesBT_Action(SBT_ExecuteHandle2);
	if(node == NULL){
		return -1;
	}
	//绑定到根节点
	result |= SpritesBT_MountToRoot(value, node);
	if(result != 0){
		return -1;
	}
	return value;
}
Sprites_t* function2(int num){
	
	int value = BT_func2(num);
	if(value <= 0){
		while(1);
	}

	Sprites_t* node = Sprites_ObjectCreate(1,8.5,8.5,0,1,1,value);//Sprites_ObjectCreate(1,9,11.5,0,1,1,value);
	
	vector pos;
	pos.x = 1.0;
	pos.y = 0.0;
	int temp = SpritesMov_SetDir(node,pos);
	if(temp != 0){
		while(1);
	}
	//创建私有数据
	privdata *data = c_malloc(sizeof(privdata));
	data->x = 0;
	data->y = 0;
	data->z = 0;
	SpritesPriv_Add(node,data,SpritesPrivateFreeHandle);
	
	return node;
}

/***********
	【坤坤】
************/
SpriteBTStatus_t SBT_ExecuteHandle_kun(Sprites_t* Sprites,void* privateData){
	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}
Sprites_t* function3(int value){
	SpritesBT_Init(value);
	
	//创建一个动作
	SpriteBTNode_t* act = SpritesBT_Action(SBT_ExecuteHandle_kun);
	SpritesBT_MountToRoot(value, act);
	
	Sprites_t* node = Sprites_ObjectCreate(1,19.5,12,0,0.5,1,value);//Sprites_ObjectCreate(1,20,12,0,0.5,1,value);//Sprites_ObjectCreate(1,9,11.5,0,1,1,value);
	
	vector pos;
	pos.x = 1.0;
	pos.y = 0.0;
	int temp = SpritesMov_SetDir(node,pos);
	if(temp != 0){
		while(1);
	}
	//创建私有数据
	privdata *data = c_malloc(sizeof(privdata));
	data->x = 0;
	data->y = 0;
	data->z = 0;
	SpritesPriv_Add(node,data,SpritesPrivateFreeHandle);
	
	return node;
}

/***********
	【子弹】
************/
int isover = 1;



SpriteBTStatus_t SBT_ExecuteHandle_4(Sprites_t* Sprites,void* privateData){
	privdata* data = (privdata*)privateData;
	vector pos;
	vector posself;
	double a;
	double b;
	double dis;
	/*坤坤检测*/
	if(iskunsleep == 0){
		pos = SpritesData_GetPos(TO_SPRITES_BASE(Spritestemp));
		posself = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
		a = pos.x - posself.x;
		b = pos.y - posself.y;
		dis = sqrt(a*a + b*b);
		if(dis <= 0.1){
			Sprites_Sleep(TO_SPRITES_BASE(Spritestemp));
			Sprites_Sleep(TO_SPRITES_BASE(Sprites));
			isover = 1;
			iskunsleep = 1;
			return SBT_SUCCESS;
			
		}
	}
	/*盾牌检测*/
	if(isdunsleep == 0){
		pos = SpritesData_GetPos(TO_SPRITES_BASE(Spritedun));
		a = pos.x - posself.x;
		b = pos.y - posself.y;
		dis = sqrt(a*a + b*b);
		if(dis <= 0.1){
			Sprites_Sleep(TO_SPRITES_BASE(Spritedun));
			Sprites_Sleep(TO_SPRITES_BASE(Sprites));
			isover = 1;
			isdunsleep = 1;
			return SBT_SUCCESS;
		}
	}
	
	SpritesMov_Forward(Sprites);
	if(Sprites_collisionWallValue(Sprites) != 0){
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));
		isover = 1;
		return SBT_SUCCESS;
	}
	
	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}
Sprites_t* function4(int value){
	SpritesBT_Init(value);
	//创建一个动作
	SpriteBTNode_t* act = SpritesBT_Action(SBT_ExecuteHandle_4);
	SpritesBT_MountToRoot(value, act);
	
	Sprites_t* node = Sprites_ObjectCreate(1,20,12,0,1.3,1,value);//Sprites_ObjectCreate(1,9,11.5,0,1,1,value);
	
	vector pos;
	pos.x = 1.0;
	pos.y = 0.0;
	int temp = SpritesMov_SetDir(node,pos);
	if(temp != 0){
		while(1);
	}
	//创建私有数据
	privdata *data = c_malloc(sizeof(privdata));
	data->x = 0;
	data->y = 0;
	data->z = 0;
	SpritesPriv_Add(node,data,SpritesPrivateFreeHandle);
	
	return node;
}

/***********
	【主函数】
************/
int main(){
	
	//创建显示器实例
	MistinkPltfrmDrv_t MistinkPlatformDrivers;
	MistinkPlatformDrivers.__Init = OLED_Init;
	MistinkPlatformDrivers.__BufferOutput = OLED_BufferOutput;
	MistinkPlatformDrivers.__Clear = OLED_Clear;
	displayer = MistinkDisplayer_Create(64,128,&MistinkPlatformDrivers);
	
	
	Camera_InitTypeDef camera_sturct;
	camera_sturct.direction.x = -1;
	camera_sturct.direction.y = 0;
	camera_sturct.FOV = 66;
	camera_sturct.moveSpeed = 0.5;//m/s
	camera_sturct.rotSpeed = 0.2;
	//用于测试前后移动
	camera_sturct.position.x = 21;
	camera_sturct.position.y = 12;
	//用于测试旋转
//	camera_sturct.position.x = 10;
//	camera_sturct.position.y = 8.5;
	//用于精灵测试
//	camera_sturct.position.x = 14;
//	camera_sturct.position.y = 12;
	
	Camera_Struct* camera;
	
	M_String(displayer,0,0,"Camera OK");
	MistinkDisplayer_BufferOutput(displayer);
	
	//画布重定向
	Canvas_SetCanvasModeToRedirect(MistinkOP_GetBufferAPI(displayer));
	
	//初始化

	camera = Camera_Init(&camera_sturct);
	
		
	M_String(displayer,16,0,"Canvas OK");
	MistinkDisplayer_BufferOutput(displayer);
	
	//添加纹理
	Texture_Add(1,kun2,48,48,BIT_TEXTURE);
	Texture_Add(2,kun2,48,48,BIT_TEXTURE);
	Texture_Add(3,kun2,48,48,BIT_TEXTURE);
	Texture_Add(4,kun2,48,48,BIT_TEXTURE);
	//添加地图
	WorldMap_Add(1,TestMap2,24,24);

	M_String(displayer,32,0,"WorldMap OK");
	MistinkDisplayer_BufferOutput(displayer);
	

	/*********
	精灵初始化
	**********/

	Sprites_t* sprites;
	Sprites_t* sprites2;
	Sprites_t* sprites3;
	Sprites_t* sprites4;
	SpritesStatic_t* sprites5;//静态精灵
	
	int index;
	int index2;
	int index3;
	int index4;
	
	if(SpritesAnim_CreateManager(1) == 0){
			index = SpritesAnim_CreateNewChain(1,36,36,BIT_SPRITES,0.5);
			index2 = SpritesAnim_CreateNewChain(1,36,36,BIT_SPRITES,1.0);
			index3 = SpritesAnim_CreateNewChain(1,45,45,BIT_SPRITES,2.5);
			index4 = SpritesAnim_CreateNewChain(1,8,8,BIT_SPRITES,0.5);
		
		
			if(index >= 0 && index2 >= 0){
				int temp = 0;
				//第一个盾牌
				temp |= SpritesAnim_InsertNewFrm(1,index,0,dun,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index,2,dun2,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index,3,dun3,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index,4,dun4,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index,5,dun5,dun_shadown);
				//第二个盾牌
				temp |= SpritesAnim_InsertNewFrm(1,index2,0,dun,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index2,6,dun6,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index2,7,dun7,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index2,8,dun8,dun_shadown);
				temp |= SpritesAnim_InsertNewFrm(1,index2,9,dun9,dun_shadown);
				//坤坤
				temp |= SpritesAnim_InsertNewFrm(1,index3,0,(unsigned char*)IKUN_TieShanKao_45x45_1,(unsigned char*)IKUN_TieShanKao_45x45_1_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,1,(unsigned char*)IKUN_TieShanKao_45x45_2,(unsigned char*)IKUN_TieShanKao_45x45_2_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,2,(unsigned char*)IKUN_TieShanKao_45x45_3,(unsigned char*)IKUN_TieShanKao_45x45_3_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,3,(unsigned char*)IKUN_TieShanKao_45x45_4,(unsigned char*)IKUN_TieShanKao_45x45_4_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,4,(unsigned char*)IKUN_TieShanKao_45x45_5,(unsigned char*)IKUN_TieShanKao_45x45_5_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,5,(unsigned char*)IKUN_TieShanKao_45x45_6,(unsigned char*)IKUN_TieShanKao_45x45_6_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,6,(unsigned char*)IKUN_TieShanKao_45x45_7,(unsigned char*)IKUN_TieShanKao_45x45_7_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,7,(unsigned char*)IKUN_TieShanKao_45x45_8,(unsigned char*)IKUN_TieShanKao_45x45_8_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,8,(unsigned char*)IKUN_TieShanKao_45x45_9,(unsigned char*)IKUN_TieShanKao_45x45_9_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,9,(unsigned char*)IKUN_TieShanKao_45x45_10,(unsigned char*)IKUN_TieShanKao_45x45_10_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,10,(unsigned char*)IKUN_TieShanKao_45x45_11,(unsigned char*)IKUN_TieShanKao_45x45_11_shadow);
				temp |= SpritesAnim_InsertNewFrm(1,index3,11,(unsigned char*)IKUN_TieShanKao_45x45_12,(unsigned char*)IKUN_TieShanKao_45x45_12_shadow);
				//子弹
				temp |= SpritesAnim_InsertNewFrm(1,index4,0,jian_8x8,jian_8x8);

				if( temp == 0){
					M_String(displayer,48,0,"Sprites OK");
					MistinkDisplayer_BufferOutput(displayer);
					
					sprites = function(1);//Sprites_DataInit(1,0,0,0,0.1,1,NULL);
					Spritedun = sprites;
					sprites2 = function2(2);
					sprites3 =	function3(3);
					Spritestemp = sprites3;
					sprites4 =	function4(4);
					
					SpritesMov_SetDirWithCamera(sprites,camera);
					SpritesIT_Enable(sprites);
					
					sprites5 = SpritesStatic_ObjectCreate(1,10,12,index,1);
					
					if(sprites){
						MistinkOP_BufferReset(displayer);
						M_String(displayer,0,0,"Canvas OK");
						M_String(displayer,16,0,"WorldMap OK");
						M_String(displayer,32,0,"Sprites OK");
						M_String(displayer,48,0,"Object OK");
						MistinkDisplayer_BufferOutput(displayer);
						goto A;
					}
					M_String(displayer,48,0,"Object NO");
				}
				
			}
		
	}
	M_String(displayer,48,0,"Sprites NO");
	MistinkDisplayer_BufferOutput(displayer);

	A:;
	
	
	MistinkOP_BufferReset(displayer);
	
	/*********
	帧率初始化
	**********/
	
	//帧率策略初始化
	DeltaTimeBaseStruct_t DeltaTimeBaseStruct;
	DeltaTimeBaseStruct.CalculateTimeInterval = func2;
	DeltaTimeBaseStruct.FreeHandle = NULL;
	DeltaTimeBaseStruct.PrivateData = NULL;
	DeltaTimeBaseStruct.TimeUnitConfig = frm;
	FrmRS_deltaTime_Init(&DeltaTimeBaseStruct);
	
	
	/*********
	精灵属性设置
	**********/
	
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(sprites),Anim_Loop);
	
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(sprites2),index2);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(sprites2),Anim_Loop);
	
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(sprites3),index3);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(sprites3),Anim_Loop);
	
	
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(sprites4),index4);
	Sprites_Sleep(TO_SPRITES_BASE(sprites4));
	
	
	SpritesData_SetVerticalHeight(TO_SPRITES_BASE(sprites5),20);//静态精灵设置高度
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(sprites5),Anim_Loop);
	
	int dir = 0;
	double lastpos = 0;
	
	
	//顶层设计
	/*********
	顶层设计
	**********/
	int ssrtemp = 0;
	ssrtemp |= SSRAnim_ManagerCreate(1,48,48,0.3);
	ssrtemp |= SSRAnim_FrmAdd(1, 0, gun_48x48_1,gun_48x48_1_shadown);
	ssrtemp |= SSRAnim_FrmAdd(1, 1, gun_48x48_2,gun_48x48_2_shadown);
	ssrtemp |= SSRAnim_FrmAdd(1, 2, gun_48x48_3,gun_48x48_3_shadown);
	ssrtemp |= SSRAnim_FrmAdd(1, 3, gun_48x48_4,gun_48x48_4_shadown);
	ssrtemp |= SSRAnim_FrmAdd(1, 4, gun_48x48_1,gun_48x48_1_shadown);
	ssrtemp |= SSRAnim_RenderEnable(camera);
	ssrtemp |= SSRAnim_SetPos(camera,16,64);
	ssrtemp |= SSR_SetCameraExecAnim(camera,1);
	
	ssrtemp |= SSRAnim_ManagerCreate(2,48,48,0.5);
	ssrtemp |= SSRAnim_FrmAdd(2, 0, gun_48x48_1,gun_48x48_1_shadown);
	
	if(ssrtemp != 0){
		while(1);
	}
	
	
	
//	int k = 0;
	int p = 0;
	int p2 = 0;
	
	while(1){
//		if(k++%800 == 0){
//			SpritesMov_SetPosToCamera(sprites4,camera);
//			SpritesMov_SetDirWithCamera(sprites4,camera);
//			Sprites_Awaken(TO_SPRITES_BASE(sprites4));
//		}
		vector position = Camera_GetPos(camera);
		if(lastpos == position.x){
			dir = 1 - dir;
		}
		lastpos = position.x;
		
		CeruleanBird_EngineRender(camera);//引擎渲染

	
	
//		M_Int(displayer,0,0,(int)(camera->moveSpeed * DeltaTime->deltaTime*1000));
//		M_Int(displayer,0,0,(int)(camera->position.x*1000));
//		M_Int(displayer,48,0,(int)(DeltaTime->deltaTime*1000));
//		M_Int(displayer,48,0,fps);
		M_Int(displayer,48,0,(int)FrmRS_GetFrmRate());
		
		//if(SSRAnim_Update(camera) == -1)	SSRAnim_ResetSingle(camera);
		
//		if(isover == 1){
//			if(p2 == 0){
//				SSRAnim_Update(camera);//更新下一帧
//			}else{
//				if(SSRAnim_Update(camera) == -1){
//					
//					SpritesMov_SetPosToCamera(sprites4,camera);
//					SpritesMov_SetDirWithCamera(sprites4,camera);
//					Sprites_Awaken(TO_SPRITES_BASE(sprites4));
//					
//					//预先让子弹飞一段距离
//					for(int i = 0;i<10;i++){
//						SpritesMov_Forward(sprites4);
//					}
//					
//					SSRAnim_ResetSingle(camera);
//					SSR_SetCameraExecAnim(camera,2);
//					p2 = 0;
//					isover = 0;
//				}
//			}
//		}
//		
//		if(p++%100 == 0){
//			p2 = 1;
//			SSR_SetCameraExecAnim(camera,1);
//		}
		if(isover == 1){
			
			
	
			if(SSRAnim_Update(camera) == -1){
				
				SpritesMov_SetPosToCamera(sprites4,camera);
				SpritesMov_SetDirWithCamera(sprites4,camera);
				Sprites_Awaken(TO_SPRITES_BASE(sprites4));
				
				//预先让子弹飞一段距离
				for(int i = 0;i<10;i++){
					SpritesMov_Forward(sprites4);
				}
				
				SSRAnim_ResetSingle(camera);
				SSR_SetCameraExecAnim(camera,2);
				p2 = 0;
				isover = 0;
			}
			
			
			
		}else{
			if(p2 == 0){
				SSR_SetCameraExecAnim(camera,1);
			}
		}
		
//		if(p++%100 == 0){
//			p2 = 1;
//			
//		}

		
		MistinkDisplayer_BufferOutput(displayer);
		if(dir == 0){
			Camera_ForwardMove(camera);
//			Camera_LeftPan(camera); 
//			
//			Camera_LeftMove(camera);
		}else{
			Camera_BackMove(camera);
//			Camera_RightPan(camera); 
//			Camera_RightMove(camera);
		}	
		

		
		MistinkOP_BufferReset(displayer);

	
	}

	
}

