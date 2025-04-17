#include "pistol.h"


bulletHead_t* bulletArr = NULL;

int indexNormalBullet;//��ͨ�ӵ�
int indexClearBullet;//͸���ӵ������ڶ̾��벻��ʾ�ӵ����ã�



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
	���ӵ��������߼���������
***************************************/

//�����ӵ�����
int bullet_Anim_Create(int AnimManagerIndex){
	if(AnimManagerIndex < 0)	return -1;
	//��������������
	if(SpritesAnim_CreateManager(AnimManagerIndex) != 0)	return -1;
	//����������
	indexNormalBullet = SpritesAnim_CreateNewChain(AnimManagerIndex,8,8,BIT_SPRITES,0.5);
	indexClearBullet = SpritesAnim_CreateNewChain(AnimManagerIndex,8,8,BIT_SPRITES,0.5);
	if(indexNormalBullet < 0 || indexClearBullet < 0){
		return -1;
	}
	int temp = SpritesAnim_InsertNewFrm(AnimManagerIndex,indexNormalBullet,0,bullet_8x8,bullet_8x8);//��ͨ
	temp |= SpritesAnim_InsertNewFrm(AnimManagerIndex,indexClearBullet,0,bulletBlank_8x8,bulletBlank_8x8);//͸���ӵ�
	if(temp != 0){
		return -1;
	}
	return 0;
}


SpriteBTStatus_t SBT_ExecuteHandle_bullet(Sprites_t* Sprites,void* privateData){
	bulletPrivate_t *data = (bulletPrivate_t *)privateData;//��ȡ˽������
	//ǰ��һ��֡����
	SpritesMov_Forward(Sprites);
	data->bullet->pos = SpritesData_GetPos(TO_SPRITES_BASE(Sprites));//��������
	
	//��ȡ�ӵ�����ҵľ���
	vector pos = Camera_GetPos(data->camera);
	double x = data->bullet->pos.x - pos.x;
	double y = data->bullet->pos.y - pos.y;
	double dis = sqrt(x*x + y*y);
	//�Ƿ���ʾ�ӵ�
	if(data->isShow == 0){
		//����ʾ�ж��ӵ�����ҵľ���

		if(dis > 0.5){
			//�ӵ�����0.5m��ͻ�����ʾ״̬
			
			//�л�������
			SpritesAnim_SwitchChain(TO_SPRITES_BASE(Sprites),indexNormalBullet);
			SpritesAnim_SetAnimType(TO_SPRITES_BASE(Sprites),Anim_Loop);
			//����Ϊ��ʾ״̬
			data->isShow = 1;
		}
	}
	
	//�ж��ӵ��Ƿ���е���
	Sprites_t* enemy = GetEnemy(data->bullet->pos);
	if(enemy != NULL){
		
		//�ӵ��������״̬�ı�
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));//�����ӵ�
		

		bullet_t* tempNode = data->bullet;

		// ������̬�������루�Ľ��汾����
		if (tempNode->previous != NULL) {//��ͷ�ڵ�
			tempNode->previous->next = tempNode->next;
		} else {
			// �����ǰ�ڵ���ͷ�ڵ㣬����ͷָ��Ϊ��һ���ڵ�
			bulletArr->runningLink = tempNode->next;
		}
		if (tempNode->next != NULL) {
			tempNode->next->previous = tempNode->previous;
		}
		bulletArr->runningCount--;

		//��ӵ�����������
		tempNode->next = bulletArr->usableLink;
		tempNode->previous = NULL; // ��������ͷ�ڵ�� previous ����Ϊ NULL
		if (bulletArr->usableLink != NULL) {
			bulletArr->usableLink->previous = tempNode; // ��ͷ�ڵ�� previous ָ���½ڵ�
		}
		bulletArr->usableLink = tempNode; // �ؼ��޸�����������ͷָ��
		bulletArr->usableCount++;
		
		data->isShow = 0;//����Ϊ͸��״̬
		return SBT_SUCCESS;
	}
	
	//�ж��ӵ��Ƿ�ײ��ǽ�������ӵ��Ƿ񳬹�����Ϊ6�����
	if(Sprites_collisionWallValue(Sprites) != 0 || dis >= 6.0){
		Sprites_Sleep(TO_SPRITES_BASE(Sprites));
		
		bullet_t* tempNode = data->bullet;
		// ������̬�������루�Ľ��汾����
		if (tempNode->previous != NULL) {
			tempNode->previous->next = tempNode->next;
		} else {
			// �����ǰ�ڵ���ͷ�ڵ㣬����ͷָ��Ϊ��һ���ڵ�
			bulletArr->runningLink = tempNode->next;
		}
		if (tempNode->next != NULL) {
			tempNode->next->previous = tempNode->previous;
		}
		bulletArr->runningCount--;
		//��ӵ�����������
		tempNode->next = bulletArr->usableLink;
		tempNode->previous = NULL; // ��������ͷ�ڵ�� previous ����Ϊ NULL
		if (bulletArr->usableLink != NULL) {
			bulletArr->usableLink->previous = tempNode; // ��ȷ����ͷ�ڵ�� previous ָ���½ڵ�
		}
		bulletArr->usableLink = tempNode; // �ؼ��޸�����������ͷָ��
		bulletArr->usableCount++;
				
		data->isShow = 0;//����Ϊ͸��״̬
		return SBT_SUCCESS;
	}
	//����
	SpritesAnim_Update(TO_SPRITES_BASE(Sprites));
	return SBT_SUCCESS;
}

//�����ӵ��߼���
int bullet_Logic_Create(int BTTreeIndex){
	if(SpritesBT_Init(BTTreeIndex) != 0){
		return -1;
	}
	//��Ϊ������
	/*��������1:
		�ӵ��Ծ���/ǽ����ײ����
	*/
	SpriteBTNode_t* act = SpritesBT_Action(SBT_ExecuteHandle_bullet);
	SpritesBT_MountToRoot(BTTreeIndex, act);
	return 0;
}

/**************************************
	���ӵ�API��
***************************************/
//ȫ�ֲ���
int PrivAnimManagerIndex;
double PrivSpeed;
int PrivBTTreeIndex;
/**
  * @brief  ��ǹ��ʼ����Ԥ�����ӵ�����������
  *
  * @param  count����(С��0Ϊ���������ӵ��������ʹ��)
  *
  * @retval �����ɹ�����0������ʧ�ܷ���-1
  */
int pistol_Init(int count,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double Speed){
	if(count == 0 || bulletArr != NULL)	return -1;

	//����Ԥ��������
	bulletArr = (bulletHead_t*)c_malloc(sizeof(bulletHead_t));
	if(bulletArr == NULL){
		return -1;
	}
	bulletArr->MaxCount = count;
	bulletArr->runningCount = 0;
	bulletArr->usableCount = 0;
	bulletArr->runningLink = NULL;
	bulletArr->usableLink = NULL;
	
	int ArrCount;//Ԥ��������������
	if(count > 0){//����0��ζ�Ŵ������������
			ArrCount = count;
	}else{//���Ϊ��������ô�ȴ���7�����������������
		ArrCount = 7;
	}
	
	bullet_t* tempArr = (bullet_t*)c_malloc(sizeof(bullet_t)*ArrCount);
	if(tempArr == NULL){
		c_free(bulletArr);
		bulletArr = NULL;
		return -1;//����ʧ��
	}
	
	//׼����ʼ������
	int temp = 0;
	//�����ӵ���������
	temp |= bullet_Anim_Create(AnimManagerIndex);
	//�����ӵ��߼�
	temp |= bullet_Logic_Create(BTTreeIndex);
	if(temp != 0){
		c_free(tempArr);
		c_free(bulletArr);
		bulletArr = NULL;
		return -1;
	}
	/*
	����һЩȫ�ֲ���
	*/
	PrivAnimManagerIndex = AnimManagerIndex;
	PrivSpeed = Speed;
	PrivBTTreeIndex = BTTreeIndex;
	//�Ե�ǰ��������л����ĳ�ʼ������
	bullet_t* probe = NULL;
	for(int i = 0;i<ArrCount;i++){
		tempArr[i].sprite = Sprites_ObjectCreate(AnimManagerIndex,0,0,indexNormalBullet,Speed,BTTreeIndex);
		tempArr[i].pos = SpritesData_GetPos(TO_SPRITES_BASE(tempArr[i].sprite));//���鴴��ʧ�ܣ�����ᱻ�˴���ס
		
		//����˽������
		bulletPrivate_t *data = c_malloc(sizeof(bulletPrivate_t));
		data->bullet = &tempArr[i];
		data->camera = camera;
		data->isShow = 0;//Ĭ�ϲ���ʾ
		SpritesPriv_Add(tempArr[i].sprite,data,bullet_PrivateFreeHandle);
		
		//���ó�ʼ��������״̬
		SpritesAnim_SwitchChain(TO_SPRITES_BASE(tempArr[i].sprite),indexClearBullet);
		SpritesAnim_SetAnimType(TO_SPRITES_BASE(tempArr[i].sprite),Anim_Loop);
		
		//��ʹ�þ���������
		bulletArr->usableCount++;
		
		tempArr[i].next = NULL;
		
		//��������
		if(i != 0){//�����ǰ��Ϊ�����׽ڵ�
			probe->next = &tempArr[i];
			tempArr[i].previous = probe;
		}else {
			// �׸��ڵ���Ҫ��������ͷ
			bulletArr->usableLink = &tempArr[i]; 
			tempArr[i].previous = NULL;
		}
		probe = &tempArr[i];
		
		//����
		Sprites_Sleep(TO_SPRITES_BASE(tempArr[i].sprite));
	}		
	
	return 0;
}


/**
  * @brief  ��ǹ����û�����ö��㶯��ֻ��ע���ӵ���
  *
  * @param  camera	�����
  *
  * @retval 
  *		����ɹ�����0��
  *		����ʧ�ܷ���-1����ʼ���������⡢��ǹ�ӵ���ʹ���ӵ�Ϊ0��
  */
int pistol_Fire(Camera_Struct* camera){
	if(camera == NULL || bulletArr == NULL){
		return -1;
	}
	
	bullet_t* target = NULL; 
	//����Ƿ��п����ӵ�����
	if(bulletArr->usableCount != 0){//����п����ӵ�
		target = bulletArr->usableLink;
		bulletArr->usableLink = bulletArr->usableLink->next;
		if(bulletArr->usableLink != NULL){
			bulletArr->usableLink->previous = NULL;
		}
		bulletArr->usableCount--;
		
	}
	else{//��������ڿ����ӵ�
		//����Ƿ������������ӵ�����
		if(bulletArr->MaxCount >= 0){//û�иò���
			return -1;//���췦��
		}
		target = (bullet_t*)c_malloc(sizeof(bullet_t));
		target->next = NULL;       // ����
		target->previous = NULL;   // ����
		if(target == NULL){
			return -1;
		}
		//��������
		target->sprite = Sprites_ObjectCreate(PrivAnimManagerIndex,0,0,indexNormalBullet,PrivSpeed,PrivBTTreeIndex);
		if(target->sprite == NULL){
			c_free(target);
			return -1;//����ʧ��
		}
		target->pos = SpritesData_GetPos(TO_SPRITES_BASE(target->sprite));
		
		//����˽������
		bulletPrivate_t *data = c_malloc(sizeof(bulletPrivate_t));
		data->bullet = target;
		data->camera = camera;
		SpritesPriv_Add(target->sprite,data,bullet_PrivateFreeHandle);
		
	}
	
	
	// �޸ĺ������̬�������
	target->previous = NULL;
	target->next = bulletArr->runningLink;
	if (bulletArr->runningLink != NULL) {
		bulletArr->runningLink->previous = target;
	}
	bulletArr->runningLink = target; // ��������������ͷָ��
	bulletArr->runningCount++;
	
	
	//�����ӵ�����
	Sprites_Awaken(TO_SPRITES_BASE(target->sprite));
	//λ���뷽��
	SpritesMov_SetPosToCamera(target->sprite,camera);
	SpritesMov_SetDirWithCamera(target->sprite,camera);
	//����
	SpritesAnim_SwitchChain(TO_SPRITES_BASE(target->sprite),indexClearBullet);
	SpritesAnim_SetAnimType(TO_SPRITES_BASE(target->sprite),Anim_Loop);
	bulletPrivate_t *data = (bulletPrivate_t*)SpritesPriv_Get(target->sprite);
	data->isShow = 0;//����Ϊ͸��̬
	
	return 0;//û�п����ӵ�
}


