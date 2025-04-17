#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "Common.h"

typedef struct EnemyHead_t EnemyHead_t;
typedef struct EnemyStatus_t EnemyStatus_t;
typedef struct Enemy_t Enemy_t;


struct EnemyHead_t{
	//���̵���
	int CloseRangeAttack_totality;//�����Ľ��̵��˵�����
	int CloseRangeAttack_remaining;//��ǰ���õĽ��̵��˵�����
	Enemy_t* CloseRangeAttack_head;//���̹�������
	
	
	//Զ�̵���
	int LongRangeAttack_totality;//�����Ľ��̵��˵�����
	int LongRangeAttack_remaining;//��ǰ���õĽ��̵��˵�����
	Enemy_t* LongRangeAttack_head;//Զ�̹�������
	
	
	//���������
	int basketball_totality;//������������������
	int basketball_remaining;//��ǰ���õ���������������
	Enemy_t* basketball_head;//���������
};

struct EnemyStatus_t{
	unsigned int isSleep:1;//�Ƿ��������� 
	unsigned int usable :1;//�Ƿ��ʹ��
};

struct Enemy_t{
	EnemyStatus_t EnemyStatus;//����״̬ 
	vector pos;//����λ��
	Sprites_t* sprite;//���˾��鱾��
};



//˽������

//����˽������
typedef struct BasketballPrivate_t{
	struct Enemy_t* Enemy;//�ӵ�����
	Camera_Struct* camera;//�����
}BasketballPrivate_t;

//Զ�̵���˽������
typedef struct LongPrivate_t{
	struct Enemy_t* Enemy;//��������
	Camera_Struct* camera;//�����
	int HP;//����ֵ
	
	struct{
		int isHit;//�Ƿ��ܻ� 0Ϊû���ܻ���1Ϊ�ܻ�
		int isFirstHit;//�Ƿ��״��ܻ���1Ϊ�״Σ�0Ϊ���״�
	}hit;//�ܻ���
	
	struct{
		double AttackInterval;//�������
		int isFirstAttack;//�Ƿ����״ι�����1Ϊ�״Σ�0Ϊ�״�
		
		double MovementWithinAttackRangeInterval;//������Χ�ڵ��ƶ�ʱ����
	}attack;
	
	struct{
		
		double action;//����һ����Ϊ���ۻ�ʱ�� 
	}CumulativeTime;//�����ۻ�ʱ������
}LongPrivate_t;

//���̵���˽������
typedef struct ClosePrivate_t{
	struct Enemy_t* Enemy;//��������
	Camera_Struct* camera;//�����
	int HP;
	
	struct{
		double AttackInterval;//�������
		
		double MovementWithinAttackRangeInterval;//������Χ�ڵ��ƶ�ʱ����
	}attack;
	
	struct{
		int isHit;//�Ƿ��ܻ� 0Ϊû���ܻ���1Ϊ�ܻ�
		int isFirstHit;//�Ƿ��״��ܻ���1Ϊ�״Σ�0Ϊ���״�
	}hit;//�ܻ���
		
	
}ClosePrivate_t;


int LongEnemy_Init(int enemyCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed);
int LongEnemy_Create(void);

int CloseEnemy_Init(int enemyCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed);
int CloseEnemy_Create(void);

int Basketball_Init(int basketballCount,Camera_Struct* camera,int AnimManagerIndex,int BTTreeIndex,double moveSpeed);
int basketball_fire(Sprites_t* sprite,Camera_Struct* camera);

void Minimap(Camera_Struct* camera);
#endif
