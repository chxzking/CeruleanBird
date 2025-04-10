#ifndef __CERULEANBIRD_H__
#define __CERULEANBIRD_H__


//��������
#include "engine_config.h"//���ýӿ�
#include "Camera_API.h"//������ӿ�
#include "Canvas_API.h"//�����ӿ�
#include "FrameRateStrategy_API.h"//֡�ʲ��Խӿ�
#include "TextureMapper_API.h"//����ӳ��ӿ�
#include "WorldMap_API.h"//��ͼ�ӿ�
#include "Sprites_API.h"//����ӿ�
#include "heap_solution_1.h"//��̬�ڴ�ӿ�
#include "EngineFault.h"	//�������������
#include "vector.h"//������

//������Ⱦ�����������һ����ѡ������������Ⱦ���߽����˶��η�װ�Է���ͳһ��Ⱦ��
void* CeruleanBird_EngineRender(Camera_Struct* camera);
#endif
