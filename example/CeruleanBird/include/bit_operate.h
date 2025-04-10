#ifndef _BIT_OPERATE_H_
#define _BIT_OPERATE_H_
#include "engine_config.h"

#define BIT_MASK 				( (unsigned char) 0x80 ) 		//位基准掩码
#define BYTE_SIZE				 8								//一个字节的位长度

/***************
 * 位操作
****************/

/**
*	@brief	位修改，以单个字节的8个位为核心，修改位的值，将一个字节抽象为8个长度的数组，
*			其中从高位往低位查询，最高位数据为下标[0]，最低位数据为下标[7]
*	@param	__format         -->目标字节的地址
*			__serial__number -->目标位的下标序号
*			__binary_number  -->修改后的值，当值不为0时，将会被默认修改为1，当值为0时，则被修改为0
*	@retval	操作成功返回0，否则返回-1；
*/
int Bit_Modify_Within_Byte(unsigned char* __format, unsigned char __serial__number, unsigned char __binary_number);
/**
*	@brief	位读取，以单个字节的8个位为核心，修改位的值，将一个字节抽象为8个长度的数组，
*			其中从高位往低位查询，最高位数据为下标[0]，最低位数据为下标[7]
*	@param	__format         -->目标字节的地址
*			__serial__number -->目标位的下标序号
*	@retval	操作成功返回目标位的值，否则返回-1；
*/
signed char Bit_Read_Within_Byte(const unsigned char* __format, unsigned char __serial__number);
/**
*	@brief	位修改，以整个数据类型为核心，修改指定位置的值，将整个数据类型抽象为一个大数组，
*			其中从低地址高位往高地址低位查询，最低地址的最高位数据为下标[0]
*	@param	__format         -->目标字节的地址
*			__size			-->目标数据类型的长度
*			__serial__number -->目标位的下标序号
*			__binary_number  -->修改后的值，当值不为0时，将会被默认修改为1，当值为0时，则被修改为0
*	@retval	操作成功返回0，否则返回-1；
*/
int Bit_Modify(void* __format, unsigned short __size, unsigned short __serial__number, unsigned char __binary_number);
/**
*	@brief	位读取，以整个数据类型为核心，读取指定位置的值，将整个数据类型抽象为一个大数组，
*			其中从低地址高位往高地址低位查询，最低地址的最高位数据为下标[0]
*	@param	__format         -->目标字节的地址
*			__size			-->目标数据类型的长度
*			__serial__number -->目标位的下标序号
*	@retval	操作成功返回读取的值，否则返回-1；
*/
signed char Bit_Read(void* __format, unsigned short __size, unsigned short __serial__number);
/**
*	@brief	将指定数量的bit位转化为字节数
*	@param	bitCount		-->		比特数
*	@retval	非法输入返回-1，合法输入返回正确结果
*/
int bitToByte(int bitCount);
#endif
