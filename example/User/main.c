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


MistinkDisplayer_t* displayer = NULL;//��ʾ��ʵ��

//���˿���
int LongS_Count = 3;//Զ�̾�����������
int LongC_Count = 3;//���̾�����������
int LongS_survivalCount = 0;//����Զ�̵�������
int LongC_survivalCount = 0;//���Ͻ��̵�������

typedef struct{
	int ShootFlag;//�����־	
	int isRunning;//���б�־
}ShootGroup_t;//����¼���
typedef struct{
	int ShootAnimValue;
	int StaticAnimVale;
}PistolAnimGroup_t;//��ǹ������

void FreeHandle(void* data){
	c_free(data);
}

void frmInit(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10000-1;// 
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;//7200 0000//0.1ms����һ��100 10ms 10/1000 n*0.1/1000
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	TIM_Cmd(TIM3,ENABLE);
}

double tick(void*data){
	double currentTime = TIM_GetCounter(TIM3);
	currentTime /= 10000.0;
	TIM_SetCounter(TIM3, 0);
	return currentTime;
}

//���Ƶ�ͼ�е�������
void EnemyQuantityControl(void){
    // ----- Զ�̵��˿����߼� -----
    // �����ǰ����û��Զ�̵��ˣ���֤��������һ��
    if (LongS_survivalCount == 0) {
        if (LongS_survivalCount < LongS_Count) {  // �˴��϶�����
            if (LongEnemy_Create() == 0) {
                LongS_survivalCount++;
            }
        }
    } else {
        // ����ȱ�ڱ�������Ӧ�����ɸ���:
        // �� LongS_survivalCount Խ�ͣ�Զ�������ޣ���(LongS_Count - LongS_survivalCount)/LongS_Count Խ�����ɸ��ʾ�Խ��
        if (LongS_survivalCount < LongS_Count) {
            float spawnProbability = (float)(LongS_Count - LongS_survivalCount) / LongS_Count;
            float randValue = (float)rand() / (float)RAND_MAX; // ����0~1֮��������
            if (randValue < spawnProbability) {
                if (LongEnemy_Create() == 0) {
                    LongS_survivalCount++;
                }
            }
        }
    }

    // ----- ���̵��˿����߼� -----
    // ͬ�ϣ����ڽ��̵��ˣ�ȷ��������һ������
    if (LongC_survivalCount == 0) {
        if (LongC_survivalCount < LongC_Count) {  // �˴��϶�����
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
//�������
int ShootAnim(int AnimManagerIndex,Camera_Struct* camera){
	int result = 0;
	//�����������
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

//������ͼ
int StaticMap(int AnimManagerIndex,Camera_Struct* camera){
	int result = 0;
	//�����������
	result |= SSRAnim_ManagerCreate(AnimManagerIndex,48,48,0.5);
	
	result |= SSRAnim_FrmAdd(AnimManagerIndex, 1, gun_48x48_1,gun_48x48_1_shadown);

	if(result != 0){
		return -1;
	}
	return 0;
}

//�������
void ShootTrigger(Camera_Struct* camera,ShootGroup_t* ShootGroup,PistolAnimGroup_t* PistolAnimGroup){
	//��������¼�
	if(ShootGroup->ShootFlag == 1 && ShootGroup->isRunning == 0){
		if(pistol_Fire(camera) != 0){
			//�ӵ�����ʧ��
			ShootGroup->ShootFlag = 0;
		}else{
			//�ɹ����������߼�
			SSR_SetCameraExecAnim(camera,PistolAnimGroup->ShootAnimValue);
			SSRAnim_Update(camera);
			ShootGroup->isRunning = 1;
		}
	}else if(ShootGroup->isRunning == 1){//����ִ������¼�
		
		if(SSRAnim_Update(camera) == -1){
			//ִ�����һ������
			ShootGroup->ShootFlag = 0;
			ShootGroup->isRunning = 0;
			SSR_SetCameraExecAnim(camera,PistolAnimGroup->StaticAnimVale);
		}
	}
}
int main(void){
	//������ʾ��ʵ��
	MistinkPltfrmDrv_t MistinkPlatformDrivers;
	MistinkPlatformDrivers.__Init = OLED_Init;
	MistinkPlatformDrivers.__BufferOutput = OLED_BufferOutput;
	MistinkPlatformDrivers.__Clear = OLED_Clear;
	displayer = MistinkDisplayer_Create(64,128,&MistinkPlatformDrivers);
	
	//������ʼ��
	Key_Init();
	
	//�������ʼ��
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
	
	//�����ض���
	Canvas_SetCanvasModeToRedirect(MistinkOP_GetBufferAPI(displayer));
	
	//��ʼ��

	camera = Camera_Init(&camera_sturct);
	
		
	M_String(displayer,16,0,"Canvas OK");
	MistinkDisplayer_BufferOutput(displayer);
	
	//�������
	Texture_Add(1,kun2,48,48,BIT_TEXTURE);
	Texture_Add(2,kun2,48,48,BIT_TEXTURE);
	Texture_Add(3,kun2,48,48,BIT_TEXTURE);
	Texture_Add(4,kun2,48,48,BIT_TEXTURE);
	//��ӵ�ͼ
	WorldMap_Add(1,worldMap,24,24);

	M_String(displayer,32,0,"WorldMap OK");
	MistinkDisplayer_BufferOutput(displayer);
	

	/*********
	�����ʼ��
	**********/	
	
	int S_result = 0;
	//Զ�̾����ʼ��
	S_result |= LongEnemy_Init(LongS_Count,camera,1,1,0.3);
	//�����ӵ���ʼ��
 	S_result |= Basketball_Init(3,camera,2,2,1);
	//�ӵ���ʼ��
	S_result |= pistol_Init(1,camera,3,3,2);
	//���̾����ʼ��
	S_result |= CloseEnemy_Init(LongC_Count,camera,4,4,0.3);
	if( S_result != 0){
		while(1);//��ʼ��ʧ��
	}
	
	M_String(displayer,48,0,"Sprites OK");
	MistinkDisplayer_BufferOutput(displayer);
	MistinkOP_BufferReset(displayer);//ˢ�»���
	
	/*********
	֡�ʳ�ʼ��
	**********/
	
	//֡�ʲ��Գ�ʼ��
	DeltaTimeBaseStruct_t DeltaTimeBaseStruct;
	DeltaTimeBaseStruct.CalculateTimeInterval = tick;
	DeltaTimeBaseStruct.FreeHandle = NULL;
	DeltaTimeBaseStruct.PrivateData = NULL;
	DeltaTimeBaseStruct.TimeUnitConfig = frmInit;
	FrmRS_deltaTime_Init(&DeltaTimeBaseStruct);
	
	/*********
	���˽������
	**********/
	int *HP = (int*)c_malloc(sizeof(int));
	if(HP == NULL){
		while(1);
	}
	*HP = 9;
	CameraPriv_Add(camera,HP,FreeHandle);
	
	/*********
	������ͼ
	**********/
	/*�����*/
	ShootGroup_t ShootGroup;
	ShootGroup.isRunning = 0;
	ShootGroup.ShootFlag = 0;
	
	/*������*/
	PistolAnimGroup_t PistolAnimGroup;
	PistolAnimGroup.StaticAnimVale = 1;//���ö�������
	PistolAnimGroup.ShootAnimValue = 2;//��ǹ��������
	
	int top = 0;
	//��������
	top |= ShootAnim(PistolAnimGroup.ShootAnimValue,camera);//��ǹ����
	top |= StaticMap(PistolAnimGroup.StaticAnimVale,camera);//���ö���
	
	
	top |= SSR_SetCameraExecAnim(camera,PistolAnimGroup.StaticAnimVale);
	top |= SSRAnim_SetPos(camera,16,80);//������ͼλ��
	top |= SSRAnim_RenderEnable(camera);//������Ⱦ
	if(top != 0){
		while(1);//��ͼ����ʧ��
	}
	
	
	
	KeyValue_t KeyValue;//����
	while(1){
		//������������
		EnemyQuantityControl();
		
		//��������
		KeyValue = Get_Key();
		
		//������Ϊ����
		switch(KeyValue){
			case K_Left:;//��ת
				Camera_LeftPan(camera);
			break;
			case K_Right:;//��ת
				Camera_RightPan(camera);
			break;
			case K_Mid:;//����
				ShootGroup.ShootFlag = 1;
			break;
			case K_Up:;//ǰ��
				Camera_ForwardMove(camera);
			break;
			case K_Down:;//����
				Camera_BackMove(camera);
			break;
			case K_Idle:;//����
			default:;
			break;			
		}
		
		
		
		ShootTrigger(camera,&ShootGroup,&PistolAnimGroup);//�����߼������������źţ�
		
		
		/************
		��������Ⱦ��
		*************/
		//������Ⱦ
		CeruleanBird_EngineRender(camera);
		
		//M_Int(displayer,48,0,(int)FrmRS_GetFrmRate());//֡����ʾ

		//С��ͼ��ӡ
		Minimap(camera);
		//������ʾ
		if(*HP<=0){
			*HP = 9;//�޵�Ѫ��
		}
		M_Int(displayer, 0,118,*HP);//��ʾ����ֵ
		
		//������ʾ
		if(enemyCore <= 99){
			M_Int(displayer, 0,25,enemyCore);//��ʾ�÷�
		}else{
			M_String(displayer,0,25,"99+");//��ʾ�÷�
		}
		
		//�������
		MistinkDisplayer_BufferOutput(displayer);//�������
		MistinkOP_BufferReset(displayer);//ˢ�»���
	}
	
}





