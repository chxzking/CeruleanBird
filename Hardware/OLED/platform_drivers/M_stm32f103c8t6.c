#include "M_stm32f103c8t6.h"
//平台标准库
#include "stm32f10x.h"  



//硬件SPI
#define PIN_SPI_CLK				GPIO_Pin_5//PA5
#define GPIO_SPI_CLK			GPIOA

#define PIN_SPI_SDA				GPIO_Pin_7//PA7
#define GPIO_SPI_SDA			GPIOA

#define PIN_SPI_CS				GPIO_Pin_2//PA2
#define GPIO_SPI_CS				GPIOA

#define PIN_SPI_DC				GPIO_Pin_3//PA3
#define GPIO_SPI_DC				GPIOA

#define PIN_SPI_RST				GPIO_Pin_4//PA4
#define GPIO_SPI_RST			GPIOA

#define SPI_CMD	0
#define SPI_DATA	1

/*******************************************
*	【SPI协议准备】
*	[基本的引脚配置]
*	[硬件屏幕初始化]
********************************************/

void OLED_Port_Init(void){
	//时钟开启
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	//普通端口
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_SPI_CS | PIN_SPI_DC | PIN_SPI_RST;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	//设置初始状态
	GPIO_SetBits(GPIO_SPI_CS, PIN_SPI_CS);//拉高片选
	GPIO_SetBits(GPIO_SPI_RST, PIN_SPI_RST);//拉高复位
	GPIO_ResetBits(GPIO_SPI_DC, PIN_SPI_DC);//拉低信号选择
	
	//SPI端口
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = PIN_SPI_CLK | PIN_SPI_SDA;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//SPI配置
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_BaudRatePrescaler  = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;  // CPHA=0
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;    // CPOL=0
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;//单线发送
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_InitStruct);
	
	//开启SPI
	SPI_Cmd(SPI1, ENABLE);
}

void OLED_WR_Byte(uint8_t data, uint8_t mode) {
    if (mode == SPI_CMD)
        GPIO_ResetBits(GPIO_SPI_DC, PIN_SPI_DC);
    else
        GPIO_SetBits(GPIO_SPI_DC, PIN_SPI_DC);

    GPIO_ResetBits(GPIO_SPI_CS, PIN_SPI_CS); // CS拉低

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); // 等待发送缓冲区空
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);   // 等待发送完成

    GPIO_SetBits(GPIO_SPI_CS, PIN_SPI_CS); // CS拉高
}
//初始化屏幕
void OLED_Init(void){
OLED_Port_Init();
	GPIO_ResetBits(GPIO_SPI_RST, PIN_SPI_RST);
	for(int i = 0;i<1000000;i++){
	}
	GPIO_SetBits(GPIO_SPI_RST, PIN_SPI_RST);
	
	    /* 初始化命令序列 */
	OLED_WR_Byte(0xAE, SPI_CMD); // 关闭显示
	OLED_WR_Byte(0xD5, SPI_CMD); // 设置时钟分频
	OLED_WR_Byte(0x80, SPI_CMD);
	OLED_WR_Byte(0xA8, SPI_CMD); // 设置复用率
	OLED_WR_Byte(0x3F, SPI_CMD); // 1/64
	OLED_WR_Byte(0xD3, SPI_CMD); // 显示偏移
	OLED_WR_Byte(0x00, SPI_CMD);
	OLED_WR_Byte(0x40, SPI_CMD); // 起始行
	OLED_WR_Byte(0x8D, SPI_CMD); // 电荷泵
	OLED_WR_Byte(0x14, SPI_CMD); // 开启电荷泵
	OLED_WR_Byte(0x20, SPI_CMD); // 内存模式
	OLED_WR_Byte(0x00, SPI_CMD); // 水平地址模式
	OLED_WR_Byte(0xA1, SPI_CMD); // 段重映射
	OLED_WR_Byte(0xC8, SPI_CMD); // COM扫描方向
	OLED_WR_Byte(0xDA, SPI_CMD); // COM硬件配置
	OLED_WR_Byte(0x12, SPI_CMD);
	OLED_WR_Byte(0x81, SPI_CMD); // 对比度
	OLED_WR_Byte(0xCF, SPI_CMD);
	OLED_WR_Byte(0xD9, SPI_CMD); // 预充电周期
	OLED_WR_Byte(0xF1, SPI_CMD);
	OLED_WR_Byte(0xDB, SPI_CMD); // VCOMH
	OLED_WR_Byte(0x40, SPI_CMD);
	OLED_WR_Byte(0xA4, SPI_CMD); // 显示全部ON
	OLED_WR_Byte(0xA6, SPI_CMD); // 正常显示
	OLED_WR_Byte(0xAF, SPI_CMD); // 开启显示
	
}




/************************************************
*	【屏幕操作】
*	[打印缓存区信息进入屏幕]
*	[清空屏幕显示]
*************************************************/

//清空显示
void OLED_Clear(void){  
	uint8_t i,n;
    for(i=0;i<8;i++)
    {
        OLED_WR_Byte(0xB0+i, SPI_CMD);
        OLED_WR_Byte(0x00, SPI_CMD);
        OLED_WR_Byte(0x10, SPI_CMD);
        for(n=0;n<128;n++)
            OLED_WR_Byte(0xff, SPI_DATA);
    }
}

//将缓存区中的数据输出到屏幕上；
void OLED_BufferOutput(const unsigned char* buffer, unsigned char arrRow, unsigned char arrCol){
	uint8_t i,n;
    for(i=0;i<arrRow;i++)
    {
        OLED_WR_Byte(0xB0+i, SPI_CMD);
        OLED_WR_Byte(0x00, SPI_CMD);
        OLED_WR_Byte(0x10, SPI_CMD);
        for(n=0;n<arrCol;n++)
            OLED_WR_Byte(buffer[i * arrCol + n], SPI_DATA);
    }
}




