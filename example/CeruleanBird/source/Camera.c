#include "Camera.h"

#include <math.h>

#include "TextureMapper_internal.h"//�����ڲ��ӿ�

#include "Canvas_API.h"			//�����ӿ�

#include "WorldMap_internal.h"	//��ͼ�ڲ��ӿ�

#include "FrameRateStrategy_internal.h"//��tģ������֡���޹�

#include "EngineFault.h"//���������

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

#define UNIT_PLANE_VERTICAL			0								//��Ԫ��������棨�����棩
#define	UNIT_PLANE_HORIZONTAL		1								//��Ԫ���ˮƽ�棨�����棩



/**
*	@brief	��ʼ�����������ͨ����
*	@param
*		@arg	Camera_InitStruct		�����ʼ���ṹ��
*	@retval
*		@arg	��ʼ���ɹ����������ַ
*		@arg	��ʼ��ʧ�ܷ��ؿ�
*/
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct){
	//�������
	if(Camera_InitStruct == C_NULL) return C_NULL;
	if(Camera_InitStruct->FOV <= 0 || Camera_InitStruct->FOV >= 180) return C_NULL;//��������ͷ���ӽ���0�ȵ�180��֮�䣬������0��180��
	 
	Camera_Struct* camera = (Camera_Struct*)malloc(sizeof(Camera_Struct) + sizeof(DeepBuffer_t)*SCREEN_COLUMN);
	if(camera == C_NULL)	return C_NULL;
	//�û��������Ը�ֵ
	//���޴�����׵ĸ�ֵ
	camera->moveSpeed = Camera_InitStruct->moveSpeed;
	camera->rotSpeed = Camera_InitStruct->rotSpeed * 3.0;//���ƶ��ٶȳ���PIֵ
	camera->position = Camera_InitStruct->position;
	camera->direction = vector_Normalize(&Camera_InitStruct->direction);//��һ����������
	
	//˽�����ݳ�ʼ��
	camera->Camera_PrivateData.data = C_NULL;
	camera->Camera_PrivateData.__FreeHandle = C_NULL;
	camera->Engine_Common_Data.data = C_NULL;
	camera->Engine_Common_Data.__FreeHandle = C_NULL;
	
	//������Ȼ�����
	camera->DeepBuffer = (DeepBuffer_t*)((char*)camera + sizeof(Camera_Struct));
	//��ȳ�ʼ����Ϊ�����
	for(int i = 0;i < SCREEN_COLUMN;i++){
		camera->DeepBuffer[i] = POSITIVE_INFINITY;
	}
	
	//����5����ʼ��
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
	
	//����FOVת��Ϊƽ�淨����
	double FOV_Radians = Camera_InitStruct->FOV * (M_PI / 180.0);// ��FOV�Ӷ���ת��Ϊ����
	//�����º���ͨ��������֤������������0.0001��Ρ�
	double planeX = camera->direction.y * tan(FOV_Radians / 2);
	double planeY = -camera->direction.x * tan(FOV_Radians / 2);
	camera->plane = vector_Create(planeX,planeY);
	//�����ķ�ƽ�������ĳ���ֻ�мȶ����ȵ�һ�룬������FOV��Ҫ10��λ���ȣ����Ƿ�ƽ�泤��Ϊ5��
	//���ҷ�ƽ�淽��Ϊ����������������
	//���������������ڼ����ʱ�򣬽����������������һ���ӽǡ�
	return camera;
}



/**
*	@brief	��ȡ���λ��
*	@param
*		@arg	camera		���
*	@retval
*		����������꣬������������������
*/
vector Camera_GetPos(Camera_Struct* camera){
	if(camera == NULL){
		//������<��ָ�����>
	EngineFault_NullPointerGuard_Handle();
	}
	return camera->position;
}



/**
*	@brief	��֡������ӽǻ���ģ����Ⱦ
*	@param
*		@arg	camera		���
*	@retval
*		none
*/
void Camera_Render(Camera_Struct* camera){
	
	//�컨���������Ⱦ
	camera->Floor.Floor_RenderHandle(camera);
	camera->Ceil.Ceil_RenderHandle(camera);
	
	//��ȡ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	
	//������Ⱦ
	for(int x = 0; x < SCREEN_COLUMN; x++){
		//����ӳ�䵽������ӽ�,��Ϊ[-1,1]��ƫ��������Χӳ�����������Է�Χ����-1��1֮�䣬��ӳ�䲻��ȫΪ���ԣ��еĴ���1�Ĳ����еĴ���2�Ĳ������Ʋ���������¡�
		double VisualAngleOffset =  x * 2 /(double)(SCREEN_COLUMN - 1) - 1;//
		
		
		//���������������ƺ��������⣬����66����Ұ���Ʋ���߷���Ӧ�ò������1�����Ǵ��ڴ���1�������
		double RayX = camera->plane.x * VisualAngleOffset + camera->direction.x;//x����Ĺ��߷���
		double RayY = camera->plane.y * VisualAngleOffset + camera->direction.y;//y����Ĺ��˷���
		

		
		//����X���򲽽�����
		double stepLengthX = fabs(1 / RayX);
		//����Y���򲽽�����
		double stepLengthY = fabs(1 / RayY);
		//�������ȹ�С������������Ⱦ
		if(stepLengthX < 1e-9 || stepLengthY < 1e-9){
			continue;	//����Сֵ1e-9����0�Ľ��ޣ�С�������������������
		}

		
		//����x���׼����
		double standardDistanceX;
		//����y���׼����
		double standardDistanceY; 

		//����X��Ĳ�������
		int stepDirX;
		//����Y��Ĳ�������
		int stepDirY;
		
		//����DDA���������ĵ�Ԫ��
		int UnitX = (int)camera->position.x;
		int UnitY = (int)camera->position.y;
		
		//X�᷽�����ó�ʼֵ�Ͳ���
		if(RayX >= 0){
			//�����������죬��������Ϊ��
			stepDirX = 1;
			//ͨ�����������������׸���ǰ��Ԫ��X���׼����
			standardDistanceX = (UnitX + 1 - camera->position.x) * stepLengthX;
		}else {
			//���߸������죬��������Ϊ��
			stepDirX = -1;
			//ͨ�����������������׸���ǰ��Ԫ��X���׼����
			standardDistanceX = (camera->position.x - UnitX) * stepLengthX;
		}
		
		//Y�᷽�����ó�ʼֵ�Ͳ���
		if(RayY >= 0){
			//�����������죬��������Ϊ��
			stepDirY = 1;
			//ͨ�����������������׸���ǰ��Ԫ��Y���׼����
			standardDistanceY = (UnitY + 1 - camera->position.y) * stepLengthY;	
		}else {
			//���߸������죬��������Ϊ��
			stepDirY = -1;
			//ͨ�����������������׸���ǰ��Ԫ��Y���׼����
			standardDistanceY = (camera->position.y - UnitY) * stepLengthY;
		}
		
		//���嵥Ԫ������
		int UnitAttribute = MOVABLE_AREA_VALUE;
		//���嵥Ԫ�񱻹��߻��еķ���
		char UnitDir = UNIT_PLANE_VERTICAL;
		//���й���ɨ�衾�ϰ�ʶ��������ɨ���յ㶼ͣ������Ԥ�ڵ�λ�á�
		while(UnitAttribute == MOVABLE_AREA_VALUE){
			//��鲢ѡ���׼����С��һ��
			if(standardDistanceX < standardDistanceY){
				standardDistanceX += stepLengthX;
				UnitX += stepDirX;//���÷����з�������
				//���ߴ�X����е�Ԫ�񣬵�Ԫ�񱻻��з���������ֱ����
				UnitDir = UNIT_PLANE_VERTICAL;
			}else{
				standardDistanceY += stepLengthY;
				UnitY += stepDirY;//���÷����з����������
				//���ߴ�Y������ϰ�����Ԫ�񱻻��з�������Ϊˮƽ����
				UnitDir = UNIT_PLANE_HORIZONTAL;
			}
			//���߻����ϰ��󷵻��ϰ�������
			UnitAttribute = WorldMap_Extract(WorldMapData,UnitX,UnitY);
		}
		
		
		
		//����̽�������ǽ�ڣ�������Ҫ����һ���������롣
		//���ƺ������쳣�������Ͼ��밴����ҰԽ��������Խ�̣����Ǵ��ڸ��������С����������������������¡�
		double RealDistance; 
		if (UnitDir == UNIT_PLANE_VERTICAL) RealDistance = (standardDistanceX - stepLengthX);
        else RealDistance = (standardDistanceY - stepLengthY);
		
		//��Ȼ���������
		Camera_DeepBuffer_Write(camera,x,RealDistance);
		
		
		//��ȡ����������Դ(����ͨ����⣬����ȷ��ȡ���������������)
		TextureMapperDef* texture = Texture_GetTexture(UnitAttribute);
		
		/*����ӳ��*/
		//�����߻��еĵ�ľ���y����
		double wallColpos;
		if(UnitDir == UNIT_PLANE_HORIZONTAL){//������߻��з�����ˮƽ���� 
			wallColpos = camera->position.x + RayX * RealDistance;
		}else{//������߻��з����Ǵ�ֱ���� 
			wallColpos = camera->position.y + RayY * RealDistance;
		}
		//�����־޴��쳣��wallColpos��ֵ�����仯��������Ӧ�������Ա仯��
		//����־2��Ŀǰ�޸��쳣����ǰ���쳣�����ڹ�������ķ���Ϊ��ֱ����ˮƽ����������ˮƽ����ֱ���ķ�ʽ�����˼��㡿
		//ת��Ϊ�����߻��еĵ�����y����	
		wallColpos -= (int)wallColpos;
		//�����־޴��쳣������y�����ӳ��ֵ�����仯��������Ӧ�������Ա仯��
		//����־2��Ŀǰ���޸��쳣������ԭ��wallColpos��ֵ�������⡣��
		//����y��ӳ��
		int Texture_Y;
		//���ߴӵ�Ԫ��ֱ��������
		if(UnitDir == UNIT_PLANE_VERTICAL){
			//���X�Ǵ�����������
			if(RayX < 0){
				Texture_Y = (texture->Column - 1) * wallColpos;
			}else{
				Texture_Y = (texture->Column - 1) * (1 - wallColpos);
			}
		}
		//���ߴӵ�Ԫ��ˮƽ��������
		else{
			if(RayY >= 0){
				Texture_Y = (texture->Column - 1) * wallColpos;
			}else{
				Texture_Y = (texture->Column - 1) * (1 - wallColpos);
			}
		}
		
		
		/**
		* ע�⣺���µ���Ⱦ���Ե������ĵ�Ϊ��׼��������ĵ���������ƶ��ᵼ��ͼ�������ƶ�
		*
		**/
		//��Ⱦ���ĵ�
		int RenderCenter = SCREEN_ROW / 2 - 1;
		
		//��ȡǽ�ĸ߶�
		int WallHight = SCREEN_ROW	/ RealDistance;
		if(WallHight > SCREEN_ROW) WallHight = SCREEN_ROW - 1;
		 
		//��ά��
		int LineHalf = WallHight/2;
		
		
		//x��ӳ�䣨������Ⱦ���壩
		int RenderHight = LineHalf*2;
		double TextureX_Step = ((double)texture->Logic_ROW)/RenderHight;//��ȡROW��ӳ�䲽������
		double Texture_X = 0;
		int Render_top = RenderCenter - LineHalf;
		for(int i = 0;i<RenderHight;i++){
			int signal = Texture_Extract(texture,(int)Texture_X,Texture_Y);
			if(signal > 0){
				//׼����ͬ�����ǽ��ɫ��
				if(UnitDir == UNIT_PLANE_VERTICAL){//������ߴ�ֱ����
					Canvas_PixelOperate(i+Render_top,x,CANVAS_PIXEL_OPEN);
				}else{//�������ˮƽ����
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
				//��������ر����ص�
				Canvas_PixelOperate(i+Render_top,x,CANVAS_PIXEL_CLOSE);
			}
			Texture_X += TextureX_Step;
		}

		
	}
}




/*******************
	����ͷ��ת��
********************/

/**
*	@brief	��ͷ�������ת
*	@param
*		@arg	camera		���
*	@retval
*		none
*/
void Camera_LeftPan(Camera_Struct* camera) {
    //���浱ǰ���������ķ�����
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
		//����Ϊ֡���޹���ת�ٶ�
		double correctionSpeed = camera->rotSpeed * DeltaTime->deltaTime; 
    //������ת���������·���������
    camera->direction.x = dirX * cos(correctionSpeed) - dirY * sin(correctionSpeed);
    camera->direction.y = oldDirX * sin(correctionSpeed) + dirY * cos(correctionSpeed);
    //���浱ǰƽ�淨������ x ������
    double oldPlaneX = camera->plane.x;
    //����ƽ�淨������
    camera->plane.x = camera->plane.x * cos(correctionSpeed) - camera->plane.y * sin(correctionSpeed);
    camera->plane.y = oldPlaneX * sin(correctionSpeed) + camera->plane.y * cos(correctionSpeed);
}

/**
*	@brief	��ͷ���Ҳ���ת
*	@param
*		@arg	camera		���
*	@retval
*		none
*/
void Camera_RightPan(Camera_Struct* camera){
		//���浱ǰ���������ķ�����
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
		//����Ϊ֡���޹���ת�ٶ�
		double correctionSpeed = camera->rotSpeed * DeltaTime->deltaTime; 
    //������ת���������·���������
    camera->direction.x = dirX * cos(-correctionSpeed) - dirY * sin(-correctionSpeed);
    camera->direction.y = oldDirX * sin(-correctionSpeed) + dirY * cos(-correctionSpeed);
    //���浱ǰƽ�淨������ x ������
    double oldPlaneX = camera->plane.x;
    //����ƽ�淨������
    camera->plane.x = camera->plane.x * cos(-correctionSpeed) - camera->plane.y * sin(-correctionSpeed);
    camera->plane.y = oldPlaneX * sin(-correctionSpeed) + camera->plane.y * cos(-correctionSpeed);
}
//�ӽ���ƫת
void Camera_LeftTilt(Camera_Struct* camera){
	
}
//�ӽ���ƫת
void Camera_RightTilt(Camera_Struct* camera){
	
}
/*******************
	����ͷ�ƶ���
********************/

/**
*	@brief	���������ƶ�
*	@param
*		@arg	camera		���
*	@retval
*		none
*/
void Camera_LeftMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	//����Ϊ֡���޹��ƶ��ٶ�
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime; 
	
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->plane.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
		camera->position.x -= camera->plane.x * correctionSpeed;
	}
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->plane.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
		camera->position.y -= camera->plane.y * correctionSpeed;
	}
   
  
}

/**
*	@brief	������Ҳ��ƶ�
*	@param
*		@arg	camera		���
*	@retval
*		none
*/
void Camera_RightMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	//����Ϊ֡���޹��ƶ��ٶ�
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime; 
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->plane.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
		camera->position.x += camera->plane.x * correctionSpeed;
	}
	
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->plane.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
        camera->position.y += camera->plane.y * correctionSpeed;
	}
}
/**
*	@brief	�����ǰ���ƶ�
*	@param
*		@arg	camera		���
*	@retval
*		none
*/
void Camera_ForwardMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	//����Ϊ֡���޹��ƶ��ٶ�
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime;
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->direction.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
        camera->position.x += camera->direction.x * correctionSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->direction.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
        camera->position.y += camera->direction.y * correctionSpeed;
	}
}

/**
*	@brief	�������ƶ�
*	@param
*		@arg	camera		���
*	@retval
*		none
*/
void Camera_BackMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	//����Ϊ֡���޹��ƶ��ٶ�
	double correctionSpeed = camera->moveSpeed * DeltaTime->deltaTime;
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->direction.x * correctionSpeed),(int)(camera->position.y)) == MOVABLE_AREA_VALUE){
        camera->position.x -= camera->direction.x * correctionSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->direction.y * correctionSpeed)) == MOVABLE_AREA_VALUE){
        camera->position.y -= camera->direction.y * correctionSpeed;
	}
}









/**
*	@brief	��Ȼ�����д��
*	@param
*		@arg	camera		���
*		@arg	index		����
*		@arg	value		д���ֵ
*	@retval
*		none
*	@note
*		ʧ�����ڽ������󣬻ᵼ�½��������ֹ״̬
*/
void Camera_DeepBuffer_Write(Camera_Struct* camera,int index,DeepBuffer_t value){
	if(camera == NULL || index < 0 || index >= SCREEN_COLUMN){
		//������<�Ƿ�����>
		EngineFault_IllegalAccess_Handle();
	}
	camera->DeepBuffer[index] = value;
	return;
}

/**
*	@brief	��Ȼ�������ȡ
*	@param
*		@arg	camera		���
*		@arg	index		����
*	@retval
*		@arg	�ɹ����ȡ��Ӧ��ֵ
*		@arg	ʧ�����ڽ������󣬻ᵼ�½��������ֹ״̬
*/
DeepBuffer_t Camera_DeepBuffer_Read(Camera_Struct* camera,int index){
	if(camera == NULL || index < 0 || index >= SCREEN_COLUMN){
		//������<�Ƿ�����>
		EngineFault_IllegalAccess_Handle();
	}
	return camera->DeepBuffer[index];
}



/*******************
	��˽����
********************/
/**
*	@brief	����������˽�б���
*	@param
*		@arg	camera		���
*		@arg	data		����(����Ϊ��)
*		@arg	__FreeHandle ���ݴ���
*	@retval
*		@arg	��ӳɹ�����0
*		@arg	���ʧ�ܷ���-1
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
*	@brief	�滻ԭ���������˽�б���
*	@param
*		@arg	camera		���
*		@arg	data		����(����Ϊ��)
*		@arg	__FreeHandle ���ݴ���
*	@retval
*		@arg	��ӳɹ�����0
*		@arg	���ʧ�ܷ���-1
*/
int CameraPriv_Replace(Camera_Struct* camera,void* data,void (*__FreeHandle)(void* ptr)){
	if(camera == NULL || data == NULL){
		return -1;
	}
	if(camera->Camera_PrivateData.data == NULL){
		//���ԭ��û�����˽�б���
		camera->Camera_PrivateData.data = data;
		camera->Camera_PrivateData.__FreeHandle = __FreeHandle;
		return 0;
	}
	//�������������ͷ�����
	if(camera->Camera_PrivateData.__FreeHandle != NULL){
		camera->Camera_PrivateData.__FreeHandle(camera->Camera_PrivateData.data);
	}
	camera->Camera_PrivateData.data = data;
	camera->Camera_PrivateData.__FreeHandle = __FreeHandle;
	return 0;
}
/**
*	@brief	����ԭ�е�˽������
*	@param
*		@arg	camera		���
*	@retval
*		@arg	��ӳɹ�����0
*		@arg	���ʧ�ܷ���-1
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
*	@brief	����ԭ�е�˽������
*	@param
*		@arg	camera		���
*	@retval
*		����˽������
*/
void* CameraPriv_Get(Camera_Struct* camera){
	if(camera == NULL){
		return NULL;
	}
	return camera->Camera_PrivateData.data;
}

/****************************************************
	
				��������Ⱦ�붥����Ⱦ��
	
*****************************************************/


/**********************
	��������Ⱦ��
***********************/
/**
*	@brief		���һ��ָ�����͵���ͼ
*	@param
*		@arg  	Row	��ͼ����
*		@arg  	Col	��ͼ����
*		@arg  	texture	��ͼ��������
*		@arg  	FC_TreeManager	��ͼ������
*	@retval
*		@arg	�ɹ���0
*		@arg	ʧ�ܷ���-1
*/

#define FC_TextureAdd(Row,Col,texture,FC_TreeManager)	do{\
	if(index <= 0 || Row <= 0 || Col <= 0 || texture == NULL){\
		return -1;\
	}\
	/*�����Ƿ��������ͼ*/\
	void* temp = search(FC_TreeManager, index);\
	if(temp != NULL){\
		/*���ڴ�����*/\
		return -1;\
	}\
	/*�����ڵ�*/\
	FC_Node_t* node = (FC_Node_t*)malloc(sizeof(FC_Node_t));\
	if(node == NULL){\
		/*�ռ����ʧ��*/\
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
*	@brief		��ȡָ�����͵���ͼ�Ľڵ�
*	@param
*		@arg  	index	�����ڵ�
*		@arg  	FC_TreeManager ��ͼ������
*	@retval
*		@arg	�ɹ���ڵ��ַ
*		@arg	ʧ�ܷ��ؿ�
*/
#define FC_GetTexture(index,FC_TreeManager)	do{\
	if(index <= 0){\
		return NULL;\
	}\
	FC_Node_t* node = (FC_Node_t*)search(FC_TreeManager, index);\
	return node;\
}while(0)

/**
*	@brief		��ȡָ�����͵���ͼ��������ȡ
*	@param
*		@arg	FC_Node		��ͼ�ڵ�����
*		@arg  	ROW_Pixel	���ؼ���������
*		@arg  	COL_Pixel	���ؼ���������
*	@retval
*		@arg	������1
*		@arg	�𷵻�0
*		@arg	�������󷵻�-1
*/
int FC_TextureExtract(FC_Node_t *FC_Node,int ROW_Pixel,int COL_Pixel){
	if(FC_Node == NULL || ROW_Pixel < 0 || COL_Pixel < 0) return -1;
	if(ROW_Pixel >= FC_Node->ROW_Logic || COL_Pixel >= FC_Node->COL) return -1;
	
	int byte = ROW_Pixel/8;
	int offset = ROW_Pixel%8;
	
	return (FC_Node->FC_texture[byte * FC_Node->COL + COL_Pixel] & (0x01<<offset)) ? 1:0;
}
/*��Ⱦʵ��---------------------------------------------------------------------------------*/

/**
 * @brief  �ذ�������Ⱦ
 * @param  
 *		@arg	camera: ָ��������ṹ��
 * @retval
 *		none
 */
void Floor_TextureRender(Camera_Struct* camera) {
	int repeatFactor = 2;
    // �����ӽ����ұ߽����ߣ����ڵذ������
    double rayDirRow0 = camera->direction.x - camera->plane.x;
    double rayDirCol0 = camera->direction.y - camera->plane.y;
    double rayDirRow1 = camera->direction.x + camera->plane.x;
    double rayDirCol1 = camera->direction.y + camera->plane.y;
    
    // ֻ����Ļ�°벿�ֽ��е�����Ⱦ����������ͨ��Ϊ�컨�壩
    for (int scrRow = SCREEN_ROW / 2 + 1; scrRow < SCREEN_ROW; scrRow++) {
        int p = scrRow - SCREEN_ROW / 2;
        double posZ = 0.5 * SCREEN_ROW;  // ģ����루������Ļ�߶����ã�
        double rowDistance = posZ / p;   // ͸��ͶӰ����
        
        // ���㵱ǰ��ÿ�����ض�Ӧ�������е��������λ�ò�ֵ
        double floorStepRow = rowDistance * (rayDirRow1 - rayDirRow0) / SCREEN_COLUMN;
        double floorStepCol = rowDistance * (rayDirCol1 - rayDirCol0) / SCREEN_COLUMN;
        
        // ��ʼ����λ�ã���ǰ��������Ӧ�ĵ�������
        double floorRowPos = camera->position.x + rowDistance * rayDirRow0;
        double floorColPos = camera->position.y + rowDistance * rayDirCol0;
        
        for (int scrCol = 0; scrCol < SCREEN_COLUMN; scrCol++) {
            // ������λ��ת��Ϊ�������֣����浥Ԫ��ʶ����С�����֣��ֲ����꣩
            int worldTileRow = (int)floorRowPos;
            int worldTileCol = (int)floorColPos;
            
            // ����������ӳ�䵽�������������У�����ֵ�Լ�Խ�������
            int mapIdxRow = ((worldTileRow % camera->Floor.Row) + camera->Floor.Row) % camera->Floor.Row;
            int mapIdxCol = ((worldTileCol % camera->Floor.Col) + camera->Floor.Col) % camera->Floor.Col;
            int floorDescIndex = mapIdxRow * camera->Floor.Col + mapIdxCol;
            
            // ��ȡ�õ��浥ԪӦʹ�õ�������
            unsigned char tileIndexFloor = camera->Floor.floorDescriptArr[floorDescIndex];
            
            // ����õ�Ԫ�ڵľֲ�λ�ã���Χ 0��1��
            double fracRow = floorRowPos - worldTileRow;
            double fracCol = floorColPos - worldTileCol;
            
            // �ؼ������ֲ���������ظ����ӣ���ȡ��С�����֣�
            // ʹ��ÿ�� 1��1 ��λ���ظ���ʾ���� repeatFactor ��
            fracRow = fracRow * repeatFactor - floor(fracRow * repeatFactor);
            fracCol = fracCol * repeatFactor - floor(fracCol * repeatFactor);
            
            // ���������Ż�ȡ��Ӧ������ڵ�
            FC_Node_t* floorTexNode = Floor_GetTexture(tileIndexFloor);
            if (floorTexNode == NULL)
                floorTexNode = Floor_GetTexture(0);
            
            // ���ݾֲ�����������ߴ����Ҫ�����ľ�����������
            int textureRow = (int)(fracRow * floorTexNode->ROW_Logic) % floorTexNode->ROW_Logic;
            int textureCol = (int)(fracCol * floorTexNode->COL) % floorTexNode->COL;
            
            // ��ȡ��ǰ�������أ�ע���ڲ�ʹ����λ�����ʽ��ȡ��
            int pixelVal = FC_TextureExtract(floorTexNode, textureRow, textureCol);
            if (pixelVal < 0)
                pixelVal = 0;
            CANVAS_PIXEL_STATUS pixelStatus = (pixelVal > 0) ? CANVAS_PIXEL_OPEN : CANVAS_PIXEL_CLOSE;
            
            // ���Ƶ�ǰ��Ļ����
            Canvas_PixelOperate(scrRow, scrCol, pixelStatus);
            
            // ���²���λ�ã������ƶ�һ������������
            floorRowPos += floorStepRow;
            floorColPos += floorStepCol;
        }
    }
}


/**
 * @brief  �컨��������Ⱦ
 * @param  
 *		@arg	camera: ָ��������ṹ��
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
*	@brief	��ȾĬ�ϵذ�ͼ��
*	@param
*		camera	��ʽֵ������ʹ��Ϊ�˼���API���
*	@retval
*		none
*/
void Floor_DefaultRender(Camera_Struct* camera){
	if(camera == NULL)	return;
	int RowStart = SCREEN_ROW/2;
	for(int i = RowStart;i<SCREEN_ROW;i++){
		for(int j = 0;j<SCREEN_COLUMN;j++){
			if(j%2 == 0){
				if(i%2 == 0){//˫���д�ӡ
					Canvas_PixelOperate(i,j,CANVAS_PIXEL_OPEN);
				}
			}
			//������
			else{
				if(i%2 != 0){//�����д�ӡ
					Canvas_PixelOperate(i,j,CANVAS_PIXEL_OPEN);	
				}	
			}
		}
	}
	return;
}	

/*�ذ����-------------------------------------------------------*/

RB_Node* FloorTreeManager = NULL;
RB_Node* CeliTreeManager = NULL;
/**
*	@brief		�����Զ���ذ���Ⱦ��
*	@param
*		@arg	camera		���
*		@arg	Row			��Ⱦ�������
*		@arg	Col			��Ⱦ�������
*		@arg	RenderArray	��Ⱦ���飬��Ⱦ�����е�ÿ����Ա������Ԥ��Ⱦ����ͼ����
*	@retval
*		@arg	�����ɹ�����0
*		@arg	����ʧ�ܷ���-1
*/
int Floor_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray){
	if(camera == NULL || RenderArray == NULL){
		return -1;
	}
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return -1;//��ͼ���ݻ�ȡ����
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
*	@brief		������Ⱦģʽ�л�ΪĬ����Ⱦ��
*	@param
*		@arg	camera		���
*	@retval
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ���-1
*/
int Floor_SwitchDefaultReaderMode(Camera_Struct* camera){
	if(camera == NULL)	return -1;
	camera->Floor.renderType = 0;
	return 0;	
}
/**
*	@brief		������Ⱦģʽ�л�Ϊ�Զ�����Ⱦ��
*	@param
*		@arg	camera		���
*	@retval
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ���-1
*/
int Floor_SwitchCustomReaderMode(Camera_Struct* camera){
	if(camera == NULL || camera->Floor.floorDescriptArr == NULL)	return -1;
	camera->Floor.renderType = 1;
	camera->Floor.Floor_RenderHandle = Floor_TextureRender;
	return 0;
}


/**
*	@brief		���һ�ŵ�ͼ����ͼ
*	@param
*		@arg	index		����(������ڵ���1)
*	@retval
*		@arg	�ɹ����ȡ��Ӧ��ֵ
*		@arg	ʧ�����ڽ������󣬻ᵼ�½��������ֹ״̬
*/
int Floor_TextureAdd(int index,int Row,int Col,const unsigned char* texture){
	FC_TextureAdd(Row,Col,texture,FloorTreeManager);//�궨�庯��
}



/**
*	@brief		��ȡ������Ӧ�ĵذ���ͼ��Ϣ
*	@param
*		@arg	index		����(������ڵ���1)
*	@retval
*		@arg	�ɹ����ȡ��Ӧ��ֵ
*		@arg	ʧ�����ڽ������󣬻ᵼ�½��������ֹ״̬
*/
FC_Node_t* Floor_GetTexture(int index){
	FC_GetTexture(index,FloorTreeManager);
}



/*�컨�����-------------------------------------------------------*/
/**
*	@brief		���һ���컨�����ͼ
*	@param
*		@arg	index		����(������ڵ���1)
*	@retval
*		@arg	�ɹ����ȡ��Ӧ��ֵ
*		@arg	ʧ�����ڽ������󣬻ᵼ�½��������ֹ״̬
*/
int Celi_TextureAdd(int index,int Row,int Col,const unsigned char* texture){
	FC_TextureAdd(Row,Col,texture,CeliTreeManager);
}


/**
*	@brief		�����Զ����컨����Ⱦ��
*	@param
*		@arg	camera		���
*		@arg	Row			��Ⱦ�������
*		@arg	Col			��Ⱦ�������
*		@arg	RenderArray	��Ⱦ���飬��Ⱦ�����е�ÿ����Ա������Ԥ��Ⱦ����ͼ����
*	@retval
*		@arg	�����ɹ�����0
*		@arg	����ʧ�ܷ���-1
*/
int Celi_CustomRenderCreate(Camera_Struct* camera,int Row,int Col,const unsigned char* RenderArray){
	if(camera == NULL || RenderArray == NULL){
		return -1;
	}
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return -1;//��ͼ���ݻ�ȡ����
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
*	@brief		�컨����Ⱦģʽ�л�ΪĬ����Ⱦ��
*	@param
*		@arg	camera		���
*	@retval
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ���-1
*/
int Celi_SwitchDefaultReaderMode(Camera_Struct* camera){
	if(camera == NULL)	return -1;
	camera->Ceil.renderType = 0;
	return 0;	
}
/**
*	@brief		�컨����Ⱦģʽ�л�Ϊ�Զ�����Ⱦ��
*	@param
*		@arg	camera		���
*	@retval
*		@arg	ִ�гɹ����� 0
*		@arg	ִ��ʧ�ܷ���-1
*/
int Celi_SwitchCustomReaderMode(Camera_Struct* camera){
	if(camera == NULL || camera->Ceil.ceilDescriptArr == NULL)	return -1;
	camera->Ceil.renderType = 1;
	camera->Ceil.Ceil_RenderHandle = Ceil_TextureRender; 
	return 0;
}


/**
*	@brief		��ȡ������Ӧ���컨����ͼ��Ϣ
*	@param
*		@arg	index		����(������ڵ���1)
*	@retval
*		@arg	�ɹ����ȡ��Ӧ��ֵ
*		@arg	ʧ�����ڽ������󣬻ᵼ�½��������ֹ״̬
*/
FC_Node_t* Celi_GetTexture(int index){
	FC_GetTexture(index,CeliTreeManager);
}



/**
*	@brief	��ȾĬ���컨ͼ��(Ĭ����ȾΪ��)
*	@param
*		camera	��ʽֵ������ʹ��Ϊ�˼���API���
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1
*/
void Celi_DefaultRender(Camera_Struct* camera){
	if(camera == NULL)	return;
	return ;
}	















/**********************
	��������Ⱦ��
***********************/

RB_Node* SSR_CoreTree = NULL;

/**
*	@brief	����һ�������������
*	@param
*		@arg	value	����������ֵ������0��
*		@arg	Row		�����������
*		@arg	Col		�����������
*		@arg	time	����һ�����ڵĲ���ʱ��
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1
*/

int SSRAnim_ManagerCreate(int value,int Row_Pixel,int Col_Pixel,double time){
	if(value <= 0 || Row_Pixel <= 0 || Col_Pixel <= 0)	return -1;
	//��֤�Ƿ��Ѿ�ע��
	void* temp = search(SSR_CoreTree,value);
	if(temp != NULL){
		return -1;
	}
	//�����ڵ�
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
	//��������
	int result = rbInsert(&SSR_CoreTree,value,manager);
	if(result != 0){
		free(manager);
		return -1;
	}
	return 0;
}

/**
*	@brief	��ָ������������Ӷ���֡
*	@param
*		@arg	value	��������ֵ������0��
*		@arg	seq		����֡�����(���ڵ���0)
*		@arg	Frm		����֡����
*		@arg	shadow	����֡Ӱ����������
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1
*/
int SSRAnim_FrmAdd(int value, int seq, const unsigned char* Frm,const unsigned char* shadow) {
    if (value <= 0 || seq < 0 || Frm == NULL)
        return -1;

    // ��ȡ������
    SSR_Anim_t* manager = (SSR_Anim_t*)search(SSR_CoreTree, value);
    if (manager == NULL)
        return -1;

    // �����½ڵ�
    SSR_Frm_t* frm = (SSR_Frm_t*)malloc(sizeof(SSR_Frm_t));
    if (frm == NULL)
        return -1;

    frm->frmArr = Frm;
    frm->value = seq;
    frm->next = NULL;
	frm->shadowFrmArr = shadow;
	
    // �������Ϊ�գ����ɵ��ڵ�ѭ������
    if (manager->count == 0) {
        manager->FirstFrm = frm;
        frm->next = frm;  // ָ������
        manager->TailFrm = frm;
        manager->count = 1;
		manager->frame_duration = manager->time / manager->count;//���µ�֡ʱ��
        return 0;
    }

    SSR_Frm_t* head = manager->FirstFrm;
    SSR_Frm_t* tail = manager->TailFrm;

    // ����������½ڵ����С��ͷ�ڵ�
    if (frm->value < head->value) {
        // ���β�ڵ��Ƿ����½ڵ��ظ�
        if (tail->value == frm->value) {
            free(frm); // �ͷ��ڴ棬��ֹ�ڴ�й©
            return -1; // ��ų�ͻ
        }
        frm->next = head;
        tail->next = frm;           // ����β�ڵ��ѭ��ָ��
        manager->FirstFrm = frm;    // ����ͷ�ڵ�ָ��
        manager->count++;
		manager->frame_duration = manager->time / manager->count;//���µ�֡ʱ��
        return 0;
    }

    // ��ͷ�ڵ㿪ʼ��Ѱ�Ҳ����
    SSR_Frm_t* current = head;
    while (current->next != head && current->next->value < frm->value) {
        current = current->next;
    }

    // ����Ƿ�����һ���ڵ������ظ�
    if (current->next->value == frm->value) {
        free(frm); // �ͷ��ڴ棬��ֹ�ڴ�й©
        return -1; // ��ų�ͻ
    }

    // ����ڵ�
    frm->next = current->next;
    current->next = frm;

    // ����������β�ڵ㣬����� TailFrm ָ��
    if (current == tail) {
        manager->TailFrm = frm;
    }

    manager->count++;
	manager->frame_duration = manager->time / manager->count;//���µ�֡ʱ��
    return 0;
}

/**
*	@brief	��ָ����������Ϊ��ǰ�����ִ�ж���(Ĭ��Ϊ��ѭ������)
*	@param
*		@arg	camera	�����
*		@arg	value	��������ֵ������0��	
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1
*/
int SSR_SetCameraExecAnim(Camera_Struct* camera,int value){
	if(camera == NULL ||value <= 0)	return -1;
	
	// ��ȡ������
    SSR_Anim_t* manager = (SSR_Anim_t*)search(SSR_CoreTree, value);
    if (manager == NULL || manager->count == 0){
        return -1;	
	}
	
	camera->SSR_AnimData.index = value;
	camera->SSR_AnimData.Anim = manager;
	camera->SSR_AnimData.ReadyFrm = manager->FirstFrm;
	camera->SSR_AnimData.accumulatedTime = 0;
	camera->SSR_AnimData.currentFrmIndex = 0;
	camera->SSR_AnimData.AnimType = 0;//��ѭ��
	return 0;
}

/**
*	@brief	���㶯��������һ֡
*	@param
*		@arg	camera	�����
*	@retval
*	���һ����������ѭ������
*		�����Ϊ0
*
*	��������������ڵ���ִ������
*		û�����һ�����ڷ��� 0
*		����Ѿ������һ�����ڷ��� -1
*/
int SSRAnim_Update(Camera_Struct* camera){
	if(camera == NULL || camera->SSR_AnimData.ReadyFrm == NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	camera->SSR_AnimData.accumulatedTime += (DeltaTime->deltaTime);
	//�ж��Ƿ񳬹��˸�����ֵ
	while(camera->SSR_AnimData.accumulatedTime >= camera->SSR_AnimData.Anim->frame_duration){
		//������ֵ
		camera->SSR_AnimData.accumulatedTime -= camera->SSR_AnimData.Anim->frame_duration;
		//�ж϶���������ѭ���������Ƿ�ѭ������
		if(camera->SSR_AnimData.AnimType == 0){
			//��ѭ������
			
			//�����ǰ����֡�������һ֡
			if(camera->SSR_AnimData.currentFrmIndex < camera->SSR_AnimData.Anim->count){
				camera->SSR_AnimData.ReadyFrm = camera->SSR_AnimData.ReadyFrm->next;
				camera->SSR_AnimData.currentFrmIndex++;
			}else{
				return -1;
			}
			
		}else{
			//ѭ������
			//����Ϊ��һ֡
			camera->SSR_AnimData.ReadyFrm = camera->SSR_AnimData.ReadyFrm->next;
		}
	}
	
	return 0;
}

/**
*	@brief	���÷�ѭ���ද��ִ֡��λ��
*	@param
*		@arg	camera	�����
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1		
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
*	@brief	��ȡ֡����Ϣ
*	@param
*		@arg	anim	֡��Ӧ�Ķ�������������
*		@arg	frm		֡
*		@arg	ROW_Pixel	��ȡ�����ص���
*		@arg	COL_Pixel	��ȡ�����ص���
*	@retval
*		����1������
*		����0������
*		����-1����͸��
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
			//Ϊ0��ʾ͸��
			return -1;
	}
	return (frm->frmArr[byte * anim->Col + COL_Pixel] & (0x01<<offset)) ? 1:0;
}


/**
*	@brief	������Ⱦʹ��
*	@param
*		@arg	camera	�����
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1
*/
int SSRAnim_RenderEnable(Camera_Struct* camera){
	if(camera == NULL){
		return -1; 
	}
	camera->SSR_AnimData.SSRAnim_Render = SSRAnim_Render;
	return 0;
}

/**
*	@brief	����ͼ����ʾλ��
*	@param
*		@arg	camera	�����
*		@arg	ROW_Pixel	��ȡ�����ص���
*		@arg	COL_Pixel	��ȡ�����ص���
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1
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
*	@brief	������Ⱦʧ��
*	@param
*		@arg	camera	�����
*	@retval
*		ִ�гɹ�����0
*		ִ��ʧ�ܷ��� -1
*/
int SSRAnim_RenderDisable(Camera_Struct* camera){
	if(camera == NULL){
		return -1; 
	}
	camera->SSR_AnimData.SSRAnim_Render = NULL;
	return 0;
}


/**
*	@brief	������Ⱦ����
*	@param
*		@arg	camera	�����
*	@retval
*		none
*/
void SSRAnim_Render(Camera_Struct* camera) {
    if (camera == NULL) return;
    
    int img_row = camera->SSR_AnimData.Anim->ROW_Logic;
    int img_col = camera->SSR_AnimData.Anim->Col;
    int pos_row = camera->SSR_AnimData.ROW_Pixel;
    int pos_col = camera->SSR_AnimData.COL_Pixel;
    
    // �����з������ʼ�ͽ�������
    int i_start = (pos_row < 0) ? -pos_row : 0;
    int i_end = img_row;
    if(pos_row + img_row > SCREEN_ROW) {
        i_end = SCREEN_ROW - pos_row;
    }
    
    // �����з������ʼ�ͽ�������
    int j_start = (pos_col < 0) ? -pos_col : 0;
    int j_end = img_col;
    if(pos_col + img_col > SCREEN_COLUMN) {
        j_end = SCREEN_COLUMN - pos_col;
    }
    
    // �����ȫ������Ļ������Ⱦ
    if(i_start >= i_end || j_start >= j_end) {
        return;
    }
    
    // ����Ⱦ��Ļ�ڿɼ��Ĳ���
    for (int i = i_start; i < i_end; i++) {
        for (int j = j_start; j < j_end; j++) {
            int result = SSRAnim_FrmExtract(camera->SSR_AnimData.Anim, camera->SSR_AnimData.ReadyFrm, i, j);
            // ��ͼ����߼��������ƫ��ӳ�䵽��Ļ����
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
*	@brief	������Ⱦ����
*	@param
*		@arg	camera	�����
*	@retval
*		none
*/
void SSRAnim_Exec(Camera_Struct* camera){
	if(camera == NULL || camera->SSR_AnimData.SSRAnim_Render == NULL)	return;
	camera->SSR_AnimData.SSRAnim_Render(camera);
	return;
}
