#include "Camera.h"

#include <math.h>

#include "TextureMapper_internal.h"//�����ڲ��ӿ�

#include "Canvas_API.h"			//�����ӿ�

#include "WorldMap_internal.h"	//��ͼ�ڲ��ӿ�


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



//��ʼ�����������ͨ����
Camera_Struct* Camera_Init(Camera_InitTypeDef* Camera_InitStruct){
	//�������
	if(Camera_InitStruct == C_NULL) return C_NULL;
	if(Camera_InitStruct->FOV <= 0 || Camera_InitStruct->FOV >= 180) return C_NULL;//��������ͷ���ӽ���0�ȵ�180��֮�䣬������0��180��
	 
	Camera_Struct* camera = (Camera_Struct*)malloc(sizeof(Camera_Struct));
	if(camera == C_NULL)	return C_NULL;
	//�û��������Ը�ֵ
	//���޴�����׵ĸ�ֵ
	camera->moveSpeed = Camera_InitStruct->moveSpeed;
	camera->rotSpeed = Camera_InitStruct->rotSpeed;
	camera->position = Camera_InitStruct->position;
	camera->direction = vector_Normalize(&Camera_InitStruct->direction);//��һ����������
	
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


//





//��֡������ӽǻ���ģ����Ⱦ
void Camera_Render(Camera_Struct* camera){
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
		int UnitAttribute = 0;
		//���嵥Ԫ�񱻹��߻��еķ���
		char UnitDir = UNIT_PLANE_VERTICAL;
		//���й���ɨ�衾�ϰ�ʶ��������ɨ���յ㶼ͣ������Ԥ�ڵ�λ�á�
		while(UnitAttribute == 0){
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
//��ͷ����
void Camera_LeftPan(Camera_Struct* camera) {
    //���浱ǰ���������ķ�����
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
    //������ת���������·���������
    camera->direction.x = dirX * cos(camera->rotSpeed) - dirY * sin(camera->rotSpeed);
    camera->direction.y = oldDirX * sin(camera->rotSpeed) + dirY * cos(camera->rotSpeed);
    //���浱ǰƽ�淨������ x ������
    double oldPlaneX = camera->plane.x;
    //����ƽ�淨������
    camera->plane.x = camera->plane.x * cos(camera->rotSpeed) - camera->plane.y * sin(camera->rotSpeed);
    camera->plane.y = oldPlaneX * sin(camera->rotSpeed) + camera->plane.y * cos(camera->rotSpeed);
}
//��ͷ����
void Camera_RightPan(Camera_Struct* camera){
	 //���浱ǰ���������ķ�����
    double dirX = camera->direction.x, dirY = camera->direction.y;
    double oldDirX = dirX;
    //������ת���������·���������
    camera->direction.x = dirX * cos(-camera->rotSpeed) - dirY * sin(-camera->rotSpeed);
    camera->direction.y = oldDirX * sin(-camera->rotSpeed) + dirY * cos(-camera->rotSpeed);
    //���浱ǰƽ�淨������ x ������
    double oldPlaneX = camera->plane.x;
    //����ƽ�淨������
    camera->plane.x = camera->plane.x * cos(-camera->rotSpeed) - camera->plane.y * sin(-camera->rotSpeed);
    camera->plane.y = oldPlaneX * sin(-camera->rotSpeed) + camera->plane.y * cos(-camera->rotSpeed);
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
//��ͷ����
void Camera_LeftMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	
	
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->plane.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
		camera->position.x -= camera->plane.x * camera->moveSpeed;
	}
	if(WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->plane.y * camera->moveSpeed)) == 0){
		camera->position.y -= camera->plane.y * camera->moveSpeed;
	}
   
  
}
//��ͷ����
void Camera_RightMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}

	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->plane.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
		camera->position.x += camera->plane.x * camera->moveSpeed;
	}
	
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->plane.y * camera->moveSpeed)) == 0){
        camera->position.y += camera->plane.y * camera->moveSpeed;
	}
}
//��ͷǰ��
void Camera_ForwardMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	//WorldMap_Extract(WorldMapData,int ROW_Pixel,int COL_Pixel)
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x + camera->direction.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
        camera->position.x += camera->direction.x * camera->moveSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y + camera->direction.y * camera->moveSpeed)) == 0){
        camera->position.y += camera->direction.y * camera->moveSpeed;
	}
}
//��ͷ����
void Camera_BackMove(Camera_Struct* camera){
	//��ȡ��ǰ��ͼ����
	WorldMapDef* WorldMapData = WorldMap_GetWorldMap(WorldMap_GetCurrentMapPos());
	if(WorldMapData == ((void*)0)){
		return;//��ͼ���ݻ�ȡ����
	}
	
	if (WorldMap_Extract(WorldMapData,(int)(camera->position.x - camera->direction.x * camera->moveSpeed),(int)(camera->position.y)) == 0){
        camera->position.x -= camera->direction.x * camera->moveSpeed;
	}
    if (WorldMap_Extract(WorldMapData,(int)(camera->position.x),(int)(camera->position.y - camera->direction.y * camera->moveSpeed)) == 0){
        camera->position.y -= camera->direction.y * camera->moveSpeed;
	}
}
