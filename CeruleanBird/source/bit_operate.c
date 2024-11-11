#include "bit_operate.h"


/***************
 * 位操作
****************/

//参数（数据指针,序号，目标值），这个函数聚焦于某一个字节，对该字节中的位进行置0或者置1操作，第三个参数默认将输入的非零值作为1，零值作为0，如果函数执行失败会返回-1
/*以单个字节的8个位为核心，修改位的值
 *将一个字节抽象为8个长度的数组，
 *其中从高位往低位查询，最高位数据为下标[0]，最低位数据为下标[7]
 *【参数】
 * __format         -->目标字节的地址
 * __serial__number -->目标位的下标序号
 * __binary_number  -->修改后的值，当值不为0时，将会被默认修改为1，当值为0时，则被修改为0
 * 操作成功返回0，否则返回-1；
 */
int Bit_Modify_Within_Byte(unsigned char* __format, unsigned char __serial__number, unsigned char __binary_number) {
    if (__serial__number >= BYTE_SIZE)//越界访问
        return -1;
    if (__binary_number)
        (*__format) |= (BIT_MASK >> __serial__number);
    else
        (*__format) &= ~(BIT_MASK >> __serial__number);
    return 0;
}
/*以单个字节的8个位为核心，读取指定位置的值
 *将一个字节抽象为8个长度的数组，
 *其中从高位往低位查询，最高位数据为下标[0]，最低位数据为下标[7]
 *【参数】
 * __format         -->目标字节的地址
 * __serial__number -->目标位的下标序号
 * 操作成功返回读取的值，否则返回-1；
 */
signed char Bit_Read_Within_Byte(const unsigned char* __format, unsigned char __serial__number) {
    if (__serial__number >= BYTE_SIZE)//越界访问
        return -1;
    unsigned char temp = ((*__format) & (BIT_MASK >> __serial__number)) ? 1 : 0;
    return temp;
}
/*以整个数据类型为核心，读取指定位置的值
 *将整个数据类型抽象为一个数组数组，
 *其中从低地址高位往高地址低位查询，最低地址的最高位数据为下标[0]
 *【参数】
 * __format         -->目标字节的地址
 * __size			-->目标数据类型的长度
 * __serial__number -->目标位的下标序号
 * 操作成功返回读取的值，否则返回-1；
 */
signed char Bit_Read(void* __format, unsigned short __size, unsigned short __serial__number) {
    unsigned short byte_num = __serial__number / BYTE_SIZE;//字节数
    unsigned char byte_bit = __serial__number % BYTE_SIZE;//字节中的位数
    if (__serial__number >= __size * BYTE_SIZE)//越界访问
        return -1;
    unsigned char* BIT_ptr = (unsigned char*)__format;
    BIT_ptr += byte_num;//移动指针
    unsigned char bit = ((*BIT_ptr) & (BIT_MASK >> byte_bit)) ? 1 : 0;
    return bit;
}
/*以整个数据类型为核心，修改指定位置的值
 *将整个数据类型抽象为一个数组数组，
 *其中从低地址高位往高地址低位查询，最低地址的最高位数据为下标[0]
 *【参数】
 * __format         -->目标字节的地址
 * __size			-->目标数据类型的长度
 * __serial__number -->目标位的下标序号
 * __binary_number  -->修改后的值，当值不为0时，将会被默认修改为1，当值为0时，则被修改为0
 * 操作成功返回0，否则返回-1；
 */
int Bit_Modify(void* __format, unsigned short __size, unsigned short __serial__number, unsigned char __binary_number) {
    unsigned short byte_num = __serial__number / BYTE_SIZE;//字节数
    unsigned char byte_bit = __serial__number % BYTE_SIZE;//字节中的位数

    if (__serial__number >= __size * BYTE_SIZE)//越界访问
        return -1;

    unsigned char* BIT_ptr = (unsigned char*)__format;
    BIT_ptr += byte_num;//移动指针

    if (__binary_number) {
        (*BIT_ptr) |= (BIT_MASK >> byte_bit);
        return 0;
    }
    else {
        (*BIT_ptr) &= ~(BIT_MASK >> byte_bit);
        return 0;
    }
}


/*
 *	将指定数量的bit位转化为字节数
 *【参数】
 * bitCount		-->		比特数
 * 非法输入返回-1，合法输入返回正确结果
 */
int bitToByte(int bitCount){
	if(bitCount < 0){
		return -1;
	}
	int byte = bitCount / 8;
	if((byte * 8) < bitCount){
		byte++;
	}
	return byte;
}
