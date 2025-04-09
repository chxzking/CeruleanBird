#ifndef __M_STM32F103C8T6_H__
#define __M_STM32F103C8T6_H__

	//初始化屏幕
void OLED_Init(void);
//清空显示
void OLED_Clear(void);
//将缓存区中的数据输出到屏幕上；
void OLED_BufferOutput(const unsigned char* buffer, unsigned char arrRow, unsigned char arrCol);
#endif
