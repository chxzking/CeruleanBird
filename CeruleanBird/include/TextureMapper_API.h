#ifndef __TEXTUREMAPPER_API_H__
#define __TEXTUREMAPPER_API_H__

/*��ǰ�ļ�Ϊһ����*/

//�������������
/**
*
*	@ref TEXTURE_TYPE
*
*/
typedef enum TEXTURE_TYPE{
	BYTE_TEXTURE,		   //��������Ԫ�ص�ֵ���ֽ�Ϊ��λ��һ�����ֽڿ���һ�����ص㣩
	BIT_TEXTURE,		   //��������Ԫ�ص�ֵ��λΪ��λ��һ��λ����һ�����ص㣩
	ERROR_TEXTURE		   //��������
}TEXTURE_TYPE;

//����ṹ��ӳ��ṹ��
typedef struct TextureMapperDef TextureMapperDef;

/**
*		@note	ע�⣬���������ֵ�������Ψһ��
*	@brief	���������������ͨ����ͼԪ�������������
*	@param	
*		@arg	value	��ͼԪ�ص�ֵ
*		@arg	Texture_Picture	��Ҫ���󶨵���������
*		@arg	Texture_Row	�����������
*		@arg	Texture_COL�����������
*		@arg	Texture_Type ������������ͣ������ֿ�ѡ���ͣ�һ��������ÿһ��
*				�ֽڴ���һ�����ص� BYTE_TEXTURE ������һ����ÿһ��λ����һ��
*				���ص� BIT_TEXTURE�������� @ref TEXTURE_TYPE
*	@retval	
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����1����ζ�Ÿ�ֵ�Ѿ�����
*		@arg	������ؽ����-1����ζ���ڴ����ʧ�ܻ��߲�������
*/
int Texture_Add(int value,unsigned char* Texture_Picture,\
	int Texture_Row_Pixel,int Texture_COL_Pixel,TEXTURE_TYPE Texture_Type);
/**
*	@brief	ɾ��������ָ��������
*	@param	
*		@arg	value	��ͼԪ�ص�ֵ
*	@retval	none��
*/
void Texture_Delete(int value);
/**
*	@brief	ɾ�����������е�����(�������������ͷ�)
*	@param	none
*	@retval	none��
*	@note	��ʱ��Խϳ�
*/
void Texture_Free(void);
/**
*	@brief	�޸İ󶨵���������󶨵�ֵ
*	@param	
*		@arg	value	ԭ�󶨵�ֵ
*		@arg	new_value	���޸ĵ�ֵ
*	@retval	�޸ĳɹ�����0���޸ĸ�ʧ�ܷ���-1
*/
int Texture_ValueModify(int value,int new_value);
/**
*	@brief	�޸İ󶨵��������
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	ROW	�������
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int Texture_ROWModify(int value,int ROW);
/**
*	@brief	�޸İ󶨵��������
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	COL	�������
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int Texture_COLModify(int value,int COL);
/**
*	@brief	�޸İ󶨵���������
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	array	�µ���������
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int Texture_ArrayModify(int value,unsigned char* array);
/**
*	@brief	�޸İ󶨵����������
*	@param	
*		@arg	value	�󶨵�ֵ
*		@arg	Texture_Type	�µ��������ͣ�BYTE_TEXTURE
*				�� BIT_TEXTURE��ѡ�������� @ref TEXTURE_TYPE
*	@retval	�޸ĳɹ�����0���޸�ʧ�ܷ���-1
*/
int Texture_TypeModify(int value,TEXTURE_TYPE Texture_Type);
/**
*	@brief	��ȡָ����ֵ������
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ���������ṹ��ָ�� @ref TextureMapDef����
*			ȡʧ�ܷ��ؿ�
*/
TextureMapperDef* Texture_GetTexture(int value);
/**
*	@brief	��ȡָ����ֵ����������
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ��������������ַ����ȡʧ�ܷ��ؿ�
*/
unsigned char* Texture_GetArray(int value);
/**
*	@brief	��ȡָ����ֵ����������
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ��������������ַ BYTE_TEXTURE �� BIT_TEXTURE
*			��ȡʧ�ܷ��� ERROR_TEXTURE �������� @ref TEXTURE_TYPE
*/
TEXTURE_TYPE Texture_GetTextureType(int value);
/**
*	@brief	��ȡָ����ֵ������������߼���
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ��������������� ����ȡʧ�ܷ���-1
*/
int Texture_GetROW(int value);
/**
*	@brief	��ȡָ����ֵ�������������
*	@param	
*		@arg	value	�󶨵�ֵ
*	@retval	��ȡ�ɹ��������������ַ���� ����ȡʧ�ܷ���-1
*/
int Texture_GetColumn(int value);

#endif
