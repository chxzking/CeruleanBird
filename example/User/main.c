#include "stm32f10x.h"                  // Device header
#include "Mistink.h"

#include "M_stm32f103c8t6.h"
#include <math.h>

#include "CeruleanBird.h"

#include "animation.h"

#include "map.h"
#include "Common.h"
#include "pistol.h"
#include "Enemy.h"
#include "key.h"


MistinkDisplayer_t* displayer = NULL;//显示器实例

//敌人控制
int LongS_Count = 3;//远程精灵数量上限
int LongC_Count = 3;//近程精灵数量上限
int LongS_survivalCount = 0;//场上远程敌人数量
int LongC_survivalCount = 0;//场上近程敌人数量

typedef struct{
	int ShootFlag;//射击标志	
	int isRunning;//运行标志
}ShootGroup_t;//射击事件组
typedef struct{
	int ShootAnimValue;
	int StaticAnimVale;
}PistolAnimGroup_t;//手枪动画组

void FreeHandle(void* data){
	c_free(data);
}

void frmInit(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10000-1;// 
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;//7200 0000//0.1ms计数一次100 10ms 10/1000 n*0.1/1000
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_Cmd(TIM3,ENABLE);
}

double tick(void*data){
	double currentTime = TIM_GetCounter(TIM3);
	currentTime /= 10000.0;
	TIM_SetCounter(TIM3, 0);
	return currentTime;
}

//控制地图中敌人数量
void EnemyQuantityControl(void){
    // ----- 远程敌人控制逻辑 -----
    // 如果当前场上没有远程敌人，则保证至少生成一个
    if (LongS_survivalCount == 0) {
        if (LongS_survivalCount < LongS_Count) {  // 此处肯定成立
            if (LongEnemy_Create() == 0) {
                LongS_survivalCount++;
            }
        }
    } else {
        // 计算缺口比例，对应的生成概率:
        // 当 LongS_survivalCount 越低（远低于上限），(LongS_Count - LongS_survivalCount)/LongS_Count 越大，生成概率就越高
        if (LongS_survivalCount < LongS_Count) {
            float spawnProbability = (float)(LongS_Count - LongS_survivalCount) / LongS_Count;
            float randValue = (float)rand() / (float)RAND_MAX; // 生成0~1之间的随机数
            if (randValue < spawnProbability) {
                if (LongEnemy_Create() == 0) {
                    LongS_survivalCount++;
                }
            }
        }
    }

    // ----- 近程敌人控制逻辑 -----
    // 同上，对于近程敌人，确保至少有一个存在
    if (LongC_survivalCount == 0) {
        if (LongC_survivalCount < LongC_Count) {  // 此处肯定成立
            if (CloseEnemy_Create() == 0) {
                LongC_survivalCount++;
            }
        }
    } else {
        if (LongC_survivalCount < LongC_Count) {
            float spawnProbability = (float)(LongC_Count - LongC_survivalCount) / LongC_Count;
            float randValue = (float)rand() / (float)RAND_MAX;
            if (randValue < spawnProbability) {
                if (CloseEnemy_Create() == 0) {
                    LongC_survivalCount++;
                }
            }
        }
    }
}
//射击动画
int ShootAnim(int AnimManagerIndex,Camera_Struct* camera){
	int result = 0;
	//创建顶层管理
	result |= SSRAnim_ManagerCreate(AnimManagerIndex,48,48,0.3);
	
	result |= SSRAnim_FrmAdd(AnimManagerIndex, 1, gun_48x48_1,gun_48x48_1_shadown);
	result |= SSRAnim_FrmAdd(AnimManagerIndex, 2, gun_48x48_2,gun_48x48_2_shadown);
	result |= SSRAnim_FrmAdd(AnimManagerIndex, 3, gun_48x48_3,gun_48x48_3_shadown);
	result |= SSRAnim_FrmAdd(AnimManagerIndex, 4, gun_48x48_4,gun_48x48_4_shadown);
	
	if(result != 0){
		return -1;
	}
	return 0;
}

//静置贴图
int StaticMap(int AnimManagerIndex,Camera_Struct* camera){
	int result = 0;
	//创建顶层管理
	result |= SSRAnim_ManagerCreate(AnimManagerIndex,48,48,0.5);
	
	result |= SSRAnim_FrmAdd(AnimManagerIndex, 1, gun_48x48_1,gun_48x48_1_shadown);

	if(result != 0){
		return -1;
	}
	return 0;
}

//触发射击
void ShootTrigger(Camera_Struct* camera,ShootGroup_t* ShootGroup,PistolAnimGroup_t* PistolAnimGroup){
	//触发射击事件
	if(ShootGroup->ShootFlag == 1 && ShootGroup->isRunning == 0){
		if(pistol_Fire(camera) != 0){
			//子弹触发失败
			ShootGroup->ShootFlag = 0;
		}else{
			//成功触发开火逻辑
			SSR_SetCameraExecAnim(camera,PistolAnimGroup->ShootAnimValue);
			SSRAnim_Update(camera);
			ShootGroup->isRunning = 1;
		}
	}else if(ShootGroup->isRunning == 1){//正在执行射击事件
		
		if(SSRAnim_Update(camera) == -1){
			//执行完成一个周期
			ShootGroup->ShootFlag = 0;
			ShootGroup->isRunning = 0;
			SSR_SetCameraExecAnim(camera,PistolAnimGroup->StaticAnimVale);
		}
	}
}
int main(void){
	//创建显示器实例
	MistinkPltfrmDrv_t MistinkPlatformDrivers;
	MistinkPlatformDrivers.__Init = OLED_Init;
	MistinkPlatformDrivers.__BufferOutput = OLED_BufferOutput;
	MistinkPlatformDrivers.__Clear = OLED_Clear;
	displayer = MistinkDisplayer_Create(64,128,&MistinkPlatformDrivers);
	
	//按键初始化
	Key_Init();
	
	//摄像机初始化
	Camera_InitTypeDef camera_sturct;
	camera_sturct.direction.x = -1;
	camera_sturct.direction.y = 0;
	camera_sturct.FOV = 66;
	camera_sturct.moveSpeed = 0.5;//m/s
	camera_sturct.rotSpeed = 0.3;
	camera_sturct.position.x = 20.5;
	camera_sturct.position.y = 12;

	
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
	WorldMap_Add(1,worldMap,24,24);

	M_String(displayer,32,0,"WorldMap OK");
	MistinkDisplayer_BufferOutput(displayer);
	

	/*********
	精灵初始化
	**********/	
	
	int S_result = 0;
	//远程精灵初始化
	S_result |= LongEnemy_Init(LongS_Count,camera,1,1,0.3);
	//篮球子弹初始化
 	S_result |= Basketball_Init(3,camera,2,2,1);
	//子弹初始化
	S_result |= pistol_Init(1,camera,3,3,2);
	//近程精灵初始化
	S_result |= CloseEnemy_Init(LongC_Count,camera,4,4,0.3);
	if( S_result != 0){
		while(1);//初始化失败
	}
	
	M_String(displayer,48,0,"Sprites OK");
	MistinkDisplayer_BufferOutput(displayer);
	MistinkOP_BufferReset(displayer);//刷新缓冲
	
	/*********
	帧率初始化
	**********/
	
	//帧率策略初始化
	DeltaTimeBaseStruct_t DeltaTimeBaseStruct;
	DeltaTimeBaseStruct.CalculateTimeInterval = tick;
	DeltaTimeBaseStruct.FreeHandle = NULL;
	DeltaTimeBaseStruct.PrivateData = NULL;
	DeltaTimeBaseStruct.TimeUnitConfig = frmInit;
	FrmRS_deltaTime_Init(&DeltaTimeBaseStruct);
	
	/*********
	玩家私有数据
	**********/
	int *HP = (int*)c_malloc(sizeof(int));
	if(HP == NULL){
		while(1);
	}
	*HP = 9;
	CameraPriv_Add(camera,HP,FreeHandle);
	
	/*********
	顶层贴图
	**********/
	/*射击组*/
	ShootGroup_t ShootGroup;
	ShootGroup.isRunning = 0;
	ShootGroup.ShootFlag = 0;
	
	/*动画组*/
	PistolAnimGroup_t PistolAnimGroup;
	PistolAnimGroup.StaticAnimVale = 1;//静置动画索引
	PistolAnimGroup.ShootAnimValue = 2;//开枪动画索引
	
	int top = 0;
	//动画加载
	top |= ShootAnim(PistolAnimGroup.ShootAnimValue,camera);//开枪动画
	top |= StaticMap(PistolAnimGroup.StaticAnimVale,camera);//静置动画
	
	
	top |= SSR_SetCameraExecAnim(camera,PistolAnimGroup.StaticAnimVale);
	top |= SSRAnim_SetPos(camera,16,80);//设置贴图位置
	top |= SSRAnim_RenderEnable(camera);//启用渲染
	if(top != 0){
		while(1);//贴图加载失败
	}
	
	
	
	KeyValue_t KeyValue;//按键
	while(1){
		//精灵数量控制
		EnemyQuantityControl();
		
		//按键监听
		KeyValue = Get_Key();
		
		//按键行为激活
		switch(KeyValue){
			case K_Left:;//左转
				Camera_LeftPan(camera);
			break;
			case K_Right:;//右转
				Camera_RightPan(camera);
			break;
			case K_Mid:;//开火
				ShootGroup.ShootFlag = 1;
			break;
			case K_Up:;//前移
				Camera_ForwardMove(camera);
			break;
			case K_Down:;//后移
				Camera_BackMove(camera);
			break;
			case K_Idle:;//空置
			default:;
			break;			
		}
		
		
		
		ShootTrigger(camera,&ShootGroup,&PistolAnimGroup);//开火逻辑（监听开火信号）
		
		
		/************
		【画面渲染】
		*************/
		//引擎渲染
		CeruleanBird_EngineRender(camera);
		
		//M_Int(displayer,48,0,(int)FrmRS_GetFrmRate());//帧率显示

		//小地图打印
		Minimap(camera);
		//生命显示
		if(*HP<=0){
			*HP = 9;//无敌血量
		}
		M_Int(displayer, 0,118,*HP);//显示生命值
		
		//分数显示
		if(enemyCore <= 99){
			M_Int(displayer, 0,25,enemyCore);//显示得分
		}else{
			M_String(displayer,0,25,"99+");//显示得分
		}
		
		//画面输出
		MistinkDisplayer_BufferOutput(displayer);//输出画面
		MistinkOP_BufferReset(displayer);//刷新缓冲
	}
	
}





