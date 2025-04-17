/**
  ******************************************************************************
  * @file    Sprites_BehaviorTree.c
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    2025-3-31
  * @brief   ���ļ������˿⾫����Ϊ���йصĺ���ʵ�֣�������ͨ��Ϊ�����жϱ�
  ******************************************************************************
  */
#include "Sprites_internal.h"
#include "engine_config.h"
#include "bit_operate.h"
#include "Camera_internal.h"//������ڲ��ӿ�
#include "WorldMap_internal.h"//�����ͼ�ڲ��ӿ�
#include "Canvas.h"		//�����ڲ��ӿ�
#include "EngineFault.h"//���������
#include "FrameRateStrategy_internal.h"//֡���Ż�

#include <string.h>
#include <stdlib.h>

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_0          //���ñ�׼��ѽ������
#include <malloc.h>
#define C_NULL NULL
#endif	//���ñ�׼��ѽ������

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //�������涯̬�ڴ�������
#include "heap_solution_1.h"

//�궨��ͳһ�����ӿ�
#define malloc(size) c_malloc(size)
#define realloc(ptr,size) c_realloc(ptr,size)
#define free(ptr)	c_free(ptr)
#endif	//�������涯̬�ڴ�������



/**
	1������һ��ͳһ�ĺ�������ڹ������е���Ϊ��

**/

/**
	*@breif ������Ϊ���Ĺ����������������ÿ���ڵ����һ�����͵���Ϊ��
	*/
static RB_Node* Sprites_BT_ManagerTree = C_NULL;	




/******************************

	������Ϊ��

*******************************/

/**
*		@note	�������������������Ϊ�����������һ����Ϊ������
*	@brief	����һ����Ϊ��
*	@param
*		@arg	value	������Ϊ����ţ�����Ҫ����0
*	@retval
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����1����ζ�Ÿ�ֵ�Ѿ�����
*		@arg	������ؽ����-1����ζ���ڴ����ʧ�ܻ��߲�������
*		@arg	������ؽ����-2����ζ����Ϊ����ʼ��ʧ��
*/
int SpritesBT_Init(int value) {
	if (value <= 0)	return -1;
	//�����Լ��
	void* temp = search(Sprites_BT_ManagerTree, value);
	if (temp != NULL)	return 1;

	//������Ϊ������
	SpriteBehaviorManger_t* SpriteBehaviorManger = malloc(sizeof(SpriteBehaviorManger_t));
	if (SpriteBehaviorManger == NULL) {
		return -1;
	}
	SpriteBehaviorManger->root = NULL;
	SpriteBehaviorManger->SpriteITManager = NULL;
	//����������
	int result = rbInsert(&Sprites_BT_ManagerTree, value, SpriteBehaviorManger);
	if (result != 0) {
		free(SpriteBehaviorManger);
	}
	return result;
}





/**
*	@brief	��������ʼ����װ���������ڵ�
*	@param
*		none
*	@retval
*		@arg	���ִ�гɹ������ؽڵ�
*		@arg  ����ռ�����ʧ�ܣ����ؿ�
*/

SpriteBTNode_t* Sprites_BT_ECNodeCreate(void) {
	//�ռ�����
	SpriteBTNode_t* node = (SpriteBTNode_t*)malloc(sizeof(SpriteBTNode_t) + sizeof(EncapsulatedContainer_t));
	if (node == NULL) {
		return NULL;
	}
	//��ȡEC�ռ��̽��
	EncapsulatedContainer_t* ECprobe = (EncapsulatedContainer_t*)(((char*)node) + sizeof(SpriteBTNode_t));
	node->ContainerType.EncapsulatedContainer = ECprobe;
	ECprobe->childCount = 0;
	ECprobe->maxChildren = SpriteBTNode_MAX_COUNT;
	ECprobe->childType = BT_STATIC;
	for (int i = 0; i < SpriteBTNode_MAX_COUNT; i++) {
		ECprobe->children.fixedChildren[i] = NULL;
	}
	return node;
}

/**
*	@brief	��������ʼ��Ҷ�����������ڵ�
*	@param
*		none
*	@retval
*		@arg	���ִ�гɹ������ؽڵ�
*		@arg  ����ռ�����ʧ�ܣ����ؿ�
*/

SpriteBTNode_t* Sprites_BT_LCNodeCreate(void) {
	//�ռ�����
	SpriteBTNode_t* node = (SpriteBTNode_t*)malloc(sizeof(SpriteBTNode_t) + sizeof(LeafContainer_t));
	if (node == NULL) {
		return NULL;
	}
	LeafContainer_t* LCprobe = (LeafContainer_t*)(((char*)node) + sizeof(SpriteBTNode_t));
	node->ContainerType.LeafContainer = LCprobe;
	LCprobe->SBT_ExecuteHandle = NULL;
	return node;
}

/**
*	@brief	ѡ������������
*	@param
*		none
*	@retval
*		@arg	���ִ�гɹ������������ĵ�ַ�������ַ���ܽ����κβ�����������Ϊ��һ��������Ψһ��ʾ��
*		@arg  ���ִ��ʧ�ܣ����ؿ�
*/
SpriteBTNode_t* SpritesBT_SelectorCreate(void) {
	SpriteBTNode_t* node = Sprites_BT_ECNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//�޸Ľڵ�����
	node->BTNodeType = SBT_Selector;
	return node;
}


/**
*	@brief	˳������������
*	@param
*		none
*	@retval
*		@arg	���ִ�гɹ������������ĵ�ַ�������ַ���ܽ����κβ�����������Ϊ��һ��������Ψһ��ʾ��
*		@arg  ���ִ��ʧ�ܣ����ؿ�
*/
SpriteBTNode_t* SpritesBT_Sequence(void) {
	SpriteBTNode_t* node = Sprites_BT_ECNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//�޸Ľڵ�����
	node->BTNodeType = SBT_Sequence;
	return node;
}

/**
*	@brief	�������ڵ㴴��
*	@param
*		@arg  SBT_ExecuteHandle �߼��жϺ���
*	@retval
*		@arg	���ִ�гɹ������������ĵ�ַ�������ַ���ܽ����κβ�����������Ϊ��һ��������Ψһ��ʾ��
*		@arg  ���ִ��ʧ�ܣ����ؿ�
*/
SpriteBTNode_t* SpritesBT_Condition(SBT_ExecuteHandle_t SBT_ExecuteHandle) {
	//�������
	if (SBT_ExecuteHandle == NULL)	return NULL;

	SpriteBTNode_t* node = Sprites_BT_LCNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//�޸Ľڵ�����
	node->BTNodeType = SBT_Condition;
	node->ContainerType.LeafContainer->SBT_ExecuteHandle = SBT_ExecuteHandle;
	return node;
}

/**
*	@brief	ִ�����ڵ㴴����Ĭ�ϴ����걸֡��
*	@param
*		@arg  SBT_ExecuteHandle �߼�ִ�к���
*	@retval
*		@arg	���ִ�гɹ������������ĵ�ַ�������ַ���ܽ����κβ�����������Ϊ��һ��������Ψһ��ʾ��
*		@arg  ���ִ��ʧ�ܣ����ؿ�
*/
SpriteBTNode_t* SpritesBT_Action(SBT_ExecuteHandle_t SBT_ExecuteHandle) {
	//�������
	if (SBT_ExecuteHandle == NULL)	return NULL;

	SpriteBTNode_t* node = Sprites_BT_LCNodeCreate();
	if (node == NULL) {
		return NULL;
	}
	//�޸Ľڵ�����
	node->BTNodeType = SBT_Action;
	node->ContainerType.LeafContainer->SBT_ExecuteHandle = SBT_ExecuteHandle;
	//�걸֡��ʼ��
	node->ContainerType.LeafContainer->SpriteActionType = SAT_STEADY;
	return node;
}

/**
*	@brief	��һ���걸���ԵĶ����ڵ�ת��Ϊ���걸����
*	@param
*		@arg  node �����ڵ��ַ
*		@arg	SpriteITType ȡ������
*		@arg	priority ���ȼ������ȡ����������ΪSIT_ProhibitSubstitution����˲��������ԣ�
*	@retval
*		@arg	���ִ�гɹ�������0
*		@arg  ���ִ��ʧ�ܣ�����-1
*/
int SpritesBT_Action_ToPhased(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority){
	//�������
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType == SAT_PHASED)	return -1;//�������ڵ��Ƿ��걸֡Ҳ���ش���

	//��������
	if(SpriteITType == SIT_AllowSubstitution){//����ȡ��
			node->ContainerType.LeafContainer->SpriteITType = SIT_AllowSubstitution;
			node->ContainerType.LeafContainer->priority = priority;
	}
	else if(SpriteITType == SIT_ProhibitSubstitution){//������ȡ��
			node->ContainerType.LeafContainer->SpriteITType = SIT_ProhibitSubstitution;
	}
	else{
		//�Ƿ�����
		return -1;
	}
	node->ContainerType.LeafContainer->SpriteActionType = SAT_PHASED;
	return 0;
}
/**
*	@brief	��һ�����걸���ԵĶ����ڵ�ת��Ϊ�걸����
*	@param
*		@arg  node �����ڵ��ַ
*	@retval
*		@arg	���ִ�гɹ�������0
*		@arg  ���ִ��ʧ�ܣ�����-1
*/
int SpritesBT_Action_ToSteady(SpriteBTNode_t* node){
	//�������
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType == SAT_STEADY)	return -1;//�������ڵ����걸֡Ҳ���ش���

	node->ContainerType.LeafContainer->SpriteActionType = SAT_STEADY;
	return 0;
}

/**
*	@brief	�޸�һ�����걸���Ե����ȼ�
*	@param
*		@arg  node �����ڵ��ַ
*		@arg	priority �����ȼ�
*	@retval
*		@arg	���ִ�гɹ�������0
*		@arg  ���ִ��ʧ�ܣ�����-1
*/
int SpritesBT_Action_ModPriority(SpriteBTNode_t* node,unsigned char priority){
	//�������
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType != SAT_PHASED)	return -1;//�������ڵ㲻�Ƿ��걸֡���ش���
	if(node->ContainerType.LeafContainer->SpriteITType != SIT_AllowSubstitution)	return -1;//�������ڵ㲻Ϊ��ȡ�����ͣ����ش���
	
	node->ContainerType.LeafContainer->priority = priority;
	return 0;
}

/**
*	@brief	����һ�����걸���ԵĽڵ��ȡ������
*	@param
*		@arg  node �����ڵ��ַ
*		@arg	SpriteITType ȡ������
*		@arg	priority �����ȼ�(���Ϊ����ȡ�����ͣ������ȼ�����������)
*	@retval
*		@arg	���ִ�гɹ�������0
*		@arg  ���ִ��ʧ�ܣ�����-1
*/
int SpritesBT_Action_ModITType(SpriteBTNode_t* node,SpriteITType_t SpriteITType,unsigned char priority){
	//�������
	if(node == NULL || node->BTNodeType != SBT_Action)	return -1;
	if(node->ContainerType.LeafContainer->SpriteActionType != SAT_PHASED)	return -1;//�������ڵ㲻�Ƿ��걸֡���ش���

	
	if(SpriteITType == SIT_AllowSubstitution){//����ȡ��
			node->ContainerType.LeafContainer->SpriteITType = SIT_AllowSubstitution;
			node->ContainerType.LeafContainer->priority = priority;
	}
	else if(SpriteITType == SIT_ProhibitSubstitution){//������ȡ��
			node->ContainerType.LeafContainer->SpriteITType = SIT_ProhibitSubstitution;
	}
	else{
		//�Ƿ�����
		return -1;
	}
	return 0;
}

/**
*	@brief	��Ϊ�ڵ�֮�����װ�ز���
*	@param
*		@arg	parent	�����صĶ���
*		@arg	child 	��Ҫ���ص�Ŀ��
*	@retval
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����-1����ζ��ִ��ʧ��
*/
int SpritesBT_InsertBehavior(SpriteBTNode_t* parent, SpriteBTNode_t* child) {
	//�����Ϸ��Լ��
	if (parent == NULL || child == NULL || parent->BTNodeType == SBT_Condition || parent->BTNodeType == SBT_Action) {
		return -1;
	}

	/*
	1�����ӽڵ��������ڵ���max����ζ����Ҫ���䶯̬���飬�������ݵ�ǰ��̬����
			1.1���max�������龲̬�������ޣ���ζ����Ҫת��Ϊ��̬����

			1.2���max�������������ޣ���ζ�ŵ�ǰ�Ƕ�̬���飬��Ҫ����

	2��������ӽڵ�С��max����ζ��ֻ��Ҫ��ӵ�β�����ɡ�
	*/

	//�ڵ��������޶ȣ���ζ���¼ӽڵ�ᷢ�����
	if (parent->ContainerType.EncapsulatedContainer->childCount == parent->ContainerType.EncapsulatedContainer->maxChildren) {

		//�����״����
		if (parent->ContainerType.EncapsulatedContainer->childType == BT_STATIC) {
			struct SpriteBTNode_t** childgroup = (struct SpriteBTNode_t**)malloc(sizeof(struct SpriteBTNode_t*) * SpriteBTNode_MAX_COUNT * 2);
			if (childgroup == NULL) {
				return -1;//�ռ����ʧ��
			}
			parent->ContainerType.EncapsulatedContainer->maxChildren = SpriteBTNode_MAX_COUNT * 2;
			//����ת��
			for (int i = 0; i < SpriteBTNode_MAX_COUNT; i++) {
				childgroup[i] = parent->ContainerType.EncapsulatedContainer->children.fixedChildren[i];
			}
			//�����µĺ�������
			childgroup[SpriteBTNode_MAX_COUNT] = child;
			parent->ContainerType.EncapsulatedContainer->childCount++;
			//����ǰ�����л�Ϊ��̬����
			parent->ContainerType.EncapsulatedContainer->children.dynamicChildren = childgroup;
			parent->ContainerType.EncapsulatedContainer->childType = BT_DYNAMIC;
			return 0;

		}
		//���״����
		else {
			struct SpriteBTNode_t** childgroup = (struct SpriteBTNode_t**)realloc(parent->ContainerType.EncapsulatedContainer->children.dynamicChildren, sizeof(struct SpriteBTNode_t*) * parent->ContainerType.EncapsulatedContainer->maxChildren * 2);
			if (childgroup == NULL) {
				return -1;//����ʧ��
			}
			//����¿ռ䲻����ԭ�ռ�����
			if (childgroup != parent->ContainerType.EncapsulatedContainer->children.dynamicChildren) {
				//����Ǩ��
				for (int i = 0; i < parent->ContainerType.EncapsulatedContainer->maxChildren; i++) {
					childgroup[i] = parent->ContainerType.EncapsulatedContainer->children.dynamicChildren[i];
				}
				//�ͷ�ԭ�ռ�
				free(parent->ContainerType.EncapsulatedContainer->children.dynamicChildren);
				//�滻ԭ�ռ�
				parent->ContainerType.EncapsulatedContainer->children.dynamicChildren = childgroup;
			}
			parent->ContainerType.EncapsulatedContainer->maxChildren *= 2;
			//����������
			childgroup[parent->ContainerType.EncapsulatedContainer->childCount] = child;
			parent->ContainerType.EncapsulatedContainer->childCount++;
			return 0;
		}
	}
	//���޶�֮��
	else {

		//��̬���������
		if (parent->ContainerType.EncapsulatedContainer->childType == BT_STATIC) {
			parent->ContainerType.EncapsulatedContainer->children.fixedChildren[parent->ContainerType.EncapsulatedContainer->childCount] = child;
		}
		//��̬���������
		else {
			parent->ContainerType.EncapsulatedContainer->children.dynamicChildren[parent->ContainerType.EncapsulatedContainer->childCount] = child;
		}
		parent->ContainerType.EncapsulatedContainer->childCount++;
		return 0;
	}
}

/**
*	@brief	�ӽڵ����
*	@param
*		@arg	parent	���׽ڵ�
*		@arg	child 	index�ӽڵ�����
*	@retval
*		@arg	���ִ�гɹ��������ӽڵ�ĵ�ַ
*		@arg	���ִ��ʧ�ܣ����ؿ�
*/
SpriteBTNode_t* Sprites_BT_GetChild(SpriteBTNode_t* parent, int index) {
	//������֤
	if (parent == NULL || index < 0) {
		return NULL;
	}
	if (index >= parent->ContainerType.EncapsulatedContainer->childCount) {
		return NULL;
	}
	//������ȡ
	//��̬�������
	if (parent->ContainerType.EncapsulatedContainer->childType == BT_STATIC) {
		return parent->ContainerType.EncapsulatedContainer->children.fixedChildren[index];
	}
	//��̬����
	else {
		return parent->ContainerType.EncapsulatedContainer->children.dynamicChildren[index];
	}
}



/**
*	@brief	��ָ���ڵ���ص���Ϊ���ĸ��ڵ���
*	@param
*		@arg	value		���ڵ������ֵ
*		@arg	node 	��Ҫ���ص�Ŀ��
*	@retval
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����-1����ζ��ִ��ʧ��
*/

int SpritesBT_MountToRoot(int value, SpriteBTNode_t* node) {
	if (value <= 0 || node == NULL || Sprites_BT_ManagerTree == NULL) {
		return -1;
	}

	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//�����������Ϊ��
	}

	SpriteBehaviorManger->root = node;
	return 0;
}



//ִ��һ����

/**
*	@brief	ִ��һ����ͨ��Ϊ��
*	@param
*		@arg	root		���ڵ�
*		@arg	Sprites ��Ϊ���ľ��鱾�壨���а����˾������Ϣ���Լ������˽�����ݿռ䣩
*	@retval
*
*		ִ�н��
*/
SpriteBTStatus_t ExecuteBehaviorTree(SpriteBTNode_t* root, Sprites_t* Sprites) {
	SpriteBTStatus_t result;
	switch (root->BTNodeType) {
	case SBT_Condition://�������ڵ㴦��
		result = root->ContainerType.LeafContainer->SBT_ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
		return	(result == SBT_FAILURE) ? SBT_FAILURE : SBT_SUCCESS;

	case SBT_Action://ִ�нڵ�Ĵ���
		result = root->ContainerType.LeafContainer->SBT_ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
		if(root->ContainerType.LeafContainer->SpriteActionType == SAT_STEADY){//�����걸���Եķ���ֵ
			return (result == SBT_FAILURE) ? SBT_FAILURE : SBT_SUCCESS;
		}
		
		//���걸������
		if (result == SBT_RUNNING) {
			//����ִ����״̬���������ֵ�ǰ��ִ��״̬���˳�
			Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_BT;//���õ�ǰִ�е�Ϊ�ڵ�����
			Sprites->BehaviorMoudle.bt_context.node.action = root;
		}
		return result;//���걸����ֱ�ӷ���
			
	case SBT_Selector://ѡ������������
		for (int i = 0; i < root->ContainerType.EncapsulatedContainer->childCount; i++) {
			//��ȡ����
			struct SpriteBTNode_t* node = Sprites_BT_GetChild(root, i);
			//ִ���ӽڵ�
			result = ExecuteBehaviorTree(node, Sprites);
			if (result == SBT_RUNNING || result == SBT_SUCCESS) {//����������ʱ����ִ�гɹ��������˳���ǰ��Ϊ������
				return result;
			}
		}
		return SBT_FAILURE;
	case SBT_Sequence:
		for (int i = 0; i < root->ContainerType.EncapsulatedContainer->childCount; i++) {
			//��ȡ����
			struct SpriteBTNode_t* node = Sprites_BT_GetChild(root, i);
			result = ExecuteBehaviorTree(node, Sprites);
			if (result == SBT_RUNNING || result == SBT_FAILURE) {//����������ʱ����ִ��ʧ�ܣ������˳���ǰ��Ϊ������
				return result;
			}
		}
		return SBT_SUCCESS;
	}

	return SBT_FAILURE;

}


//��ִ�к���
/**
*	@brief	��Ϊ��ִ������
*	@param
*		@arg	Sprites ��Ϊ���ľ��鱾�壨���а����˾������Ϣ���Լ������˽�����ݿռ䣩
*	@retval
*		@arg	������صĽ����0����ζ��ִ�гɹ�
*		@arg	������ؽ����-1����ζ��ִ��ʧ��
*/
int SpritesBT_BusExecute(Sprites_t* Sprites) {
	if (Sprites_BT_ManagerTree == NULL) {
		return -1;
	}
	int value = Sprites->BehaviorMoudle.value;
	if(value <= 0){
		return -1;
	}
	//�ҳ�ָ����Ϊ��
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//�����������Ϊ��
	}

	if (SpriteBehaviorManger->root == NULL) {
		return 0;//���ڵ�Ϊ�ղ���Ҫִ�У�ֱ�ӷ���
	}
	
	//����Ƿ�����ж�
	if(Sprites->BehaviorMoudle.IT_Enable == 1){//ִ���жϱ�
		//ִ���ж�
		
		int result = SpritesIT_Exec(Sprites,SpriteBehaviorManger);
		switch(result){
			case 0:
				return 0;
			case 1:
				goto BT_SAT_EXEC_FLAG;//��ת��ͨ��Ϊ��ִ�л���걸֡��֤����ִ��
			default:
				return -1;
		}
	}
	
BT_SAT_EXEC_FLAG:;//�걸����걸ִ�б��
	if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType != SBB_NO){//����ϴ�ִ�еĽڵ㲻Ϊ�սڵ�
		SpritesPhased_Exec(Sprites);
		return 0;
	}
	
	//ִ����Ϊ��
	ExecuteBehaviorTree(SpriteBehaviorManger->root, Sprites);
		
	
	return 0;
}

/******************************

	�����жϱ�

*******************************/

/**
*	@brief	��ָ����Ϊ���д����жϱ�
*	@param
*		@arg	value		��Ϊ��������ֵ
*		@arg    SIT_MapIndexHandle �Զ����ж�ƥ�亯��
*	@retval
*		@arg	������صĽ����0����ζ�Ŵ����ɹ�
*		@arg	������ؽ����-1����ζ�Ŵ���ʧ��
*/

int SpritesIT_Register(int value,SIT_MapIndexHandle_t SIT_MapIndexHandle){
	if(value <= 0 || SIT_MapIndexHandle == NULL)	return -1;
	//�ҳ�ָ����Ϊ��
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//�����������Ϊ��
	}
	if(SpriteBehaviorManger->SpriteITManager != NULL){
		return -1;//��ǰ��Ϊ���Ѿ��������жϱ�
	}
	
	//�����жϹ���ڵ�
	struct SpriteITManageNode_t* SpriteITManager = (struct SpriteITManageNode_t*)malloc(sizeof(struct SpriteITManageNode_t));
	if(SpriteITManager == NULL){
		return -1;
	}
	SpriteITManager->ITcount = 0;
	SpriteITManager->Storage_strategy = SIT_ARR;
	SpriteITManager->table.head = (struct SpriteITNode_t*)malloc(sizeof(struct SpriteITNode_t) * SIT_NODE_MAX_COUNT);
	if(SpriteITManager->table.head == NULL){
		free(SpriteITManager);
		return -1;
	}
	for(int i = 0;i<SIT_NODE_MAX_COUNT;i++){
		SpriteITManager->table.head[i].ExecuteHandle = NULL;
	}
	SpriteITManager->SIT_MapIndexHandle = SIT_MapIndexHandle;
	
	//����
	SpriteBehaviorManger->SpriteITManager = SpriteITManager;
	return 0;
}

/**
*	@brief	������������Ե��жϱ�
*	@param
*		@arg	SpriteBehaviorManger		������Ϊ������
*		@arg	arr							���ݵ������������
*	@retval
*		@arg	�����ɹ�����0
*		@arg	����ʧ�ܷ���-1
*/
int SpritesIT_RBStorageStrategyCreate(SpriteBehaviorManger_t* SpriteBehaviorManger,SpriteITNode_t* arr){
	SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree = NULL;
	int i;
	for(i = 0;i<SIT_NODE_MAX_COUNT;i++){
		if(arr[i].ExecuteHandle != NULL){//����Ǩ��
			SpriteITNode_t* node = (SpriteITNode_t*)malloc(sizeof(SpriteITNode_t));
			if(node == NULL){
				//�ڴ����ʧ��
				break;
			}
			node->value = i;
			node->priority = arr[i].priority;
			node->SpriteITType = arr[i].SpriteITType;
			node->ExecuteHandle = arr[i].ExecuteHandle;
			int result =  rbInsert(&SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, i,node);
			if(result != 0){
				free(node);
				break;
			}
		}
	}
	if(i != SIT_NODE_MAX_COUNT){
		//�����˺�������ʧ�ܵ����,������Դ����
		i--;//��ǰָ������������û����ӣ���Ҫ����һ��
		while(i >= 0){
			//�ͷ�֮ǰ��ӵĽڵ�
			if(arr[i].ExecuteHandle != NULL){
				rbDelete(&SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree,i);
			}
			
			i--;
		}
		return -1;
	}	
	return 0;
}

/**
*	@brief	��ָ���жϱ��д����жϴ���ڵ�
*	@param
*		@arg	value			  ��Ϊ��������ֵ
*		@arg	IT_index		  �ж�������ţ��жϺŲ���С��0��
*		@arg	priority		  �ж����ȼ�
*		@arg	SpriteITType	  �ж�ȡ������
*		@arg    SBT_ExecuteHandle �жϴ�����
*	@retval
*		@arg	������صĽ����0����ζ�Ŵ����ɹ�
*		@arg	������ؽ����-1����ζ�Ŵ���ʧ��
*/
int SpritesIT_Add(int value ,SITindex_t IT_index,unsigned char priority,SpriteITType_t SpriteITType,SBT_ExecuteHandle_t SBT_ExecuteHandle){
	//�������
	if(value <= 0 || IT_index < 0 || SBT_ExecuteHandle == NULL){
		return -1;
	}
	//��Ϊ������
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, value);
	if (SpriteBehaviorManger == NULL) {
		return -1;//�����������Ϊ��
	}
	if(SpriteBehaviorManger->SpriteITManager == NULL){
		return -1;//û��ע���жϱ�
	}
	
strategy_rejudgment_flag:;	//��������жϽڵ�
	//���Է���
	if(SpriteBehaviorManger->SpriteITManager->Storage_strategy == SIT_ARR){//��ǰ���õ����������
		//������֤
		
		//����������������ޣ�Ӧ����ת��������� ���� �����������Ӧ���²�ȡ���������
		if(IT_index >= SIT_NODE_MAX_COUNT || SpriteBehaviorManger->SpriteITManager->ITcount >= SIT_NODE_MAX_COUNT){
			//��ʱ����֮ǰ���������
			struct SpriteITNode_t* temp = SpriteBehaviorManger->SpriteITManager->table.head;
			
			//���������
			int result = SpritesIT_RBStorageStrategyCreate(SpriteBehaviorManger,temp);
			if(result != 0){
				SpriteBehaviorManger->SpriteITManager->table.head = temp;//�ָ�������Ե�ֵ
				return -1;//���������ʧ��
			}
			free(temp);//�ͷ�ԭ�����ڴ�
			SpriteBehaviorManger->SpriteITManager->Storage_strategy = SIT_RBTREE;
			goto strategy_rejudgment_flag;//�����жϲ��Է���
		}
		
		//�ж��Ƿ��Ѿ����ڸýڵ�
		if(SpriteBehaviorManger->SpriteITManager->table.head[IT_index].ExecuteHandle != NULL){
			return -1;
		}
		
		//������������������
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].value = value;
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].priority = priority;
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].SpriteITType = SpriteITType;
		SpriteBehaviorManger->SpriteITManager->table.head[IT_index].ExecuteHandle = SBT_ExecuteHandle;
		//�жϽڵ�������һ
		SpriteBehaviorManger->SpriteITManager->ITcount++;
		return 0;
		
	}
	else{//��ǰ���Բ��õ��Ǻ��������
		
		//����Ƿ��Ѿ������˸ýڵ�
		void* temp = search(SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, value);
		if(temp != NULL){
			return -1;
		}
		
		SpriteITNode_t* node = (SpriteITNode_t*)malloc(sizeof(SpriteITNode_t));
		if(node == NULL){
			//�ڴ����ʧ��
			return -1;
		}
		node->value = IT_index;
		node->priority = priority;
		node->SpriteITType = SpriteITType;
		node->ExecuteHandle = SBT_ExecuteHandle;
		
		int result =  rbInsert(&SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, value,node);
		if(result != 0){
			free(node);
			return -1;
		}
		//�жϽڵ�������һ
		SpriteBehaviorManger->SpriteITManager->ITcount++;
		return 0;
	}
}


/**
*	@brief	����ָ���������Ϊ�����ж�ʹ��
*	@param
*		@arg	Sprites			  ��������
*	@retval
*		@arg	����ɹ�����0
*		@arg	����ʧ�ܷ���-1
*/
int SpritesIT_Enable(Sprites_t* Sprites){
	if(Sprites == NULL){
		return -1;
	}
	//�ҳ�ָ����Ϊ��
	SpriteBehaviorManger_t* SpriteBehaviorManger = (SpriteBehaviorManger_t*)search(Sprites_BT_ManagerTree, Sprites->BehaviorMoudle.value);
	if (SpriteBehaviorManger == NULL || SpriteBehaviorManger->SpriteITManager == NULL || SpriteBehaviorManger->SpriteITManager->SIT_MapIndexHandle == NULL) {
		return -1;//�����������Ϊ�� ���� �жϱ�δ���� ���� �жϱ��Ϸ� 
	}
	Sprites->BehaviorMoudle.IT_Enable = 1;
	return 0;
}
/**
*	@brief	ָ���������Ϊ�����ж�ʧ��
*	@param
*		@arg	Sprites			  ��������
*	@retval
*		@arg	����ɹ�����0
*		@arg	����ʧ�ܷ���-1
*/
int SpritesIT_Disable(Sprites_t* Sprites){
	if(Sprites == NULL){
		return -1;
	}
	Sprites->BehaviorMoudle.IT_Enable = 0;
	Sprites->BehaviorMoudle.IT_running = 0;
	//������������жϣ�������������
	if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType == SBB_IT){
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;
		Sprites->BehaviorMoudle.bt_context.node.ITnode = NULL;
	}
	return 0;
}

/**
*	@brief	��ȡ�жϴ�����
*	@param
*		@arg	IT_index			 �жϺ�
*		@arg	SpriteBehaviorManger ��Ϊ����ڵ�
*	@retval
*		@arg	�ɹ������жϽڵ�
*		@arg	ʧ�ܷ��ؿ�
*/
SpriteITNode_t* SpritesIT_GetITNode(SpriteBehaviorManger_t* SpriteBehaviorManger,SITindex_t IT_index){
	//����洢����
	if(SpriteBehaviorManger->SpriteITManager->Storage_strategy == SIT_ARR){
		return &SpriteBehaviorManger->SpriteITManager->table.head[IT_index];
	}
	else if(SpriteBehaviorManger->SpriteITManager->Storage_strategy == SIT_RBTREE){
		SpriteITNode_t* node = (SpriteITNode_t*)search(SpriteBehaviorManger->SpriteITManager->table.Sprites_IT_ManagerTree, IT_index);
		return node;
	}
	else{
		return NULL;
	}
}


/**
*	@brief	�ж�ִ�к���
*	@param
*		@arg	Sprites			 ����
*		@arg	SpriteBehaviorManger ��Ϊ����ڵ�
*	@retval
*		@arg	ִ����ɣ�����0
*		@arg	��ת����Ϊ����ִͨ�л���걸֡����ִ�У�����1
*/
int SpritesIT_Exec(Sprites_t* Sprites,SpriteBehaviorManger_t* SpriteBehaviorManger){
	//��ȡ�жϺ�
	int IT_index = SpriteBehaviorManger->SpriteITManager->SIT_MapIndexHandle(Sprites,SpritesPriv_Get(Sprites));
	if(IT_index < 0){
		//����ʧ��
		return 1;
	}
	
//	//����жϱ�־
//	Sprites->BehaviorMoudle.IT_flag = 0;
	/*������������
		�����һ֡���ж��������У�����ͱ��ν�ִ�е��жϺţ������ڶ�֡�жϷ�������ͬ�жϴ�������
		��ֱ������������жϣ����Ч�ʡ�
		
		1��ֻ���жϲŻᴥ����������
		2��ֻ������ִ����ͬ���ж�
		3���жϱ���Ϊ��֡������Ϊ��֡��ִ�к�IT_running�ᱻ�ü٣�
	*/
	if(Sprites->BehaviorMoudle.IT_running == 1){
		//�����ظ�ִ��
		if(Sprites->BehaviorMoudle.bt_context.Previous_IT_index == IT_index){//��֮ǰ��ִ��һ��
			return 1;
		}
	}
	
	
	
	//��ȡ�жϽڵ�
	SpriteITNode_t* ITnode = SpritesIT_GetITNode(SpriteBehaviorManger,IT_index);
	if(ITnode == NULL){
		//�жϽڵ����ʧ��
		return 1;
	}
	
	//����ϴ�ִ�еĽڵ�
	if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType == SBB_IT){//����ϴ�ִ�е����ж����͵Ľڵ���Ҫ���⴦��
		//����ϴ�ִ�еĽڵ��Ƿ���Ա�ȡ��
		if(ITnode->SpriteITType == SIT_ProhibitSubstitution){//��ֹ��ȡ��
			//��ת����ִ�У���ǰ���жϻᱻ����
			//goto SAT_EXEC_FLAG;
			return 1;
		}
		else if(ITnode->SpriteITType == SIT_AllowSubstitution){//����ȡ��
			//�Ա����ߵ����ȼ�
			if(ITnode->priority <= Sprites->BehaviorMoudle.bt_context.node.ITnode->priority){
				//�жϽڵ��֮ǰ���ж����ȼ���ͬ���߸��ͣ�������ϴ�ִ��
				//goto SAT_EXEC_FLAG;
				Sprites->BehaviorMoudle.bt_context.Previous_IT_index = IT_index;//���浱ǰִ�е��ж�
				return 1;
			}
		}
		else{
			//������<ö�ٴ������>
			EngineFault_ExceptionEnumParam_Handle();
		}
	}
	else if(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType == SBB_BT){//����ϴ�ִ�е�����Ϊ�����͵Ľڵ���Ҫ���⴦��
		//����ϴ�ִ�е���Ϊ���ڵ��Ƿ�Ϊ���걸֡
		if(Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->SpriteActionType == SAT_PHASED){
			//���÷��걸֡�Ƿ���Ա��ж�ȡ��
			if(Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->SpriteITType == SIT_ProhibitSubstitution){
				//��ֹ��ȡ��
				//goto SAT_EXEC_FLAG;//��ת����ִ�У���ǰ���жϻᱻ����
				return 1;
			}
			else if(Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->SpriteITType == SIT_AllowSubstitution){
				//����ȡ��
				//�Ա����ߵ����ȼ�
				if(ITnode->priority <= Sprites->BehaviorMoudle.bt_context.node.action->ContainerType.LeafContainer->priority){
					//�жϽڵ��֮ǰ����Ϊ���ڵ����ȼ���ͬ���߸��ͣ�������ϴ�ִ��
					//goto SAT_EXEC_FLAG;	
					return 1;
				}
			}
			else{
				//������<ö�ٴ������>
				EngineFault_ExceptionEnumParam_Handle();
			}
		}
	}
	//����ϴ�ִ�е��ǿ����ͽڵ㣬�����κδ���ֱ��ִ���жϡ�
	
	
	
	
	//ִ�У�Ԥִ��һ�Σ����Թ��˵�֡�߼����жϣ����߶�֡��ִ��һ���߼���
	SpriteBTStatus_t result = ITnode->ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
	if(result == SBT_FAILURE || result == SBT_SUCCESS){
		//�û�ִ�з���ʧ�ܻ�����ɶ���ζ�ŵ�ǰ�жϲ�����Ҫ����ִ��
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;//����ִ�еĽڵ������ÿ�
		Sprites->BehaviorMoudle.bt_context.node.action = NULL;
		Sprites->BehaviorMoudle.IT_running = 0;//���Ϊ�ж�ִ��̬
	}
	else if(result == SBT_RUNNING){
		//�û�ִ�з�������ִ�еĽ������ζ����Ҫ����¼��ǰִ�еĽڵ���Ϣ��Ϊ�´μ���ִ����׼��
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_IT;//��¼��ǰִ�е����жϽڵ�
		Sprites->BehaviorMoudle.bt_context.node.ITnode = ITnode;//��¼����ִ�е��жϽڵ�
		Sprites->BehaviorMoudle.IT_running = 1;//����Ϊ�ж�ִ��̬
		Sprites->BehaviorMoudle.bt_context.Previous_IT_index = IT_index;//�����������ж�ִ�У����浱ǰִ�е��ж�
	}
	else{
		//������<ö�ٴ������>
		EngineFault_ExceptionEnumParam_Handle();
	}
	return 0;
}
/**
*	@brief	���걸֡����ִ�к���
*	@param
*		@arg	Sprites			 ����
*	@retval
*		none
*/
void SpritesPhased_Exec(Sprites_t* Sprites){
	//�жϽڵ�����
	SpriteBTStatus_t result;
	SpriteITNode_t *ITnode = NULL;
	SpriteBTNode_t* BTnode = NULL;
	switch(Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType){
		case SBB_IT:;//�ϴ�ִ�еĽڵ����ж�����
			//ִ��
			ITnode = Sprites->BehaviorMoudle.bt_context.node.ITnode;
			result = ITnode->ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
			
			//�������
		
			if(result == SBT_RUNNING){
				//�û�ִ�з�������ִ�еĽ������ζ����Ҫ����¼��ǰִ�еĽڵ���Ϣ��Ϊ�´μ���ִ����׼��
				Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_IT;//��¼��ǰִ�е����жϽڵ�
				Sprites->BehaviorMoudle.bt_context.node.ITnode = ITnode;//��¼����ִ�е��жϽڵ�
				
				return;
			}

			break;
		case SBB_BT:;//�ϴ�ִ�еĽڵ�����Ϊ������
			//ִ��
			BTnode = Sprites->BehaviorMoudle.bt_context.node.action;
			result = BTnode->ContainerType.LeafContainer->SBT_ExecuteHandle(Sprites,SpritesPriv_Get(Sprites));
			
			//�������
			if(result == SBT_RUNNING){
				//�û�ִ�з�������ִ�еĽ������ζ����Ҫ����¼��ǰִ�еĽڵ���Ϣ��Ϊ�´μ���ִ����׼��
				Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_BT;//��¼��ǰִ�е����жϽڵ�
				Sprites->BehaviorMoudle.bt_context.node.action = BTnode;//��¼����ִ�е��жϽڵ�
				
				return;
			}
			break;
		default:;
			//������<ö�ٴ������>
			EngineFault_ExceptionEnumParam_Handle();
	}
	
	if(result == SBT_FAILURE || result == SBT_SUCCESS){
		//�û�ִ�з���ʧ�ܻ�����ɶ���ζ�ŵ�ǰ�жϲ�����Ҫ����ִ��
		Sprites->BehaviorMoudle.bt_context.SBB_NodeHandleType = SBB_NO;//����ִ�еĽڵ������ÿ�
		Sprites->BehaviorMoudle.bt_context.node.action = NULL;
		
		/*���֮ǰִ�е�����ͨ�ڵ㣬��ô�ж����б�־����0�����֮ǰִ�е����жϣ������ж���ɣ���Ҫ��0*/
		Sprites->BehaviorMoudle.IT_running = 0;//�������б�־
	}

	else{
		//������<ö�ٴ������>
		EngineFault_ExceptionEnumParam_Handle();
	}
}
