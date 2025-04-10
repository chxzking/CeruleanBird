#include "RB_Tree.h"

#include "engine_config.h"

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_0          //启用标准库堆解决方案
#include <malloc.h>
#define C_NULL NULL
#endif	//启用标准库堆解决方案

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //启用引擎动态内存解决方案
#include "heap_solution_1.h"

//宏定义统一函数接口
#define malloc(size) c_malloc(size)
#define realloc(ptr,size) c_realloc(ptr,size)
#define free(ptr)	c_free(ptr)
#endif	//启用引擎动态内存解决方案


//红黑树函数
//节点创建
RB_Node* createNode(int value,void* resource) {
    RB_Node* node = (RB_Node*)malloc(sizeof(RB_Node));
    //检查是否分配失败
    if (node == C_NULL) {
        return node;
    }
    //数据初始化
    node->resource = resource;
    //节点初始化
    node->value = value;
    node->right = C_NULL;
    node->left = C_NULL;
    node->parent = C_NULL;
    node->color = RB_RED; // 新节点默认为红色
    //返回节点
    return node;
}
//左旋
void leftRotate(RB_Node** root, RB_Node* x) {
    RB_Node* y = x->right;
    x->right = y->left;
    if (y->left != C_NULL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == C_NULL) {
        *root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}
//右旋
void rightRotate(RB_Node** root, RB_Node* y) {
    RB_Node* x = y->left;
    y->left = x->right;
    if (x->right != C_NULL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == C_NULL) {
        *root = x;
    }
    else if (y == y->parent->right) {
        y->parent->right = x;
    }
    else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

//查找
void* search(RB_Node* root, int value) {
    while (root != C_NULL && root->value != value) {
        if (value < root->value) {
            root = root->left;
        }
        else {
            root = root->right;
        }
    }
	if(root == C_NULL){
		return C_NULL;
	}
    return root->resource;
}

//红黑树插入
//插入调整函数
void AdjustInsert(RB_Node** root, RB_Node* k) {
    //定义叔叔节点
    RB_Node* u;
    //循环条件，当前节点的父亲节点存在，并且为红色
    while (k->parent && k->parent->color == RB_RED) {
        //当父亲节点是祖先节点的左节点时
        if (k->parent == k->parent->parent->left) {
            // 获取叔叔节点
            u = k->parent->parent->right;
            //叔叔节点为红色
            if (u && u->color == RB_RED) {
                //父亲节点变黑
                k->parent->color = RB_BLACK;
                //叔叔节点变黑
                u->color = RB_BLACK;
                //祖先节点变成红色
                k->parent->parent->color = RB_RED;
                //将当前节点提升到祖先节点的位置
                k = k->parent->parent;
            }
            //叔叔节点为黑色
            else {
                //检查旋转类型
                if (k == k->parent->right) {
                    //如果是右节点插入，是LR型，先将子树左旋转化为LL型
                    k = k->parent;
                    leftRotate(root, k);
                }
                // LL型处理
                k->parent->color = RB_BLACK;
                k->parent->parent->color = RB_RED;
                rightRotate(root, k->parent->parent);
            }
        }
        //当父亲节点是祖先节点的右节点时
        else {
            u = k->parent->parent->left; // 叔叔节点
            // 叔叔是红色
            if (u && u->color == RB_RED) {
                //父亲节点变黑
                k->parent->color = RB_BLACK;
                //叔叔节点变黑
                u->color = RB_BLACK;
                //祖先节点变成红色
                k->parent->parent->color = RB_RED;
                //将当前节点提升到祖先节点的位置
                k = k->parent->parent;
            }
            //叔叔节点为黑色
            else {
                //检查旋转类型
                if (k == k->parent->left) {
                    //如果是左节点插入，是RL型，先将子树左旋转化为RR型
                    k = k->parent;
                    rightRotate(root, k);
                }
                // RR型处理	
                k->parent->color = RB_BLACK;
                k->parent->parent->color = RB_RED;
                leftRotate(root, k->parent->parent);
            }
        }

    }
    (*root)->color = RB_BLACK; // 根节点始终为黑色
}

//在红黑树中插入一个节点
int rbInsert(RB_Node** root, int value,void* resource) {
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //启用引擎动态内存解决方案
		memory_pool_init();
	#endif	//启用引擎动态内存解决方案
	
    if (resource == C_NULL)  return -1;
    /*寻找插入位置*/
    RB_Node* y = C_NULL;
    RB_Node* x = *root;
    while (x != C_NULL) {
        y = x;
        //如果目标值小于节点值，左遍历
        if (value < x->value) {
            x = x->left;
        }
        //如果目标值大于节点值，右遍历
        else if (value > x->value) {
            x = x->right;
        }
        //如果当前值小于节点值，退出
        else {
            return 1;//当前值已经存在
        }
    }
    //当代码执行到此处，意味着通过验证并到达了合适的位置，此时请求插入一个新节点
    //创建一个节点
    RB_Node* node = createNode(value,resource);
    if (node == C_NULL) {
        return -1;//请求分配内存失败
    }

    /*节点插入*/
    node->parent = y;//更新父亲节点
    if (y == C_NULL) {
        *root = node; // 树是空的
    }
    else if (node->value < y->value) {
        y->left = node;
    }
    else {
        y->right = node;
    }
    /*树的调整*/
    AdjustInsert(root, node);
    return 0;
}

//红黑树删除



//左旋

RB_Node* rotateLeft(RB_Node* root) {

    //根节点为NULL则直接退出

    if (root == C_NULL) return C_NULL;

    //创建一个新的根节点

    RB_Node* newRoot = root->right;//新根节点不可能出现C_NULL情况

    //更新新根节点父亲

    newRoot->parent = root->parent;

    //调整旧右孩子左节点的父亲

    root->right = newRoot->left;

    if (newRoot->left != C_NULL) {

        newRoot->left->parent = root;

    }

    //调整旧根节点父亲

    newRoot->left = root;

    root->parent = newRoot;

    return newRoot;

}

//右旋

RB_Node* rotateRight(RB_Node* root) {

    //根节点为C_NULL则直接退出

    if (root == C_NULL) return C_NULL;

    RB_Node* newRoot = root->left;//新根节点不可能出现C_NULL情况

    //更新新根节点父亲

    newRoot->parent = root->parent;

    //调整旧左孩子右节点的父亲

    root->left = newRoot->right;

    if (newRoot->right != C_NULL) {

        newRoot->right->parent = root;

    }

    //调整旧根节点父亲

    newRoot->right = root;

    root->parent = newRoot;

    return newRoot;

}

//返回树的根节点

RB_Node* getRoot(RB_Node* node) {

    if (node == C_NULL) return C_NULL;

    while (node->parent != C_NULL) {

        node = node->parent;

    }

    return node;

}

//获取兄弟节点

RB_Node* brotherNode(RB_Node* node) {

    //校验父亲节点的正确性

    if (node->parent == C_NULL) return C_NULL;

    //查询兄弟节点

    if (node != node->parent->left) {

        return node->parent->left;

    }

    else {

        return node->parent->right;

    }

}

//获取节点颜色

RB_COLOR NodeColor(RB_Node* node) {

    return (node == C_NULL) ? RB_BLACK : node->color;

}
/*第一部分：节点查找*/

//后继交换函数

RB_Node* HandleNodeWithTwoChildren(RB_Node* node) {

    //校验

    if (node == C_NULL) return C_NULL;

    //查找

    RB_Node* temp = node;

    node = node->right;

    while (node->left != C_NULL) {

        node = node->left;

    }

    temp->value = node->value;

    return node;

}

//寻找删除节点

RB_Node* FindTargetNode(RB_Node* root, int data) {

    //判断红黑树是否存在

    if (root == C_NULL) return C_NULL;

    //声明一个用于遍历的变量

    RB_Node* cur = root;

    //遍历

    while (cur != C_NULL) {

        //如果目标节点小于当前节点的值,那么往左遍历

        if (data < cur->value) {

            cur = cur->left;

        }

        //如果目标节点大于当前节点的值,那么往右遍历

        else if (data > cur->value) {

            cur = cur->right;

        }

        //找到目标节点

        else {

            //如果度为2则要进行额外处理

            if (cur->left != C_NULL && cur->right != C_NULL) {

                cur = HandleNodeWithTwoChildren(cur);

            }

            //返回找到的节点

            return cur;

        }

    }

    //没有找到目标节点

    return C_NULL;

}

/*第二部分：删除及其调整*/

//删除度为1的点

RB_Node* DeleteNodeWithOneChild(RB_Node* target) {

    //获取孩子节点

    RB_Node* child = (target->left != C_NULL) ? (target->left) : (target->right);

    //孩子节点变黑

    child->color = RB_BLACK;

    //如果删除的是根节点

    if (target->parent == C_NULL) {

        child->parent = C_NULL;
        free(target);

        return child;

    }

    //如果删除的节点是普通节点

    child->parent = target->parent;

    //如果target是父亲节点的左孩子

    if (target == target->parent->left) {

        target->parent->left = child;

    }

    //如果target是父亲节点的右孩子

    else {

        target->parent->right = child;

    }

    free(target);

    return child;

}



//度为0的大类：如果删除节点是红色

RB_Node* DeleteRedLeafNode(RB_Node* target) {

    RB_Node* father = target->parent;

    (father->left == target) ? (father->left = C_NULL) : (father->right = C_NULL);

    free(target);

    return father;

}



//如果兄弟节点是黑色并且存在红色节点（返回调整后的局部子树根节点）

RB_Node* DeleteWhenHasRedNephews(RB_Node* brother) {

    //获取祖先节点

    RB_Node* ancester = brother->parent->parent;

    //获取父亲节点

    RB_Node* father = brother->parent;

    RB_Node* father_copy = father;



    //LL型

    if (brother == father->left && NodeColor(brother->left) == RB_RED) {

        //颜色修改

        brother->left->color = brother->color;

        brother->color = father->color;

        father->color = RB_BLACK;

        //旋转

        father = rotateRight(father);

    }

    //RR型

    else if (brother == father->right && NodeColor(brother->right) == RB_RED) {

        //颜色修改

        brother->right->color = brother->color;

        brother->color = father->color;

        father->color = RB_BLACK;

        //旋转

        father = rotateLeft(father);

    }

    //LR型

    else if (brother == father->left && NodeColor(brother->left) == RB_BLACK) {

        //首次调整，转化为LL型

        brother = rotateLeft(brother);

        father->left = brother;

        //二次旋转

        father = rotateRight(father);

    }

    //RL型

    else if (brother == father->right && NodeColor(brother->right) == RB_BLACK) {

        //首次调整，转化为RR型

        brother = rotateRight(brother);

        father->right = brother;

        //二次旋转

        father = rotateLeft(father);

    }

    //将新father节点与祖先节点连接起来。

    //如果祖先不存在，则直接返回

    if (ancester == C_NULL) {

        return father;

    }

    //如果父亲节点是祖先的左孩子

    if (ancester->left == father_copy) {

        ancester->left = father;

    }

    //如果父亲节点是祖先的右孩子

    else {

        ancester->right = father;

    }

    return father;

}



//如果删除的是黑色节点，且兄弟为红色（返回调整后的局部子树根节点）

RB_Node* DeleteBlackLeafNodeWithRedSibling(RB_Node* brother) {

    //声明变量

    RB_Node* father = brother->parent;

    RB_Node* ancestor = father->parent;

    RB_Node* resultNode = father;

    //颜色交换

    RB_COLOR temp = brother->color;

    brother->color = father->color;

    father->color = temp;



    //旋转调整

    if (brother == father->left) {//右旋

        father = rotateRight(father);

    }

    else if (brother == father->right) {//左旋

        father = rotateLeft(father);

    }

    //父亲节点是整棵树的根节点

    if (ancestor == C_NULL) {

        return resultNode;

    }

    //更新祖先节点与父亲节点的关系

    (ancestor->left == resultNode) ? (ancestor->left = father) : (ancestor->right = father);

    return resultNode;

}
//删除函数

RB_Node* TreeDelete(RB_Node* root, int data) {

    //获取目标节点

    RB_Node* cur = FindTargetNode(root, data);

    if (cur == C_NULL) return root;//节点查找出错
	//删除节点资源
	free(cur->resource);
    //根据度的情况分类处理
    
    //如果度为0

    if (cur->left == C_NULL && cur->right == C_NULL) {

        //如果删除的是根节点

        if (cur->parent == C_NULL) {

            free(cur);

            return C_NULL;

        }

        //观察删除的节点是否为红色，如果删除节点是红色

        if (cur->color == RB_RED) {

            cur = DeleteRedLeafNode(cur);

            return getRoot(cur);

        }

        //如果删除节点是黑色

        //获取兄弟节点

        RB_Node* brother = brotherNode(cur);

        //删除操作

        (cur == cur->parent->left) ? (cur->parent->left = C_NULL) : (cur->parent->right = C_NULL);

        free(cur);

        cur = C_NULL;

        //调整

        while (1) {

            //观察兄弟节点是否为黑色，如果为黑色

            if (NodeColor(brother) == RB_BLACK) {

                //判断兄弟节点是否存在红色孩子，如果不存在红色

                if (NodeColor(brother->left) == RB_BLACK && NodeColor(brother->right) == RB_BLACK) {

                    brother->color = RB_RED;//兄弟节点变成红色

                    //检查父亲节点是否为红色

                    if (NodeColor(brother->parent) == RB_RED) {

                        brother->parent->color = RB_BLACK;//父亲节点变黑

                        return getRoot(brother);

                    }

                    //如果父亲节点为黑色，并且是整个红黑树的根节点时

                    else if (NodeColor(brother->parent) == RB_BLACK && brother->parent->parent == C_NULL) {

                        return brother->parent;//由于父亲节点是根节点，直接返回

                    }

                    //如果父亲节点为黑色，并且父亲节点之上存在更高维度的树

                    else {

                        //更新兄弟节点，注意点提升到更高层次的维度上

                        RB_Node* father = brother->parent;

                        RB_Node* ancestor = father->parent;

                        brother = father != ancestor->left ? ancestor->left : ancestor->right;

                    }

                }

                //如果为存在红色

                else {

                    //printf("我进入了0度无双黑的位置\n");

                    cur = DeleteWhenHasRedNephews(brother);

                    return getRoot(cur);

                }

            }

            //如果是红色

            else {

                RB_Node* temp = DeleteBlackLeafNodeWithRedSibling(brother);

                brother = temp->left ? temp->left : temp->right;

            }

        }

    }

    //如果度为1

    else {

        cur = DeleteNodeWithOneChild(cur);

        return getRoot(cur);

    }

}

//删除（外部转接接口）
void rbDelete(RB_Node** root, int value) {
    *root = TreeDelete(*root, value);
}





//删除整棵红黑树
void RB_Tree_Free(RB_Node** root){
	if(root == C_NULL)	return;
	int value;
	while((*root) != C_NULL){
		value = (*root)->value;
		rbDelete(root, value);
	}
}
