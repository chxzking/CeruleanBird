/*��ǰ�ļ�Ϊ������*/
#ifndef __WORLDMAP_INTERNAL_H__
#define __WORLDMAP_INTERNAL_H__
#include "WorldMap_API.h"
/**
*
*	@ref TextureMapDef
*
*/
struct WorldMapDef{
	unsigned char* worldmap_array;	//�õ�ͼ�������ַ��ע�⣬����ֻ��Ϊһά���飩
	int Row;						//�õ�ͼ�������
	int Column;						//�õ�ͼ�������
};

/**
*	@brief	��ȡָ����ֵ�ĵ�ͼ
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ����ص�ͼ�ṹ��ָ�� @ref WorldMapDef����
*			ȡʧ�ܷ��ؿ�
*/
WorldMapDef* WorldMap_GetWorldMap(int value);
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
int WorldMap_Extract(WorldMapDef* worldmap,int ROW,int COL);
#endif
