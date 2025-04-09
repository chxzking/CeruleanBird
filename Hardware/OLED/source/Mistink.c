
/****************************************************************************

名称：【水墨晕染】Mistink,即Mist ink组合的新词，意为如雾状散开的墨水



*****************************************************************************/
/**
  ******************************************************************************
  * @file    Mistink.c
  * @author  红笺画文（chxzking）
  * @version V1.0
  * @date    7-2-2025
  * @brief   这个文件实现了对缓存区的操作，包括字库读取、像素操作、图形绘制等
  ******************************************************************************
  */
	
	
/* Includes ------------------------------------------------------------------*/	
#include "Mistink_Com.h"


/* 定义 ----------------------------------------------------------------------*/	

/** 
	*	@brief  由于缓存区本身是一个一维数组，而对缓存区的操作逻辑是二维数组，所以需要
	*				  将一维数组映射为二维数组(注意映射出来的是地址，而不是值)
  */
#define MapDisplayer(ptr,row,col)	(ptr->buffer + row * ptr->Pixel_Col + col)	



/* 位操作接口 ----------------------------------------------------------------*/	

/**
  * @brief  修改指定字节的位
  * @param  __format: 待修改的字节的地址
  * @param  __serial__number: 待修改的位的索引，以最低位为0位置
	* @param  __binary_number: 像素点状态，它的定义可以在此处查看 @ref PixelStatus_t
  * @retval 操作成功返回0，操作失败返回-1
  */
int Bit_Modification(unsigned char* __format,unsigned char __serial__number,PixelStatus_t __binary_number){
    if(__serial__number >= 8)//越界访问
        return -1;

    if(__binary_number == PixelOpen){
        (*__format) |= (0x01 << __serial__number);
        return 0; 
    }else{
        (*__format) &= ~(0x01 << __serial__number);
        return 0; 
    }
}

/**
  * @brief  获取指定字节的指定位
  * @param  __format: 待修改的字节的地址
  * @param  __serial__number: 待修改的位的索引，以最低位为0位置
  * @retval 操作成功返回位的数据，操作失败返回-1
  */
signed char Bit_loading(const unsigned char* __format,unsigned char __serial__number){
    if(__serial__number >= 8)//越界访问
        return -1;
    unsigned char temp = ((*__format)&( 0x01<<__serial__number))? 1 : 0;
    return temp;
}



/* 像素操作接口 --------------------------------------------------------------*/	

/**
  * @brief  操作指定像素点
  * @param  displayer: 显示器实例
  * @param  __ROW: 目标像素点的行
	* @param  __Columns: 目标像素点的列
	* @param  status: 像素点状态，它的定义可以在此处查看 @ref PixelStatus_t
  * @retval 操作成功返回0，操作失败返回-1
  */
int MistinkPixel_Operate(MistinkDisplayer_t* displayer,Buffer_t __ROW,Buffer_t __Columns,PixelStatus_t status){
	unsigned char register_position = __ROW/8;
	unsigned char bit_position = __ROW%8;
	if(__ROW >=displayer->Pixel_Row || __Columns >= displayer->Pixel_Col)
		return -1;//函数操作失败
	if(!Bit_Modification(MapDisplayer(displayer,register_position,__Columns),bit_position,status)){
		return 0;//函数操作成功
	}
	return -1;//函数操作失败
}

/**
  * @brief  操作指定显存寄存器
  * @param  displayer: 显示器实例
  * @param  __ROW: 目标寄存器的行
	* @param  __Columns: 目标寄存器的列
	* @param  Register_value:寄存器的值
  * @retval 操作成功返回0，操作失败返回-1
  */
int MistinkPixel_RegisterOP(MistinkDisplayer_t* displayer,Buffer_t __ROW,Buffer_t __Columns,unsigned char Register_value){
	if(__ROW >= displayer->real_Row || __Columns >= displayer->Pixel_Col)
		return -1;//函数操作失败
	*MapDisplayer(displayer,__ROW,__Columns) = Register_value;
	return 0;
}

/* 缓存区操作接口 --------------------------------------------------------------*/	

/**
  * @brief  清除缓存区中的数据
  * @param  displayer: 显示器实例
  * @retval none
  */
void MistinkOP_BufferReset(MistinkDisplayer_t* displayer){
		if(displayer == NULL){
			return;
		}
		int buffersize = displayer->real_Row * displayer->Pixel_Col;
		for(int i = 0;i < buffersize;i++){
				displayer->buffer[i] = 0;
		}
}

/**
  * @brief  返回指定缓存区
  * @param  displayer: 显示器实例
  * @retval 操作成功返回对应缓存区地址，操作失败返回空
  */
unsigned char* MistinkOP_GetBufferAPI(MistinkDisplayer_t* displayer){
	if(displayer == NULL)	return NULL;
	return displayer->buffer;
}

/**
  * @brief  坐标化图片
  * @param  ROW: 查找像素目标的行
  * @param  Columns: 查找像素目标的列
  * @param  picture: 图片地址
  * @param  picture_length: 图片的长度
  * @param  picture_width: 图片的宽度
  * @retval 返回像素的亮与灭
  */
unsigned char Picture_Coordinate(unsigned char ROW,unsigned char Columns,const unsigned char* picture,unsigned char picture_length,unsigned char picture_width){//图片坐标化
	
	if(ROW>=picture_width  || Columns>picture_length)
		return 0;
	unsigned char register_position = ROW/8;
	unsigned char bit_position = ROW%8;

	return (picture[register_position*picture_length+Columns] & (0x01<<bit_position))?1:0;
}
	
/**
  * @brief  加载一张图片
	* @param  displayer: 显示器实例
  * @param  ROW: 图片左上角起始点在屏幕的行
  * @param  Columns: 图片左上角起始点在屏幕的列
  * @param  picture: 图片地址
  * @param  picture_length: 图片的长度
  * @param  picture_width: 图片的宽度
  * @retval 操作成功返回0，操作失败返回-1
  */
int M_PictureLoad(MistinkDisplayer_t* displayer,Pos_t __ROW,Pos_t __Columns,const unsigned char* picture,PictureSize_t picture_length,PictureSize_t picture_width){
	if((__ROW >=displayer->Pixel_Row) || (__Columns >= displayer->Pixel_Col) || (__ROW <= -displayer->Pixel_Row) || (__Columns <= -displayer->Pixel_Col))//缓存区溢出
		return -1;//函数操作失败

	signed char start_x = (__ROW < 0) ? -__ROW : 0;
	signed char start_y = (__Columns < 0) ? -__Columns : 0;

	signed char end_x = (__ROW + picture_width - 1 > 63) ? 63 - __ROW : picture_width - 1;
	signed char end_y = (__Columns + picture_length - 1 > 127) ? 127 - __Columns : picture_length - 1;

	for(unsigned char i = start_x; i <= end_x; i++){
		for(unsigned char j = start_y; j <= end_y; j++){
			MistinkPixel_Operate(displayer,__ROW + i, __Columns + j, (PixelStatus_t)Picture_Coordinate(i, j, picture, picture_length, picture_width));
		}
	}
	return 0; //函数操作成功
}





/**
  * @brief  读取字模
  * @param  ROW: 字模中的目标点位行坐标
  * @param  Columns: 字模中的目标点位列坐标
  * @param  serial_numbers: 被查找的字模序号
  * @retval 返回像素的亮与灭
  */
int Font_Read(unsigned char ROW,unsigned char Columns,char serial_numbers){
	if(ROW>15  || Columns>8)
		return -1;
	unsigned char register_position = ROW/8;
	unsigned char bit_position = ROW%8;
	
	return (OLED_F8x16[serial_numbers][register_position*8+Columns] & (0x01<<bit_position))?1:0;
}
/**
  * @brief  输出字符串
	* @param  displayer: 显示器实例
  * @param  __ROW: 字符串在屏幕位置的行坐标
  * @param  __Columns: 字符串在屏幕位置的列坐标
	* @param  str:字符串
  * @retval none
  */
void M_String(MistinkDisplayer_t* displayer, Pos_t __ROW,Pos_t __Columns,char* str){
	unsigned char count = strlen(str);
	signed char x = __ROW;
	signed char y = __Columns;
	for(unsigned char serial_numbers = 0 ; serial_numbers < count;serial_numbers++){
		//y+=serial_numbers*8;
		for(signed char i = x; i < x+16;i++){
			signed char start_x = (__ROW < 0) ? -__ROW : 0;
			signed char start_y = (y < 0) ? -y : 0;

			signed char end_x = (__ROW + 16 - 1 > 63) ? 63 - __ROW : 16 - 1;
			signed char end_y = (y + 8 - 1 > 127) ? 127 - y : 8 - 1;

			for(unsigned char i = start_x; i <= end_x; i++){
				for(unsigned char j = start_y; j <= end_y; j++){
					MistinkPixel_Operate(displayer,__ROW + i, y + j,(PixelStatus_t)Font_Read(i, j, str[serial_numbers]-32));
				}
			}
		}
		y+=8;
	}
	
}
/**
  * @brief  输出整型数据
	* @param  displayer: 显示器实例
  * @param  __ROW: 整数在屏幕位置的行坐标
  * @param  __Columns: 整数在屏幕位置的列坐标
	* @param  num:整数的值
  * @retval none
  */
void M_Int(MistinkDisplayer_t* displayer, Pos_t __ROW,Pos_t __Columns,int num){
	char temp[17];
	sprintf(temp,"%d",num);
	temp[16] = '\0';
	M_String(displayer,__ROW, __Columns,temp);
}


///**
//  * @brief  OLED显示一个字符
//  * @param  Line 行位置，范围：1~4
//  * @param  Column 列位置，范围：1~16
//  * @param  Char 要显示的一个字符，范围：ASCII可见字符
//  * @retval 无
//  */
//void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
//{      	
//	uint8_t i;
//	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
//	for (i = 0; i < 8; i++)
//	{
//		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
//	}
//	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
//	for (i = 0; i < 8; i++)
//	{
//		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
//	}
//}

/*图形与算法--------------------------------------------------------------------*/



