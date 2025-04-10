/**
  ******************************************************************************
  * @file    Sprites_Animation.c
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    2025-2-7
  * @brief   ���ļ������˿⾫���߼��йصĺ���ʵ�֣����羫������
  ******************************************************************************
  */
#include "Sprites_internal.h"
#include "engine_config.h"
#include "RB_Tree_API.h"
#include "bit_operate.h"
#include "Camera_internal.h"//������ڲ��ӿ�
#include "WorldMap_internal.h"//�����ͼ�ڲ��ӿ�
#include "Canvas.h"		//�����ڲ��ӿ�
#include "EngineFault.h"//���������
#include "FrameRateStrategy_internal.h"//֡���Ż�

#include <string.h>
#include <stdlib.h>
#include <math.h>

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_0          //���ñ�׼��ѽ������
#include <malloc.h>
#define C_NULL NULL
#endif	//���ñ�׼��ѽ������

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //�������涯̬�ڴ�������
#include "heap_solution_1.h"

//�궨��ͳһ�����ӿ�
#define malloc(size) c_malloc(size)
#define realloc(ptr,size) c_realloc(ptr,size)
#define free(ptr)	c_free(ptr)
#endif	//�������涯̬�ڴ�������


extern RB_Node* Sprites_Header;	


/*

	  ����������߼�
			|
			V
		 ��������	
			|
			V
     �������������꣨������꣩
			|
			V
  �������Ұ����ϵ�е�����
  
	
*/

/****************************************************
	
	
	
									���������ݲ�����
	
	
	
*****************************************************/





/**********************
	�����������߲�����
***********************/
/**
*	@brief	��������ִ�д�������(��ǰ��û�д���̬����)
*	@param	
*		@arg	camera	���������
*	@retval	
*		none
*/
void* Sprites_Exec(Camera_Struct* camera){
	if(camera == C_NULL)	return C_NULL;
	//��������Ⱦ˳��
	SpritesTree_Sort(camera);
	//�𲽾������
	SpritesTreeNode_t* spritesNode = SpritesTree->head;
	while(spritesNode != NULL){
		//���¾�������
		if(Sprites_SingleUpdate(spritesNode->SpriteTrDescript.sprite, spritesNode->SpriteTrDescript.SpriteFeature) != 0){
			break;//�������ʧ�ܣ������˾���
		}
		
		//��Ⱦ�˾���
		Sprites_SingleRender(spritesNode->SpriteTrDescript.sprite ,camera);
		//���½ڵ�
		spritesNode = spritesNode->next;
	}
	//�������湲�����ݿռ�
	return camera->Engine_Common_Data.data;
}

/**
*	@brief	������״̬����
*	@param	
*		@arg	camera	���������
*	@retval	
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ���-1
*/
int Sprites_SingleUpdate(SpritesBase_t* sprite, SpriteFeature_t SpriteFeature){
	if(sprite == NULL){
		return -1;
	}
	if(SpriteFeature == NormalSprites){//��ͨ���鴦��
		return SpritesBT_BusExecute((Sprites_t*)sprite);
	}
	else if(SpriteFeature == StaticSprites){//��̬���鴦��
		SpritesAnim_Update(sprite);
	}else{
		//������<ö�ٴ������>
		EngineFault_ExceptionEnumParam_Handle();
	}
	return 0;
}



/**
*	@brief	��Bresenhamֱ���㷨��ǽ���赲̽��
*	@param	
*		@arg	x1	self�ĺ�����
*		@arg	y1	self��������
*		@arg	x2	Ŀ��ĺ�����
*		@arg	y2	Ŀ���������
*	@retval	
*		@arg	���������ǽ���赲���򷵻�0
*		@arg	���ִ�д���ǽ���赲���򷵻�-1
*/
int BresenhamLine_WallDetect(int x1, int y1, int x2, int y2) {
	
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        //��ͼ��ȡ�����������ش��󣬽������������ֹ
		//������<�Ƿ�����>
		EngineFault_IllegalAccess_Handle();
    }
	
    int dx = x2 - x1;
    int dy = y2 - y1;

    int x = x1;
    int y = y1;

    int sx = (dx > 0) ? 1 : -1;  // x��������
    int sy = (dy > 0) ? 1 : -1;  // y��������

    dx = abs(dx);
    dy = abs(dy);

    int err = (dx > dy ? dx : -dy) / 2;  // ��ʼ���ֵ
    int e2;

    while (1) {
        // ��ֹ����
        if (x == x2 && y == y2) {
            return 0;
        }
		
		//��ͼ��������ȡ
		if(WorldMap_Extract(WorldMapData,x,y) != MOVABLE_AREA_VALUE){//x����ROW��y�����ͼ��COL
			//����ǽ��
			return 1;
		}
		
        e2 = err;

        // ��������λ��
        if (e2 > -dx) {
            err -= dy;
            x += sx;
        }
        if (e2 < dy) {
            err += dx;
            y += sy;
        }
    }
}



/**********************
	����ͨ�����������
***********************/

/**
*	@brief	ʵ����һ����ͨ���������
*	@param	
*		@arg	AnimationManageIndex	����ģ�����(���������)
*		@arg	posRow	����λ�ú�����
*		@arg	posCol	����λ��������
*		@arg	defaultAnimationChainIndex ����Ĭ�϶����������
*		@arg	moveSpeed ������ƶ��ٶ�
*		@arg  map_value ��ͼ����
*		@arg  StateHandle ״̬�����
*	@retval	
*		@arg	����ʧ�ܷ��� �գ�
*		@arg	�����ɹ����� �����ַ
*/
Sprites_t* Sprites_ObjectCreate(int AnimationManageIndex,double posRow,double posCol,unsigned char defaultAnimationChainIndex,\
	double moveSpeed,int map_value,int BehaviorTreeValue){
	//�����ڴ�
	Sprites_t* sprites = (Sprites_t*)malloc(sizeof(Sprites_t));
	if(sprites == C_NULL)	return C_NULL;
		
	//λ�ó�ʼ��
	sprites->SpritesBase.posRow = posRow;
	sprites->SpritesBase.posCol = posCol;
	sprites->dir.x = 1.0;
	sprites->dir.y = 0.0;	
	sprites->SpritesBase.VerticalOffset = 0.0;
		
	//�ۻ�ֵ��ʼ��		
	sprites->SpritesBase.accumulatedTime = 0.0;	
	sprites->SpritesBase.map_value = map_value;	
	sprites->MoveSpeed = moveSpeed;		
		
	//����������
	sprites->SpritesBase.AnimMoudle.value = AnimationManageIndex;
	sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.defaultAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.currentFrmIndex = 0;
	sprites->SpritesBase.AnimMoudle.AnimationType = Anim_Once;//��ʼ��Ϊ��ѭ������
	
	//��Ϊ������
	sprites->BehaviorMoudle.value = BehaviorTreeValue;
	sprites->BehaviorMoudle.IT_Enable = 0;//Ĭ�Ϲر��ж�
	sprites->BehaviorMoudle.bt_context.node.action = NULL;
	sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;
	sprites->BehaviorMoudle.IT_running = 0;//Ĭ������Ϊ��ǰû���ж�����
	sprites->BehaviorMoudle.bt_context.Previous_IT_index = -1;//�������һ����ֵ����ǰִ�е�ռλ�жϺţ��жϺŲ���Ϊ����
	
	//��ʼ��˽������
	sprites->Private.data = C_NULL;
	sprites->Private.SpritesPrivateFreeHandle = C_NULL;

	//��ʼǽ����ײֵ����
	sprites->wall = MOVABLE_AREA_VALUE;
	
	//��¼������
	
	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,sprites->SpritesBase.AnimMoudle.value);
	if(SpritesManager == C_NULL){
		free(sprites);
		return C_NULL;//Ŀ������������ڣ����ش���ֵ
	}
	
	//��ȡĿ�궯����
	if(sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex >= SpritesManager->AnimationChainCount){
		free(sprites);
		return C_NULL;//�������������ڴ���
	}

	//�������׵�ַ���뾫������Ϊ��ʼ������
	if(SpritesManager->SpritesRecordGroup == C_NULL){
		//�����������ڶ���֡
		free(sprites);
		return C_NULL;
	}
	
	//��ȡ�׸�����֡
	sprites->SpritesBase.AnimMoudle.ReadyAnimation = SpritesManager->SpritesRecordGroup[defaultAnimationChainIndex].AnimationChainGroup;
	sprites->SpritesBase.AnimMoudle.SpritesManager = SpritesManager;
	
	SpritesManager->SpritesCount++;
	
	
	SpritesTree_Add(sprites);
	
	//���ؾ�����
	return sprites;
}



/**
*	@brief	���þ���ķ���
*	@param	
*		@arg	sprites ������
*	@retval	
*		@arg	���óɹ����� 0
*		@arg	����ʧ�ܷ��� -1
*/
int SpritesMov_SetDir(Sprites_t* sprites,vector dir){
	//�������
	if(sprites == NULL){
		return -1;
	}
	// ��ֵ���
	if ((dir.x > -1e-9 && dir.x < 1e-9) && (dir.y > -1e-9 && dir.y < 1e-9)) {
		return -1;//������������
	}
	//��һ����������
	sprites->dir = vector_Normalize(&dir);
	return 0;
}
/**
*	@brief	��ȡ���鷽��
*	@param	
*		@arg	sprites ������
*	@retval	
*		@arg	���ؾ��鷽������(��ָ����ʻᵼ����������������״̬)
*/
vector SpritesMov_GetDir(Sprites_t* sprites){
	//�������
	if(sprites == NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	return sprites->dir;
}
/**
*	@brief	�����鷽������Ϊ���������
*	@param	
*		@arg	sprites ������
*		@arg	camera	�����
*	@retval	
*		@arg	���óɹ����� 0
*		@arg	����ʧ�ܷ��� -1
*/
int SpritesMov_SetDirToCamera(Sprites_t* sprites,Camera_Struct* camera){
	//�������
	if(sprites == NULL || camera == NULL){
		return -1;
	}
	//��������
	vector dir;
	dir.x = camera->position.x - sprites->SpritesBase.posRow;
	dir.y = camera->position.y - sprites->SpritesBase.posCol;
	
	//��֤�Ƿ����������غ�
	if(( dir.x  > -1e-9 && dir.x < 1e-9) && (dir.y > -1e-9 && dir.y < 1e-9)){
		return -1;//�����غ�Ϊ����ʧ��
	}
	//��һ����������
	sprites->dir = vector_Normalize(&dir);
	return 0;
}
/**
*	@brief	�����鷽������Ϊ�������һ��
*	@param	
*		@arg	sprites ������
*		@arg	camera	�����
*	@retval	
*		@arg	���óɹ����� 0
*		@arg	����ʧ�ܷ��� -1
*/
int SpritesMov_SetDirWithCamera(Sprites_t* sprites,Camera_Struct* camera){
	//�������
	if(sprites == NULL || camera == NULL){
		return -1;
	}
	sprites->dir = camera->direction;
	return 0;
}


/**
*	@brief	���鳯����ƶ�(��������ǽ��)
*	@param	
*		@arg	sprites ������
*	@retval	
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ��� -1
*/
int SpritesMov_Left(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // ��ͼ���ݻ�ȡ����
    }

    // ����Ϊ֡���޹��ƶ��ٶ�
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // �������Ʒ��������
    vector leftDir;
    leftDir.x = -sprite->dir.y; // ���ƴ�ֱ�ڵ�ǰ����
    leftDir.y = sprite->dir.x;

    // ����Ƿ�����ƶ�����λ��
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow + leftDir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol + leftDir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow += leftDir.x * correctionSpeed;
        sprite->SpritesBase.posCol += leftDir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}


/**
*	@brief	���鳯�Ҳ��ƶ�(��������ǽ��)
*	@param	
*		@arg	sprites ������
*	@retval	
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ��� -1
*/
int SpritesMov_Right(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // ��ͼ���ݻ�ȡ����
    }

    // ����Ϊ֡���޹��ƶ��ٶ�
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // �������Ʒ��������
    vector rightDir;
    rightDir.x = sprite->dir.y;  // ���ƴ�ֱ�ڵ�ǰ����
    rightDir.y = -sprite->dir.x;

    // ����Ƿ�����ƶ�����λ��
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow + rightDir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol + rightDir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow += rightDir.x * correctionSpeed;
        sprite->SpritesBase.posCol += rightDir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}


/**
*	@brief	���鳯ǰ���ƶ�(��������ǽ��)
*	@param	
*		@arg	sprites ������
*	@retval	
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ��� -1
*/
int SpritesMov_Forward(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // ��ͼ���ݻ�ȡ����
    }

    // ����Ϊ֡���޹��ƶ��ٶ�
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // ����Ƿ�����ƶ�����λ��
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow + sprite->dir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol + sprite->dir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow += sprite->dir.x * correctionSpeed;
        sprite->SpritesBase.posCol += sprite->dir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}


/**
*	@brief	���鳯���ƶ�(��������ǽ��)
*	@param	
*		@arg	sprites ������
*	@retval	
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ��� -1
*/
int SpritesMov_Backward(Sprites_t* sprite) {
    if (sprite == NULL) return -1;

    WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
    if (WorldMapData == ((void*)0)) {
        return -1; // ��ͼ���ݻ�ȡ����
    }

    // ����Ϊ֡���޹��ƶ��ٶ�
    double correctionSpeed = sprite->MoveSpeed * DeltaTime->deltaTime;

    // ����Ƿ�����ƶ�����λ��
	int wall = WorldMap_Extract(WorldMapData,
                         (int)(sprite->SpritesBase.posRow - sprite->dir.x * correctionSpeed),
                         (int)(sprite->SpritesBase.posCol - sprite->dir.y * correctionSpeed));
    if (wall == MOVABLE_AREA_VALUE) {
        sprite->SpritesBase.posRow -= sprite->dir.x * correctionSpeed;
        sprite->SpritesBase.posCol -= sprite->dir.y * correctionSpeed;
    }else{
		sprite->wall = wall;
	}

    return 0;
}

/**
*	@brief	��ȡ������ײ��ǽ�ڵ�ֵ
*	@param	
*		@arg	sprites ������
*	@retval	
*		���ؾ�����ײ��ǽ��
*/
int Sprites_collisionWallValue(Sprites_t* sprite){
	if(sprite == NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	int wall = sprite->wall;
	sprite->wall = MOVABLE_AREA_VALUE;//�����µ�ֵ
	return wall;
}

/**
*	@brief	���þ����������λ���غ�λ��
*	@param	
*		@arg	sprites ������
*		@arg	camera �����
*	@retval	
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ��� -1
*/
int SpritesMov_SetPosToCamera(Sprites_t* sprites,Camera_Struct* camera){
	if(sprites == NULL || camera == NULL) return -1;
	sprites->SpritesBase.posRow = camera->position.x;
	sprites->SpritesBase.posCol = camera->position.y;
	return 0;
}


/**********************
	����̬�����������
***********************/

/**
*	@brief	ʵ����һ����̬���������
*	@param	
*		@arg	AnimationManageIndex	����Ԫ�ش��ţ����������������
*		@arg	posRow	����λ�ú�����
*		@arg	posCol	����λ��������
*		@arg	defaultAnimationChainIndex ����Ĭ�϶����������
*		@arg  	map_value ��ͼ����
*	@retval	
*		@arg	����ʧ�ܷ��� �գ�
*		@arg	�����ɹ����� �����ַ
*/
SpritesStatic_t* SpritesStatic_ObjectCreate(int AnimationManageIndex,double posRow,double posCol,unsigned char defaultAnimationChainIndex,int map_value){
	//�����ڴ�
	SpritesStatic_t* sprites = (SpritesStatic_t*)malloc(sizeof(SpritesStatic_t));
	if(sprites == C_NULL)	return C_NULL;
	//��ʼ��
	sprites->SpritesBase.posRow = posRow;
	sprites->SpritesBase.posCol = posCol;	
	sprites->SpritesBase.accumulatedTime = 0.0;
	sprites->SpritesBase.map_value = map_value;	
	sprites->SpritesBase.VerticalOffset = 0.0;
		
	sprites->SpritesBase.AnimMoudle.value = AnimationManageIndex;
	sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.defaultAnimationChainIndex = defaultAnimationChainIndex;
	sprites->SpritesBase.AnimMoudle.currentFrmIndex = 0;
	sprites->SpritesBase.AnimMoudle.AnimationType = Anim_Once;//��ʼ��Ϊ��ѭ������

	
	//��¼������
	
	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,sprites->SpritesBase.AnimMoudle.value);
	if(SpritesManager == C_NULL){
		free(sprites);
		return C_NULL;//Ŀ������������ڣ����ش���ֵ
	}
	
	//��ȡĿ�궯����
	if(sprites->SpritesBase.AnimMoudle.CurrentAnimationChainIndex >= SpritesManager->AnimationChainCount){
		free(sprites);
		return C_NULL;//�������������ڴ���
	}

	//�������׵�ַ���뾫������Ϊ��ʼ������
	if(SpritesManager->SpritesRecordGroup == C_NULL){
		//�����������ڶ���֡
		free(sprites);
		return C_NULL;
	}
	
	//��ȡ�׸�����֡
	sprites->SpritesBase.AnimMoudle.ReadyAnimation = SpritesManager->SpritesRecordGroup[defaultAnimationChainIndex].AnimationChainGroup;
	sprites->SpritesBase.AnimMoudle.SpritesManager = SpritesManager;
	
	SpritesManager->SpritesCount++;
	
	//���뾫����
	SpritesTree_StaticAdd(sprites);
	
	//���ؾ�����
	return sprites;
		
}

/**********************
	������ͨ�ò�����
***********************/
/**
*	@brief	��ȡָ�������λ��
*	@param	
*		@arg	sprites	�������
*	@ret	
*		@arg	�����ɹ����� λ��
*		@arg	�����쳣���������̬��
*/
vector SpritesData_GetPos(SpritesBase_t* sprites){
		if(sprites == C_NULL){
			//������<��ָ�����>
			EngineFault_NullPointerGuard_Handle();
		}
		vector pos;
		pos.x = sprites->posRow;
		pos.y = sprites->posCol;
		return pos;
}
/**
*	@brief	���þ���Ĵ�ֱ�����ƫ����
*	@param	
*		@arg	sprites	�������
*		@arg	VerticalHeight ���鴹ֱƫ����(��������ƫ�ƣ���������ƫ�ƣ���λ������)
*	@retval	
*		@arg	����ʧ�ܷ��� -1��
*		@arg	�����ɹ����� 0
*/
int SpritesData_SetVerticalHeight(SpritesBase_t* sprites,int VerticalHeight){
		if(sprites == C_NULL){
				return -1;
		}
		sprites->VerticalOffset = VerticalHeight;
		return 0;
}
/**
*	@brief	��ȡ����Ĵ�ֱ�����ƫ����
*	@param	
*		@arg	sprites	�������
*	@retval	
*		@arg	���ش�ֱƫ����(��������ƫ�ƣ���������ƫ�ƣ���λ������)
*		�����쳣�����������̬
*/
int SpritesData_GetVerticalHeight(SpritesBase_t* sprites){
	if(sprites == C_NULL){
				//������<��ָ�����>
			EngineFault_NullPointerGuard_Handle();
		}
		return sprites->VerticalOffset;
}

/**
*	@brief		���鴹ֱ����
*	@param	
*		@arg	sprites	�������
*		@arg	VerticalHeight �������Ƶ�ƫ����(������ڵ���0����λ������) 
*	@retval	
*		@arg	����ʧ�ܷ��� -1��
*		@arg	�����ɹ����� 0
*/
int SpritesData_VerticalHeight_MoveUp(SpritesBase_t* sprites,int VerticalHeight){
		if(sprites == C_NULL || VerticalHeight < 0){
				return -1;
		}
		sprites->VerticalOffset += VerticalHeight;
		return 0;
}
/**
*	@brief		���鴹ֱ����
*	@param	
*		@arg	sprites	�������
*		@arg	VerticalHeight �������Ƶ�ƫ����(������ڵ���0����λ������) 
*	@retval	
*		@arg	����ʧ�ܷ��� -1��
*		@arg	�����ɹ����� 0
*/
int SpritesData_VerticalHeight_MoveDown(SpritesBase_t* sprites,int VerticalHeight){
		if(sprites == C_NULL || VerticalHeight < 0){
				return -1;
		}
		sprites->VerticalOffset -= VerticalHeight;
		return 0;
}

/**********************
	��˽�����ݲ�����
***********************/
/**
*	@brief	��ָ��������������˽������
*	@param	
*		@arg	sprites	��ͨ�������
*		@arg	data	  ˽������
*		@arg	FreeHandle	˽�����ݴ���
*	@retval	
*		@arg	����ʧ�ܷ��ش���ֵ -1��
*		@arg	�����ɹ����� 0
*/
int SpritesPriv_Add(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle){
	if(sprites == C_NULL || sprites->Private.data != C_NULL)	return -1;
	sprites->Private.data = data;
	sprites->Private.SpritesPrivateFreeHandle = FreeHandle;
	return 0;
}
/**
*	@brief	�滻ָ����������е�˽������
*	@param	
*		@arg	sprites	��ͨ�������
*		@arg	data	  ˽������
*		@arg	FreeHandle	˽�����ݴ���
*	@retval	
*		@arg	����ʧ�ܷ��ش���ֵ -1��
*		@arg	�����ɹ����� 0
*/
int SpritesPriv_Replace(Sprites_t* sprites,void* data,SpritesPrivateFreeHandle_t FreeHandle){
	if(sprites == C_NULL)	return -1;
	SpritesPriv_Reset(sprites);
	sprites->Private.data = data;
	sprites->Private.SpritesPrivateFreeHandle = FreeHandle;
	return 0;
}
/**
*	@brief	����ָ�������˽������(˽�����ݸ�λΪ�գ�ͬʱ������Դ���վ��)
*	@param	
*		@arg	sprites	��ͨ�������
*	@retval	
*		none
*/
void SpritesPriv_Reset(Sprites_t* sprites){
		if(sprites == C_NULL)	return;
	
		if(sprites->Private.SpritesPrivateFreeHandle != C_NULL){
				sprites->Private.SpritesPrivateFreeHandle(sprites->Private.data);
		}
	
		sprites->Private.data = C_NULL;
		sprites->Private.SpritesPrivateFreeHandle = C_NULL;
	
		return;
}

/**
*	@brief	��ȡָ����������е�˽������
*	@param	
*		@arg	sprites	��ͨ�������
*	@retval	
*		@arg	����ʧ�ܷ��� �գ�
*		@arg	�����ɹ����� ���ݵ�ַ
*/
void* SpritesPriv_Get(Sprites_t* sprites){
	return (sprites == C_NULL ? C_NULL :sprites->Private.data);
}


	







/**************************************************

 ��Ⱦ

***************************************************/


double CalculateStepDistance(double x1, double y1, double x2, double y2) {
    // ���߷������
    double RayX = x2 - x1;
    double RayY = y2 - y1;

    // �������ȼ���
    double stepLengthX = fabs(1 / RayX);
    double stepLengthY = fabs(1 / RayY);
	if(stepLengthX < 1e-9 || stepLengthY < 1e-9){
		return 1e-9;
	}

    double distance = 0.0; // �ܲ�������
    double currentX = x1; // ��ǰX����
    double currentY = y1; // ��ǰY����

    // ģ�����������Ŀ���
    while (fabs(currentX - x2) > stepLengthX || fabs(currentY - y2) > stepLengthY) {
        if (fabs(x2 - currentX) > fabs(y2 - currentY)) {
            currentX += stepLengthX * ((x2 > currentX) ? 1 : -1);
        } else {
            currentY += stepLengthY * ((y2 > currentY) ? 1 : -1);
        }
        distance += sqrt(stepLengthX * stepLengthX + stepLengthY * stepLengthY); // �ۼӲ�������
    }
    return distance;
}


/**
 * 	@brief �Ե����������Ⱦ���Ѿ�����ǽ��͸�����⣩
 *	@param	
 *		@arg	sprites	�������
 *		@arg	camera	���������
 *	@retval	
 *		none
 * �޸�˵����
 * 1. ����ԭ������ת����͸�Ӽ��㹫ʽ��ȷ���������������ϵ�е�λ�ü���ȼ��㲻�䡣
 * 2. ����ƫ�Ƽ������ԭ���빫ʽ�������޸Ľ����������ϵ������ľ���λ��ƫ�
 * 3. ������Ծ��pixelSkip�����ָ�Ϊ������������Ļ�ϳߴ�ϴ�ʱ������ֱ�ӷ��أ�
 *    ���ǲ��ýϴ������ֵ���Ӷ����ܽ��������Ż����ֲ�Ӱ���ϲ��߼�ִ�н��ࡣ
 * 4. �ü�������ӳ���߼�����ԭ���뷽ʽ�����С����м����������꣬ȷ�����Ʋ�������α䡣
 */
void Sprites_SingleRender(SpritesBase_t* sprites, Camera_Struct* camera) {
    // ����ת��������������ת��Ϊ����ֲ�����ϵ
    double relativeCoordSys_Row = sprites->posRow - camera->position.x;
    double relativeCoordSys_Col = sprites->posCol - camera->position.y;

    // ʹ�����ƽ���뷽����������任ϵ��
    double coefficient = 1.0 / (camera->plane.x * camera->direction.y - camera->direction.x * camera->plane.y);

    // ���㾫������������ϵ���ȣ�Dir_component��
    double Dir_component = coefficient * (-camera->plane.y * relativeCoordSys_Row + camera->plane.x * relativeCoordSys_Col);
    if (Dir_component <= 0) {
        // ����������󷽣�ֱ�ӷ���
        return;
    }

    // ���㾫�������ƽ�棨���򣩵�����
    double Plane_component = coefficient * (camera->direction.y * relativeCoordSys_Row - camera->direction.x * relativeCoordSys_Col);

    // ����ƫ�Ʋ���ԭ���빫ʽ����֤����ˮƽλ����ȷ
    int HorizontalOffset = (int)((SCREEN_COLUMN / 2) * (1 + Plane_component / Dir_component));
    // ��ֱƫ�Ʊ���ԭ�����߼�
    int VerticalOffset = (sprites->VerticalOffset == 0) ? 0 : -(int)(sprites->VerticalOffset / Dir_component);

    // ���㾫������Ļ�ϵĸ߶ȺͿ�ȣ�͸�����ţ�
    int spriteHeight = abs((int)(sprites->AnimMoudle.SpritesManager->
                        SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Logic_ROW / Dir_component));
    int spriteWidth  = abs((int)(sprites->AnimMoudle.SpritesManager->
                        SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Column / Dir_component));

    // ���㴹ֱ�������򣬾��в����ϴ�ֱƫ��
    int drawStartY = -spriteHeight / 2 + SCREEN_ROW / 2 + VerticalOffset;
    int drawEndY   = spriteHeight / 2 + SCREEN_ROW / 2 + VerticalOffset;

    // ��ֱ�ü���ͬʱ������Ӧ�������꣨texStartY, texEndY��
    int texStartY = 0;
    int texEndY = sprites->AnimMoudle.SpritesManager->
                  SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Logic_ROW;
    if (drawStartY < 0) {
        texStartY = (-drawStartY) * texEndY / (drawEndY - drawStartY);
        drawStartY = 0;
    }
    if (drawEndY >= SCREEN_ROW) {
        texEndY = texEndY - ((drawEndY - SCREEN_ROW + 1) * texEndY) / (drawEndY - drawStartY);
        drawEndY = SCREEN_ROW - 1;
    }

    // ����ˮƽ��������
    int drawStartX = -spriteWidth / 2 + HorizontalOffset;
    int drawEndX = spriteWidth / 2 + HorizontalOffset;
    int texStartX = 0;
    int texEndX = sprites->AnimMoudle.SpritesManager->
                  SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].Column;
    if (drawStartX < 0) {
        texStartX = (-drawStartX) * texEndX / (drawEndX - drawStartX);
        drawStartX = 0;
    }
    if (drawEndX >= SCREEN_COLUMN) {
        texEndX = texEndX - ((drawEndX - SCREEN_COLUMN + 1) * texEndX) / (drawEndX - drawStartX);
        drawEndX = SCREEN_COLUMN - 1;
    }

    // �������������Ч����ֱ���˳�
    if (drawStartX > drawEndX || drawStartY > drawEndY) {
        return;
    }

    // ��������ӳ��ʱÿ�к�ÿ�еĲ���
    double step_col = (texEndX - texStartX) / (double)(drawEndX - drawStartX);
    double step_row = (texEndY - texStartY) / (double)(drawEndY - drawStartY);

    // ����������Ļ�ϵĳߴ�ϴ�ʱ��Ϊ������Ч�ʲ�����Ծ����
    int pixelSkip = 1;
    if (spriteHeight > SCREEN_ROW || spriteWidth > SCREEN_COLUMN) {
        double differHight = spriteHeight / (double)SCREEN_ROW;
        double differWidth  = spriteWidth / (double)SCREEN_COLUMN;
        if (differHight > 1.5 || differWidth > 1.5) {
            // ԭ����ֱ�ӷ��ؿ��ܵ����ϲ��߼�ִ�й��죬�����Ϊ���ýϴ�����ֵ���Ա�����Ⱦ����
            pixelSkip = 16;
        }
        else if (differHight > 1.35 || differWidth > 1.35) {
            pixelSkip = 8;
        }
        else if (differHight > 1.2 || differWidth > 1.2) {
            pixelSkip = 4;
        }
        else {
            pixelSkip = 2;
        }
    }

    // ���㾫�������������ŷ�Ͼ��룬����������ȼ��
    double dx = sprites->posRow - camera->position.x;
    double dy = sprites->posCol - camera->position.y;
    double dis = sqrt(dx * dx + dy * dy);

    // ����ӳ��ͻ��ƣ����б�����ʹ�� pixelSkip ���в����Ż���
    double texX = texStartX;
    for (int col = drawStartX; col <= drawEndX; col += pixelSkip) {
		//Խ��Լ��
        if (col < 0 || col >= SCREEN_COLUMN) continue;
           
        // ������ȼ�⣺ֻ�е��������С�ڵ�ǰ��������ʱ�Ż��Ƹ���
        if (dis < Camera_DeepBuffer_Read(camera, col)) {
            double currTexX = texX;  // ��֤ÿ�ж��������������
            double texY = texStartY;
            for (int row = drawStartY; row <= drawEndY; row += pixelSkip) {
                int signal = SpritesAnim_ExtractFrmData(sprites, (int)texY, (int)currTexX);
                if (signal == 0)
                    Canvas_PixelOperate(row, col, CANVAS_PIXEL_CLOSE);
                else if (signal == 1)
                    Canvas_PixelOperate(row, col, CANVAS_PIXEL_OPEN);
                texY += step_row * pixelSkip;
            }
        }
        texX += step_col * pixelSkip;
    }
}


/**
*	@brief	����һ������
*	@param	
*		@arg	Sprite ����
*	@retval	
*		@arg	�����ɹ�����0
*		@arg	����ʧ�ܷ���-1
*/
int Sprites_Sleep(SpritesBase_t* Sprite){
	if(Sprite == NULL)	return -1;
	
	//��ȡ����()
	SpritesTreeNode_t* node = SpritesTree_NodeExtract(Sprite,SpritesTree);
	if(node == C_NULL){
		return -1;//�����ȡʧ����ζ�ž����ַ�������ڣ������Ѿ�����
	}
	
	//���뵽��������
	node->next = SpritesSleepTree->head;
	SpritesSleepTree->count++;
	SpritesSleepTree->head = node;
	
	return 0;
}


/**
*	@brief	����һ������
*	@param	
*		@arg	Sprite ����
*	@retval	
*		@arg	�����ɹ�����0
*		@arg	����ʧ�ܷ���-1
*/
int Sprites_Awaken(SpritesBase_t* Sprite){
	if(Sprite == NULL)	return -1;
	
	//��ȡ����()
	SpritesTreeNode_t* node = SpritesTree_NodeExtract(Sprite,SpritesSleepTree);
	if(node == C_NULL){
		return -1;//�����ȡʧ����ζ�ž����ַ�������ڣ������Ѿ�����
	}
	
	//���뵽��������
	node->next = SpritesTree->head;
	SpritesTree->count++;
	SpritesTree->head = node;
	return 0;
}


/**
*	@brief	��ָ�����鵱ǰ��֡�������ţ����������ű�����ȡ��Դ֡������״̬(ֻ�����������Ŵ�)
*	@param	
*		@arg	spritesNode	����ڵ�
*		@arg	ROW_Pixel	�����������
*		@arg	row_div	�е����ű���	
*		@arg	col_div	�е����ű���
*	@retval	
*		@arg	��ʾ͸��ֵ ����-1��
*		@arg	��ʾ��ɫֵ ����0��
*		@arg	��ʾ��ɫֵ ����1��
*/
//int SpritesAnim_AdjustScale(SpritesBase_t* sprites,int ROW_Pixel,int COL_Pixel,int row_div,int col_div){
//	
//	//SpritesAnim_ExtractFrmData:��sprites�Ķ�ά��ɫ����ͼ����ָ����������ص���ж�ȡ���������1����������ص�������������0����������ص����������-1���򱣳�֮ǰ������״̬�����ı�
//	int result = SpritesAnim_ExtractFrmData(TO_SPRITES_BASE(sprites),ROW_Pixel,COL_Pixel);

//}

















/**************************************************

������

***************************************************/

SpritesTree_t* SpritesTree = C_NULL;	
SpritesTree_t* SpritesSleepTree = C_NULL;

/**
*	@brief	��������ʼ��
*	@param	
*		none
*	@retval	
*		@arg	�����ɹ� ����0��
*		@arg	����ʧ�� ����-1��
*/
int SpritesTree_Init(void){
	if(SpritesTree != C_NULL)	return 0;
	//��������ʼ��
	SpritesTree = (SpritesTree_t*)malloc(sizeof(SpritesTree_t));
	if(SpritesTree == C_NULL){
		return -1;
	}
	SpritesTree->count = 0;
	SpritesTree->head = C_NULL;
	
	//��������ʼ��
	SpritesSleepTree = (SpritesTree_t*)malloc(sizeof(SpritesTree_t));
	if(SpritesSleepTree == C_NULL){
		free(SpritesTree);
		SpritesTree = C_NULL;
		return -1;
	}
	SpritesSleepTree->count = 0;
	SpritesSleepTree->head = C_NULL;
	return 0;
}
/**
*	@brief	���������ؾ���(���ܹ�����ͨ����)
*	@param	
*		@arg	sprites �������
*	@retval	
*		@arg	�����ɹ� ����0��
*		@arg	����ʧ�� ����-1��
*/
int SpritesTree_Add(Sprites_t* sprites){
	if(sprites == C_NULL || SpritesTree == C_NULL)	return -1;
	SpritesTreeNode_t* node = (SpritesTreeNode_t*)malloc(sizeof(SpritesTreeNode_t));
	if(node == C_NULL)	return -1;
	node->next = SpritesTree->head;
	node->SpriteTrDescript.sprite = (SpritesBase_t*)sprites;
	node->SpriteTrDescript.SpriteFeature = NormalSprites;
	SpritesTree->count++;
	SpritesTree->head = node;
	return 0;
}
/**
*	@brief	���������ؾ�̬����
*	@param	
*		@arg	sprites �������
*	@retval	
*		@arg	�����ɹ� ����0��
*		@arg	����ʧ�� ����-1��
*/
int SpritesTree_StaticAdd(SpritesStatic_t* sprites){
	if(sprites == C_NULL || SpritesTree == C_NULL)	return -1;
	SpritesTreeNode_t* node = (SpritesTreeNode_t*)malloc(sizeof(SpritesTreeNode_t));
	if(node == C_NULL)	return -1;
	node->next = SpritesTree->head;
	node->SpriteTrDescript.sprite = (SpritesBase_t*)sprites;
	node->SpriteTrDescript.SpriteFeature = StaticSprites;
	SpritesTree->count++;
	SpritesTree->head = node;
	return 0;
}

/**
*	@brief	�����������Զ��������
*	@param	
*		@arg	camera ���������
*	@retval	
*		none
*/
void SpritesTree_Sort(Camera_Struct* camera) {
    if (SpritesTree->head == NULL || SpritesTree->head->next == NULL) {
        return;
    }
    int swapped;
    SpritesTreeNode_t* ptr1;
    SpritesTreeNode_t* lptr = NULL;

    // ð������
    do {
        swapped = 0;
        ptr1 = SpritesTree->head;

        while (ptr1->next != lptr) {
            SpritesBase_t* sprite1 = (SpritesBase_t*)(ptr1->SpriteTrDescript.sprite);
            SpritesBase_t* sprite2 = (SpritesBase_t*)(ptr1->next->SpriteTrDescript.sprite);

            float distance1 = SpritesTree_DistanceFromplayer(sprite1, camera);
            float distance2 = SpritesTree_DistanceFromplayer(sprite2, camera);

            // �޸ıȽ��������Ӵ�С����
            if (distance1 < distance2) {
								SpriteTrDescript_t temp = ptr1->SpriteTrDescript;
								ptr1->SpriteTrDescript = ptr1->next->SpriteTrDescript;
								ptr1->next->SpriteTrDescript = temp;
							
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

/**
*	@brief	�Ӿ���������ȡ��һ���ڵ�
*	@param	
*		@arg	Sprite ����
*	@retval	
*		@arg	�����ɹ����� �ڵ��ַ
*		@arg	����ʧ�ܷ��� ��
*/
SpritesTreeNode_t* SpritesTree_NodeExtract(SpritesBase_t* Sprite, SpritesTree_t* Tree) {
    if (Sprite == NULL || Tree == NULL || Tree->head == NULL) {
        return C_NULL; // �����������Ƿ���Ч
    }

    SpritesTreeNode_t* current = Tree->head;
    SpritesTreeNode_t* previous = C_NULL;

    // ���������ҵ�Ŀ��ڵ�
    while (current != C_NULL) {
        if (current->SpriteTrDescript.sprite == Sprite) {
            // �ҵ�ƥ��ڵ�
            if (previous == C_NULL) {
                // �����ͷ�ڵ�
                Tree->head = current->next;
            } else {
                // �������ͷ�ڵ�
                previous->next = current->next;
            }

            // �������������
            Tree->count--;

            // ���ڵ�����������
            current->next = C_NULL;
            return current; // �����ҵ��Ľڵ�
        }

        // ��������
        previous = current;
        current = current->next;
    }

    return C_NULL; // ���δ�ҵ�ƥ��ڵ�
}



















