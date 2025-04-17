#ifndef __PISTOL_H__
#define __PISTOL_H__

/*手枪库*/
#include "Common.h"

typedef struct bulletHead_t bulletHead_t;
typedef struct bulletStatus_t bulletStatus_t;
typedef struct bullet_t bullet_t;
typedef struct bulletPrivate_t bulletPrivate_t;

struct bulletHead_t{
	//新的想法
	int MaxCount;//允许创建的子弹的最大数量
	
	//待命
	int usableCount;//可使用的子弹精灵数量
	bullet_t* usableLink;//可使用的链
	
	//正在运行
	int runningCount;//正在运行的子弹数量
	bullet_t* runningLink;//正在运行的链
	
};


struct bullet_t{
	vector pos;//精灵位置
	Sprites_t* sprite;//子弹精灵本体
	struct bullet_t* next;//下一个节点
	struct bullet_t* previous;//上一个节点
};

//子弹的私有数据
struct bulletPrivate_t{
	struct bullet_t* bullet;//子弹描述
	Camera_Struct* camera;//摄像机
	int isShow;//是否显示
};



int pistol_Init(int count,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double Speed);

int pistol_Fire(Camera_Struct* camera);
#endif 

