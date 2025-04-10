#ifndef __RB_TREE_API_H__
#define __RB_TREE_API_H__

//红黑树节点声明
typedef struct RB_Node RB_Node;
/**
*	@brief	在红黑树中通过序号查找资源的地址
*	@param	
*		@arg	root	根节点
*		@arg	value	序号
*	@retval	获取成功返回资源的地址 ，获取失败返回空
*	@note	资源应该为一个结构体，包含了资源长宽等信息，并且应当是一个
*			动态内存地址
*/
void* search(RB_Node* root, int value);
/**
*	@brief	在红黑树中添加一个节点
*	@param	
*		@arg	root	根节点
*		@arg	value	序号
*		@arg	resource	资源地址
*	@note	资源应该为一个结构体，包含了资源长宽等信息，并且应当是一个
*			动态内存地址。请不要传递为全局或局部变量的地址
*	@retval	操作成功返回空间首地址，否则返回空；
*		@arg	如果返回的结果是0，意味着添加成功
*		@arg	如果返回结果是1，意味着该值已经存在
*		@arg	如果返回结果是-1，意味着内存分配失败或者参数错误
*/
int rbInsert(RB_Node** root, int value,void* resource);
/**
*	@brief	红黑树中通过序号删除节点
*	@param	
*		@arg	root	根节点
*		@arg	value	序号
*	@retval	none
*	@note	该函数会同时删除节点中资源占用的动态内存空间，该函数调用时
*			会消耗较多的栈空间，请注意可能存在的栈溢出问题
*/
void rbDelete(RB_Node** root, int value);
/**
*	@brief	删除整棵红黑树
*	@param	
*		@arg	root	根节点
*	@retval	none
*	@note	该函数会消耗相对较长的时间
*/
void RB_Tree_Free(RB_Node** root);
#endif 
