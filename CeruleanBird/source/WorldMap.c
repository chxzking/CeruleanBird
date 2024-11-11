#include "RB_Tree_API.h"
#include "bit_operate.h"
#include "engine_config.h"
#include "WorldMap_internal.h"

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


RB_Node* WorldMap_Header = C_NULL;	//����ͷ

int WorldMap_Pos = 0;	//��ǰ���ڵĵ�ͼ���ĸ���ͼ��

//��ͼ�������
int WorldMap_Add(int value,unsigned char* WorldMap_Data,int WorldMap_Row,int WorldMap_COL){
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //�������涯̬�ڴ�������
		memory_pool_init();
	#endif	//�������涯̬�ڴ�������
		
	//����У��
	if(WorldMap_Data == C_NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	//�ڴ����
	WorldMapDef* node = (WorldMapDef*)malloc(sizeof(WorldMapDef));
	if(node == C_NULL)	return -1;
	node->worldmap_array = WorldMap_Data;
	node->Column = WorldMap_COL;
	node->Row = WorldMap_Row;

	//��ӽ�������
	int result = rbInsert(&WorldMap_Header, value,(void*)node);
	if(result != 0){
		//��ӳ�������
		free(node);
		return result;
	}
	//���µ�ǰ�ĵ�ͼ��λ
	WorldMap_Pos = value;
	return 0;	
}

/**
*	@brief	ɾ��������ָ���ĵ�ͼ
*	@param	
*		@arg	value	��ͼԪ�ص�ֵ
*	@retval	none��
*/
void WorldMap_Delete(int value){
	if(WorldMap_Header == C_NULL)	return;
	rbDelete(&WorldMap_Header, value);
}
/**
*	@brief	ɾ�����������еĵ�ͼ(���ŵ�ͼ�����ͷ�)
*	@param	none
*	@retval	none��
*	@note	��ʱ��Խϳ�
*/
void WorldMap_Free(void){
	RB_Tree_Free(&WorldMap_Header);
}
/**
*	@brief	�޸İ󶨵ĵ�ͼ����󶨵�ֵ
*	@param	
*		@arg	value	ԭ�󶨵�ֵ
*		@arg	new_value	���޸ĵ�ֵ
*	@retval	�޸ĳɹ�����0���޸ĸ�ʧ�ܷ���-1
*/
int WorldMap_ValueModify(int value,int new_value){
	//����Ŀ��ֵ�Ƿ����
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL)	return -1;//����ʧ��
	
	//������ֵ�Ƿ��Ѿ�����
	WorldMapDef* new_result = (WorldMapDef*)search(WorldMap_Header, new_value);
	if(new_result != C_NULL)	return -1;//���ִ��ڣ��򲻿ɸ���
	
	//�����½ڵ�
	int temp = WorldMap_Add(new_value,new_result->worldmap_array,new_result->Row ,new_result->Column);
	if(temp != 0)	return -1;//�½ڵ㴴��ʧ��
	
	//ɾ��ԭ�ڵ�
	WorldMap_Delete(value);
	return 0;
}
/**
*	@brief	�޸İ󶨵ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	ROW	��ͼ����
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int WorldMap_ROWModify(int value,int ROW){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Row = ROW;
	return 0;
}
/**
*	@brief	�޸İ󶨵ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	COL	��ͼ����
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int WorldMap_COLModify(int value,int COL){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Column= COL;
	return 0;
}
/**
*	@brief	�޸İ󶨵ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	array	�µĵ�ͼ����
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int WorldMap_ArrayModify(int value,unsigned char* array){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->worldmap_array = array;
	return 0;
}
/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ�ṹ��ָ�� @ref WorldMapDef����
*			ȡʧ�ܷ��ؿ�
*/
WorldMapDef* WorldMap_GetWorldMap(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	return result;
}
/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ�����ַ����ȡʧ�ܷ��ؿ�
*/
unsigned char* WorldMap_GetArray(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return C_NULL;
	}
	return result->worldmap_array;
}
/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ������߼���
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ������ ����ȡʧ�ܷ���-1
*/
int WorldMap_GetROW(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Row;
}
/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ�������
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ�����ַ���� ����ȡʧ�ܷ���-1
*/
int WorldMap_GetColumn(int value){
	WorldMapDef* result = (WorldMapDef*)search(WorldMap_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Column;
}
/**
*	@brief	��ʾ��ͼָ��λ�õ�Ԫ��
*	@param	
*		@arg	worldmap	 ��ͼ��Դ
*		@arg	ROW			��
*		@arg	COL			��
*	@retval	
*		@arg	�쳣��������ת�����쳣����
*		@arg	��ȡ�ɹ����ص�ͼ��ֵ
*/
int WorldMap_Extract(WorldMapDef* worldmap,int ROW,int COL){
	//�������
	if(worldmap == C_NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	if((ROW >= worldmap->Row) || (COL >= worldmap->Column)){
		//������<Խ�����>
		EngineFault_IllegalAccess_Handle();
	}
	return worldmap->worldmap_array[ROW * worldmap->Column + COL ];
}

//���õ�ǰ�ĵ�ͼ��
void WorldMap_SetCurrentMapPos(int MapPos){
	WorldMap_Pos = MapPos;
}
//��ȡ��ǰ��ͼ���λ��
int WorldMap_GetCurrentMapPos(void){
	return WorldMap_Pos;
}




