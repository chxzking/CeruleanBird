#include "engine_config.h"
/**************************************

		�ڴ�ģ�ͽ������

***************************************/
#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //���������ж�

#ifndef _HEAP_SOLUTION_1_H_
#define _HEAP_SOLUTION_1_H_

#define C_NULL						( ( void* ) 0 )			//����һ����ָ���ַ
//����

/**
*	@brief	��ʼ���ڴ��
*	@param	none
*	@retval	none
*/
void memory_pool_init(void);
/**
*		@note	�ú�������ǰ�������Ѿ�ִ�й�memory_pool_init()��ʼ��������ᵼ��δ�������
*	@brief	�����ȡָ����С�ڴ�ռ�
*	@param	size -->	Ԥ����ռ��С
*	@retval	�����ɹ����ؿռ��׵�ַ�����򷵻ؿգ�
*/
void* c_malloc(unsigned int size);
/**
*		@note	�ú�������ǰ�������Ѿ�ִ�й�memory_pool_init()��ʼ��������ᵼ��δ�������
*	@brief	�����ȡ����ָ����С�ڴ�ռ�
*		@note	ע��ֻ�����ſռ䲻����С�ռ�
*	@param	size -->	Ԥ����ռ��С
*			ptr	-->		ԭ�ռ��ֵ
*	@retval	�����ɹ������¿ռ��׵�ַ�����򷵻ؿգ�
*		@note	ע�Ᵽ��ԭ�ռ�ĵ�ַ���Է��ռ����ʧ�ܶ�ʧԭ�ռ��ַ����ڴ�й¶
*/
void* c_realloc(void* ptr,unsigned int size);
/**
*	@brief	�ͷŷ���ռ�
*	@param	ptr	-->	�ռ��׵�ַ
*	@retval	none
*/
void c_free(void* ptr);


#endif	//_HEAP_SOLUTION_1_H_

#endif	//HEAP_SCHEME == HEAP_SOLUTION_1

