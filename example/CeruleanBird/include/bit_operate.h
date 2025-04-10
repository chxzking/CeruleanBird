#ifndef _BIT_OPERATE_H_
#define _BIT_OPERATE_H_
#include "engine_config.h"

#define BIT_MASK 				( (unsigned char) 0x80 ) 		//λ��׼����
#define BYTE_SIZE				 8								//һ���ֽڵ�λ����

/***************
 * λ����
****************/

/**
*	@brief	λ�޸ģ��Ե����ֽڵ�8��λΪ���ģ��޸�λ��ֵ����һ���ֽڳ���Ϊ8�����ȵ����飬
*			���дӸ�λ����λ��ѯ�����λ����Ϊ�±�[0]�����λ����Ϊ�±�[7]
*	@param	__format         -->Ŀ���ֽڵĵ�ַ
*			__serial__number -->Ŀ��λ���±����
*			__binary_number  -->�޸ĺ��ֵ����ֵ��Ϊ0ʱ�����ᱻĬ���޸�Ϊ1����ֵΪ0ʱ�����޸�Ϊ0
*	@retval	�����ɹ�����0�����򷵻�-1��
*/
int Bit_Modify_Within_Byte(unsigned char* __format, unsigned char __serial__number, unsigned char __binary_number);
/**
*	@brief	λ��ȡ���Ե����ֽڵ�8��λΪ���ģ��޸�λ��ֵ����һ���ֽڳ���Ϊ8�����ȵ����飬
*			���дӸ�λ����λ��ѯ�����λ����Ϊ�±�[0]�����λ����Ϊ�±�[7]
*	@param	__format         -->Ŀ���ֽڵĵ�ַ
*			__serial__number -->Ŀ��λ���±����
*	@retval	�����ɹ�����Ŀ��λ��ֵ�����򷵻�-1��
*/
signed char Bit_Read_Within_Byte(const unsigned char* __format, unsigned char __serial__number);
/**
*	@brief	λ�޸ģ���������������Ϊ���ģ��޸�ָ��λ�õ�ֵ���������������ͳ���Ϊһ�������飬
*			���дӵ͵�ַ��λ���ߵ�ַ��λ��ѯ����͵�ַ�����λ����Ϊ�±�[0]
*	@param	__format         -->Ŀ���ֽڵĵ�ַ
*			__size			-->Ŀ���������͵ĳ���
*			__serial__number -->Ŀ��λ���±����
*			__binary_number  -->�޸ĺ��ֵ����ֵ��Ϊ0ʱ�����ᱻĬ���޸�Ϊ1����ֵΪ0ʱ�����޸�Ϊ0
*	@retval	�����ɹ�����0�����򷵻�-1��
*/
int Bit_Modify(void* __format, unsigned short __size, unsigned short __serial__number, unsigned char __binary_number);
/**
*	@brief	λ��ȡ����������������Ϊ���ģ���ȡָ��λ�õ�ֵ���������������ͳ���Ϊһ�������飬
*			���дӵ͵�ַ��λ���ߵ�ַ��λ��ѯ����͵�ַ�����λ����Ϊ�±�[0]
*	@param	__format         -->Ŀ���ֽڵĵ�ַ
*			__size			-->Ŀ���������͵ĳ���
*			__serial__number -->Ŀ��λ���±����
*	@retval	�����ɹ����ض�ȡ��ֵ�����򷵻�-1��
*/
signed char Bit_Read(void* __format, unsigned short __size, unsigned short __serial__number);
/**
*	@brief	��ָ��������bitλת��Ϊ�ֽ���
*	@param	bitCount		-->		������
*	@retval	�Ƿ����뷵��-1���Ϸ����뷵����ȷ���
*/
int bitToByte(int bitCount);
#endif
