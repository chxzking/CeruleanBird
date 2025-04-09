/**
  ******************************************************************************
  * @file    Sprites_Animation.c
  * @author  ��㻭�ģ�chxzking��
  * @version V1.0
  * @date    7-2-2025
  * @brief   ���ļ������˿⾫�鶯���йصĺ���ʵ��
  ******************************************************************************
  */

#include "Sprites_internal.h"
#include "engine_config.h"
#include "RB_Tree_API.h"
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


//����������ָ��
RB_Node* Sprites_Header = C_NULL;	

//�����ھ�������
int Sprites_Count = 0;

/*
ע������
���������л���
		�����õ�ǰ�洢�ĵ�ǰ֡
		�����ۼ�ʱ��
		����ı䶯������

*/


/**********************
	�����������������
***********************/
/**
*		@note	���������һ����������Ļ��������ܺ�������ʲô����������Ҫ�Ƚ��иú�������һ�����������
*	@brief	����һ�����͵ľ��������
*	@param	
*		@arg	value	�������
*	@retval	
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����1����ζ�Ÿ�ֵ�Ѿ�����
*		@arg	������ؽ����-1����ζ���ڴ����ʧ�ܻ��߲�������
*		@arg	������ؽ����-2����ζ�ž�������ʼ��ʧ��
*/
int SpritesAnim_CreateManager(int value){	
	if(SpritesTree_Init()){//��ʼ��������
		return -2;
	}
	//�����ڵ�
	SpritesAnimationManager_t* node = (SpritesAnimationManager_t*)malloc(sizeof(SpritesAnimationManager_t));
	if(node == C_NULL)	return -1;			//�ڴ�����ʧ��
	node->value = value;								//�������͵Ĵ���
	node->PreallocationChainCount = 0;	//Ԥ����Ķ������������
	node->AnimationChainCount = 0;			//���鶯����������
	node->SpritesCount = 0;							//���ó�ʼʹ�ô˶����ľ�������Ϊ0
	node->SpritesRecordGroup = C_NULL;	//���鶯������
	
	//��������
	int result = rbInsert(&Sprites_Header,value,(void*)node);
	if(result != 0){
		//����ʧ��
		free(node);//�ͷſռ�
		return result;//���ش���ֵ
	}
	//����ɹ�
	return 0;
}
/**
*		@note	
*	@brief	ɾ��һ�����͵ľ��������
*	@param	
*		@arg	value	�������
*	@retval	
*		@arg	������صĽ����0�������ɹ�
*		@arg	������ؽ����-1������ʧ��
*/
int SpritesAnim_DelManager(int value){
		/*
	
			��������������������������������������������������������������������������
	
		*/
	
		return -1;
}
/**
*		@note	
*	@brief	��ȡָ������Ĺ���������
*	@param	
*		@arg	sprites	�������
*	@retval	
*		@arg	������صĽ��������ţ�����������ִ�����ἤ�������
*/
int SpritesAnim_GetManagerIndex(SpritesBase_t* sprites){
		if(sprites == C_NULL){
			//������<��ָ�����>
			EngineFault_NullPointerGuard_Handle();
		}
		return sprites->AnimMoudle.value;	
}

/**********************
	�����鶯����������
***********************/
/**
*		
*	@brief	��ָ�����������´���һ���µĶ�������
*	@param	
*		@arg	value	�������
*		@arg	ROW_Pixel	�����������
*		@arg	COL_Pixel	�����������
*		@arg	SpritesType	���������	������ѡ��BYTE_SPRITES��BIT_SPRITES�������� @ref SPRITES_TYPE
*		@arg  	Anim_duration �˶�����������������ĵ�ʱ�� ����λ���룩
*	@retval	
*		@arg	����ʧ�ܷ��ش���ֵ -1��
*		@arg	�����ɹ����ظö�����������ֵ����������������
*/
int SpritesAnim_CreateNewChain(int value,int ROW_Pixel,int COL_Pixel,SPRITES_TYPE SpritesType,float Anim_duration){
	//����ָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,value);
	if(SpritesManager == C_NULL)	return -1;//Ŀ������������ڣ����ش���ֵ
	
	//���Ԥ����������0
	if(SpritesManager->PreallocationChainCount == 0){
		/*
			����һ����¼��,ԭ�����£�
			1�����ǵ���Щ�Ǿ�̬�����������ڶ����������
				Ԥ�������������ϵĿռ�ᵼ�¿ռ��˷�
		*/
		SpritesRecord_t* record = (SpritesRecord_t*)malloc(sizeof(SpritesRecord_t) * 1);//��Ԥ����һ����¼��
		if(record == C_NULL) 	return -1;//�ռ�����ʧ��
		//���¹�����
		SpritesManager->PreallocationChainCount = 1;
		SpritesManager->AnimationChainCount = 1;
		SpritesManager->SpritesRecordGroup = record;
		//��ʼ����ǰ��¼��
		record->Logic_ROW = ROW_Pixel;
		record->Column = COL_Pixel;
		record->SpritesType = SpritesType;
		record->AnimationChainGroup = C_NULL;
		record->frame_count = 0;
		record->Anim_duration = Anim_duration;
		record->frame_duration = 0.0;
		//ʵ���м���
		if(SpritesType == BIT_SPRITES){
			//���bit����
			record->Row = bitToByte(ROW_Pixel);
		}else if(SpritesType == BYTE_SPRITES){
			//�����byte����
			record->Row = ROW_Pixel;//ֱ�ӱ���
		}
		//������ɣ����ض�����������
		return 0;
	}
	//��ǰ��¼����Ҫ�洢���鶯��
	else{	
		//��鵱ǰ�����ı��Ƿ��Ѿ���д��
		
		//�Ѿ���д��
		if(SpritesManager->PreallocationChainCount == SpritesManager->AnimationChainCount){
			//��ȡ��ǰ���ڴ��С
			int size = SpritesManager->PreallocationChainCount;
			//����������λ����
			size+=2;
			//��ʱ�洢ԭ��ַ
			SpritesRecord_t* InitialPtr = SpritesManager->SpritesRecordGroup;
			//���ݶ�̬����
			SpritesRecord_t* temp = (SpritesRecord_t*)realloc(SpritesManager->SpritesRecordGroup,size*sizeof(SpritesRecord_t));
			if(temp == C_NULL)	return -1;//�ڴ�����ʧ��
			//�ڴ��������
			SpritesManager->SpritesRecordGroup = temp;
			SpritesManager->PreallocationChainCount = size;
			
			//����ַ�Ƿ����˱仯��
			if(InitialPtr != temp){
				//����µļ�¼���ַ�����˱仯������Ҫ����֮ǰ���е�record��λ
				for(int i = 0;i<SpritesManager->AnimationChainCount;i++){
					SpritesLinkNode_t* head = SpritesManager->SpritesRecordGroup[i].AnimationChainGroup;
					SpritesLinkNode_t* current = head;
					do{
						current->record = &(SpritesManager->SpritesRecordGroup[i]);
						current = current->next;
					}while(current != head);
				}
			}
			
		}
		//û�б�д��,���ж��������
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Logic_ROW = ROW_Pixel;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Column = COL_Pixel;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].SpritesType = SpritesType;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].AnimationChainGroup = C_NULL;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].frame_count = 0;
		SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Anim_duration = Anim_duration;
		//ʵ���м���
		if(SpritesType == BIT_SPRITES){
			//���bit����
			SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Row = bitToByte(ROW_Pixel);
		}else if(SpritesType == BYTE_SPRITES){
			//�����byte����
			SpritesManager->SpritesRecordGroup[SpritesManager->AnimationChainCount].Row = ROW_Pixel;//ֱ�ӱ���
		}
		
		//���µ�ǰ�����������
		SpritesManager->AnimationChainCount++;
		//��������ӵ���ı��
		return SpritesManager->AnimationChainCount - 1;
	}
}



/**
*		
*	@brief	��ָ�����鵱ǰ�Ķ������л��µĶ�����,������֡������¼ֵ�ᱻ����Ϊ֡��,�ۼ�ʱ��ᱻ������л�����ı䶯�����͡�ѭ�����߷�ѭ������
*	@param	
*		@arg	sprites	�������
*		@arg	NewtAnimationChainIndex	�¶���������
*	@retval	
*		@arg	����ʧ�ܷ��ش���ֵ -1��
*		@arg	�����ɹ����� 0
*/
int SpritesAnim_SwitchChain(SpritesBase_t* sprites,AnimChain_t NewtAnimationChainIndex){
	if(sprites == C_NULL)	return -1;//�Ƿ�����
	
	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//Ŀ������������ڣ����ش���
	
	//��֤��¼��
	if(NewtAnimationChainIndex >= SpritesManager->AnimationChainCount){
		return -1;//��¼��Ƿ�����
	}
	
	//��֤��ɣ��޸Ķ�����
	SpritesLinkNode_t* temp = SpritesManager->SpritesRecordGroup[NewtAnimationChainIndex].AnimationChainGroup;
	if(temp == C_NULL)	return -1;//��ǰ�Ӷ�������û����Ӷ���
	
	sprites->AnimMoudle.ReadyAnimation = temp;
	sprites->AnimMoudle.CurrentAnimationChainIndex = NewtAnimationChainIndex;
	sprites->AnimMoudle.currentFrmIndex = 0;
	sprites->accumulatedTime = 0.0;//�ۼ�ʱ�����
	return 0;
}
/**
*		
*	@brief	��ָ�����鵱ǰ�Ķ������л�ΪĬ�ϵĶ�������������֡������¼ֵ�ᱻ����Ϊ֡�ף�
*	@param	
*		@arg	sprites	�������
*	@retval	
*		@arg	����ʧ�ܷ��ش���ֵ -1��
*		@arg	�����ɹ����� 0
*/
int SpritesAnim_SwitchDefaultChain(SpritesBase_t* sprites){
	if(sprites == C_NULL)	return -1;
	return SpritesAnim_SwitchChain(sprites,sprites->AnimMoudle.defaultAnimationChainIndex);
}
/**
*		
*	@brief	����һ�������ָ��������ΪĬ�϶�����,���ǵ�ǰ�����������л������ɱ����ڵ�ǰ��������
*	@param	
*		@arg	sprites	�������
*		@arg  newDefaultChainIdx �µ�Ĭ�϶�����
*	@retval	
*		@arg	����ʧ�ܷ��ش���ֵ -1��
*		@arg	�����ɹ����� 0
*/
int SpritesAnim_SetDefaultChain(SpritesBase_t* sprites,AnimChain_t newDefaultChainIdx){
	if(sprites == C_NULL)	return -1;
	
	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//Ŀ������������ڣ����ش���
	
	//��֤��¼��
	if(newDefaultChainIdx >= SpritesManager->AnimationChainCount){
		return -1;//��¼��Ƿ�����
	}
	
	sprites->AnimMoudle.defaultAnimationChainIndex = newDefaultChainIdx;
	return 0;
}
/**
*		
*	@brief	��ȡһ������ָ����������֡������
*	@param	
*		@arg	sprites	�������
*		@arg  ChainIdx ����������
*	@retval	
*		@arg	����ʧ�ܷ��ش���ֵ -1��
*		@arg	�����ɹ����� ��������֡������
*/
int SpritesAnim_GetFrmCountOfChain(SpritesBase_t* sprites,AnimChain_t ChainIdx){
	if(sprites == C_NULL)	return -1;
	
	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//Ŀ������������ڣ����ش���
	
	//��֤��¼��
	if(ChainIdx >= SpritesManager->AnimationChainCount){
		return -1;//��¼��Ƿ�����
	}
	
	return SpritesManager->SpritesRecordGroup[ChainIdx].frame_count;
}
/**
*		
*	@brief	��ȡָ�����鵱ǰ���ڵĶ�����������
*	@param	
*		@arg	sprites	�������
*	@retval	
*		@arg	����ʧ�ܣ������쳣����������
*		@arg	�����ɹ����� ����
*/
AnimChain_t SpritesAnim_GetChainIndex(SpritesBase_t* sprites){
		if(sprites == C_NULL)	{
				//������<��ָ�����>
				EngineFault_NullPointerGuard_Handle();
		}
		return sprites->AnimMoudle.CurrentAnimationChainIndex;
}
/**********************
	�����鶯��֡������
***********************/
/**
*		
*	@brief	Ϊָ�����������һ������֡
*	@param	
*		@arg	AnimationManageIndex	��������������
*		@arg	AnimationChainIndex	���鶯����������
*		@arg	AnimationFrameSeq	����֡����ţ� @note ע�⣬�������������������֡�Ĳ���˳���Ҳ��ܳ�����ͬ�����
*		@arg	SpritesData	������������
*		@arg	SpritesShadow	����Ӱ������ @note �������������� SpritesData ��Ա����Ч���֣��������ʹ��BYTE_SPRITES����˲����ᱻ����
*	@retval	
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����-1����ζ���ڴ����ʧ�ܻ��߲�������
*/
int SpritesAnim_InsertNewFrm(int AnimationManageIndex,int AnimationChainIndex,int AnimationFrameSeq,\
	const unsigned char* SpritesData,const unsigned char* SpritesShadow){
	//����ָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = (SpritesAnimationManager_t*)search(Sprites_Header,AnimationManageIndex);
	if(SpritesManager == C_NULL)	return -1;//Ŀ������������ڣ����ش���ֵ
	
	//��֤�Ƿ���ڶ�Ӧ�Ķ�����¼������(���ڼ�¼���¼��������������������������Ҫ������-1)
	if(AnimationChainIndex >= SpritesManager->AnimationChainCount){
		return -1;
	}
	
	//��������֡
	SpritesLinkNode_t* node = (SpritesLinkNode_t*)malloc(sizeof(SpritesLinkNode_t));
	if(node == C_NULL)	return -1;//�ڴ�����ʧ��
	//��ʼ���ڵ�
	node->record = &(SpritesManager->SpritesRecordGroup[AnimationChainIndex]);
	node->next = C_NULL;
	node->SerialNumber = AnimationFrameSeq;
	node->SpritesData = SpritesData;
	node->SpritesShadow = SpritesShadow;
	//������붯������
	if(SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup == C_NULL){
		//���֮ǰ�������в����ڶ���֡����ôֱ�Ӳ���
		node->next = node;
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup = node;
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count++;//����֡��������
		//���µ�֡����ʱ��
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_duration = SpritesManager->SpritesRecordGroup[AnimationChainIndex].Anim_duration / \
																																						 SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count;
		return 0;
	}else{
		//������ڶ���֡������˳�����
		//����һ��̽�����ڱ���
		SpritesLinkNode_t* current = SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup;
		SpritesLinkNode_t* head = current;
		SpritesLinkNode_t* prev = C_NULL;//��ָ��
		
		// ���������ҵ�����λ�� 
		do {
			// ����Ƿ�����ظ���SerialNumber
			if (current->SerialNumber == node->SerialNumber) {
				// ����SerialNumber�ظ������ 
				free(node);//�ͷŽڵ�
				return -1;//���ش���
			}
			
			// �ҵ�����λ��
			if (current->SerialNumber > node->SerialNumber) {
				break;
			}
			
			prev = current;
			current = current->next;
		}while (current != head);
		
		
		// �����½ڵ�
		node->next = current;
		if (prev != C_NULL){
			//��ָ�벻���ڿգ���ζ�Ž��������ָ��
			prev->next = node;//��ָ����Ϊ��һ��ָ������node�ڵ�
		}else{
			// ����ͷ���������� 
			SpritesLinkNode_t* tail = head; 
			while (tail->next != head) { 
				tail = tail->next; 
			} 
			tail->next = node; 
			node->next = head;
			//�����µ�ͷ��Ϊ����������֡
			SpritesManager->SpritesRecordGroup[AnimationChainIndex].AnimationChainGroup = node;
		}
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count++;//����֡��������
		//���µ�֡����ʱ��
		SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_duration = SpritesManager->SpritesRecordGroup[AnimationChainIndex].Anim_duration / \
																																						 SpritesManager->SpritesRecordGroup[AnimationChainIndex].frame_count;
		return 0;
	}
}

/**
*		
*	@brief	ָ������Ķ�������
*	@param	
* @arg	sprites	�������
*	@retval	
*		none
*/
void SpritesAnim_Update(SpritesBase_t* sprites){
	if(sprites == C_NULL || sprites->AnimMoudle.ReadyAnimation == C_NULL)	return;
	
	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return ;//Ŀ������������ڣ����ش���
	
	//�����ۼ���ֵ
	sprites->accumulatedTime += (float)(DeltaTime->deltaTime);
	
	//�ж��Ƿ񳬹��˸�����ֵ
	while(sprites->accumulatedTime >= SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_duration){
		//������ֵ
		sprites->accumulatedTime -= SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_duration;
		//�ж϶���������ѭ���������Ƿ�ѭ������
		if(sprites->AnimMoudle.AnimationType == Anim_Once){
			//��ѭ������
			
			//�����ǰ����֡�������һ֡
			if(sprites->AnimMoudle.currentFrmIndex < SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_count){
				sprites->AnimMoudle.ReadyAnimation = sprites->AnimMoudle.ReadyAnimation->next;
				sprites->AnimMoudle.currentFrmIndex++;
			}
			
		}else{
			//ѭ������
			//����Ϊ��һ֡
			sprites->AnimMoudle.ReadyAnimation = sprites->AnimMoudle.ReadyAnimation->next;
		}
	}
}
/**
*		
*	@brief	ָ������Ķ����л�Ϊ��ǰ�������ĵ�һ֡
*	@param	
* @arg	sprites	�������
*	@retval	
*		@arg	����ʧ�ܷ��� -1��
*		@arg	�����ɹ����� 0
*/
int SpritesAnim_SwitchFirstFrm(SpritesBase_t* sprites){
	//�������
	if(sprites == C_NULL || sprites->AnimMoudle.ReadyAnimation == C_NULL)	return -1;
	
	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//Ŀ������������ڣ����ش���
	
	sprites->AnimMoudle.ReadyAnimation = SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].AnimationChainGroup;
	sprites->AnimMoudle.currentFrmIndex = 0;
	return 0;
}
/**
*		
*	@brief	������Ϣ��ȡ
*	@param	
*		@arg	spritesNode	����ڵ�
*		@arg	ROW_Pixel	�����������
*		@arg	COL_Pixel	�����������
*	@retval	
*		@arg	��ʾ͸��ֵ ����-1��
*		@arg	��ʾ��ɫֵ ����0��
*		@arg	��ʾ��ɫֵ ����1��
*/
int SpritesAnim_ExtractFrmData(SpritesBase_t* sprites,int ROW_Pixel,int COL_Pixel){
	//�����Ϸ��Լ��
	if(sprites == C_NULL)	return -1;
	if(ROW_Pixel < 0 || ROW_Pixel >= sprites->AnimMoudle.ReadyAnimation->record->Logic_ROW)	return -1;
	if(COL_Pixel < 0 || COL_Pixel >= sprites->AnimMoudle.ReadyAnimation->record->Column)	return -1;	
	
	//����������ֽ���
	if(sprites->AnimMoudle.ReadyAnimation->record->SpritesType == BYTE_SPRITES){
		return sprites->AnimMoudle.ReadyAnimation->SpritesData[ROW_Pixel* sprites->AnimMoudle.ReadyAnimation->record->Column + COL_Pixel];
	}
	//���������bit��
	else if(sprites->AnimMoudle.ReadyAnimation->record->SpritesType == BIT_SPRITES){
		int byte = ROW_Pixel/8;
		int offset = ROW_Pixel%8;
		//���Ӱ���Ƿ�Ϊ1
		int data_shadow = (sprites->AnimMoudle.ReadyAnimation->SpritesShadow[byte * sprites->AnimMoudle.ReadyAnimation->record->Column + COL_Pixel] & (0x01<<offset)) ? 1:0;
		if(data_shadow == 0){
			//Ϊ0��ʾ͸��
			return -1;
		}
		//Ϊ1��ʾ����ͼ��
		return (sprites->AnimMoudle.ReadyAnimation->SpritesData[byte * sprites->AnimMoudle.ReadyAnimation->record->Column + COL_Pixel] & (0x01<<offset)) ? 1:0;
	}
	return -1;
}

/**
*		
*	@brief	���þ���Ķ�������
*	@param	
*		@arg	sprites	�������
*		@arg	AnimationType	�������Ͷ����� @ref AnimationType_t
*	@retval	
*		�����ɹ����� 0
*		�����ɹ����� -1
*/
int SpritesAnim_SetAnimType(SpritesBase_t* sprites,AnimationType_t AnimationType){
	if(sprites == C_NULL)	return -1;
	sprites->AnimMoudle.AnimationType = AnimationType;
	return 0;
}
/**
*		
*	@brief	��ȡ����Ķ�������
*	@param	
*		@arg	sprites	�������
*	@retval	
*		�����ɹ����� ���ͣ������� @ref AnimationType_t
*		�����ɹ����� -1
*/
AnimationType_t SpritesAnim_GetAnimType(SpritesBase_t* sprites){
	if(sprites == C_NULL){
		//������<ö�ٴ������>
		EngineFault_ExceptionEnumParam_Handle();
		return Anim_Error;
	}
	return sprites->AnimMoudle.AnimationType;
}
/**
*		
*	@brief	��ȡ���鶯���Ƿ��˶�������β֡��ע��ֻ�з�ѭ���������ܼ��㣬���򷵻ش���ֵ(���������ƣ���Ϊ��ѭ�������������һ֡���޷��ж�����ִ����û��)
*	@param	
*		@arg	sprites	�������
*	@retval	
*		����β֡���� 1
*		û�е���β֡���� 0
*		����ʧ�ܷ��� -1
*/
int SpritesAnim_IsEndFrm(SpritesBase_t* sprites){
	if(sprites == C_NULL || sprites->AnimMoudle.AnimationType == Anim_Loop)	return -1;

	//��ȡָ���ľ��������
	SpritesAnimationManager_t* SpritesManager = sprites->AnimMoudle.SpritesManager;
	if(SpritesManager == C_NULL)	return -1;//Ŀ������������ڣ����ش���
	
	if(sprites->AnimMoudle.currentFrmIndex == (SpritesManager->SpritesRecordGroup[sprites->AnimMoudle.CurrentAnimationChainIndex].frame_count - 1)){
		return 1;
	}else{
		return 0;
	}
}



