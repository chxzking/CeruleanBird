#include "key.h"
void Key_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;

    // ʹ��GPIOA��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // ����GPIOB�˿ڵ�PB12, PB13, PB14, PB15Ϊ�������루�͵�ƽ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // IPU��Input Pull-Upģʽ
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // ����GPIOA�˿ڵ�PA8, PA9, PA10Ϊ�������루�͵�ƽ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
}

KeyValue_t Get_Key(void)
{
    KeyValue_t key = K_Idle;  // Ĭ�Ϸ���K_Idle
    
    // ���GPIOB��PB12�����·���K_Idle
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == RESET)
    {
        key = K_Idle;
        return key;
    }
    // ���GPIOB��PB13�����·���K_Idle
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
    {
        key = K_Idle;
        return key;
	}
    // ���GPIOB��PB14�����·���K_Mid
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == RESET)
    {
        key = K_Mid;
        return key;
    }
    // ���GPIOB��PB15�����·���K_Right
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == RESET)
    {
        key = K_Right;
        return key;
    }
    // ���GPIOA��PA8�����·���K_Left
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == RESET)
    {
        key = K_Left;
        return key;
    }
    // ���GPIOA��PA9�����·���K_Down
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == RESET)
    {
        key = K_Down;
        return key;
    }
    // ���GPIOA��PA10�����·���K_Up
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == RESET)
    {
        key = K_Up;
        return key;
    }
    
 
        // ���ͬʱ���»����ް������¾�����K_Idle
    return K_Idle;
    
}

