#ifndef __RB_TREE_H__
#define __RB_TREE_H__

#include "RB_Tree_API.h"
//�������ɫ
typedef enum RB_COLOR {
	RB_RED,				//��ɫ
	RB_BLACK			//��ɫ
}RB_COLOR;

//�����
struct RB_Node {
    int value;                  //��ʶ�ţ���ţ�
	void* resource;		        //��Դ��Ϣ
	struct RB_Node* right;		//�Һ���
	struct RB_Node* left;		//����
	struct RB_Node* parent;		//���׽ڵ�
	RB_COLOR color;				//�������ɫ
};
//�ڵ㴴��
RB_Node* createNode(int value,void* resource); 
//����
void leftRotate(RB_Node** root, RB_Node* x) ;
//����
void rightRotate(RB_Node** root, RB_Node* y);
//�����������
void AdjustInsert(RB_Node** root, RB_Node* k);

//�ڵ�ɾ��
//����
RB_Node* rotateLeft(RB_Node* root); 
//����
RB_Node* rotateRight(RB_Node* root);
//�������ĸ��ڵ�
RB_Node* getRoot(RB_Node* node); 
//��ȡ�ֵܽڵ�
RB_Node* brotherNode(RB_Node* node);
//��ȡ�ڵ���ɫ
RB_COLOR NodeColor(RB_Node* node);
//��̽�������
RB_Node* HandleNodeWithTwoChildren(RB_Node* node);
//Ѱ��ɾ���ڵ�
RB_Node* FindTargetNode(RB_Node* root, int data);
//ɾ����Ϊ1�ĵ�
RB_Node* DeleteNodeWithOneChild(RB_Node* target);
//��Ϊ0�Ĵ��ࣺ���ɾ���ڵ��Ǻ�ɫ
RB_Node* DeleteRedLeafNode(RB_Node* target);
//����ֵܽڵ��Ǻ�ɫ���Ҵ��ں�ɫ�ڵ㣨���ص�����ľֲ��������ڵ㣩
RB_Node* DeleteWhenHasRedNephews(RB_Node* brother);
//���ɾ�����Ǻ�ɫ�ڵ㣬���ֵ�Ϊ��ɫ�����ص�����ľֲ��������ڵ㣩
RB_Node* DeleteBlackLeafNodeWithRedSibling(RB_Node* brother);
//ɾ������
RB_Node* TreeDelete(RB_Node* root, int data);
#endif 
