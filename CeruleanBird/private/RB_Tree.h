#ifndef __RB_TREE_H__
#define __RB_TREE_H__

#include "RB_Tree_API.h"
//红黑树颜色
typedef enum RB_COLOR {
	RB_RED,				//红色
	RB_BLACK			//黑色
}RB_COLOR;

//红黑树
struct RB_Node {
    int value;                  //标识号（序号）
	void* resource;		        //资源信息
	struct RB_Node* right;		//右孩子
	struct RB_Node* left;		//左孩子
	struct RB_Node* parent;		//父亲节点
	RB_COLOR color;				//红黑树颜色
};
//节点创建
RB_Node* createNode(int value,void* resource); 
//左旋
void leftRotate(RB_Node** root, RB_Node* x) ;
//右旋
void rightRotate(RB_Node** root, RB_Node* y);
//插入调整函数
void AdjustInsert(RB_Node** root, RB_Node* k);

//节点删除
//左旋
RB_Node* rotateLeft(RB_Node* root); 
//右旋
RB_Node* rotateRight(RB_Node* root);
//返回树的根节点
RB_Node* getRoot(RB_Node* node); 
//获取兄弟节点
RB_Node* brotherNode(RB_Node* node);
//获取节点颜色
RB_COLOR NodeColor(RB_Node* node);
//后继交换函数
RB_Node* HandleNodeWithTwoChildren(RB_Node* node);
//寻找删除节点
RB_Node* FindTargetNode(RB_Node* root, int data);
//删除度为1的点
RB_Node* DeleteNodeWithOneChild(RB_Node* target);
//度为0的大类：如果删除节点是红色
RB_Node* DeleteRedLeafNode(RB_Node* target);
//如果兄弟节点是黑色并且存在红色节点（返回调整后的局部子树根节点）
RB_Node* DeleteWhenHasRedNephews(RB_Node* brother);
//如果删除的是黑色节点，且兄弟为红色（返回调整后的局部子树根节点）
RB_Node* DeleteBlackLeafNodeWithRedSibling(RB_Node* brother);
//删除函数
RB_Node* TreeDelete(RB_Node* root, int data);
#endif 
