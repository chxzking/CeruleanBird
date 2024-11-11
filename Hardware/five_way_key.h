#ifndef _FIVE_WAY_KEY
#define _FIVE_WAY_KEY

typedef enum KEY_DIRECTION KEY_DIRECTION ;

extern KEY_DIRECTION KeyNum;

enum KEY_DIRECTION{
	KEY_EMPTY = 0,	//按键空置
	KEY_RESET,
	KEY_SET,
	KEY_MID,	//中间键
	KEY_RIGHT,	//右
	KEY_LEFT,	//左
	KEY_DOWN,	//下
	KEY_UP		//上
};

void FiveWayKey_Init(void);
//uint8_t FiveKey_Num(void);
KEY_DIRECTION FiveKey_Num(void);
//char* FiveKey_char(void);



#endif
