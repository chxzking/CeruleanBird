#include "TextureMapper_internal.h"


#include "RB_Tree_API.h"
#include "bit_operate.h"
#include "engine_config.h"

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


RB_Node* TextureMapper_Header = C_NULL;

/****************
	��������ӡ�
*****************/

//�������
int Texture_Add(int value,unsigned char* Texture_Picture,\
	int Texture_Row_Pixel,int Texture_COL_Pixel,TEXTURE_TYPE Texture_Type){
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //�������涯̬�ڴ�������
		memory_pool_init();
	#endif	//�������涯̬�ڴ�������
	
	//����У��
	if(Texture_Picture == C_NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	//�ڴ����
	TextureMapperDef* node = (TextureMapperDef*)malloc(sizeof(TextureMapperDef));
	if(node == C_NULL)	return -1;
	node->texture_array = Texture_Picture;
	node->texture_type = Texture_Type;
	node->Column = Texture_COL_Pixel;
	node->Logic_ROW = Texture_Row_Pixel;
	if(Texture_Type == BIT_TEXTURE){
		node->Row = bitToByte(Texture_Row_Pixel);
	}else{
		node->Row = Texture_Row_Pixel;
	}
	//��ӽ�������
	int result = rbInsert(&TextureMapper_Header, value,(void*)node);
	if(result != 0){
		//��ӳ�������
		free(node);
		return result;
	}
	return 0;
}

/****************
	��������ҡ�
*****************/

//�������
TextureMapperDef* Texture_GetTexture(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	return result;
}
unsigned char* Texture_GetArray(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return C_NULL;
	}
	return result->texture_array;
}
TEXTURE_TYPE Texture_GetTextureType(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return ERROR_TEXTURE;
	}
	return result->texture_type;
}
int Texture_GetROW(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Logic_ROW;
}
int Texture_GetColumn(int value){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	return result->Column;
}
/****************
  ����������޸ġ�
*****************/
int Texture_ValueModify(int value,int new_value){
	//����Ŀ��ֵ�Ƿ����
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL)	return -1;//����ʧ��
	
	//������ֵ�Ƿ��Ѿ�����
	TextureMapperDef* new_result = (TextureMapperDef*)search(TextureMapper_Header, new_value);
	if(new_result != C_NULL)	return -1;//���ִ��ڣ��򲻿ɸ���
	
	//�����½ڵ�
	int temp = Texture_Add(new_value,new_result->texture_array,new_result->Logic_ROW ,new_result->Column,new_result->texture_type);
	if(temp != 0)	return -1;//�½ڵ㴴��ʧ��
	
	//ɾ��ԭ�ڵ�
	Texture_Delete(value);
	return 0;
}

int Texture_ROWModify(int value,int ROW){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Logic_ROW = ROW;
	if(result->texture_type == BIT_TEXTURE){
		result->Row = bitToByte(ROW);
	}else{
		result->Row = ROW;
	}
	return 0;
}
int Texture_COLModify(int value,int COL){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->Column= COL;
	return 0;
}
int Texture_ArrayModify(int value,unsigned char* array){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	if(array == C_NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	result->texture_array = array;
	return 0;
}
int Texture_TypeModify(int value,TEXTURE_TYPE Texture_Type){
	TextureMapperDef* result = (TextureMapperDef*)search(TextureMapper_Header, value);
	if(result == C_NULL){
		return -1;
	}
	result->texture_type = Texture_Type;
	return 0;
}

/****************
	������ɾ����
*****************/

//��������ɾ��
void Texture_Delete(int value){
	if(TextureMapper_Header == C_NULL)	return;
	rbDelete(&TextureMapper_Header, value);
}

//�ͷ���������
void Texture_Free(void){
	RB_Tree_Free(&TextureMapper_Header);
}

/****************
	��������Ϣ��ȡ��
*****************/
//��ʾָ��λ�õ�Ԫ����
int Texture_Extract(TextureMapperDef* texture,int ROW_Pixel,int COL_Pixel){
	//�������
	if(texture == C_NULL){
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	if((ROW_Pixel < 0) || (ROW_Pixel >= texture->Logic_ROW))	return -1;
	if((COL_Pixel < 0) || (COL_Pixel >= texture->Column))	return -1;
	//��λģʽ
	if(texture->texture_type == BIT_TEXTURE){
		int byte = ROW_Pixel/8;
		int offset = ROW_Pixel%8;
//		unsigned char texture_Unit = texture->texture_array[byte*texture->Column + COL_Pixel];
//		return Bit_Read_Within_Byte(&texture_Unit,7 - offset);
		//����ʹ�ôӵ�λ����λ��ȡ������Ϊȡģ�������Ļ��λΪ�ֽڵ�λ������10000000����ʾ��������1��������
		//�������ȡģ���������ط������ҵ�ͼƬ��ת���Ҳ����˼���Сʱ�Ĵ������⣬���շ�����ȡģ������⣬�����ˡ�
		return (texture->texture_array[byte*texture->Column + COL_Pixel] & (0x01<<offset)) ? 1:0;
	}
	//�ֽ�ģʽ
	else if(texture->texture_type == BYTE_TEXTURE){
		return texture->texture_array[ROW_Pixel*texture->Column + COL_Pixel];
	}
	return -1;
}
