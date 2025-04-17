#include "key.h"
void Key_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOA和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 配置GPIOB端口的PB12, PB13, PB14, PB15为上拉输入（低电平触发）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // IPU：Input Pull-Up模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置GPIOA端口的PA8, PA9, PA10为上拉输入（低电平触发）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
}

KeyValue_t Get_Key(void)
{
    KeyValue_t key = K_Idle;  // 默认返回K_Idle
    
    // 检测GPIOB的PB12，按下返回K_Idle
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == RESET)
    {
        key = K_Idle;
        return key;
    }
    // 检测GPIOB的PB13，按下返回K_Idle
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
    {
        key = K_Idle;
        return key;
	}
    // 检测GPIOB的PB14，按下返回K_Mid
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == RESET)
    {
        key = K_Mid;
        return key;
    }
    // 检测GPIOB的PB15，按下返回K_Right
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET)
    {
        key = K_Right;
        return key;
    }
    // 检测GPIOA的PA8，按下返回K_Left
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET)
    {
        key = K_Left;
        return key;
    }
    // 检测GPIOA的PA9，按下返回K_Down
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == RESET)
    {
        key = K_Down;
        return key;
    }
    // 检测GPIOA的PA10，按下返回K_Up
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == RESET)
    {
        key = K_Up;
        return key;
    }
    
 
        // 多键同时按下或者无按键按下均返回K_Idle
    return K_Idle;
    
}

