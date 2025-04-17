#include "Enemy.h"
#include "Sprites_internal.h"
#include "Camera_internal.h"
EnemyHead_t* EnemyHead = NULL;
#define ENEMY_HP_LONG 5
#define ENEMY_HP_CLOSE 5
//共用函数

void PrifreeHandle(void* pdata){
	c_free(pdata);
}

#define M_PI 3.1415

// 辅助函数：计算两个角度的最小差值（结果范围 [0, PI]）
static double angle_diff(double a, double b) {
    double diff = fmod(b - a, 2 * M_PI);
    if(diff < -M_PI)
         diff += 2 * M_PI;
    if(diff > M_PI)
         diff -= 2 * M_PI;
    return fabs(diff);
}

//小地图打印
void Minimap(Camera_Struct* camera){
	
	//地图打印
	for(int i = 0;i<24;i++){
		for(int j = 0;j<24;j++){
			int temp = *(worldMap + i*24 + j);
			if(temp){
				MistinkPixel_Operate(displayer,i,j ,PixelOpen);
			}else{
				MistinkPixel_Operate(displayer,i,j ,PixelClose);
			}
		}
	}
	
	//玩家打印
	vector p_pos = Camera_GetPos(camera);
	MistinkPixel_Operate(displayer,(int)p_pos.x,(int)p_pos.y ,PixelOpen);
	
	//远程精灵打印
	for(int i = 0;i<EnemyHead->LongRangeAttack_totality;i++){
		if(EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable == 0){
			vector s_pos = EnemyHead->LongRangeAttack_head[i].pos;
			MistinkPixel_Operate(displayer,(int)s_pos.x,(int)s_pos.y ,PixelOpen);
		}
	}
	
	//近程精灵打印
	for(int i = 0;i<EnemyHead->CloseRangeAttack_totality;i++){
		if(EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable == 0){
			vector s_pos = EnemyHead->CloseRangeAttack_head[i].pos;
			MistinkPixel_Operate(displayer,(int)s_pos.x,(int)s_pos.y ,PixelOpen);
		}
	}
}
	

vector RandPos(Camera_Struct* camera){
    // 获取当前地图信息
    int index = WorldMap_GetCurrentMapPos();
    int MapRow = WorldMap_GetROW(index);
    int MapCol = WorldMap_GetColumn(index);
    unsigned char* map = WorldMap_GetArray(index);
	
	vector result;
    int count = 0; // 用于统计遍历到的数值为0的坐标数量

    for (int i = 0; i < MapRow; i++)
    {
        for (int j = 0; j < MapCol; j++)
        {
            // 根据数组索引方式访问当前(i,j)位置
            if (map[i * MapCol + j] == 0)
            {
                count++;  // 找到一个合适的点
                // 以1/count的概率更新候选结果
                if (rand() % count == 0)
                {
                    result.x = i;
                    result.y = j;
                }
            }
        }
    }

    // 如果遍历后未找到任何数值为0的点，则返回一个错误标识，如坐标(-1, -1)
    if (count == 0)
    {
        result.x = -1;
        result.y = -1;
    }

    return result;
}

/**************************************
	【近程攻击配置】
***************************************/

int closeMoveIndex;
int closeDamageIndex;

//创建近程攻击动画
int close_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//创建动画管理器
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
	//创建动画链
	
	//移动动画
	closeMoveIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,45,45,BIT_SPRITES,2.5);
	//受击动画
	closeDamageIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,45,45,BIT_SPRITES,0.5);
	if(closeMoveIndex < 0  || closeDamageIndex < 0){
		return -1;
	}
	int temp = 0;
	//移动动画帧
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,0,IKUN_TieShanKao_45x45_1,IKUN_TieShanKao_45x45_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,1,IKUN_TieShanKao_45x45_2,IKUN_TieShanKao_45x45_2_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,2,IKUN_TieShanKao_45x45_3,IKUN_TieShanKao_45x45_3_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,3,IKUN_TieShanKao_45x45_4,IKUN_TieShanKao_45x45_4_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,4,IKUN_TieShanKao_45x45_5,IKUN_TieShanKao_45x45_5_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,5,IKUN_TieShanKao_45x45_6,IKUN_TieShanKao_45x45_6_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,6,IKUN_TieShanKao_45x45_7,IKUN_TieShanKao_45x45_7_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,7,IKUN_TieShanKao_45x45_8,IKUN_TieShanKao_45x45_8_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,8,IKUN_TieShanKao_45x45_9,IKUN_TieShanKao_45x45_9_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,9,IKUN_TieShanKao_45x45_10,IKUN_TieShanKao_45x45_10_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,10,IKUN_TieShanKao_45x45_11,IKUN_TieShanKao_45x45_11_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeMoveIndex,11,IKUN_TieShanKao_45x45_12,IKUN_TieShanKao_45x45_12_shadow);

	
	//受击动画帧
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,0,IKUN_TieShanKao_45x45_1,IKUN_TieShanKao_45x45_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,1,IKUN_TieShanKao_45x45_1_shadow,IKUN_TieShanKao_45x45_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,2,IKUN_TieShanKao_45x45_1,IKUN_TieShanKao_45x45_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,3,IKUN_TieShanKao_45x45_1_shadow,IKUN_TieShanKao_45x45_1_shadow);
	
	if(temp != 0){
		return -1;
	}
	return 0;
}
/*中断表*/
//中断表映射逻辑
SITindex_t SIT_MapIndexHandle_Close(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	//检查是否需要进入死亡中断
	if(data->HP <= 0){
		return 1;
	}
	
	//检查是否进入受击中断
	if(data->hit.isHit == 1){
		return 2;
	}
	
	//没有触发中断
	return -1;
}
//近程攻击——死亡中断逻辑
SpriteBTStatus_t SBT_ExecuteHandle_Close_Die(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	//重置精灵状态
	data->HP = ENEMY_HP_CLOSE;
	data->hit.isHit = 0;
	
	//切换为移动动画
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),closeMoveIndex);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//循环逻辑
	
	//休眠精灵
	data->Enemy->EnemyStatus.isSleep = 1;
	data->Enemy->EnemyStatus.usable = 1;
	Sprites_Sleep(TO_SPRITES_BASE(Sprites));
	EnemyHead->CloseRangeAttack_remaining++;
	enemyCore++;//得分加1
	LongC_survivalCount--;
	return SBT_SUCCESS;//执行完成
	
}
//近程攻击——受击中断逻辑
SpriteBTStatus_t SBT_ExecuteHandle_Close_Hit(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	
	if(data->hit.isFirstHit == 1){//是否首次受击
		//设置为受击动画
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),closeDamageIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Once);//单次逻辑
		data->hit.isFirstHit = 0;
	}
	
	/*受击逻辑*/
	
	//检查是否受击动画结束
	if(SpritesAnim_IsEndFrm(TO_SPRITES_BASE(Sprites)) == 0){//动画没有结束
		SpritesAnim_Update(TO_SPRITES_BASE(Sprites));//更新动画
		return SBT_RUNNING;
	}else{
		data->HP--;
		//切换成为移动逻辑
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),closeMoveIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//循环逻辑
		data->hit.isHit = 0;//设置为没有受击状态
		data->hit.isFirstHit = 1;//恢复首次受击标志
		
		return SBT_SUCCESS;
	}
}

/*行为树*/
// [动作] 向玩家移动（完备帧）
SpriteBTStatus_t SBT_ExecuteHandle_Close_Move(Sprites_t* Sprites, void* privateData){	
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
    // 获取敌人及玩家（摄像机）位置
    vector Sprite_Pos = data->Enemy->pos;
    vector Camera_Pos = Camera_GetPos(data->camera);

    // 获取当前地图信息
    int index = WorldMap_GetCurrentMapPos();
    int MapRow = WorldMap_GetROW(index);
    int MapCol = WorldMap_GetColumn(index);
    unsigned char* map = WorldMap_GetArray(index);

    // 将精灵和玩家的位置转换为网格坐标（按行、列）
    int enemyRow = (int)(Sprite_Pos.x);
    int enemyCol = (int)(Sprite_Pos.y);
    int playerRow = (int)(Camera_Pos.x);
    int playerCol = (int)(Camera_Pos.y);

    // 计算从敌人指向玩家的基本方向（注意：x 为行，y 为列，转换为传统坐标时将列视为水平、行视为垂直）
    double dx = (double)(playerCol - enemyCol); // 水平差
    double dy = (double)(playerRow - enemyRow); // 垂直差
    double baseAngle = atan2(dy, dx);

    // 加入随机扰动，使期望方向偏离玩家正中心，扰动范围为 -45° 到 +45°（即 -PI/4 到 +PI/4）
    double randPert = ((double)rand() / (double)RAND_MAX) * (M_PI / 2.0) - (M_PI / 4.0);
    double desiredAngle = baseAngle + randPert;

    // 定义8个候选方向（每次步长均为1格）
    vector candidateMoves[8] = {
        {-1.0f, -1.0f},  // 上左
        {-1.0f,  0.0f},  // 上
        {-1.0f,  1.0f},  // 上右
        { 0.0f, -1.0f},  // 左
        { 0.0f,  1.0f},  // 右
        { 1.0f, -1.0f},  // 下左
        { 1.0f,  0.0f},  // 下
        { 1.0f,  1.0f}   // 下右
    };

    double bestCost = 1e9;
    int bestCandidate = -1;
    // 当前与玩家的曼哈顿距离
    int currentManhattan = abs(playerRow - enemyRow) + abs(playerCol - enemyCol);

    // 遍历所有候选方向
    for (int i = 0; i < 8; i++) {
        int newRow = enemyRow + (int)(candidateMoves[i].x);
        int newCol = enemyCol + (int)(candidateMoves[i].y);
        // 检查地图边界及通行性
        if (newRow < 0 || newRow >= MapRow || newCol < 0 || newCol >= MapCol)
            continue;
        if (map[newRow * MapCol + newCol] != MOVABLE_AREA_VALUE)
            continue;
        // 计算候选位置与玩家之间的曼哈顿距离
        int candidateManhattan = abs(playerRow - newRow) + abs(playerCol - newCol);
        double distanceCost = 0.0;
        if (candidateManhattan < currentManhattan) {
            distanceCost = 0.0;      // 实际拉近了距离，无额外惩罚
        } else if (candidateManhattan == currentManhattan) {
            distanceCost = 1.0;      // 仅维持距离，加一点惩罚
        } else {
            distanceCost = 100.0;    // 增加距离则极大惩罚
        }
        // 计算候选移动方向的角度（注意：对于候选向量，其水平分量为 candidateMoves[i].y，垂直分量为 candidateMoves[i].x）
        double candAngle = atan2((double)candidateMoves[i].x, (double)candidateMoves[i].y);
        double angDiff = angle_diff(desiredAngle, candAngle);
        // 最终代价 = 角度差 + 距离惩罚，再加上一个微小随机噪声以打破完全一致性
        double cost = angDiff + distanceCost + (((double)rand() / (double)RAND_MAX) * 0.1);
        if (cost < bestCost) {
            bestCost = cost;
            bestCandidate = i;
        }
    }

    vector NewDir;
    if (bestCandidate != -1) {
        NewDir = candidateMoves[bestCandidate];
    } else {
        // 若周围均不可移动，则选用一个默认方向，比如向右移动
        NewDir.x = 0.0f;
        NewDir.y = 1.0f;
    }

    // 设置精灵的新方向，执行移动和动画更新
    SpritesMov_SetDir(Sprites, NewDir);
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
    return SBT_SUCCESS;
}



//[动作] 发起攻击（低优先级为3的可中断非完备帧）	
SpriteBTStatus_t SBT_ExecuteHandle_Close_Attack(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	
	vector e_pos = data->Enemy->pos;//获取精灵位置
	vector p_pos = Camera_GetPos(data->camera);//获取玩家位置
	
	double x = e_pos.x - p_pos.x;
	double y = e_pos.y - p_pos.y;
	
	double dis = sqrt(x*x+y*y);//获取敌人与玩家位置
	if(dis >= 0.15){
		//如果距离大于等于0.15米，则认为没有在伤害范围内
		return SBT_FAILURE;
	}
	
	int * Health =  (int*)CameraPriv_Get(data->camera);
	*Health -= 1;//玩家减一滴血
	
	
	return SBT_SUCCESS;
}


//[动作] 攻击范围内的移动行为
SpriteBTStatus_t SBT_ExecuteHandle_AttackRangeMoveAct_close(Sprites_t* Sprites, void* privateData) {
    ClosePrivate_t* data = (ClosePrivate_t*)privateData;
    
    // 每隔 1 秒切换一次移动方向
    if (data->attack.MovementWithinAttackRangeInterval > 1.0f) {
        data->attack.MovementWithinAttackRangeInterval = 0; // 刷新累计时间
        vector NewDir; // 新方向
        
        // 获取敌人及玩家（摄像机）的位置
        vector Sprite_Pos = data->Enemy->pos;
        vector Camera_Pos = Camera_GetPos(data->camera);
        
        // 获取当前地图信息，如需进一步做障碍检测，可利用这些信息
        int index = WorldMap_GetCurrentMapPos();
        int MapRow = WorldMap_GetROW(index);
        int MapCol = WorldMap_GetColumn(index);
        unsigned char* map = WorldMap_GetArray(index);
        
        /* 改变方向代码：
         * 1. 计算向玩家移动的最短路径
         * 2. 智能避开墙壁干扰
         */
        {
            // 计算敌人到玩家的差分向量
            vector diff;
            diff.x = Camera_Pos.x - Sprite_Pos.x;
            diff.y = Camera_Pos.y - Sprite_Pos.y;
            float distance = sqrt(diff.x * diff.x + diff.y * diff.y);
            
            if (distance > 0.001f) {  // 避免除以零
                // 归一化差分向量：得到期望的移动方向
                vector desiredDir;
                desiredDir.x = diff.x / distance;
                desiredDir.y = diff.y / distance;
                
                // 设定一步移动的距离（可根据实际情况调整）
                float stepSize = 1.0f;
                
                // 检查沿直线方向移动一步是否会碰到障碍
                vector candidatePos;
                candidatePos.x = Sprite_Pos.x + desiredDir.x * stepSize;
                candidatePos.y = Sprite_Pos.y + desiredDir.y * stepSize;
                
                int candidateTileCol = (int)(candidatePos.x);
                int candidateTileRow = (int)(candidatePos.y);
                int isBlocked = 0;
                if (candidateTileCol < 0 || candidateTileCol >= MapCol ||
                    candidateTileRow < 0 || candidateTileRow >= MapRow) {
                    isBlocked = 1;
                } else if (map[candidateTileRow * MapCol + candidateTileCol] == 1) {  
                    // 假设地图中 1 表示墙壁，阻挡移动
                    isBlocked = 1;
                }
                
                if (isBlocked) {
                    // 如果直线方向被阻挡，尝试以左右 45° 角偏转来避开障碍
                    #define ROTATE_VECTOR(v, angle) ( (vector){ \
                        (v).x * cos(angle) - (v).y * sin(angle), \
                        (v).x * sin(angle) + (v).y * cos(angle) } )
                    
                    float angleOffset = 45.0f * 3.14159265f / 180.0f;  // 45°转换为弧度
                    
                    // 尝试左转 45°
                    vector leftDir = ROTATE_VECTOR(desiredDir, angleOffset);
                    vector leftCandidatePos = { Sprite_Pos.x + leftDir.x * stepSize, 
                                                Sprite_Pos.y + leftDir.y * stepSize };
                    int leftTileCol = (int)(leftCandidatePos.x);
                    int leftTileRow = (int)(leftCandidatePos.y);
                    int isLeftBlocked = 0;
                    if (leftTileCol < 0 || leftTileCol >= MapCol ||
                        leftTileRow < 0 || leftTileRow >= MapRow) {
                        isLeftBlocked = 1;
                    } else if (map[leftTileRow * MapCol + leftTileCol] == 1) {
                        isLeftBlocked = 1;
                    }
                    
                    if (!isLeftBlocked) {
                        NewDir = leftDir;
                    } else {
                        // 若左转同样被挡，则尝试右转 45°
                        vector rightDir = ROTATE_VECTOR(desiredDir, -angleOffset);
                        NewDir = rightDir;
                    }
                    
                    #undef ROTATE_VECTOR
                } else {
                    // 没有障碍，直接设置期望方向
                    NewDir = desiredDir;
                }
            } else {
                // 当敌人与玩家非常接近时，可保持原有方向或设定为 0
                NewDir.x = 0.0f;
                NewDir.y = 0.0f;
            }
        }
        
        // 设置新方向
        SpritesMov_SetDir(Sprites, NewDir);
    }
    
    // 更新时间累积（单位：秒）
    data->attack.MovementWithinAttackRangeInterval += FrmRS_deltaTime_Get();
    
    // 更新位置与动画显示
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
    
    return SBT_SUCCESS;
}



/*条件集合*/
//[条件] 判断玩家是否在精灵攻击范围内
SpriteBTStatus_t SBT_ExecuteHandle_IsPlayerInAttackRange_close(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	
	vector e_pos = data->Enemy->pos;//获取精灵位置
	vector p_pos = Camera_GetPos(data->camera);//获取玩家位置
	
	double x = e_pos.x - p_pos.x;
	double y = e_pos.y - p_pos.y;
	
	double dis = sqrt(x*x+y*y);//获取敌人与玩家位置
	if(dis >= 1.5){
		//如果距离大于等于0.15米，则认为没有在攻击范围内
		return SBT_FAILURE;
	}
	return SBT_SUCCESS;
}

//[条件] 判断攻击冷却时间是否结束
SpriteBTStatus_t SBT_ExecuteHandle_IsAttackCooldownOver_close(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	if(data->attack.AttackInterval > 3.0){
		//如果攻击间隔时间超过3秒，意味着可以攻击，即攻击冷却结束
		data->attack.AttackInterval = 0;//重置冷却
		return SBT_SUCCESS;
	}
	//冷却时间没有结束，获取当前的帧时间，用于冷却时间叠加
	data->attack.AttackInterval += FrmRS_deltaTime_Get();
	return SBT_FAILURE;
}

//创建近程攻击逻辑
int close_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	
		/*
		行为：
			 [选择]
				 |--[顺序](逻辑：攻击)
				 |		|--[条件] 判断玩家是否在精灵攻击范围内
				 |		|--[条件] 判断攻击冷却时间是否结束
				 |		|--[动作] 发起攻击（低优先级为3的可中断非完备帧）	
				 |--[顺序](逻辑：攻击范围内的运动)
				 |		|--[条件] 判断玩家是否在精灵攻击范围内
				 |		|--[动作] 攻击范围内的移动行为（完备帧）		
				 |--[动作] 向玩家移动（完备帧）

		中断：
			死亡：优先级设为10 （不可取代）
			受击：优先级设为5  （可取代）
	*/
	int result = 0;
	/*普通行为树 ：没有设计完成*/
	
	
	/******************
		条件设计
	*******************/
	//条件：判断是否在攻击范围内
	SpriteBTNode_t* IsPlayerInAttackRange = SpritesBT_Condition(SBT_ExecuteHandle_IsPlayerInAttackRange_close);
	//条件：判断攻击冷却时间是否结束
	SpriteBTNode_t* IsAttackCooldownOver = SpritesBT_Condition(SBT_ExecuteHandle_IsAttackCooldownOver_close);
	
	
	/******************
		动作设计
	*******************/
	//动作：发起攻击（低优先级为3的可中断非完备帧，行为设计位置调整动作属性）	
	SpriteBTNode_t* AttackAct = SpritesBT_Action(SBT_ExecuteHandle_Close_Attack);
	//[动作] 向玩家移动
	SpriteBTNode_t* MoveAct = SpritesBT_Action(SBT_ExecuteHandle_Close_Move);
	//[动作] 攻击范围内的移动行为
	SpriteBTNode_t* AttackRangeMoveAct = SpritesBT_Action(SBT_ExecuteHandle_AttackRangeMoveAct_close);
	
	/******************
		行为设计
	*******************/
	//(逻辑：攻击)
	SpriteBTNode_t* AttackContainer = SpritesBT_Sequence();//容器创建
	result |= SpritesBT_InsertBehavior(AttackContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackContainer, IsAttackCooldownOver);
	//设置动作属性
	result |= SpritesBT_Action_ToPhased(AttackAct,SIT_AllowSubstitution,3);//设置攻击动作为：低优先级为3的可中断非完备帧
	result |= SpritesBT_InsertBehavior(AttackContainer, AttackAct);
	if(result != 0){
		return -1;//(逻辑：攻击)创建失败
	}
	
	//[顺序](逻辑：攻击范围内的运动)
	SpriteBTNode_t* AttackMoveContainer = SpritesBT_Sequence();//容器创建
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, AttackRangeMoveAct);
	if(result != 0){
		return -1;//(逻辑：攻击范围内的运动)创建失败
	}
	
	//总行为装载
	SpriteBTNode_t* SelectContainer = SpritesBT_SelectorCreate();
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackContainer);//攻击逻辑装载
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackMoveContainer); //攻击范围内的运动
	result |= SpritesBT_InsertBehavior(SelectContainer,MoveAct);//移动逻辑装载
	
	//挂载根节点
	result |= SpritesBT_MountToRoot(BTTreeIndex, SelectContainer);
	if(result != 0){
		return -1;//总行为创建失败
	}
	
	
	/*中断表*/
	//注册表
	result |= SpritesIT_Register(BTTreeIndex,SIT_MapIndexHandle_Close);
	//添加死亡行为节点
	result |= SpritesIT_Add(BTTreeIndex ,1,10,SIT_ProhibitSubstitution,SBT_ExecuteHandle_Close_Die);
	//添加受击行为节点
	result |= SpritesIT_Add(BTTreeIndex ,2,5,SIT_AllowSubstitution,SBT_ExecuteHandle_Close_Hit);
	if(result != 0){
		return -1;
	}
	
	
	
	return 0;
}



//近程敌人私有数据初始化
void closePri_Init(Camera_Struct* camera,Enemy_t* Enemy){
	ClosePrivate_t *data = c_malloc(sizeof(ClosePrivate_t));
	data->camera = camera;
	data->Enemy = Enemy;
	data->HP = ENEMY_HP_CLOSE;//初始血
	//受击组
	data->hit.isHit = 0;//默认没有受击
	data->hit.isFirstHit = 1;//默认首次受击
	//攻击组
	data->attack.AttackInterval = 0;//攻击间隔累积时间
	data->attack.MovementWithinAttackRangeInterval = 0;//攻击范围内的移动操作累积时间

	SpritesPriv_Add(Enemy->sprite,data,PrifreeHandle);
}
/**
  * @brief  近程敌人初始化
  *
  * @param	basketballCount	创建的篮球数量
  *
  * @retval 操作成功返回0，操作失败返回-1
  */
int CloseEnemy_Init(int enemyCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed){
	if(enemyCount <= 0 || camera == NULL){
		return -1;
	}
	if(EnemyHead == NULL){
		EnemyHead = (EnemyHead_t*)c_malloc(sizeof(EnemyHead_t));
		if(EnemyHead == NULL){
			return -1;
		}
	}
	EnemyHead->CloseRangeAttack_head = (Enemy_t*)c_malloc(sizeof(Enemy_t) * enemyCount);
	if(EnemyHead->CloseRangeAttack_head == NULL){
		return -1;
	}
	EnemyHead->CloseRangeAttack_remaining = enemyCount;
	EnemyHead->CloseRangeAttack_totality = enemyCount;
	
		
	int temp = 0;
	//动画创建
	temp |= close_Anim_Create(AnimManagerIndex);
	//逻辑创建
	temp |= close_Logic_Create(BTTreeIndex);
	if(temp != 0){
		return -1;
	}
	
	//初始化节点(默认所有的近程敌人休眠，需要主动启动)
	for(int i = 0;i<enemyCount;i++){
		EnemyHead->CloseRangeAttack_head[i].sprite = Sprites_ObjectCreate(AnimManagerIndex,0,0,closeMoveIndex,moveSpeed,BTTreeIndex);
		EnemyHead->CloseRangeAttack_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite));
		EnemyHead->CloseRangeAttack_head[i].EnemyStatus.isSleep = 1;
		EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable = 1;
		
		//当前动画为移动动画，设置为循环类型
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite),Anim_Loop);
		
		SpritesIT_Enable(EnemyHead->CloseRangeAttack_head[i].sprite);//开启中断
		Sprites_Sleep(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite));//休眠
		
		//私有数据初始化
		closePri_Init(camera,&EnemyHead->CloseRangeAttack_head[i]);
	}
	return 0;
}


//生成一个近程攻击的敌人
int CloseEnemy_Create(void){
	if(EnemyHead == NULL){
		return -1;
	}
	for(int i = 0;i<EnemyHead->CloseRangeAttack_remaining;i++){
		if(EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable == 1){
			//找到了可用的精灵
			EnemyHead->CloseRangeAttack_head[i].EnemyStatus.isSleep = 0;
			EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable = 0;
			
			//唤醒
			Sprites_Awaken(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite));
			//SpritesMov_SetPos(EnemyHead->CloseRangeAttack_head[i].sprite,1,1);
			//SpritesMov_SetPos(EnemyHead->CloseRangeAttack_head[i].sprite,18.5,12);//测试
			ClosePrivate_t *data = SpritesPriv_Get(EnemyHead->LongRangeAttack_head[i].sprite);
			vector NewPos = RandPos(data->camera);
			SpritesMov_SetPos(EnemyHead->CloseRangeAttack_head[i].sprite,NewPos.x,NewPos.y);
			
			SpritesMov_SetPos(EnemyHead->LongRangeAttack_head[i].sprite,NewPos.x,NewPos.y);
			EnemyHead->CloseRangeAttack_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite));
			
			return 0;
		}
		
	}
	return -1;
}

/**************************************
	【远程攻击配置】
***************************************/

int LongMoveIndex;
int LongAttackIndex;
int LongDamageIndex;
//创建远程攻击动画
int Long_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//创建动画管理器
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
	//创建动画链
	
	//攻击动画
	LongAttackIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,48,48,BIT_SPRITES,0.75);
	//移动动画
	LongMoveIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,48,48,BIT_SPRITES,1.5);
	//受击动画
	LongDamageIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,48,48,BIT_SPRITES,0.5);
	if(LongAttackIndex < 0  || LongDamageIndex < 0 || LongMoveIndex < 0){
		return -1;
	}
	int temp = 0;
	
	//移动动画帧
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,0,IKUN_LongMove_48x48_1,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,1,IKUN_LongMove_48x48_2,IKUN_LongMove_48x48_2_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,2,IKUN_LongMove_48x48_3,IKUN_LongMove_48x48_3_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,3,IKUN_LongMove_48x48_4,IKUN_LongMove_48x48_4_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,4,IKUN_LongMove_48x48_5,IKUN_LongMove_48x48_5_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,5,IKUN_LongMove_48x48_6,IKUN_LongMove_48x48_6_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,6,IKUN_LongMove_48x48_7,IKUN_LongMove_48x48_7_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,7,IKUN_LongMove_48x48_8,IKUN_LongMove_48x48_8_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,8,IKUN_LongMove_48x48_9,IKUN_LongMove_48x48_9_shadow);
	
	//攻击动画帧
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,0,IKUN_Attack_48x48_1,IKUN_Attack_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,1,IKUN_Attack_48x48_2,IKUN_Attack_48x48_2_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,2,IKUN_Attack_48x48_3,IKUN_Attack_48x48_3_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,3,IKUN_Attack_48x48_4,IKUN_Attack_48x48_4_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,4,IKUN_Attack_48x48_5,IKUN_Attack_48x48_5_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,5,IKUN_Attack_48x48_6,IKUN_Attack_48x48_6_shadow);
	
	//受击动画帧
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,0,IKUN_LongMove_48x48_1,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,1,IKUN_LongMove_48x48_1_shadow,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,2,IKUN_LongMove_48x48_1,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,3,IKUN_LongMove_48x48_1_shadow,IKUN_LongMove_48x48_1_shadow);
	
	
	
	if(temp != 0){
		return -1;
	}
	return 0;
}


/*中断表*/
//中断表映射逻辑
SITindex_t SIT_MapIndexHandle_Long(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	//检查是否需要进入死亡中断
	if(data->HP <= 0){
		return 1;
	}
	
	//检查是否进入受击中断
	if(data->hit.isHit == 1){
		return 2;
	}
	
	//没有触发中断
	return -1;
}
//远程攻击——死亡中断逻辑
SpriteBTStatus_t SBT_ExecuteHandle_Long_Die(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	//重置精灵状态
	data->HP = ENEMY_HP_LONG;
	data->hit.isHit = 0;
	
	//切换为移动动画
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongMoveIndex);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//循环逻辑
	
	//休眠精灵
	data->Enemy->EnemyStatus.isSleep = 1;
	data->Enemy->EnemyStatus.usable = 1;
	Sprites_Sleep(TO_SPRITES_BASE(Sprites));
	EnemyHead->LongRangeAttack_remaining++;
	enemyCore++;//得分加1
	LongS_survivalCount--;
	return SBT_SUCCESS;//执行完成
	
}
//远程攻击——受击中断逻辑
SpriteBTStatus_t SBT_ExecuteHandle_Long_Hit(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	
	if(data->hit.isFirstHit == 1){//是否首次受击
		//设置为受击动画
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongDamageIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Once);//单次逻辑
		data->hit.isFirstHit = 0;
	}
	
	/*受击逻辑*/
	
	//检查是否受击动画结束
	if(SpritesAnim_IsEndFrm(TO_SPRITES_BASE(Sprites)) == 0){//动画没有结束
		SpritesAnim_Update(TO_SPRITES_BASE(Sprites));//更新动画
		return SBT_RUNNING;
	}else{
		data->HP--;
		//切换成为移动逻辑
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongMoveIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//循环逻辑
		data->hit.isHit = 0;//设置为没有受击状态
		data->hit.isFirstHit = 1;//恢复首次受击标志
		
		//如果被打断之前是正在攻击，
		if(data->attack.isFirstAttack == 0){
			data->attack.isFirstAttack = 1;//重置首次攻击状态
		}
		return SBT_SUCCESS;
	}
}

/*行为树*/
// [动作] 向玩家移动（完备帧）
SpriteBTStatus_t SBT_ExecuteHandle_Long_Move(Sprites_t* Sprites, void* privateData){	
    LongPrivate_t* data = (LongPrivate_t*)privateData;
    // 获取敌人及玩家（摄像机）位置
    vector Sprite_Pos = data->Enemy->pos;
    vector Camera_Pos = Camera_GetPos(data->camera);

    // 获取当前地图信息
    int index = WorldMap_GetCurrentMapPos();
    int MapRow = WorldMap_GetROW(index);
    int MapCol = WorldMap_GetColumn(index);
    unsigned char* map = WorldMap_GetArray(index);

    // 将精灵和玩家的位置转换为网格坐标（按行、列）
    int enemyRow = (int)(Sprite_Pos.x);
    int enemyCol = (int)(Sprite_Pos.y);
    int playerRow = (int)(Camera_Pos.x);
    int playerCol = (int)(Camera_Pos.y);

    // 计算从敌人指向玩家的基本方向（注意：x 为行，y 为列，转换为传统坐标时将列视为水平、行视为垂直）
    double dx = (double)(playerCol - enemyCol); // 水平差
    double dy = (double)(playerRow - enemyRow); // 垂直差
    double baseAngle = atan2(dy, dx);

    // 加入随机扰动，使期望方向偏离玩家正中心，扰动范围为 -45° 到 +45°（即 -PI/4 到 +PI/4）
    double randPert = ((double)rand() / (double)RAND_MAX) * (M_PI / 2.0) - (M_PI / 4.0);
    double desiredAngle = baseAngle + randPert;

    // 定义8个候选方向（每次步长均为1格）
    vector candidateMoves[8] = {
        {-1.0f, -1.0f},  // 上左
        {-1.0f,  0.0f},  // 上
        {-1.0f,  1.0f},  // 上右
        { 0.0f, -1.0f},  // 左
        { 0.0f,  1.0f},  // 右
        { 1.0f, -1.0f},  // 下左
        { 1.0f,  0.0f},  // 下
        { 1.0f,  1.0f}   // 下右
    };

    double bestCost = 1e9;
    int bestCandidate = -1;
    // 当前与玩家的曼哈顿距离
    int currentManhattan = abs(playerRow - enemyRow) + abs(playerCol - enemyCol);

    // 遍历所有候选方向
    for (int i = 0; i < 8; i++) {
        int newRow = enemyRow + (int)(candidateMoves[i].x);
        int newCol = enemyCol + (int)(candidateMoves[i].y);
        // 检查地图边界及通行性
        if (newRow < 0 || newRow >= MapRow || newCol < 0 || newCol >= MapCol)
            continue;
        if (map[newRow * MapCol + newCol] != MOVABLE_AREA_VALUE)
            continue;
        // 计算候选位置与玩家之间的曼哈顿距离
        int candidateManhattan = abs(playerRow - newRow) + abs(playerCol - newCol);
        double distanceCost = 0.0;
        if (candidateManhattan < currentManhattan) {
            distanceCost = 0.0;      // 实际拉近了距离，无额外惩罚
        } else if (candidateManhattan == currentManhattan) {
            distanceCost = 1.0;      // 仅维持距离，加一点惩罚
        } else {
            distanceCost = 100.0;    // 增加距离则极大惩罚
        }
        // 计算候选移动方向的角度（注意：对于候选向量，其水平分量为 candidateMoves[i].y，垂直分量为 candidateMoves[i].x）
        double candAngle = atan2((double)candidateMoves[i].x, (double)candidateMoves[i].y);
        double angDiff = angle_diff(desiredAngle, candAngle);
        // 最终代价 = 角度差 + 距离惩罚，再加上一个微小随机噪声以打破完全一致性
        double cost = angDiff + distanceCost + (((double)rand() / (double)RAND_MAX) * 0.1);
        if (cost < bestCost) {
            bestCost = cost;
            bestCandidate = i;
        }
    }

    vector NewDir;
    if (bestCandidate != -1) {
        NewDir = candidateMoves[bestCandidate];
    } else {
        // 若周围均不可移动，则选用一个默认方向，比如向右移动
        NewDir.x = 0.0f;
        NewDir.y = 1.0f;
    }

    // 设置精灵的新方向，执行移动和动画更新
    SpritesMov_SetDir(Sprites, NewDir);
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
    return SBT_SUCCESS;
}



//[动作] 发起攻击（低优先级为3的可中断非完备帧）	
SpriteBTStatus_t SBT_ExecuteHandle_Long_Attack(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	//检查是否为首次攻击
	if(data->attack.isFirstAttack == 1){
		//如果是首次进入攻击状态
		
		//切换为攻击动画
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongAttackIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Once);//单次逻辑
		
		data->attack.isFirstAttack = 0;//切换为非首次进入攻击状态
	}
	
	
	//执行动画
	if(SpritesAnim_IsEndFrm(TO_SPRITES_BASE(Sprites)) == 0){//如果动画没有执行完成，执行动画
		SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
		return SBT_RUNNING;
	}
	
	//动画执行结束，攻击末期，准备发起攻击逻辑
	
	//发射篮球
	basketball_fire(Sprites,data->camera);//不管篮球是否发射成功，都视为本来发射攻击结束
	data->attack.isFirstAttack = 1;//切换为首次进入攻击状态
	//攻击行为结束，切换为移动动画
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongMoveIndex);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//循环逻辑
	return SBT_SUCCESS;
}


//[动作] 攻击范围内的移动行为
SpriteBTStatus_t SBT_ExecuteHandle_AttackRangeMoveAct(Sprites_t* Sprites, void* privateData) {
    LongPrivate_t* data = (LongPrivate_t*)privateData;
    
    // 是否触发移动方向切换（每隔1秒切换一次方向）
    if(data->attack.MovementWithinAttackRangeInterval > 1.0) {
        data->attack.MovementWithinAttackRangeInterval = 0; // 刷新累积时间
        vector NewDir; // 新方向

        // 获取敌人及玩家（摄像机）的位置
        vector Sprite_Pos = data->Enemy->pos;
        vector Camera_Pos = Camera_GetPos(data->camera);

        // 获取当前地图信息，如需进一步做障碍检测，可利用这些信息
        int index = WorldMap_GetCurrentMapPos();
        int MapRow = WorldMap_GetROW(index);
        int MapCol = WorldMap_GetColumn(index);
        unsigned char* map = WorldMap_GetArray(index);

        /* 改变方向代码：
         * 1. 先计算从玩家到敌人的向量差 diff = Sprite_Pos - Camera_Pos。
         * 2. 求出该向量的垂直方向（左右随机选择其一），即 NewDir 设为 (-diff.y, diff.x) 或 (diff.y, -diff.x)。
         * 3. 为了让移动更加灵动，再加入一个较小幅度的随机扰动。
         * 4. 最后归一化 NewDir，确保移动速度不受方向长度影响。
         */

        double diffX = Sprite_Pos.x - Camera_Pos.x;
        double diffY = Sprite_Pos.y - Camera_Pos.y;
        double diffLen = sqrt(diffX * diffX + diffY * diffY);

        if(diffLen < 0.0001) {
            // 如果敌人与摄像机位置几乎重合，则随机生成一个方向
            NewDir.x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            NewDir.y = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        } else {
            // 计算垂直方向（横躲方向），左右随机选其一
            double perpX, perpY;
            if(rand() % 2) {
                perpX = -diffY;
                perpY = diffX;
            } else {
                perpX = diffY;
                perpY = -diffX;
            }
            // 加入随机扰动（扰动幅度可根据实际情况调整）
            double noiseScale = 0.3;  // 调整扰动系数
            double noiseX = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * noiseScale;
            double noiseY = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * noiseScale;

            NewDir.x = perpX + noiseX;
            NewDir.y = perpY + noiseY;
        }

        // 设置新方向
        SpritesMov_SetDir(Sprites, NewDir);
    }
    
    // 更新时间累积（单位：秒）
    data->attack.MovementWithinAttackRangeInterval += FrmRS_deltaTime_Get();
    
    // 更新位置与动画显示
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));

    return SBT_SUCCESS;
}


/*条件集合*/
//[条件] 判断玩家是否在精灵攻击范围内
SpriteBTStatus_t SBT_ExecuteHandle_IsPlayerInAttackRange(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	
	vector e_pos = data->Enemy->pos;//获取精灵位置
	vector p_pos = Camera_GetPos(data->camera);//获取玩家位置
	
	double x = e_pos.x - p_pos.x;
	double y = e_pos.y - p_pos.y;
	
	double dis = sqrt(x*x+y*y);//获取敌人与玩家位置
	if(dis >= 3){
		//如果距离大于等于3米，则认为没有在攻击范围内
		return SBT_FAILURE;
	}
	return SBT_SUCCESS;
}

//[条件] 判断攻击冷却时间是否结束
SpriteBTStatus_t SBT_ExecuteHandle_IsAttackCooldownOver(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	if(data->attack.AttackInterval > 3.0){
		//如果攻击间隔时间超过3秒，意味着可以攻击，即攻击冷却结束
		data->attack.AttackInterval = 0;//重置冷却
		return SBT_SUCCESS;
	}
	//冷却时间没有结束，获取当前的帧时间，用于冷却时间叠加
	data->attack.AttackInterval += FrmRS_deltaTime_Get();
	return SBT_FAILURE;
}

//创建远程攻击逻辑
int Long_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	/*
		行为：
			 [选择]
				 |--[顺序](逻辑：攻击)
				 |		|--[条件] 判断玩家是否在精灵攻击范围内
				 |		|--[条件] 判断攻击冷却时间是否结束
				 |		|--[动作] 发起攻击（低优先级为3的可中断非完备帧）	
				 |--[顺序](逻辑：攻击范围内的运动)
				 |		|--[条件] 判断玩家是否在精灵攻击范围内
				 |		|--[动作] 攻击范围内的移动行为（完备帧）		
				 |--[动作] 向玩家移动（完备帧）

		中断：
			死亡：优先级设为10 （不可取代）
			受击：优先级设为5  （可取代）
	*/
	int result = 0;
	/*普通行为树 ：没有设计完成*/
	
	
	/******************
		条件设计
	*******************/
	//条件：判断是否在攻击范围内
	SpriteBTNode_t* IsPlayerInAttackRange = SpritesBT_Condition(SBT_ExecuteHandle_IsPlayerInAttackRange);
	//条件：判断攻击冷却时间是否结束
	SpriteBTNode_t* IsAttackCooldownOver = SpritesBT_Condition(SBT_ExecuteHandle_IsAttackCooldownOver);
	
	
	/******************
		动作设计
	*******************/
	//动作：发起攻击（低优先级为3的可中断非完备帧，行为设计位置调整动作属性）	
	SpriteBTNode_t* AttackAct = SpritesBT_Action(SBT_ExecuteHandle_Long_Attack);
	//[动作] 向玩家移动
	SpriteBTNode_t* MoveAct = SpritesBT_Action(SBT_ExecuteHandle_Long_Move);
	//[动作] 攻击范围内的移动行为
	SpriteBTNode_t* AttackRangeMoveAct = SpritesBT_Action(SBT_ExecuteHandle_AttackRangeMoveAct);
	
	/******************
		行为设计
	*******************/
	//(逻辑：攻击)
	SpriteBTNode_t* AttackContainer = SpritesBT_Sequence();//容器创建
	result |= SpritesBT_InsertBehavior(AttackContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackContainer, IsAttackCooldownOver);
	//设置动作属性
	result |= SpritesBT_Action_ToPhased(AttackAct,SIT_AllowSubstitution,3);//设置攻击动作为：低优先级为3的可中断非完备帧
	result |= SpritesBT_InsertBehavior(AttackContainer, AttackAct);
	if(result != 0){
		return -1;//(逻辑：攻击)创建失败
	}
	
	//[顺序](逻辑：攻击范围内的运动)
	SpriteBTNode_t* AttackMoveContainer = SpritesBT_Sequence();//容器创建
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, AttackRangeMoveAct);
	if(result != 0){
		return -1;//(逻辑：攻击范围内的运动)创建失败
	}
	
	//总行为装载
	SpriteBTNode_t* SelectContainer = SpritesBT_SelectorCreate();
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackContainer);//攻击逻辑装载
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackMoveContainer); //攻击范围内的运动
	result |= SpritesBT_InsertBehavior(SelectContainer,MoveAct);//移动逻辑装载
	
	//挂载根节点
	result |= SpritesBT_MountToRoot(BTTreeIndex, SelectContainer);
	if(result != 0){
		return -1;//总行为创建失败
	}
	
	
	/*中断表*/
	//注册表
	result |= SpritesIT_Register(BTTreeIndex,SIT_MapIndexHandle_Long);
	//添加死亡行为节点
	result |= SpritesIT_Add(BTTreeIndex ,1,10,SIT_ProhibitSubstitution,SBT_ExecuteHandle_Long_Die);
	//添加受击行为节点
	result |= SpritesIT_Add(BTTreeIndex ,2,5,SIT_AllowSubstitution,SBT_ExecuteHandle_Long_Hit);
	if(result != 0){
		return -1;
	}
	
	
	
	return 0;
}


//远程敌人私有数据初始化
void LongPri_Init(Camera_Struct* camera,Enemy_t* Enemy){
	LongPrivate_t *data = c_malloc(sizeof(LongPrivate_t));
	data->camera = camera;
	data->Enemy = Enemy;
	data->HP = ENEMY_HP_LONG;//初始血
	//受击组
	data->hit.isHit = 0;//默认没有受击
	data->hit.isFirstHit = 1;//默认首次受击
	//攻击组
	data->attack.AttackInterval = 0;//攻击间隔累积时间
	data->attack.isFirstAttack = 1;//默认设置为首次攻击
	data->attack.MovementWithinAttackRangeInterval = 0;//攻击范围内的移动操作累积时间

	SpritesPriv_Add(Enemy->sprite,data,PrifreeHandle);
}
/**
  * @brief  远程敌人初始化
  *
  * @param	basketballCount	创建的篮球数量
  *
  * @retval 操作成功返回0，操作失败返回-1
  */
int LongEnemy_Init(int enemyCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed){
	if(enemyCount <= 0 || camera == NULL){
		return -1;
	}
	if(EnemyHead == NULL){
		EnemyHead = (EnemyHead_t*)c_malloc(sizeof(EnemyHead_t));
		if(EnemyHead == NULL){
			return -1;
		}
	}
	EnemyHead->LongRangeAttack_head = (Enemy_t*)c_malloc(sizeof(Enemy_t) * enemyCount);
	if(EnemyHead->LongRangeAttack_head == NULL){
		return -1;
	}
	EnemyHead->LongRangeAttack_remaining = enemyCount;
	EnemyHead->LongRangeAttack_totality = enemyCount;
	
	int temp = 0;
	//动画创建
	temp |= Long_Anim_Create(AnimManagerIndex);
	//逻辑创建
	temp |= Long_Logic_Create(BTTreeIndex);
	if(temp != 0){
		return -1;
	}
	
	//初始化节点(默认所有的远程敌人休眠，需要主动启动)
	for(int i = 0;i<enemyCount;i++){
		EnemyHead->LongRangeAttack_head[i].sprite = Sprites_ObjectCreate(AnimManagerIndex,0,0,LongMoveIndex,moveSpeed,BTTreeIndex);
		EnemyHead->LongRangeAttack_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite));
		EnemyHead->LongRangeAttack_head[i].EnemyStatus.isSleep = 1;
		EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable = 1;
		
		//当前动画为移动动画，设置为循环类型
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite),Anim_Loop);
		
		SpritesIT_Enable(EnemyHead->LongRangeAttack_head[i].sprite);//开启中断
		Sprites_Sleep(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite));//休眠
		
		//私有数据初始化
		LongPri_Init(camera,&EnemyHead->LongRangeAttack_head[i]);
	}
	return 0;
}


//生成一个远程攻击的敌人
int LongEnemy_Create(void){
	if(EnemyHead == NULL){
		return -1;
	}
	for(int i = 0;i<EnemyHead->LongRangeAttack_remaining;i++){
		if(EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable == 1){
			//找到了可用的精灵
			EnemyHead->LongRangeAttack_head[i].EnemyStatus.isSleep = 0;
			EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable = 0;
			
			//唤醒
			Sprites_Awaken(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite));
			//SpritesMov_SetPos(EnemyHead->LongRangeAttack_head[i].sprite,1,1);
			//SpritesMov_SetPos(EnemyHead->LongRangeAttack_head[i].sprite,19,12);//测试
			LongPrivate_t *data = SpritesPriv_Get(EnemyHead->LongRangeAttack_head[i].sprite);
			vector NewPos = RandPos(data->camera);
			SpritesMov_SetPos(EnemyHead->LongRangeAttack_head[i].sprite,NewPos.x,NewPos.y);
			EnemyHead->LongRangeAttack_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite));
			
			return 0;
		}
		
	}
	return -1;
}

/**************************************
	【篮球配置】配置完成
***************************************/

int basketballindex;
//创建篮球飞行物动画
int basketball_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//创建动画管理器
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
		//篮球动画
	basketballindex = SpritesAnim_CreateNewChain(AnimManagerIndex,24,24,BIT_SPRITES,1.0);
	if(basketballindex < 0){
		return -1;
	}
	int temp = 0;
	//篮球动画帧
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,basketballindex,0,basketball_24x24,basketball_24x24_shadown);
	if(temp != 0){
		return -1;
	}
	return 0;
}


SpriteBTStatus_t SBT_ExecuteHandle_basketball(Sprites_t* Sprites,void* privateData){
	BasketballPrivate_t *data = (BasketballPrivate_t *)privateData;//获取私有数据
	data->Enemy->EnemyStatus.usable = 0;//设置为不可用状态
	//前进一个帧距离
	SpritesMov_Forward(Sprites);
	data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));//更新坐标
	
	//检查是否攻击到了玩家
	vector pos = Camera_GetPos(data->camera);
	double x = pos.x - data->Enemy->pos.x;
	double y = pos.y - data->Enemy->pos.y;
	double dis = sqrt(x*x+y*y);
	if(dis < 0.15){
		//如果子弹和玩家距离差在0.15，意味着玩家被攻击
		int * Health =  (int*)CameraPriv_Get(data->camera);
		*Health -= 1;//玩家减一滴血
		//篮球休眠
		data->Enemy->EnemyStatus.isSleep = 1;
		data->Enemy->EnemyStatus.usable = 1;
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));//休眠篮球
		return SBT_SUCCESS;
	}
	
	//检查是否碰到墙
	if(Sprites_collisionWallValue(Sprites) != 0){
		//碰到墙壁
		data->Enemy->EnemyStatus.isSleep = 1;
		data->Enemy->EnemyStatus.usable = 1;
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));//休眠篮球
		return SBT_SUCCESS;
	}
	//更新
	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}

//创建篮球运动逻辑
int basketball_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	SpriteBTNode_t* act = SpritesBT_Action(SBT_ExecuteHandle_basketball);
	SpritesBT_MountToRoot(BTTreeIndex, act);
	return 0;
}



/**
  * @brief  篮球初始化
  *
  * @param	basketballCount	创建的篮球数量
  *
  * @retval 操作成功返回0，操作失败返回-1
  */
int Basketball_Init(int basketballCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed){
	if(basketballCount <= 0 || camera == NULL){
		return -1;
	}
	if(EnemyHead == NULL){
		EnemyHead = (EnemyHead_t*)c_malloc(sizeof(EnemyHead_t));
		if(EnemyHead == NULL){
			return -1;
		}
	}
	EnemyHead->basketball_head = (Enemy_t*)c_malloc(sizeof(Enemy_t)* basketballCount);
	if(EnemyHead->basketball_head == NULL){
		return -1;
	}
	EnemyHead->basketball_remaining = basketballCount;
	EnemyHead->basketball_totality = basketballCount;
	
	int temp = 0;
	//动画创建
	temp |= basketball_Anim_Create(AnimManagerIndex);
	//逻辑创建
	temp |= basketball_Logic_Create(BTTreeIndex);
	if(temp != 0){
		return -1;
	}
	
	for(int i = 0 ;i < basketballCount;i++){
		EnemyHead->basketball_head[i].sprite = Sprites_ObjectCreate(AnimManagerIndex,0,0,basketballindex,moveSpeed,BTTreeIndex);
		EnemyHead->basketball_head[i].EnemyStatus.isSleep = 1;
		EnemyHead->basketball_head[i].EnemyStatus.usable = 1;
		
		EnemyHead->basketball_head[i].pos.x = 0;
		EnemyHead->basketball_head[i].pos.y = 0;
		
		//休眠篮球
		Sprites_Sleep(TO_SPRITES_BASE(EnemyHead->basketball_head[i].sprite));
		
		//创建私有数据
		BasketballPrivate_t *data = c_malloc(sizeof(BasketballPrivate_t));
		data->Enemy = &EnemyHead->basketball_head[i];
		data->camera = camera;
		SpritesPriv_Add(EnemyHead->basketball_head[i].sprite,data,PrifreeHandle);
	}
	return 0;
}
/**
  * @brief  发射篮球
  *
  * @param	basketballCount	创建的篮球数量
  *
  * @retval 操作成功返回0，操作失败返回-1
  */
int basketball_fire(Sprites_t* sprite,Camera_Struct* camera){
	if(sprite == NULL || camera == NULL || EnemyHead == NULL){
		return -1;
	}
	if(EnemyHead->basketball_remaining == 0){
		return -1;//没有可用的篮球
	}
	for(int i = 0;i<EnemyHead->basketball_totality;i++){
		if(EnemyHead->basketball_head[i].EnemyStatus.usable == 1){
			//找到一个可用的篮球
			
			
			//设置篮球的位置
			SpritesMov_SetPosToSprite(EnemyHead->basketball_head[i].sprite,sprite);
			//设置篮球的方向指向摄像头
			SpritesMov_SetDirToCamera(EnemyHead->basketball_head[i].sprite,camera);
		
			//唤醒
			Sprites_Awaken(TO_SPRITES_BASE(EnemyHead->basketball_head[i].sprite));
			EnemyHead->basketball_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->basketball_head[i].sprite));
			EnemyHead->basketball_head[i].EnemyStatus.isSleep = 0;
			EnemyHead->basketball_head[i].EnemyStatus.usable = 0;
			
			return 0;
		}
	}
	return -1;
}















///////特殊函数API
//获取碰撞的敌人
Sprites_t* GetEnemy(vector Bullent_pos) {
    if (EnemyHead == NULL) {
        return NULL;
    }
    
    const float threshold = 0.1f;
    float minDist = threshold;
    Enemy_t* result = NULL;
    int isLongRange = 0; //敌人类型标志（是否为远距离敌人）
    
    // 遍历近程攻击的敌人
    for (int i = 0; i < EnemyHead->CloseRangeAttack_totality; i++) {
        Enemy_t* enemy = &EnemyHead->CloseRangeAttack_head[i];
        
        // 仅处理处于不可用状态
        if (enemy->EnemyStatus.usable == 1) {
            continue;
        }
        
        // 计算欧式距离
        float dx = enemy->pos.x - Bullent_pos.x;
        float dy = enemy->pos.y - Bullent_pos.y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        // 当距离小于阈值，且比当前记录的最小距离更短的时候，更新结果
        if (distance < threshold && distance < minDist) {
            minDist = distance;
            result = enemy;
            isLongRange = 0; // Mark as close-range enemy
        }
    }
    
    // 遍历远程攻击的敌人
    for (int i = 0; i < EnemyHead->LongRangeAttack_totality; i++) {
        Enemy_t* enemy = &EnemyHead->LongRangeAttack_head[i];
		
        // 仅处理处于不可用状态
        if (enemy->EnemyStatus.usable == 1) {
            continue;
        }
        
        float dx = enemy->pos.x - Bullent_pos.x;
        float dy = enemy->pos.y - Bullent_pos.y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        if (distance < threshold && distance < minDist) {
            minDist = distance;
            result = enemy;
            isLongRange = 1; // Mark as long-range enemy
        }
    }
    
    // 检查是否找到了符合要求的敌人
    if (result != NULL) {
        if (isLongRange) {
            LongPrivate_t* data = SpritesPriv_Get(result->sprite);
            data->hit.isHit = 1; // 设置为受击
        } else {
            ClosePrivate_t* data = SpritesPriv_Get(result->sprite);
            data->hit.isHit = 1; // 设置为受击
        }
    }
    
    return result ? result->sprite : NULL;
}

