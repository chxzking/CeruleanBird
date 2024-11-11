#include "stm32f10x.h"
#include "OLED_Font.h"
#include "OLED.h"
#include <math.h>
#include <string.h>
#define GPIO_SCL_PIN GPIO_Pin_8  //设置OLED的SCL引脚
#define GPIO_SDA_SDA GPIO_Pin_9  //设置OLED的SDA引脚

/*引脚配置*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_SCL_PIN, (BitAction)(x))  //注意BitAction是一个枚举
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_SDA_SDA, (BitAction)(x))


//屏幕缓存区
static unsigned char SCREEN_CACHE[8][128] = {0};


//缓存区接口
unsigned char* GetBufferAPI(void){
	unsigned char* temp = (unsigned char*)SCREEN_CACHE;
	return temp;
}

/*******************************************
*	【IIC协议准备】
*	[基本的引脚配置]
*	[通信开始与结束标志]
*	[硬件屏幕初始化]
********************************************/

/*引脚初始化*/
void OLED_I2C_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//启动外设时钟
	//初始化引脚配置
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//设置为开漏输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_SCL_PIN;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_SDA_SDA;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//默认为高电平
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}


//通信开始标志
void OLED_I2C_Start(void){
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

//通信结束标志
void OLED_I2C_Stop(void){
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

//向OLED屏幕发送从高位向低位的顺序发送一个字节的信息，可以是数据也可以是命令
void OLED_I2C_SendByte(unsigned char Byte){
	unsigned char i;
	for (i = 0; i < 8; i++){
		OLED_W_SDA(Byte & (0x80 >> i));//发送字节的位
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}


//用于向从机发送一个命令
void OLED_WriteCommand(unsigned char Command){
	OLED_I2C_Start();//发送标志
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();//停止发送标志
}

//用于向从机发送一个数据
void OLED_WriteData(unsigned char Data){
	OLED_I2C_Start();//发送标志
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();//停止发送标志
}

//以页的模式设置光标的位置，Y是页号，X是列号
void OLED_SetCursor(unsigned char Y, unsigned char X){
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}


//初始化屏幕
void OLED_Init(void){
	unsigned short i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时用于稳定系统
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
	
	
	 // 配置NVIC
	EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 设置NVIC中断分组2:2位抢占优先级，2位响应优先级 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 配置软件中断 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // 抢占优先级最高，数值越小优先级越高
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // 响应优先级最高，数值越小优先级越高
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // 使能中断
    NVIC_Init(&NVIC_InitStructure);

    /* 配置EXTI Line10 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;     // 上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}




/************************************************
*	【屏幕操作】
*	[打印缓存区信息进入屏幕]
*	[清空屏幕显示]
*************************************************/

//清空显示
void OLED_Clear(void){  
	unsigned char i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

//将缓存区中的数据输出到屏幕上；
void Buffer_Output(void){
	unsigned char i , j;
	for(i = 0; i < 8;i++){
		OLED_SetCursor(i, 0);
		for(j = 0;j < 128;j++){
			OLED_WriteData(SCREEN_CACHE[i][j]);
		}
	}
}
//原子操作，将缓存区的值输出到屏幕上
void print(void){
	EXTI_GenerateSWInterrupt(EXTI_Line10);
}

//中断ISR
void EXTI15_10_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
		Buffer_Output();
        // 清除中断标志位
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
}

//

/************************************************
*	【缓存区操作】
*	[打印缓存区信息进入屏幕]
*	[清空屏幕显示]
*************************************************/

//修改指定字节的位
int Bit_Modification(unsigned char* __format,unsigned char __serial__number,BOLT __binary_number){
    if(__serial__number >= 8)//越界访问
        return -1;

    if(__binary_number == OPEN){
        (*__format) |= (0x01 << __serial__number);
        return 0; 
    }else{
        (*__format) &= ~(0x01 << __serial__number);
        return 0; 
    }
}

	
//参数（数据指针,序号），这个函数聚焦于某一个字节，用于输出指定字节中的指定位的值，如果函数执行失败会返回-1
signed char Bit_loading(const unsigned char* __format,unsigned char __serial__number){
    if(__serial__number >= 8)//越界访问
        return -1;
    unsigned char temp = ((*__format)&( 0x01<<__serial__number))? 1 : 0;
    return temp;
}


//操控指定位置像素点(横坐标（0-63），纵坐标（0-127）) 【已经通过测试】
int Pixel_Operate(unsigned char __ROW,unsigned char __Columns,BOLT Bolt){
	unsigned char register_position = __ROW/8;
	unsigned char bit_position = __ROW%8;
	if(__ROW >=64 || __Columns >= 128)
		return -1;//函数操作失败
	if(!Bit_Modification(&SCREEN_CACHE[register_position][__Columns],bit_position,Bolt)){
		return 0;//函数操作成功
	}
	return -1;//函数操作失败
}



//给指定显存赋值(横坐标（0-7），纵坐标（0-127）)【已经通过测试】
int Display_register(unsigned char __ROW,unsigned char __Columns,unsigned char Register_value){
	if(__ROW >=8 || __Columns >= 128)
		return -1;//函数操作失败
	SCREEN_CACHE[__ROW][__Columns] = Register_value;
	return 0;
}
//刷新缓存区
int Buffer_Refresh(void){
	for(unsigned char i = 0;i<8;i++){
		for(unsigned char j = 0;j<128;j++){
			Display_register(i,j,0x00);
		}
	}
	return 0;
}
////选中动画
int Load_animation(unsigned char ROW,unsigned char Columns,unsigned char length,unsigned width){//局部刷新
	if(length == width){
		unsigned char x = ROW;
		unsigned char y = Columns;
		unsigned char r = width/2+2;
		unsigned char count = 0;
		for(float rad = 0; rad <= 360;rad++){	
			float x1 = r* cos(rad*(3.14/180));
			float y1 = r* sin(rad*(3.14/180));
			Pixel_Operate(x1+x,y1+y,OPEN);
			count++;
			if(count%60 == 0){
				print();		
			}
		}
		count = 0;
		for(float rad = 0; rad <= 360;rad++){	
			float x1 = r* cos(rad*(3.14/180));
			float y1 = r* sin(rad*(3.14/180));
			Pixel_Operate(x1+x,y1+y,CLOSE);
			count++;
			if(count%60 == 0){
				print();		
			}
		}
	}
	return 0;
}


//前两个参数分别是要取的图片的坐标
unsigned char Picture_Coordinate(unsigned char ROW,unsigned char Columns,unsigned char* picture,unsigned char picture_length,unsigned char picture_width){//图片坐标化
	
	if(ROW>=picture_width  || Columns>picture_length)
		return 0;
	unsigned char register_position = ROW/8;
	unsigned char bit_position = ROW%8;

	return (picture[register_position*picture_length+Columns] & (0x01<<bit_position))?1:0;
}
	
//加载图片，参数：（图片位置，图片的数据，图片的长宽）
int Picture_Loading(signed char __ROW,signed char __Columns,unsigned char* picture,unsigned char picture_length,unsigned char picture_width){
	if(__ROW >63 || __Columns > 127||__ROW < -63 || __Columns < -127)//缓存区溢出
		return -1;//函数操作失败

	signed char start_x = (__ROW < 0) ? -__ROW : 0;
	signed char start_y = (__Columns < 0) ? -__Columns : 0;

	signed char end_x = (__ROW + picture_width - 1 > 63) ? 63 - __ROW : picture_width - 1;
	signed char end_y = (__Columns + picture_length - 1 > 127) ? 127 - __Columns : picture_length - 1;

	for(unsigned char i = start_x; i <= end_x; i++){
		for(unsigned char j = start_y; j <= end_y; j++){
			Pixel_Operate(__ROW + i, __Columns + j, (BOLT)Picture_Coordinate(i, j, picture, picture_length, picture_width));
		}
	}
	return 0; //函数操作成功
}


//圆角矩形
void Rectangles_With_Rounded_Corners(unsigned char ROW,unsigned char Columns,unsigned char length,unsigned char width,BOLT Bolt,unsigned char radian){
	//unsigned char r = 10;
	//横线
	for(unsigned char i = radian;i<length -radian;i++){
		Pixel_Operate(ROW,Columns+i,Bolt);
		Pixel_Operate(ROW+width,Columns+i,Bolt);
	}
	//竖线
	for(unsigned char j = radian;j<width -radian+1;j++){
		Pixel_Operate(ROW+j,Columns,Bolt);
		Pixel_Operate(ROW+j,Columns+length-1,Bolt);
	}
	//圆弧
	//左上
	for(float rad = 180; rad <= 270;rad++){				
		Pixel_Operate(radian * cos(rad*(3.14/180))+ROW+radian,radian * sin(rad*(3.14/180))+Columns+radian,Bolt);	
	}
	//左下
	for(float rad = 270; rad <= 360;rad++){				
		Pixel_Operate(radian * cos(rad*(3.14/180))+ROW+ width-radian+1,radian * sin(rad*(3.14/180))+Columns+radian,Bolt);	
	}
	//右上
	for(float rad = 90; rad <= 180;rad++){				
		Pixel_Operate(radian * cos(rad*(3.14/180))+ROW+radian,radian * sin(rad*(3.14/180))+Columns+length-radian,Bolt);	
	}
	//右下
	for(float rad = 0; rad <= 90;rad++){				
		Pixel_Operate(radian * cos(rad*(3.14/180))+ROW+ width-radian+1,radian * sin(rad*(3.14/180))+Columns+length-radian,Bolt);	
	}
}
//画圆
void Draw_Circle(int center_ROW, int center_Columns, int radius, BOLT Bolt) {
    for(float angle = 0; angle < 360; angle ++) {
        int ROW = radius * cos(angle * 3.14 / 180) + center_ROW;
        int Columns = radius * sin(angle * 3.14 / 180) + center_Columns;
        Pixel_Operate(ROW, Columns, Bolt);
    }
}

//矩形
void Rectangle(signed char ROW,signed char Columns,unsigned char length, unsigned char width){
	for(signed char x = ROW;x<ROW+width;x++ ){
		Pixel_Operate(x,Columns,OPEN);
		Pixel_Operate(x,Columns+length,OPEN);
	}
	for(signed char y = Columns;y<Columns+length;y++){
		Pixel_Operate(ROW,y,OPEN);
		Pixel_Operate(ROW+width,y,OPEN);
	}
}

//滑块按钮组件（左边是关，右边是开） [原始值 新值]最后一个参数是按钮的半径，按钮移动速度
int Slide_Button_Asssembly(BOLT* Original_Value,BOLT* New_Value,unsigned char ROW,unsigned char Columns,unsigned char length, unsigned char width, unsigned char step){
	if(Columns+length>127){//超出左右范围
		return -1;
	}
	unsigned char x,yc,yo;
	unsigned char radian = width/2;
	x = ROW+radian;
	yc =  Columns + radian;
	yo = Columns + length - radian;
	
	if(*Original_Value == *New_Value){//新值和旧值一样，不生成动画
		Rectangles_With_Rounded_Corners(ROW,Columns,length,width,OPEN,radian);
		if(*Original_Value == CLOSE)
			Draw_Circle(x,yc,radian-1,OPEN);
		else
			Draw_Circle(x,yo,radian-1,OPEN);
		print();
		return 1;
	}
	
	if((int)*Original_Value - (int)*New_Value > 0){//由open变为close 
		Rectangles_With_Rounded_Corners(ROW,Columns,length,width,OPEN,radian);
		for(unsigned char y = yo;y>=yc;y-=step){
			Draw_Circle(x,y,radian-1,OPEN);
			print();
			Draw_Circle(x,y,radian-1,CLOSE);
		}
		Draw_Circle(x,yc,radian-1,OPEN);
		
	}else{//由close变为open
		Rectangles_With_Rounded_Corners(ROW,Columns,length,width,OPEN,radian);		
		for(unsigned char y = yc;y<=yo;y+=step){
			Draw_Circle(x,y,radian-1,OPEN);
			print();
			Draw_Circle(x,y,radian-1,CLOSE);
		}
		Draw_Circle(x,yo,radian-1,OPEN);
		
	}
	print();
	*Original_Value = *New_Value;//新值替换旧值
	return 0;
}


int Font_Read(unsigned char ROW,unsigned char Columns,char serial_numbers){
	if(ROW>15  || Columns>8)
		return -1;
	unsigned char register_position = ROW/8;
	unsigned char bit_position = ROW%8;
	
	return (OLED_F8x16[serial_numbers][register_position*8+Columns] & (0x01<<bit_position))?1:0;
}

void Hj_String(signed char __ROW,signed char __Columns,char* str){
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
					Pixel_Operate(__ROW + i, y + j, (BOLT)Font_Read(i, j, str[serial_numbers]-32));
				}
			}
		}
		y+=8;
	}
	
}

void Hj_Int(signed char __ROW,signed char __Columns,int num){
	char temp[17];
	sprintf(temp,"%d",num);
	temp[16] = '\0';
	Hj_String( __ROW, __Columns,temp);
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

///**
//  * @brief  OLED显示字符串
//  * @param  Line 起始行位置，范围：1~4
//  * @param  Column 起始列位置，范围：1~16
//  * @param  String 要显示的字符串，范围：ASCII可见字符
//  * @retval 无
//  */
//void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
//{
//	uint8_t i;
//	for (i = 0; String[i] != '\0'; i++)
//	{
//		OLED_ShowChar(Line, Column + i, String[i]);
//	}
//}

///**
//  * @brief  OLED次方函数
//  * @retval 返回值等于X的Y次方
//  */
//uint32_t OLED_Pow(uint32_t X, uint32_t Y)
//{
//	uint32_t Result = 1;
//	while (Y--)
//	{
//		Result *= X;
//	}
//	return Result;
//}

///**
//  * @brief  OLED显示数字（十进制，正数）
//  * @param  Line 起始行位置，范围：1~4
//  * @param  Column 起始列位置，范围：1~16
//  * @param  Number 要显示的数字，范围：0~4294967295
//  * @param  Length 要显示数字的长度，范围：1~10
//  * @retval 无
//  */
//void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
//{
//	uint8_t i;
//	for (i = 0; i < Length; i++)							
//	{
//		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
//	}
//}

///**
//  * @brief  OLED显示数字（十进制，带符号数）
//  * @param  Line 起始行位置，范围：1~4
//  * @param  Column 起始列位置，范围：1~16
//  * @param  Number 要显示的数字，范围：-2147483648~2147483647
//  * @param  Length 要显示数字的长度，范围：1~10
//  * @retval 无
//  */
//void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
//{
//	uint8_t i;
//	uint32_t Number1;
//	if (Number >= 0)
//	{
//		OLED_ShowChar(Line, Column, '+');
//		Number1 = Number;
//	}
//	else
//	{
//		OLED_ShowChar(Line, Column, '-');
//		Number1 = -Number;
//	}
//	for (i = 0; i < Length; i++)							
//	{
//		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
//	}
//}

///**
//  * @brief  OLED显示数字（十六进制，正数）
//  * @param  Line 起始行位置，范围：1~4
//  * @param  Column 起始列位置，范围：1~16
//  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
//  * @param  Length 要显示数字的长度，范围：1~8
//  * @retval 无
//  */
//void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
//{
//	uint8_t i, SingleNumber;
//	for (i = 0; i < Length; i++)							
//	{
//		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
//		if (SingleNumber < 10)
//		{
//			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
//		}
//		else
//		{
//			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
//		}
//	}
//}

///**
//  * @brief  OLED显示数字（二进制，正数）
//  * @param  Line 起始行位置，范围：1~4
//  * @param  Column 起始列位置，范围：1~16
//  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
//  * @param  Length 要显示数字的长度，范围：1~16
//  * @retval 无
//  */
//void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
//{
//	uint8_t i;
//	for (i = 0; i < Length; i++)							
//	{
//		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
//	}
//}



