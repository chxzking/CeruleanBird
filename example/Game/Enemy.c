#include "Enemy.h"
#include "Sprites_internal.h"
#include "Camera_internal.h"
EnemyHead_t* EnemyHead = NULL;
#define ENEMY_HP_LONG 5
#define ENEMY_HP_CLOSE 5
//���ú���

void PrifreeHandle(void* pdata){
	c_free(pdata);
}

#define M_PI 3.1415

// �������������������Ƕȵ���С��ֵ�������Χ [0, PI]��
static double angle_diff(double a, double b) {
    double diff = fmod(b - a, 2 * M_PI);
    if(diff < -M_PI)
         diff += 2 * M_PI;
    if(diff > M_PI)
         diff -= 2 * M_PI;
    return fabs(diff);
}

//С��ͼ��ӡ
void Minimap(Camera_Struct* camera){
	
	//��ͼ��ӡ
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
	
	//��Ҵ�ӡ
	vector p_pos = Camera_GetPos(camera);
	MistinkPixel_Operate(displayer,(int)p_pos.x,(int)p_pos.y ,PixelOpen);
	
	//Զ�̾����ӡ
	for(int i = 0;i<EnemyHead->LongRangeAttack_totality;i++){
		if(EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable == 0){
			vector s_pos = EnemyHead->LongRangeAttack_head[i].pos;
			MistinkPixel_Operate(displayer,(int)s_pos.x,(int)s_pos.y ,PixelOpen);
		}
	}
	
	//���̾����ӡ
	for(int i = 0;i<EnemyHead->CloseRangeAttack_totality;i++){
		if(EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable == 0){
			vector s_pos = EnemyHead->CloseRangeAttack_head[i].pos;
			MistinkPixel_Operate(displayer,(int)s_pos.x,(int)s_pos.y ,PixelOpen);
		}
	}
}
	

vector RandPos(Camera_Struct* camera){
    // ��ȡ��ǰ��ͼ��Ϣ
    int index = WorldMap_GetCurrentMapPos();
    int MapRow = WorldMap_GetROW(index);
    int MapCol = WorldMap_GetColumn(index);
    unsigned char* map = WorldMap_GetArray(index);
	
	vector result;
    int count = 0; // ����ͳ�Ʊ���������ֵΪ0����������

    for (int i = 0; i < MapRow; i++)
    {
        for (int j = 0; j < MapCol; j++)
        {
            // ��������������ʽ���ʵ�ǰ(i,j)λ��
            if (map[i * MapCol + j] == 0)
            {
                count++;  // �ҵ�һ�����ʵĵ�
                // ��1/count�ĸ��ʸ��º�ѡ���
                if (rand() % count == 0)
                {
                    result.x = i;
                    result.y = j;
                }
            }
        }
    }

    // ���������δ�ҵ��κ���ֵΪ0�ĵ㣬�򷵻�һ�������ʶ��������(-1, -1)
    if (count == 0)
    {
        result.x = -1;
        result.y = -1;
    }

    return result;
}

/**************************************
	�����̹������á�
***************************************/

int closeMoveIndex;
int closeDamageIndex;

//�������̹�������
int close_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//��������������
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
	//����������
	
	//�ƶ�����
	closeMoveIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,45,45,BIT_SPRITES,2.5);
	//�ܻ�����
	closeDamageIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,45,45,BIT_SPRITES,0.5);
	if(closeMoveIndex < 0  || closeDamageIndex < 0){
		return -1;
	}
	int temp = 0;
	//�ƶ�����֡
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

	
	//�ܻ�����֡
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,0,IKUN_TieShanKao_45x45_1,IKUN_TieShanKao_45x45_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,1,IKUN_TieShanKao_45x45_1_shadow,IKUN_TieShanKao_45x45_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,2,IKUN_TieShanKao_45x45_1,IKUN_TieShanKao_45x45_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,closeDamageIndex,3,IKUN_TieShanKao_45x45_1_shadow,IKUN_TieShanKao_45x45_1_shadow);
	
	if(temp != 0){
		return -1;
	}
	return 0;
}
/*�жϱ�*/
//�жϱ�ӳ���߼�
SITindex_t SIT_MapIndexHandle_Close(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	//����Ƿ���Ҫ���������ж�
	if(data->HP <= 0){
		return 1;
	}
	
	//����Ƿ�����ܻ��ж�
	if(data->hit.isHit == 1){
		return 2;
	}
	
	//û�д����ж�
	return -1;
}
//���̹������������ж��߼�
SpriteBTStatus_t SBT_ExecuteHandle_Close_Die(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	//���þ���״̬
	data->HP = ENEMY_HP_CLOSE;
	data->hit.isHit = 0;
	
	//�л�Ϊ�ƶ�����
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),closeMoveIndex);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//ѭ���߼�
	
	//���߾���
	data->Enemy->EnemyStatus.isSleep = 1;
	data->Enemy->EnemyStatus.usable = 1;
	Sprites_Sleep(TO_SPRITES_BASE(Sprites));
	EnemyHead->CloseRangeAttack_remaining++;
	enemyCore++;//�÷ּ�1
	LongC_survivalCount--;
	return SBT_SUCCESS;//ִ�����
	
}
//���̹��������ܻ��ж��߼�
SpriteBTStatus_t SBT_ExecuteHandle_Close_Hit(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	
	if(data->hit.isFirstHit == 1){//�Ƿ��״��ܻ�
		//����Ϊ�ܻ�����
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),closeDamageIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Once);//�����߼�
		data->hit.isFirstHit = 0;
	}
	
	/*�ܻ��߼�*/
	
	//����Ƿ��ܻ���������
	if(SpritesAnim_IsEndFrm(TO_SPRITES_BASE(Sprites)) == 0){//����û�н���
		SpritesAnim_Update(TO_SPRITES_BASE(Sprites));//���¶���
		return SBT_RUNNING;
	}else{
		data->HP--;
		//�л���Ϊ�ƶ��߼�
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),closeMoveIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//ѭ���߼�
		data->hit.isHit = 0;//����Ϊû���ܻ�״̬
		data->hit.isFirstHit = 1;//�ָ��״��ܻ���־
		
		return SBT_SUCCESS;
	}
}

/*��Ϊ��*/
// [����] ������ƶ����걸֡��
SpriteBTStatus_t SBT_ExecuteHandle_Close_Move(Sprites_t* Sprites, void* privateData){	
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
    // ��ȡ���˼���ң��������λ��
    vector Sprite_Pos = data->Enemy->pos;
    vector Camera_Pos = Camera_GetPos(data->camera);

    // ��ȡ��ǰ��ͼ��Ϣ
    int index = WorldMap_GetCurrentMapPos();
    int MapRow = WorldMap_GetROW(index);
    int MapCol = WorldMap_GetColumn(index);
    unsigned char* map = WorldMap_GetArray(index);

    // ���������ҵ�λ��ת��Ϊ�������꣨���С��У�
    int enemyRow = (int)(Sprite_Pos.x);
    int enemyCol = (int)(Sprite_Pos.y);
    int playerRow = (int)(Camera_Pos.x);
    int playerCol = (int)(Camera_Pos.y);

    // ����ӵ���ָ����ҵĻ�������ע�⣺x Ϊ�У�y Ϊ�У�ת��Ϊ��ͳ����ʱ������Ϊˮƽ������Ϊ��ֱ��
    double dx = (double)(playerCol - enemyCol); // ˮƽ��
    double dy = (double)(playerRow - enemyRow); // ��ֱ��
    double baseAngle = atan2(dy, dx);

    // ��������Ŷ���ʹ��������ƫ����������ģ��Ŷ���ΧΪ -45�� �� +45�㣨�� -PI/4 �� +PI/4��
    double randPert = ((double)rand() / (double)RAND_MAX) * (M_PI / 2.0) - (M_PI / 4.0);
    double desiredAngle = baseAngle + randPert;

    // ����8����ѡ����ÿ�β�����Ϊ1��
    vector candidateMoves[8] = {
        {-1.0f, -1.0f},  // ����
        {-1.0f,  0.0f},  // ��
        {-1.0f,  1.0f},  // ����
        { 0.0f, -1.0f},  // ��
        { 0.0f,  1.0f},  // ��
        { 1.0f, -1.0f},  // ����
        { 1.0f,  0.0f},  // ��
        { 1.0f,  1.0f}   // ����
    };

    double bestCost = 1e9;
    int bestCandidate = -1;
    // ��ǰ����ҵ������پ���
    int currentManhattan = abs(playerRow - enemyRow) + abs(playerCol - enemyCol);

    // �������к�ѡ����
    for (int i = 0; i < 8; i++) {
        int newRow = enemyRow + (int)(candidateMoves[i].x);
        int newCol = enemyCol + (int)(candidateMoves[i].y);
        // ����ͼ�߽缰ͨ����
        if (newRow < 0 || newRow >= MapRow || newCol < 0 || newCol >= MapCol)
            continue;
        if (map[newRow * MapCol + newCol] != MOVABLE_AREA_VALUE)
            continue;
        // �����ѡλ�������֮��������پ���
        int candidateManhattan = abs(playerRow - newRow) + abs(playerCol - newCol);
        double distanceCost = 0.0;
        if (candidateManhattan < currentManhattan) {
            distanceCost = 0.0;      // ʵ�������˾��룬�޶���ͷ�
        } else if (candidateManhattan == currentManhattan) {
            distanceCost = 1.0;      // ��ά�־��룬��һ��ͷ�
        } else {
            distanceCost = 100.0;    // ���Ӿ����򼫴�ͷ�
        }
        // �����ѡ�ƶ�����ĽǶȣ�ע�⣺���ں�ѡ��������ˮƽ����Ϊ candidateMoves[i].y����ֱ����Ϊ candidateMoves[i].x��
        double candAngle = atan2((double)candidateMoves[i].x, (double)candidateMoves[i].y);
        double angDiff = angle_diff(desiredAngle, candAngle);
        // ���մ��� = �ǶȲ� + ����ͷ����ټ���һ��΢С��������Դ�����ȫһ����
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
        // ����Χ�������ƶ�����ѡ��һ��Ĭ�Ϸ��򣬱��������ƶ�
        NewDir.x = 0.0f;
        NewDir.y = 1.0f;
    }

    // ���þ�����·���ִ���ƶ��Ͷ�������
    SpritesMov_SetDir(Sprites, NewDir);
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
    return SBT_SUCCESS;
}



//[����] ���𹥻��������ȼ�Ϊ3�Ŀ��жϷ��걸֡��	
SpriteBTStatus_t SBT_ExecuteHandle_Close_Attack(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	
	vector e_pos = data->Enemy->pos;//��ȡ����λ��
	vector p_pos = Camera_GetPos(data->camera);//��ȡ���λ��
	
	double x = e_pos.x - p_pos.x;
	double y = e_pos.y - p_pos.y;
	
	double dis = sqrt(x*x+y*y);//��ȡ���������λ��
	if(dis >= 0.15){
		//���������ڵ���0.15�ף�����Ϊû�����˺���Χ��
		return SBT_FAILURE;
	}
	
	int * Health =  (int*)CameraPriv_Get(data->camera);
	*Health -= 1;//��Ҽ�һ��Ѫ
	
	
	return SBT_SUCCESS;
}


//[����] ������Χ�ڵ��ƶ���Ϊ
SpriteBTStatus_t SBT_ExecuteHandle_AttackRangeMoveAct_close(Sprites_t* Sprites, void* privateData) {
    ClosePrivate_t* data = (ClosePrivate_t*)privateData;
    
    // ÿ�� 1 ���л�һ���ƶ�����
    if (data->attack.MovementWithinAttackRangeInterval > 1.0f) {
        data->attack.MovementWithinAttackRangeInterval = 0; // ˢ���ۼ�ʱ��
        vector NewDir; // �·���
        
        // ��ȡ���˼���ң����������λ��
        vector Sprite_Pos = data->Enemy->pos;
        vector Camera_Pos = Camera_GetPos(data->camera);
        
        // ��ȡ��ǰ��ͼ��Ϣ�������һ�����ϰ���⣬��������Щ��Ϣ
        int index = WorldMap_GetCurrentMapPos();
        int MapRow = WorldMap_GetROW(index);
        int MapCol = WorldMap_GetColumn(index);
        unsigned char* map = WorldMap_GetArray(index);
        
        /* �ı䷽����룺
         * 1. ����������ƶ������·��
         * 2. ���ܱܿ�ǽ�ڸ���
         */
        {
            // ������˵���ҵĲ������
            vector diff;
            diff.x = Camera_Pos.x - Sprite_Pos.x;
            diff.y = Camera_Pos.y - Sprite_Pos.y;
            float distance = sqrt(diff.x * diff.x + diff.y * diff.y);
            
            if (distance > 0.001f) {  // ���������
                // ��һ������������õ��������ƶ�����
                vector desiredDir;
                desiredDir.x = diff.x / distance;
                desiredDir.y = diff.y / distance;
                
                // �趨һ���ƶ��ľ��루�ɸ���ʵ�����������
                float stepSize = 1.0f;
                
                // �����ֱ�߷����ƶ�һ���Ƿ�������ϰ�
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
                    // �����ͼ�� 1 ��ʾǽ�ڣ��赲�ƶ�
                    isBlocked = 1;
                }
                
                if (isBlocked) {
                    // ���ֱ�߷����赲������������ 45�� ��ƫת���ܿ��ϰ�
                    #define ROTATE_VECTOR(v, angle) ( (vector){ \
                        (v).x * cos(angle) - (v).y * sin(angle), \
                        (v).x * sin(angle) + (v).y * cos(angle) } )
                    
                    float angleOffset = 45.0f * 3.14159265f / 180.0f;  // 45��ת��Ϊ����
                    
                    // ������ת 45��
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
                        // ����תͬ��������������ת 45��
                        vector rightDir = ROTATE_VECTOR(desiredDir, -angleOffset);
                        NewDir = rightDir;
                    }
                    
                    #undef ROTATE_VECTOR
                } else {
                    // û���ϰ���ֱ��������������
                    NewDir = desiredDir;
                }
            } else {
                // ����������ҷǳ��ӽ�ʱ���ɱ���ԭ�з�����趨Ϊ 0
                NewDir.x = 0.0f;
                NewDir.y = 0.0f;
            }
        }
        
        // �����·���
        SpritesMov_SetDir(Sprites, NewDir);
    }
    
    // ����ʱ���ۻ�����λ���룩
    data->attack.MovementWithinAttackRangeInterval += FrmRS_deltaTime_Get();
    
    // ����λ���붯����ʾ
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
    
    return SBT_SUCCESS;
}



/*��������*/
//[����] �ж�����Ƿ��ھ��鹥����Χ��
SpriteBTStatus_t SBT_ExecuteHandle_IsPlayerInAttackRange_close(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	
	vector e_pos = data->Enemy->pos;//��ȡ����λ��
	vector p_pos = Camera_GetPos(data->camera);//��ȡ���λ��
	
	double x = e_pos.x - p_pos.x;
	double y = e_pos.y - p_pos.y;
	
	double dis = sqrt(x*x+y*y);//��ȡ���������λ��
	if(dis >= 1.5){
		//���������ڵ���0.15�ף�����Ϊû���ڹ�����Χ��
		return SBT_FAILURE;
	}
	return SBT_SUCCESS;
}

//[����] �жϹ�����ȴʱ���Ƿ����
SpriteBTStatus_t SBT_ExecuteHandle_IsAttackCooldownOver_close(Sprites_t* Sprites,void* privateData){
	ClosePrivate_t* data = (ClosePrivate_t*)privateData;
	if(data->attack.AttackInterval > 3.0){
		//����������ʱ�䳬��3�룬��ζ�ſ��Թ�������������ȴ����
		data->attack.AttackInterval = 0;//������ȴ
		return SBT_SUCCESS;
	}
	//��ȴʱ��û�н�������ȡ��ǰ��֡ʱ�䣬������ȴʱ�����
	data->attack.AttackInterval += FrmRS_deltaTime_Get();
	return SBT_FAILURE;
}

//�������̹����߼�
int close_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	
		/*
		��Ϊ��
			 [ѡ��]
				 |--[˳��](�߼�������)
				 |		|--[����] �ж�����Ƿ��ھ��鹥����Χ��
				 |		|--[����] �жϹ�����ȴʱ���Ƿ����
				 |		|--[����] ���𹥻��������ȼ�Ϊ3�Ŀ��жϷ��걸֡��	
				 |--[˳��](�߼���������Χ�ڵ��˶�)
				 |		|--[����] �ж�����Ƿ��ھ��鹥����Χ��
				 |		|--[����] ������Χ�ڵ��ƶ���Ϊ���걸֡��		
				 |--[����] ������ƶ����걸֡��

		�жϣ�
			���������ȼ���Ϊ10 ������ȡ����
			�ܻ������ȼ���Ϊ5  ����ȡ����
	*/
	int result = 0;
	/*��ͨ��Ϊ�� ��û��������*/
	
	
	/******************
		�������
	*******************/
	//�������ж��Ƿ��ڹ�����Χ��
	SpriteBTNode_t* IsPlayerInAttackRange = SpritesBT_Condition(SBT_ExecuteHandle_IsPlayerInAttackRange_close);
	//�������жϹ�����ȴʱ���Ƿ����
	SpriteBTNode_t* IsAttackCooldownOver = SpritesBT_Condition(SBT_ExecuteHandle_IsAttackCooldownOver_close);
	
	
	/******************
		�������
	*******************/
	//���������𹥻��������ȼ�Ϊ3�Ŀ��жϷ��걸֡����Ϊ���λ�õ����������ԣ�	
	SpriteBTNode_t* AttackAct = SpritesBT_Action(SBT_ExecuteHandle_Close_Attack);
	//[����] ������ƶ�
	SpriteBTNode_t* MoveAct = SpritesBT_Action(SBT_ExecuteHandle_Close_Move);
	//[����] ������Χ�ڵ��ƶ���Ϊ
	SpriteBTNode_t* AttackRangeMoveAct = SpritesBT_Action(SBT_ExecuteHandle_AttackRangeMoveAct_close);
	
	/******************
		��Ϊ���
	*******************/
	//(�߼�������)
	SpriteBTNode_t* AttackContainer = SpritesBT_Sequence();//��������
	result |= SpritesBT_InsertBehavior(AttackContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackContainer, IsAttackCooldownOver);
	//���ö�������
	result |= SpritesBT_Action_ToPhased(AttackAct,SIT_AllowSubstitution,3);//���ù�������Ϊ�������ȼ�Ϊ3�Ŀ��жϷ��걸֡
	result |= SpritesBT_InsertBehavior(AttackContainer, AttackAct);
	if(result != 0){
		return -1;//(�߼�������)����ʧ��
	}
	
	//[˳��](�߼���������Χ�ڵ��˶�)
	SpriteBTNode_t* AttackMoveContainer = SpritesBT_Sequence();//��������
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, AttackRangeMoveAct);
	if(result != 0){
		return -1;//(�߼���������Χ�ڵ��˶�)����ʧ��
	}
	
	//����Ϊװ��
	SpriteBTNode_t* SelectContainer = SpritesBT_SelectorCreate();
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackContainer);//�����߼�װ��
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackMoveContainer); //������Χ�ڵ��˶�
	result |= SpritesBT_InsertBehavior(SelectContainer,MoveAct);//�ƶ��߼�װ��
	
	//���ظ��ڵ�
	result |= SpritesBT_MountToRoot(BTTreeIndex, SelectContainer);
	if(result != 0){
		return -1;//����Ϊ����ʧ��
	}
	
	
	/*�жϱ�*/
	//ע���
	result |= SpritesIT_Register(BTTreeIndex,SIT_MapIndexHandle_Close);
	//���������Ϊ�ڵ�
	result |= SpritesIT_Add(BTTreeIndex ,1,10,SIT_ProhibitSubstitution,SBT_ExecuteHandle_Close_Die);
	//����ܻ���Ϊ�ڵ�
	result |= SpritesIT_Add(BTTreeIndex ,2,5,SIT_AllowSubstitution,SBT_ExecuteHandle_Close_Hit);
	if(result != 0){
		return -1;
	}
	
	
	
	return 0;
}



//���̵���˽�����ݳ�ʼ��
void closePri_Init(Camera_Struct* camera,Enemy_t* Enemy){
	ClosePrivate_t *data = c_malloc(sizeof(ClosePrivate_t));
	data->camera = camera;
	data->Enemy = Enemy;
	data->HP = ENEMY_HP_CLOSE;//��ʼѪ
	//�ܻ���
	data->hit.isHit = 0;//Ĭ��û���ܻ�
	data->hit.isFirstHit = 1;//Ĭ���״��ܻ�
	//������
	data->attack.AttackInterval = 0;//��������ۻ�ʱ��
	data->attack.MovementWithinAttackRangeInterval = 0;//������Χ�ڵ��ƶ������ۻ�ʱ��

	SpritesPriv_Add(Enemy->sprite,data,PrifreeHandle);
}
/**
  * @brief  ���̵��˳�ʼ��
  *
  * @param	basketballCount	��������������
  *
  * @retval �����ɹ�����0������ʧ�ܷ���-1
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
	//��������
	temp |= close_Anim_Create(AnimManagerIndex);
	//�߼�����
	temp |= close_Logic_Create(BTTreeIndex);
	if(temp != 0){
		return -1;
	}
	
	//��ʼ���ڵ�(Ĭ�����еĽ��̵������ߣ���Ҫ��������)
	for(int i = 0;i<enemyCount;i++){
		EnemyHead->CloseRangeAttack_head[i].sprite = Sprites_ObjectCreate(AnimManagerIndex,0,0,closeMoveIndex,moveSpeed,BTTreeIndex);
		EnemyHead->CloseRangeAttack_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite));
		EnemyHead->CloseRangeAttack_head[i].EnemyStatus.isSleep = 1;
		EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable = 1;
		
		//��ǰ����Ϊ�ƶ�����������Ϊѭ������
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite),Anim_Loop);
		
		SpritesIT_Enable(EnemyHead->CloseRangeAttack_head[i].sprite);//�����ж�
		Sprites_Sleep(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite));//����
		
		//˽�����ݳ�ʼ��
		closePri_Init(camera,&EnemyHead->CloseRangeAttack_head[i]);
	}
	return 0;
}


//����һ�����̹����ĵ���
int CloseEnemy_Create(void){
	if(EnemyHead == NULL){
		return -1;
	}
	for(int i = 0;i<EnemyHead->CloseRangeAttack_remaining;i++){
		if(EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable == 1){
			//�ҵ��˿��õľ���
			EnemyHead->CloseRangeAttack_head[i].EnemyStatus.isSleep = 0;
			EnemyHead->CloseRangeAttack_head[i].EnemyStatus.usable = 0;
			
			//����
			Sprites_Awaken(TO_SPRITES_BASE(EnemyHead->CloseRangeAttack_head[i].sprite));
			//SpritesMov_SetPos(EnemyHead->CloseRangeAttack_head[i].sprite,1,1);
			//SpritesMov_SetPos(EnemyHead->CloseRangeAttack_head[i].sprite,18.5,12);//����
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
	��Զ�̹������á�
***************************************/

int LongMoveIndex;
int LongAttackIndex;
int LongDamageIndex;
//����Զ�̹�������
int Long_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//��������������
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
	//����������
	
	//��������
	LongAttackIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,48,48,BIT_SPRITES,0.75);
	//�ƶ�����
	LongMoveIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,48,48,BIT_SPRITES,1.5);
	//�ܻ�����
	LongDamageIndex = SpritesAnim_CreateNewChain(AnimManagerIndex,48,48,BIT_SPRITES,0.5);
	if(LongAttackIndex < 0  || LongDamageIndex < 0 || LongMoveIndex < 0){
		return -1;
	}
	int temp = 0;
	
	//�ƶ�����֡
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,0,IKUN_LongMove_48x48_1,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,1,IKUN_LongMove_48x48_2,IKUN_LongMove_48x48_2_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,2,IKUN_LongMove_48x48_3,IKUN_LongMove_48x48_3_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,3,IKUN_LongMove_48x48_4,IKUN_LongMove_48x48_4_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,4,IKUN_LongMove_48x48_5,IKUN_LongMove_48x48_5_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,5,IKUN_LongMove_48x48_6,IKUN_LongMove_48x48_6_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,6,IKUN_LongMove_48x48_7,IKUN_LongMove_48x48_7_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,7,IKUN_LongMove_48x48_8,IKUN_LongMove_48x48_8_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongMoveIndex,8,IKUN_LongMove_48x48_9,IKUN_LongMove_48x48_9_shadow);
	
	//��������֡
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,0,IKUN_Attack_48x48_1,IKUN_Attack_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,1,IKUN_Attack_48x48_2,IKUN_Attack_48x48_2_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,2,IKUN_Attack_48x48_3,IKUN_Attack_48x48_3_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,3,IKUN_Attack_48x48_4,IKUN_Attack_48x48_4_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,4,IKUN_Attack_48x48_5,IKUN_Attack_48x48_5_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongAttackIndex,5,IKUN_Attack_48x48_6,IKUN_Attack_48x48_6_shadow);
	
	//�ܻ�����֡
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,0,IKUN_LongMove_48x48_1,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,1,IKUN_LongMove_48x48_1_shadow,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,2,IKUN_LongMove_48x48_1,IKUN_LongMove_48x48_1_shadow);
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,LongDamageIndex,3,IKUN_LongMove_48x48_1_shadow,IKUN_LongMove_48x48_1_shadow);
	
	
	
	if(temp != 0){
		return -1;
	}
	return 0;
}


/*�жϱ�*/
//�жϱ�ӳ���߼�
SITindex_t SIT_MapIndexHandle_Long(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	//����Ƿ���Ҫ���������ж�
	if(data->HP <= 0){
		return 1;
	}
	
	//����Ƿ�����ܻ��ж�
	if(data->hit.isHit == 1){
		return 2;
	}
	
	//û�д����ж�
	return -1;
}
//Զ�̹������������ж��߼�
SpriteBTStatus_t SBT_ExecuteHandle_Long_Die(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	//���þ���״̬
	data->HP = ENEMY_HP_LONG;
	data->hit.isHit = 0;
	
	//�л�Ϊ�ƶ�����
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongMoveIndex);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//ѭ���߼�
	
	//���߾���
	data->Enemy->EnemyStatus.isSleep = 1;
	data->Enemy->EnemyStatus.usable = 1;
	Sprites_Sleep(TO_SPRITES_BASE(Sprites));
	EnemyHead->LongRangeAttack_remaining++;
	enemyCore++;//�÷ּ�1
	LongS_survivalCount--;
	return SBT_SUCCESS;//ִ�����
	
}
//Զ�̹��������ܻ��ж��߼�
SpriteBTStatus_t SBT_ExecuteHandle_Long_Hit(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	
	if(data->hit.isFirstHit == 1){//�Ƿ��״��ܻ�
		//����Ϊ�ܻ�����
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongDamageIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Once);//�����߼�
		data->hit.isFirstHit = 0;
	}
	
	/*�ܻ��߼�*/
	
	//����Ƿ��ܻ���������
	if(SpritesAnim_IsEndFrm(TO_SPRITES_BASE(Sprites)) == 0){//����û�н���
		SpritesAnim_Update(TO_SPRITES_BASE(Sprites));//���¶���
		return SBT_RUNNING;
	}else{
		data->HP--;
		//�л���Ϊ�ƶ��߼�
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongMoveIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//ѭ���߼�
		data->hit.isHit = 0;//����Ϊû���ܻ�״̬
		data->hit.isFirstHit = 1;//�ָ��״��ܻ���־
		
		//��������֮ǰ�����ڹ�����
		if(data->attack.isFirstAttack == 0){
			data->attack.isFirstAttack = 1;//�����״ι���״̬
		}
		return SBT_SUCCESS;
	}
}

/*��Ϊ��*/
// [����] ������ƶ����걸֡��
SpriteBTStatus_t SBT_ExecuteHandle_Long_Move(Sprites_t* Sprites, void* privateData){	
    LongPrivate_t* data = (LongPrivate_t*)privateData;
    // ��ȡ���˼���ң��������λ��
    vector Sprite_Pos = data->Enemy->pos;
    vector Camera_Pos = Camera_GetPos(data->camera);

    // ��ȡ��ǰ��ͼ��Ϣ
    int index = WorldMap_GetCurrentMapPos();
    int MapRow = WorldMap_GetROW(index);
    int MapCol = WorldMap_GetColumn(index);
    unsigned char* map = WorldMap_GetArray(index);

    // ���������ҵ�λ��ת��Ϊ�������꣨���С��У�
    int enemyRow = (int)(Sprite_Pos.x);
    int enemyCol = (int)(Sprite_Pos.y);
    int playerRow = (int)(Camera_Pos.x);
    int playerCol = (int)(Camera_Pos.y);

    // ����ӵ���ָ����ҵĻ�������ע�⣺x Ϊ�У�y Ϊ�У�ת��Ϊ��ͳ����ʱ������Ϊˮƽ������Ϊ��ֱ��
    double dx = (double)(playerCol - enemyCol); // ˮƽ��
    double dy = (double)(playerRow - enemyRow); // ��ֱ��
    double baseAngle = atan2(dy, dx);

    // ��������Ŷ���ʹ��������ƫ����������ģ��Ŷ���ΧΪ -45�� �� +45�㣨�� -PI/4 �� +PI/4��
    double randPert = ((double)rand() / (double)RAND_MAX) * (M_PI / 2.0) - (M_PI / 4.0);
    double desiredAngle = baseAngle + randPert;

    // ����8����ѡ����ÿ�β�����Ϊ1��
    vector candidateMoves[8] = {
        {-1.0f, -1.0f},  // ����
        {-1.0f,  0.0f},  // ��
        {-1.0f,  1.0f},  // ����
        { 0.0f, -1.0f},  // ��
        { 0.0f,  1.0f},  // ��
        { 1.0f, -1.0f},  // ����
        { 1.0f,  0.0f},  // ��
        { 1.0f,  1.0f}   // ����
    };

    double bestCost = 1e9;
    int bestCandidate = -1;
    // ��ǰ����ҵ������پ���
    int currentManhattan = abs(playerRow - enemyRow) + abs(playerCol - enemyCol);

    // �������к�ѡ����
    for (int i = 0; i < 8; i++) {
        int newRow = enemyRow + (int)(candidateMoves[i].x);
        int newCol = enemyCol + (int)(candidateMoves[i].y);
        // ����ͼ�߽缰ͨ����
        if (newRow < 0 || newRow >= MapRow || newCol < 0 || newCol >= MapCol)
            continue;
        if (map[newRow * MapCol + newCol] != MOVABLE_AREA_VALUE)
            continue;
        // �����ѡλ�������֮��������پ���
        int candidateManhattan = abs(playerRow - newRow) + abs(playerCol - newCol);
        double distanceCost = 0.0;
        if (candidateManhattan < currentManhattan) {
            distanceCost = 0.0;      // ʵ�������˾��룬�޶���ͷ�
        } else if (candidateManhattan == currentManhattan) {
            distanceCost = 1.0;      // ��ά�־��룬��һ��ͷ�
        } else {
            distanceCost = 100.0;    // ���Ӿ����򼫴�ͷ�
        }
        // �����ѡ�ƶ�����ĽǶȣ�ע�⣺���ں�ѡ��������ˮƽ����Ϊ candidateMoves[i].y����ֱ����Ϊ candidateMoves[i].x��
        double candAngle = atan2((double)candidateMoves[i].x, (double)candidateMoves[i].y);
        double angDiff = angle_diff(desiredAngle, candAngle);
        // ���մ��� = �ǶȲ� + ����ͷ����ټ���һ��΢С��������Դ�����ȫһ����
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
        // ����Χ�������ƶ�����ѡ��һ��Ĭ�Ϸ��򣬱��������ƶ�
        NewDir.x = 0.0f;
        NewDir.y = 1.0f;
    }

    // ���þ�����·���ִ���ƶ��Ͷ�������
    SpritesMov_SetDir(Sprites, NewDir);
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));
    return SBT_SUCCESS;
}



//[����] ���𹥻��������ȼ�Ϊ3�Ŀ��жϷ��걸֡��	
SpriteBTStatus_t SBT_ExecuteHandle_Long_Attack(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	//����Ƿ�Ϊ�״ι���
	if(data->attack.isFirstAttack == 1){
		//������״ν��빥��״̬
		
		//�л�Ϊ��������
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongAttackIndex);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Once);//�����߼�
		
		data->attack.isFirstAttack = 0;//�л�Ϊ���״ν��빥��״̬
	}
	
	
	//ִ�ж���
	if(SpritesAnim_IsEndFrm(TO_SPRITES_BASE(Sprites)) == 0){//�������û��ִ����ɣ�ִ�ж���
		SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
		return SBT_RUNNING;
	}
	
	//����ִ�н���������ĩ�ڣ�׼�����𹥻��߼�
	
	//��������
	basketball_fire(Sprites,data->camera);//���������Ƿ���ɹ�������Ϊ�������乥������
	data->attack.isFirstAttack = 1;//�л�Ϊ�״ν��빥��״̬
	//������Ϊ�������л�Ϊ�ƶ�����
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),LongMoveIndex);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);//ѭ���߼�
	return SBT_SUCCESS;
}


//[����] ������Χ�ڵ��ƶ���Ϊ
SpriteBTStatus_t SBT_ExecuteHandle_AttackRangeMoveAct(Sprites_t* Sprites, void* privateData) {
    LongPrivate_t* data = (LongPrivate_t*)privateData;
    
    // �Ƿ񴥷��ƶ������л���ÿ��1���л�һ�η���
    if(data->attack.MovementWithinAttackRangeInterval > 1.0) {
        data->attack.MovementWithinAttackRangeInterval = 0; // ˢ���ۻ�ʱ��
        vector NewDir; // �·���

        // ��ȡ���˼���ң����������λ��
        vector Sprite_Pos = data->Enemy->pos;
        vector Camera_Pos = Camera_GetPos(data->camera);

        // ��ȡ��ǰ��ͼ��Ϣ�������һ�����ϰ���⣬��������Щ��Ϣ
        int index = WorldMap_GetCurrentMapPos();
        int MapRow = WorldMap_GetROW(index);
        int MapCol = WorldMap_GetColumn(index);
        unsigned char* map = WorldMap_GetArray(index);

        /* �ı䷽����룺
         * 1. �ȼ������ҵ����˵������� diff = Sprite_Pos - Camera_Pos��
         * 2. ����������Ĵ�ֱ�����������ѡ����һ������ NewDir ��Ϊ (-diff.y, diff.x) �� (diff.y, -diff.x)��
         * 3. Ϊ�����ƶ������鶯���ټ���һ����С���ȵ�����Ŷ���
         * 4. ����һ�� NewDir��ȷ���ƶ��ٶȲ��ܷ��򳤶�Ӱ�졣
         */

        double diffX = Sprite_Pos.x - Camera_Pos.x;
        double diffY = Sprite_Pos.y - Camera_Pos.y;
        double diffLen = sqrt(diffX * diffX + diffY * diffY);

        if(diffLen < 0.0001) {
            // ��������������λ�ü����غϣ����������һ������
            NewDir.x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            NewDir.y = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        } else {
            // ���㴹ֱ���򣨺�㷽�򣩣��������ѡ��һ
            double perpX, perpY;
            if(rand() % 2) {
                perpX = -diffY;
                perpY = diffX;
            } else {
                perpX = diffY;
                perpY = -diffX;
            }
            // ��������Ŷ����Ŷ����ȿɸ���ʵ�����������
            double noiseScale = 0.3;  // �����Ŷ�ϵ��
            double noiseX = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * noiseScale;
            double noiseY = (((double)rand() / RAND_MAX) * 2.0 - 1.0) * noiseScale;

            NewDir.x = perpX + noiseX;
            NewDir.y = perpY + noiseY;
        }

        // �����·���
        SpritesMov_SetDir(Sprites, NewDir);
    }
    
    // ����ʱ���ۻ�����λ���룩
    data->attack.MovementWithinAttackRangeInterval += FrmRS_deltaTime_Get();
    
    // ����λ���붯����ʾ
    SpritesMov_Forward(Sprites);
    SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
    data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));

    return SBT_SUCCESS;
}


/*��������*/
//[����] �ж�����Ƿ��ھ��鹥����Χ��
SpriteBTStatus_t SBT_ExecuteHandle_IsPlayerInAttackRange(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	
	vector e_pos = data->Enemy->pos;//��ȡ����λ��
	vector p_pos = Camera_GetPos(data->camera);//��ȡ���λ��
	
	double x = e_pos.x - p_pos.x;
	double y = e_pos.y - p_pos.y;
	
	double dis = sqrt(x*x+y*y);//��ȡ���������λ��
	if(dis >= 3){
		//���������ڵ���3�ף�����Ϊû���ڹ�����Χ��
		return SBT_FAILURE;
	}
	return SBT_SUCCESS;
}

//[����] �жϹ�����ȴʱ���Ƿ����
SpriteBTStatus_t SBT_ExecuteHandle_IsAttackCooldownOver(Sprites_t* Sprites,void* privateData){
	LongPrivate_t* data = (LongPrivate_t*)privateData;
	if(data->attack.AttackInterval > 3.0){
		//����������ʱ�䳬��3�룬��ζ�ſ��Թ�������������ȴ����
		data->attack.AttackInterval = 0;//������ȴ
		return SBT_SUCCESS;
	}
	//��ȴʱ��û�н�������ȡ��ǰ��֡ʱ�䣬������ȴʱ�����
	data->attack.AttackInterval += FrmRS_deltaTime_Get();
	return SBT_FAILURE;
}

//����Զ�̹����߼�
int Long_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	/*
		��Ϊ��
			 [ѡ��]
				 |--[˳��](�߼�������)
				 |		|--[����] �ж�����Ƿ��ھ��鹥����Χ��
				 |		|--[����] �жϹ�����ȴʱ���Ƿ����
				 |		|--[����] ���𹥻��������ȼ�Ϊ3�Ŀ��жϷ��걸֡��	
				 |--[˳��](�߼���������Χ�ڵ��˶�)
				 |		|--[����] �ж�����Ƿ��ھ��鹥����Χ��
				 |		|--[����] ������Χ�ڵ��ƶ���Ϊ���걸֡��		
				 |--[����] ������ƶ����걸֡��

		�жϣ�
			���������ȼ���Ϊ10 ������ȡ����
			�ܻ������ȼ���Ϊ5  ����ȡ����
	*/
	int result = 0;
	/*��ͨ��Ϊ�� ��û��������*/
	
	
	/******************
		�������
	*******************/
	//�������ж��Ƿ��ڹ�����Χ��
	SpriteBTNode_t* IsPlayerInAttackRange = SpritesBT_Condition(SBT_ExecuteHandle_IsPlayerInAttackRange);
	//�������жϹ�����ȴʱ���Ƿ����
	SpriteBTNode_t* IsAttackCooldownOver = SpritesBT_Condition(SBT_ExecuteHandle_IsAttackCooldownOver);
	
	
	/******************
		�������
	*******************/
	//���������𹥻��������ȼ�Ϊ3�Ŀ��жϷ��걸֡����Ϊ���λ�õ����������ԣ�	
	SpriteBTNode_t* AttackAct = SpritesBT_Action(SBT_ExecuteHandle_Long_Attack);
	//[����] ������ƶ�
	SpriteBTNode_t* MoveAct = SpritesBT_Action(SBT_ExecuteHandle_Long_Move);
	//[����] ������Χ�ڵ��ƶ���Ϊ
	SpriteBTNode_t* AttackRangeMoveAct = SpritesBT_Action(SBT_ExecuteHandle_AttackRangeMoveAct);
	
	/******************
		��Ϊ���
	*******************/
	//(�߼�������)
	SpriteBTNode_t* AttackContainer = SpritesBT_Sequence();//��������
	result |= SpritesBT_InsertBehavior(AttackContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackContainer, IsAttackCooldownOver);
	//���ö�������
	result |= SpritesBT_Action_ToPhased(AttackAct,SIT_AllowSubstitution,3);//���ù�������Ϊ�������ȼ�Ϊ3�Ŀ��жϷ��걸֡
	result |= SpritesBT_InsertBehavior(AttackContainer, AttackAct);
	if(result != 0){
		return -1;//(�߼�������)����ʧ��
	}
	
	//[˳��](�߼���������Χ�ڵ��˶�)
	SpriteBTNode_t* AttackMoveContainer = SpritesBT_Sequence();//��������
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, IsPlayerInAttackRange);
	result |= SpritesBT_InsertBehavior(AttackMoveContainer, AttackRangeMoveAct);
	if(result != 0){
		return -1;//(�߼���������Χ�ڵ��˶�)����ʧ��
	}
	
	//����Ϊװ��
	SpriteBTNode_t* SelectContainer = SpritesBT_SelectorCreate();
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackContainer);//�����߼�װ��
	result |= SpritesBT_InsertBehavior(SelectContainer,AttackMoveContainer); //������Χ�ڵ��˶�
	result |= SpritesBT_InsertBehavior(SelectContainer,MoveAct);//�ƶ��߼�װ��
	
	//���ظ��ڵ�
	result |= SpritesBT_MountToRoot(BTTreeIndex, SelectContainer);
	if(result != 0){
		return -1;//����Ϊ����ʧ��
	}
	
	
	/*�жϱ�*/
	//ע���
	result |= SpritesIT_Register(BTTreeIndex,SIT_MapIndexHandle_Long);
	//���������Ϊ�ڵ�
	result |= SpritesIT_Add(BTTreeIndex ,1,10,SIT_ProhibitSubstitution,SBT_ExecuteHandle_Long_Die);
	//����ܻ���Ϊ�ڵ�
	result |= SpritesIT_Add(BTTreeIndex ,2,5,SIT_AllowSubstitution,SBT_ExecuteHandle_Long_Hit);
	if(result != 0){
		return -1;
	}
	
	
	
	return 0;
}


//Զ�̵���˽�����ݳ�ʼ��
void LongPri_Init(Camera_Struct* camera,Enemy_t* Enemy){
	LongPrivate_t *data = c_malloc(sizeof(LongPrivate_t));
	data->camera = camera;
	data->Enemy = Enemy;
	data->HP = ENEMY_HP_LONG;//��ʼѪ
	//�ܻ���
	data->hit.isHit = 0;//Ĭ��û���ܻ�
	data->hit.isFirstHit = 1;//Ĭ���״��ܻ�
	//������
	data->attack.AttackInterval = 0;//��������ۻ�ʱ��
	data->attack.isFirstAttack = 1;//Ĭ������Ϊ�״ι���
	data->attack.MovementWithinAttackRangeInterval = 0;//������Χ�ڵ��ƶ������ۻ�ʱ��

	SpritesPriv_Add(Enemy->sprite,data,PrifreeHandle);
}
/**
  * @brief  Զ�̵��˳�ʼ��
  *
  * @param	basketballCount	��������������
  *
  * @retval �����ɹ�����0������ʧ�ܷ���-1
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
	//��������
	temp |= Long_Anim_Create(AnimManagerIndex);
	//�߼�����
	temp |= Long_Logic_Create(BTTreeIndex);
	if(temp != 0){
		return -1;
	}
	
	//��ʼ���ڵ�(Ĭ�����е�Զ�̵������ߣ���Ҫ��������)
	for(int i = 0;i<enemyCount;i++){
		EnemyHead->LongRangeAttack_head[i].sprite = Sprites_ObjectCreate(AnimManagerIndex,0,0,LongMoveIndex,moveSpeed,BTTreeIndex);
		EnemyHead->LongRangeAttack_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite));
		EnemyHead->LongRangeAttack_head[i].EnemyStatus.isSleep = 1;
		EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable = 1;
		
		//��ǰ����Ϊ�ƶ�����������Ϊѭ������
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite),Anim_Loop);
		
		SpritesIT_Enable(EnemyHead->LongRangeAttack_head[i].sprite);//�����ж�
		Sprites_Sleep(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite));//����
		
		//˽�����ݳ�ʼ��
		LongPri_Init(camera,&EnemyHead->LongRangeAttack_head[i]);
	}
	return 0;
}


//����һ��Զ�̹����ĵ���
int LongEnemy_Create(void){
	if(EnemyHead == NULL){
		return -1;
	}
	for(int i = 0;i<EnemyHead->LongRangeAttack_remaining;i++){
		if(EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable == 1){
			//�ҵ��˿��õľ���
			EnemyHead->LongRangeAttack_head[i].EnemyStatus.isSleep = 0;
			EnemyHead->LongRangeAttack_head[i].EnemyStatus.usable = 0;
			
			//����
			Sprites_Awaken(TO_SPRITES_BASE(EnemyHead->LongRangeAttack_head[i].sprite));
			//SpritesMov_SetPos(EnemyHead->LongRangeAttack_head[i].sprite,1,1);
			//SpritesMov_SetPos(EnemyHead->LongRangeAttack_head[i].sprite,19,12);//����
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
	���������á��������
***************************************/

int basketballindex;
//������������ﶯ��
int basketball_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//��������������
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
		//���򶯻�
	basketballindex = SpritesAnim_CreateNewChain(AnimManagerIndex,24,24,BIT_SPRITES,1.0);
	if(basketballindex < 0){
		return -1;
	}
	int temp = 0;
	//���򶯻�֡
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,basketballindex,0,basketball_24x24,basketball_24x24_shadown);
	if(temp != 0){
		return -1;
	}
	return 0;
}


SpriteBTStatus_t SBT_ExecuteHandle_basketball(Sprites_t* Sprites,void* privateData){
	BasketballPrivate_t *data = (BasketballPrivate_t *)privateData;//��ȡ˽������
	data->Enemy->EnemyStatus.usable = 0;//����Ϊ������״̬
	//ǰ��һ��֡����
	SpritesMov_Forward(Sprites);
	data->Enemy->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));//��������
	
	//����Ƿ񹥻��������
	vector pos = Camera_GetPos(data->camera);
	double x = pos.x - data->Enemy->pos.x;
	double y = pos.y - data->Enemy->pos.y;
	double dis = sqrt(x*x+y*y);
	if(dis < 0.15){
		//����ӵ�����Ҿ������0.15����ζ����ұ�����
		int * Health =  (int*)CameraPriv_Get(data->camera);
		*Health -= 1;//��Ҽ�һ��Ѫ
		//��������
		data->Enemy->EnemyStatus.isSleep = 1;
		data->Enemy->EnemyStatus.usable = 1;
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));//��������
		return SBT_SUCCESS;
	}
	
	//����Ƿ�����ǽ
	if(Sprites_collisionWallValue(Sprites) != 0){
		//����ǽ��
		data->Enemy->EnemyStatus.isSleep = 1;
		data->Enemy->EnemyStatus.usable = 1;
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));//��������
		return SBT_SUCCESS;
	}
	//����
	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}

//���������˶��߼�
int basketball_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	SpriteBTNode_t* act = SpritesBT_Action(SBT_ExecuteHandle_basketball);
	SpritesBT_MountToRoot(BTTreeIndex, act);
	return 0;
}



/**
  * @brief  �����ʼ��
  *
  * @param	basketballCount	��������������
  *
  * @retval �����ɹ�����0������ʧ�ܷ���-1
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
	//��������
	temp |= basketball_Anim_Create(AnimManagerIndex);
	//�߼�����
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
		
		//��������
		Sprites_Sleep(TO_SPRITES_BASE(EnemyHead->basketball_head[i].sprite));
		
		//����˽������
		BasketballPrivate_t *data = c_malloc(sizeof(BasketballPrivate_t));
		data->Enemy = &EnemyHead->basketball_head[i];
		data->camera = camera;
		SpritesPriv_Add(EnemyHead->basketball_head[i].sprite,data,PrifreeHandle);
	}
	return 0;
}
/**
  * @brief  ��������
  *
  * @param	basketballCount	��������������
  *
  * @retval �����ɹ�����0������ʧ�ܷ���-1
  */
int basketball_fire(Sprites_t* sprite,Camera_Struct* camera){
	if(sprite == NULL || camera == NULL || EnemyHead == NULL){
		return -1;
	}
	if(EnemyHead->basketball_remaining == 0){
		return -1;//û�п��õ�����
	}
	for(int i = 0;i<EnemyHead->basketball_totality;i++){
		if(EnemyHead->basketball_head[i].EnemyStatus.usable == 1){
			//�ҵ�һ�����õ�����
			
			
			//���������λ��
			SpritesMov_SetPosToSprite(EnemyHead->basketball_head[i].sprite,sprite);
			//��������ķ���ָ������ͷ
			SpritesMov_SetDirToCamera(EnemyHead->basketball_head[i].sprite,camera);
		
			//����
			Sprites_Awaken(TO_SPRITES_BASE(EnemyHead->basketball_head[i].sprite));
			EnemyHead->basketball_head[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(EnemyHead->basketball_head[i].sprite));
			EnemyHead->basketball_head[i].EnemyStatus.isSleep = 0;
			EnemyHead->basketball_head[i].EnemyStatus.usable = 0;
			
			return 0;
		}
	}
	return -1;
}















///////���⺯��API
//��ȡ��ײ�ĵ���
Sprites_t* GetEnemy(vector Bullent_pos) {
    if (EnemyHead == NULL) {
        return NULL;
    }
    
    const float threshold = 0.1f;
    float minDist = threshold;
    Enemy_t* result = NULL;
    int isLongRange = 0; //�������ͱ�־���Ƿ�ΪԶ������ˣ�
    
    // �������̹����ĵ���
    for (int i = 0; i < EnemyHead->CloseRangeAttack_totality; i++) {
        Enemy_t* enemy = &EnemyHead->CloseRangeAttack_head[i];
        
        // �������ڲ�����״̬
        if (enemy->EnemyStatus.usable == 1) {
            continue;
        }
        
        // ����ŷʽ����
        float dx = enemy->pos.x - Bullent_pos.x;
        float dy = enemy->pos.y - Bullent_pos.y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        // ������С����ֵ���ұȵ�ǰ��¼����С������̵�ʱ�򣬸��½��
        if (distance < threshold && distance < minDist) {
            minDist = distance;
            result = enemy;
            isLongRange = 0; // Mark as close-range enemy
        }
    }
    
    // ����Զ�̹����ĵ���
    for (int i = 0; i < EnemyHead->LongRangeAttack_totality; i++) {
        Enemy_t* enemy = &EnemyHead->LongRangeAttack_head[i];
		
        // �������ڲ�����״̬
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
    
    // ����Ƿ��ҵ��˷���Ҫ��ĵ���
    if (result != NULL) {
        if (isLongRange) {
            LongPrivate_t* data = SpritesPriv_Get(result->sprite);
            data->hit.isHit = 1; // ����Ϊ�ܻ�
        } else {
            ClosePrivate_t* data = SpritesPriv_Get(result->sprite);
            data->hit.isHit = 1; // ����Ϊ�ܻ�
        }
    }
    
    return result ? result->sprite : NULL;
}

