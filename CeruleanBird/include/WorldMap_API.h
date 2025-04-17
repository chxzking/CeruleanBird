/*��ǰ�ļ�Ϊһ����*/
#ifndef __WORLDMAP_API_H__
#define __WORLDMAP_API_H__

/*
	����ͼ����ģ�͡�����û�о���������Ŀǰ�ǵ�λ���ϣ�
	0
   0+----------------------------->(Y)
	|
	|
	|					��4��Ϊ0����5��Ϊ0����6��Ϊ0����7��Ϊ0
	|						  |
	|			��λ����   0xF0
	|						| 
	|					��0��Ϊ1����1��Ϊ1����2��Ϊ1����3��Ϊ1
	|
	|
	|
	|
	|
	v
   (X)
*/

/*��Ϸ��ͼ������ֻ��ʹ��byte��*/

/****************************

	��Ŀǰ�����뷨��
	1����ͼ�ṹ����ȫ���أ�����¶�κ���Ϣ���û����棬
		����ͼ�����ݲ�ѯͨ�� [��ͼ������] ���� 
	
*******************************/

typedef struct WorldMapDef WorldMapDef;
/**
*		@note	ע�⣬�����ͼ��ֵ�������Ψһ��
*	@brief	����������ӵ�ͼ��ͨ����ͼԪ�����ͼ�����
*	@param	
*		@arg	value	��ͼԪ�ص�ֵ
*		@arg	WorldMap_Data	��Ҫ���󶨵ĵ�ͼ����
*		@arg	WorldMap_Row	��ͼ�������
*		@arg	WorldMap_COL ��ͼ�������
*	@retval	
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����1����ζ�Ÿ�ֵ�Ѿ�����
*		@arg	������ؽ����-1����ζ���ڴ����ʧ�ܻ��߲�������
*/
int WorldMap_Add(int value,unsigned char* WorldMap_Data,int WorldMap_Row,int WorldMap_COL);

/*****************************
		��ɾ��������
******************************/

/**
*	@brief	ɾ��������ָ���ĵ�ͼ
*	@param	
*		@arg	value	��ͼԪ�ص�ֵ
*	@retval	none��
*/
void WorldMap_Delete(int value);
/**
*	@brief	ɾ�����������еĵ�ͼ(���ŵ�ͼ�����ͷ�)
*	@param	none
*	@retval	none��
*	@note	��ʱ��Խϳ�
*/
void WorldMap_Free(void);
/*****************************
		���޸Ĳ�����
******************************/
/**
*	@brief	�޸İ󶨵ĵ�ͼ����󶨵�ֵ
*	@param	
*		@arg	value	ԭ�󶨵�ֵ
*		@arg	new_value	���޸ĵ�ֵ
*	@retval	�޸ĳɹ�����0���޸ĸ�ʧ�ܷ���-1
*/
int WorldMap_ValueModify(int value,int new_value);
/**
*	@brief	�޸İ󶨵ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	ROW	��ͼ����
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int WorldMap_ROWModify(int value,int ROW);
/**
*	@brief	�޸İ󶨵ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	COL	��ͼ����
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int WorldMap_COLModify(int value,int COL);
/**
*	@brief	�޸İ󶨵ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	array	�µĵ�ͼ����
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int WorldMap_ArrayModify(int value,unsigned char* array);
/*****************************
		����ѯ������
******************************/	
/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ����
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ�����ַ����ȡʧ�ܷ��ؿ�
*/
unsigned char* WorldMap_GetArray(int value);
/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ������߼���
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ������ ����ȡʧ�ܷ���-1
*/
int WorldMap_GetROW(int value);
/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ�������
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ�����ַ���� ����ȡʧ�ܷ���-1
*/
int WorldMap_GetColumn(int value);
/**
*	@brief	���õ�ǰ�ĵ�ͼ��
*	@param	
*		@arg	MapPos	ѡ�е�ָ����ͼ
*	@retval	none
*/
void WorldMap_SetCurrentMapPos(int MapPos);
/**
*	@brief	��ȡ��ǰ��ͼ���λ��
*	@param	none
*	@retval	��ͼλ��
*/
int WorldMap_GetCurrentMapPos(void);

#endif
