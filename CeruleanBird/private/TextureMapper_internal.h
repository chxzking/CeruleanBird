/*��ǰ�ļ�Ϊ������*/
#ifndef __TEXTUREMAPPER_INTERNAL_H__
#define __TEXTUREMAPPER_INTERNAL_H__
#include "TextureMapper_API.h"
/**
*
*	@ref TextureMapDef
*
*/
struct TextureMapperDef{
	unsigned char* texture_array;	//������������ַ��ע�⣬����ֻ��Ϊһά���飩
	int Row;						//������������У��������������ε��У�
	int Logic_ROW;					//��������߼��У���ʹ��λͼ����������ʱ������λͼ����(����ʹ��λͼ�������Ч)
	int Column;						//�������������
	TEXTURE_TYPE texture_type;		//�����������Ա������
};

/**
*	@brief	��ʾָ��λ�õ�Ԫ��
*	@param	
*		@arg	texture	 ������Դ
*		@arg	ROW_Pixel	�У�������Ϊ��λ��
*		@arg	COL_Pixel	�У�������Ϊ��λ��
*	@retval	
*		@arg	��ȡʧ�ܷ���-1
*		@arg	��ȡ���źŷ��ط�0����
*		@arg	��ȡ���źŷ���0
*/
int Texture_Extract(TextureMapperDef* texture,int ROW_Pixel,int COL_Pixel);
#endif
