#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "Common.h"

typedef struct EnemyHead_t EnemyHead_t;
typedef struct EnemyStatus_t EnemyStatus_t;
typedef struct Enemy_t Enemy_t;


struct EnemyHead_t{
	//近程敌人
	int CloseRangeAttack_totality;//创建的近程敌人的总数
	int CloseRangeAttack_remaining;//当前可用的近程敌人的总数
	Enemy_t* CloseRangeAttack_head;//近程攻击敌人
	
	
	//远程敌人
	int LongRangeAttack_totality;//创建的近程敌人的总数
	int LongRangeAttack_remaining;//当前可用的近程敌人的总数
	Enemy_t* LongRangeAttack_head;//远程攻击敌人
	
	
	//篮球飞行物
	int basketball_totality;//创建篮球飞行物的总数
	int basketball_remaining;//当前可用的篮球飞行物的总数
	Enemy_t* basketball_head;//篮球飞行物
};

struct EnemyStatus_t{
	unsigned int isSleep:1;//是否正在休眠 
	unsigned int usable :1;//是否可使用
};

struct Enemy_t{
	EnemyStatus_t EnemyStatus;//精灵状态 
	vector pos;//精灵位置
	Sprites_t* sprite;//敌人精灵本体
};



//私有数据

//篮球私有数据
typedef struct BasketballPrivate_t{
	struct Enemy_t* Enemy;//子弹描述
	Camera_Struct* camera;//摄像机
}BasketballPrivate_t;

//远程敌人私有数据
typedef struct LongPrivate_t{
	struct Enemy_t* Enemy;//敌人描述
	Camera_Struct* camera;//摄像机
	int HP;//声明值
	
	struct{
		int isHit;//是否受击 0为没有受击，1为受击
		int isFirstHit;//是否首次受击，1为首次，0为非首次
	}hit;//受击组
	
	struct{
		double AttackInterval;//攻击间隔
		int isFirstAttack;//是否是首次攻击，1为首次，0为首次
		
		double MovementWithinAttackRangeInterval;//攻击范围内的移动时间间隔
	}attack;
	
	struct{
		
		double action;//保持一个行为的累积时间 
	}CumulativeTime;//各种累积时间属性
}LongPrivate_t;

//近程敌人私有数据
typedef struct ClosePrivate_t{
	struct Enemy_t* Enemy;//敌人描述
	Camera_Struct* camera;//摄像机
	int HP;
	
	struct{
		double AttackInterval;//攻击间隔
		
		double MovementWithinAttackRangeInterval;//攻击范围内的移动时间间隔
	}attack;
	
	struct{
		int isHit;//是否受击 0为没有受击，1为受击
		int isFirstHit;//是否首次受击，1为首次，0为非首次
	}hit;//受击组
		
	
}ClosePrivate_t;


int LongEnemy_Init(int enemyCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed);
int LongEnemy_Create(void);

int CloseEnemy_Init(int enemyCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed);
int CloseEnemy_Create(void);

int Basketball_Init(int basketballCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed);
int basketball_fire(Sprites_t* sprite,Camera_Struct* camera);

void Minimap(Camera_Struct* camera);
#endif
