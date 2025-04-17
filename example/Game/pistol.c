#include "pistol.h"


bulletHead_t* bulletArr = NULL;

int indexNormalBullet;//普通子弹
int indexClearBullet;//透明子弹（用于短距离不显示子弹所用）



void bullet_PrivateFreeHandle(void* pdata){
	c_free(pdata);
}


const unsigned char bullet_8x8[]={
	0x00,0x00,0x1C,0x3E,0x3E,0x1C,0x00,0x00
};
const unsigned char bulletBlank_8x8[]={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

/**************************************
	【子弹动画与逻辑配置区】
***************************************/

//创建子弹动画
int bullet_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//创建动画管理器
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
	//创建动画链
	indexNormalBullet = SpritesAnim_CreateNewChain(AnimManagerIndex,8,8,BIT_SPRITES,0.5);
	indexClearBullet = SpritesAnim_CreateNewChain(AnimManagerIndex,8,8,BIT_SPRITES,0.5);
	if(indexNormalBullet < 0 || indexClearBullet < 0){
		return -1;
	}
	int temp = SpritesAnim_InsertNewFrm(AnimManagerIndex,indexNormalBullet,0,bullet_8x8,bullet_8x8);//普通
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,indexClearBullet,0,bulletBlank_8x8,bulletBlank_8x8);//透明子弹
	if(temp != 0){
		return -1;
	}
	return 0;
}


SpriteBTStatus_t SBT_ExecuteHandle_bullet(Sprites_t* Sprites,void* privateData){
	bulletPrivate_t *data = (bulletPrivate_t *)privateData;//获取私有数据
	//前进一个帧距离
	SpritesMov_Forward(Sprites);
	data->bullet->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));//更新坐标
	
	//获取子弹与玩家的距离
	vector pos = Camera_GetPos(data->camera);
	double x = data->bullet->pos.x - pos.x;
	double y = data->bullet->pos.y - pos.y;
	double dis = sqrt(x*x + y*y);
	//是否显示子弹
	if(data->isShow == 0){
		//不显示判断子弹与玩家的距离

		if(dis > 0.5){
			//子弹大于0.5m后就会变成显示状态
			
			//切换动画链
			SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),indexNormalBullet);
			SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);
			//设置为显示状态
			data->isShow = 1;
		}
	}
	
	//判断子弹是否击中敌人
	Sprites_t* enemy = GetEnemy(data->bullet->pos);
	if(enemy != NULL){
		
		//子弹自身进行状态改变
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));//休眠子弹
		

		bullet_t* tempNode = data->bullet;

		// 从运行态链中脱离（改进版本）：
		if (tempNode->previous != NULL) {//非头节点
			tempNode->previous->next = tempNode->next;
		} else {
			// 如果当前节点是头节点，更新头指针为下一个节点
			bulletArr->runningLink = tempNode->next;
		}
		if (tempNode->next != NULL) {
			tempNode->next->previous = tempNode->previous;
		}
		bulletArr->runningCount--;

		//添加到空闲链表中
		tempNode->next = bulletArr->usableLink;
		tempNode->previous = NULL; // 新增：新头节点的 previous 必须为 NULL
		if (bulletArr->usableLink != NULL) {
			bulletArr->usableLink->previous = tempNode; // 旧头节点的 previous 指向新节点
		}
		bulletArr->usableLink = tempNode; // 关键修复：更新链表头指针
		bulletArr->usableCount++;
		
		data->isShow = 0;//设置为透明状态
		return SBT_SUCCESS;
	}
	
	//判断子弹是否撞到墙，或者子弹是否超过距离为6的射程
	if(Sprites_collisionWallValue(Sprites) != 0 || dis >= 6.0){
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));
		
		bullet_t* tempNode = data->bullet;
		// 从运行态链中脱离（改进版本）：
		if (tempNode->previous != NULL) {
			tempNode->previous->next = tempNode->next;
		} else {
			// 如果当前节点是头节点，更新头指针为下一个节点
			bulletArr->runningLink = tempNode->next;
		}
		if (tempNode->next != NULL) {
			tempNode->next->previous = tempNode->previous;
		}
		bulletArr->runningCount--;
		//添加到空闲链表中
		tempNode->next = bulletArr->usableLink;
		tempNode->previous = NULL; // 新增：新头节点的 previous 必须为 NULL
		if (bulletArr->usableLink != NULL) {
			bulletArr->usableLink->previous = tempNode; // 正确：旧头节点的 previous 指向新节点
		}
		bulletArr->usableLink = tempNode; // 关键修复：更新链表头指针
		bulletArr->usableCount++;
				
		data->isShow = 0;//设置为透明状态
		return SBT_SUCCESS;
	}
	//更新
	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}

//创建子弹逻辑树
int bullet_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	//行为树创建
	/*创建动作1:
		子弹对精灵/墙壁碰撞动作
	*/
	SpriteBTNode_t* act = SpritesBT_Action(SBT_ExecuteHandle_bullet);
	SpritesBT_MountToRoot(BTTreeIndex, act);
	return 0;
}

/**************************************
	【子弹API】
***************************************/
//全局参数
int PrivAnimManagerIndex;
double PrivSpeed;
int PrivBTTreeIndex;
/**
  * @brief  手枪初始化（预创建子弹精灵数量）
  *
  * @param  count数量(小于0为创建无限子弹精灵待命使用)
  *
  * @retval 操作成功返回0，操作失败返回-1
  */
int pistol_Init(int count,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double Speed){
	if(count == 0 || bulletArr != NULL)	return -1;

	//创建预分配数组
	bulletArr = (bulletHead_t*)c_malloc(sizeof(bulletHead_t));
	if(bulletArr == NULL){
		return -1;
	}
	bulletArr->MaxCount = count;
	bulletArr->runningCount = 0;
	bulletArr->usableCount = 0;
	bulletArr->runningLink = NULL;
	bulletArr->usableLink = NULL;
	
	int ArrCount;//预创建的数组数量
	if(count > 0){//大于0意味着创建了最大上限
			ArrCount = count;
	}else{//如果为负数，那么先创建7个待命，后续再添加
		ArrCount = 7;
	}
	
	bullet_t* tempArr = (bullet_t*)c_malloc(sizeof(bullet_t)*ArrCount);
	if(tempArr == NULL){
		c_free(bulletArr);
		bulletArr = NULL;
		return -1;//创建失败
	}
	
	//准备初始化精灵
	int temp = 0;
	//创建子弹动画管理
	temp |= bullet_Anim_Create(AnimManagerIndex);
	//创建子弹逻辑
	temp |= bullet_Logic_Create(BTTreeIndex);
	if(temp != 0){
		c_free(tempArr);
		c_free(bulletArr);
		bulletArr = NULL;
		return -1;
	}
	/*
	设置一些全局参数
	*/
	PrivAnimManagerIndex = AnimManagerIndex;
	PrivSpeed = Speed;
	PrivBTTreeIndex = BTTreeIndex;
	//对当前的数组进行基本的初始化配置
	bullet_t* probe = NULL;
	for(int i = 0;i<ArrCount;i++){
		tempArr[i].sprite = Sprites_ObjectCreate(AnimManagerIndex,0,0,indexNormalBullet,Speed,BTTreeIndex);
		tempArr[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(tempArr[i].sprite));//精灵创建失败，程序会被此处卡住
		
		//创建私有数据
		bulletPrivate_t *data = c_malloc(sizeof(bulletPrivate_t));
		data->bullet = &tempArr[i];
		data->camera = camera;
		data->isShow = 0;//默认不显示
		SpritesPriv_Add(tempArr[i].sprite,data,bullet_PrivateFreeHandle);
		
		//设置初始动画精灵状态
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(tempArr[i].sprite),indexClearBullet);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(tempArr[i].sprite),Anim_Loop);
		
		//可使用精灵数自增
		bulletArr->usableCount++;
		
		tempArr[i].next = NULL;
		
		//插入链表
		if(i != 0){//如果当前不为链表首节点
			probe->next = &tempArr[i];
			tempArr[i].previous = probe;
		}else {
			// 首个节点需要设置链表头
			bulletArr->usableLink = &tempArr[i]; 
			tempArr[i].previous = NULL;
		}
		probe = &tempArr[i];
		
		//休眠
		Sprites_Sleep(TO_SPRITES_BASE(tempArr[i].sprite));
	}		
	
	return 0;
}


/**
  * @brief  手枪开火（没有设置顶层动画只关注了子弹）
  *
  * @param  camera	摄像机
  *
  * @retval 
  *		开火成功返回0，
  *		开火失败返回-1（初始化存在问题、手枪子弹可使用子弹为0）
  */
int pistol_Fire(Camera_Struct* camera){
	if(camera == NULL || bulletArr == NULL){
		return -1;
	}
	
	bullet_t* target = NULL; 
	//检查是否有可用子弹精灵
	if(bulletArr->usableCount != 0){//如果有可用子弹
		target = bulletArr->usableLink;
		bulletArr->usableLink = bulletArr->usableLink->next;
		if(bulletArr->usableLink != NULL){
			bulletArr->usableLink->previous = NULL;
		}
		bulletArr->usableCount--;
		
	}
	else{//如果不存在可用子弹
		//检查是否启用了无限子弹策略
		if(bulletArr->MaxCount >= 0){//没有该策略
			return -1;//回天乏术
		}
		target = (bullet_t*)c_malloc(sizeof(bullet_t));
		target->next = NULL;       // 新增
		target->previous = NULL;   // 新增
		if(target == NULL){
			return -1;
		}
		//创建精灵
		target->sprite = Sprites_ObjectCreate(PrivAnimManagerIndex,0,0,indexNormalBullet,PrivSpeed,PrivBTTreeIndex);
		if(target->sprite == NULL){
			c_free(target);
			return -1;//创建失败
		}
		target->pos = SpritesData_GetPos(TO_SPRITES_BASE(target->sprite));
		
		//创建私有数据
		bulletPrivate_t *data = c_malloc(sizeof(bulletPrivate_t));
		data->bullet = target;
		data->camera = camera;
		SpritesPriv_Add(target->sprite,data,bullet_PrivateFreeHandle);
		
	}
	
	
	// 修改后的运行态链表插入
	target->previous = NULL;
	target->next = bulletArr->runningLink;
	if (bulletArr->runningLink != NULL) {
		bulletArr->runningLink->previous = target;
	}
	bulletArr->runningLink = target; // 新增：更新链表头指针
	bulletArr->runningCount++;
	
	
	//设置子弹属性
	Sprites_Awaken(TO_SPRITES_BASE(target->sprite));
	//位置与方向
	SpritesMov_SetPosToCamera(target->sprite,camera);
	SpritesMov_SetDirWithCamera(target->sprite,camera);
	//动画
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(target->sprite),indexClearBullet);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(target->sprite),Anim_Loop);
	bulletPrivate_t *data = (bulletPrivate_t*)SpritesPriv_Get(target->sprite);
	data->isShow = 0;//设置为透明态
	
	return 0;//没有可用子弹
}


