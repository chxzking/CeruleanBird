#ifndef __RB_TREE_API_H__
#define __RB_TREE_API_H__

//������ڵ�����
typedef struct RB_Node RB_Node;
/**
*	@brief	�ں������ͨ����Ų�����Դ�ĵ�ַ
*	@param	
*		@arg	root	���ڵ�
*		@arg	value	���
*	@retval	��ȡ�ɹ�������Դ�ĵ�ַ ����ȡʧ�ܷ��ؿ�
*	@note	��ԴӦ��Ϊһ���ṹ�壬��������Դ�������Ϣ������Ӧ����һ��
*			��̬�ڴ��ַ
*/
void* search(RB_Node* root, int value);
/**
*	@brief	�ں���������һ���ڵ�
*	@param	
*		@arg	root	���ڵ�
*		@arg	value	���
*		@arg	resource	��Դ��ַ
*	@note	��ԴӦ��Ϊһ���ṹ�壬��������Դ�������Ϣ������Ӧ����һ��
*			��̬�ڴ��ַ���벻Ҫ����Ϊȫ�ֻ�ֲ������ĵ�ַ
*	@retval	�����ɹ����ؿռ��׵�ַ�����򷵻ؿգ�
*		@arg	������صĽ����0����ζ����ӳɹ�
*		@arg	������ؽ����1����ζ�Ÿ�ֵ�Ѿ�����
*		@arg	������ؽ����-1����ζ���ڴ����ʧ�ܻ��߲�������
*/
int rbInsert(RB_Node** root, int value,void* resource);
/**
*	@brief	�������ͨ�����ɾ���ڵ�
*	@param	
*		@arg	root	���ڵ�
*		@arg	value	���
*	@retval	none
*	@note	�ú�����ͬʱɾ���ڵ�����Դռ�õĶ�̬�ڴ�ռ䣬�ú�������ʱ
*			�����Ľ϶��ջ�ռ䣬��ע����ܴ��ڵ�ջ�������
*/
void rbDelete(RB_Node** root, int value);
/**
*	@brief	ɾ�����ú����
*	@param	
*		@arg	root	���ڵ�
*	@retval	none
*	@note	�ú�����������Խϳ���ʱ��
*/
void RB_Tree_Free(RB_Node** root);
#endif 
