#include "stm32f10x.h"// Device header
//#include "Delay.h"
//#include "string.h"
#include "stdlib.h"
#include "five_way_key.h"
//#include "OLED.h"

KEY_DIRECTION KeyNum = KEY_EMPTY;

void FiveWayKey_Init(void){
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|
								  GPIO_Pin_2|
								  GPIO_Pin_3|
								  GPIO_Pin_4|
								  GPIO_Pin_5|
								  GPIO_Pin_6|
								  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
}


/*
1:¸´Î»¼ü
2£ºset¼ü
3£ºÖÐ¼ä¼ü
4£ºÓÒ¼ü
5£º×ó¼ü
6£ºÏÂ¼ü
7£ºÉÏ¼ü
*/

//uint8_t FiveKey_Num(void){
//	uint8_t key_flag = 1;
//	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0){
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0);
//		KeyNum = KEY_RESET;//reset
//		return key_flag;
//	}
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0){
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0);
//		KeyNum = KEY_SET;//set
//		return key_flag;
//	}
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0){
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0);
//		KeyNum = KEY_MID;//ÖÐ¼ä¼ü
//		return key_flag;
//	}		
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0){
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0);
//		KeyNum = KEY_RIGHT;//ÓÒ¼ü
//		return key_flag;
//	}	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0){
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0);
//		KeyNum = KEY_LEFT;//×ó¼ü
//		return key_flag;
//	}	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0){
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0);
//		KeyNum = KEY_DOWN;//ÏÂ¼ü
//		return key_flag;
//	}	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0){
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0);
//		KeyNum = KEY_UP;//ÉÏ¼ü
//		return key_flag;
//	}
//	key_flag = 0;
//	return key_flag;
//}

KEY_DIRECTION FiveKey_Num(void){
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0){
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0);
		KeyNum = KEY_RESET;//reset
		return KeyNum;
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0){
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0);
		KeyNum = KEY_SET;//set
		return KeyNum;
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0){
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0);
		KeyNum = KEY_MID;//ÖÐ¼ä¼ü
		return KeyNum;
	}		
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0){
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0);
		KeyNum = KEY_RIGHT;//ÓÒ¼ü
		return KeyNum;
	}	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0){
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0);
		KeyNum = KEY_LEFT;//×ó¼ü
		return KeyNum;
	}	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0){
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0);
		KeyNum = KEY_DOWN;//ÏÂ¼ü
		return KeyNum;
	}	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0){
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0);
		KeyNum = KEY_UP;//ÉÏ¼ü
		return KeyNum;
	}
	return KEY_EMPTY;
}

//char* FiveKey_char(void){
//	char* Keychar = (char*)malloc(6);
//	
//	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0){
//		
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0);
//	
//		strcpy(Keychar,"reset");
//		//reset
//	}
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0){
//	
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0);
//	
//		strcpy(Keychar,"set");//set
//	}
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0){
//	
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)==0);
//	
//		strcpy(Keychar,"mid");//ÖÐ¼ä¼ü
//	}		
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0){
//		
//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)==0);
//	
//		strcpy(Keychar,"right");//ÓÒ¼ü
//	}	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0){

//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0);

//		strcpy(Keychar,"left");//×ó¼ü
//	}	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0){

//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)==0);

//		strcpy(Keychar,"down");//ÏÂ¼ü
//	}	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0){

//		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)==0);

//		strcpy(Keychar,"up");//ÉÏ¼ü
//	}
//	

//	
//	return Keychar;
//}
