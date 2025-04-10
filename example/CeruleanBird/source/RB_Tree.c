#include "RB_Tree.h"

#include "engine_config.h"

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


//���������
//�ڵ㴴��
RB_Node* createNode(int value,void* resource) {
    RB_Node* node = (RB_Node*)malloc(sizeof(RB_Node));
    //����Ƿ����ʧ��
    if (node == C_NULL) {
        return node;
    }
    //���ݳ�ʼ��
    node->resource = resource;
    //�ڵ��ʼ��
    node->value = value;
    node->right = C_NULL;
    node->left = C_NULL;
    node->parent = C_NULL;
    node->color = RB_RED; // �½ڵ�Ĭ��Ϊ��ɫ
    //���ؽڵ�
    return node;
}
//����
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
//����
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

//����
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

//���������
//�����������
void AdjustInsert(RB_Node** root, RB_Node* k) {
    //��������ڵ�
    RB_Node* u;
    //ѭ����������ǰ�ڵ�ĸ��׽ڵ���ڣ�����Ϊ��ɫ
    while (k->parent && k->parent->color == RB_RED) {
        //�����׽ڵ������Ƚڵ����ڵ�ʱ
        if (k->parent == k->parent->parent->left) {
            // ��ȡ����ڵ�
            u = k->parent->parent->right;
            //����ڵ�Ϊ��ɫ
            if (u && u->color == RB_RED) {
                //���׽ڵ���
                k->parent->color = RB_BLACK;
                //����ڵ���
                u->color = RB_BLACK;
                //���Ƚڵ��ɺ�ɫ
                k->parent->parent->color = RB_RED;
                //����ǰ�ڵ����������Ƚڵ��λ��
                k = k->parent->parent;
            }
            //����ڵ�Ϊ��ɫ
            else {
                //�����ת����
                if (k == k->parent->right) {
                    //������ҽڵ���룬��LR�ͣ��Ƚ���������ת��ΪLL��
                    k = k->parent;
                    leftRotate(root, k);
                }
                // LL�ʹ���
                k->parent->color = RB_BLACK;
                k->parent->parent->color = RB_RED;
                rightRotate(root, k->parent->parent);
            }
        }
        //�����׽ڵ������Ƚڵ���ҽڵ�ʱ
        else {
            u = k->parent->parent->left; // ����ڵ�
            // �����Ǻ�ɫ
            if (u && u->color == RB_RED) {
                //���׽ڵ���
                k->parent->color = RB_BLACK;
                //����ڵ���
                u->color = RB_BLACK;
                //���Ƚڵ��ɺ�ɫ
                k->parent->parent->color = RB_RED;
                //����ǰ�ڵ����������Ƚڵ��λ��
                k = k->parent->parent;
            }
            //����ڵ�Ϊ��ɫ
            else {
                //�����ת����
                if (k == k->parent->left) {
                    //�������ڵ���룬��RL�ͣ��Ƚ���������ת��ΪRR��
                    k = k->parent;
                    rightRotate(root, k);
                }
                // RR�ʹ���	
                k->parent->color = RB_BLACK;
                k->parent->parent->color = RB_RED;
                leftRotate(root, k->parent->parent);
            }
        }

    }
    (*root)->color = RB_BLACK; // ���ڵ�ʼ��Ϊ��ɫ
}

//�ں�����в���һ���ڵ�
int rbInsert(RB_Node** root, int value,void* resource) {
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //�������涯̬�ڴ�������
		memory_pool_init();
	#endif	//�������涯̬�ڴ�������
	
    if (resource == C_NULL)  return -1;
    /*Ѱ�Ҳ���λ��*/
    RB_Node* y = C_NULL;
    RB_Node* x = *root;
    while (x != C_NULL) {
        y = x;
        //���Ŀ��ֵС�ڽڵ�ֵ�������
        if (value < x->value) {
            x = x->left;
        }
        //���Ŀ��ֵ���ڽڵ�ֵ���ұ���
        else if (value > x->value) {
            x = x->right;
        }
        //�����ǰֵС�ڽڵ�ֵ���˳�
        else {
            return 1;//��ǰֵ�Ѿ�����
        }
    }
    //������ִ�е��˴�����ζ��ͨ����֤�������˺��ʵ�λ�ã���ʱ�������һ���½ڵ�
    //����һ���ڵ�
    RB_Node* node = createNode(value,resource);
    if (node == C_NULL) {
        return -1;//��������ڴ�ʧ��
    }

    /*�ڵ����*/
    node->parent = y;//���¸��׽ڵ�
    if (y == C_NULL) {
        *root = node; // ���ǿյ�
    }
    else if (node->value < y->value) {
        y->left = node;
    }
    else {
        y->right = node;
    }
    /*���ĵ���*/
    AdjustInsert(root, node);
    return 0;
}

//�����ɾ��



//����

RB_Node* rotateLeft(RB_Node* root) {

    //���ڵ�ΪNULL��ֱ���˳�

    if (root == C_NULL) return C_NULL;

    //����һ���µĸ��ڵ�

    RB_Node* newRoot = root->right;//�¸��ڵ㲻���ܳ���C_NULL���

    //�����¸��ڵ㸸��

    newRoot->parent = root->parent;

    //�������Һ�����ڵ�ĸ���

    root->right = newRoot->left;

    if (newRoot->left != C_NULL) {

        newRoot->left->parent = root;

    }

    //�����ɸ��ڵ㸸��

    newRoot->left = root;

    root->parent = newRoot;

    return newRoot;

}

//����

RB_Node* rotateRight(RB_Node* root) {

    //���ڵ�ΪC_NULL��ֱ���˳�

    if (root == C_NULL) return C_NULL;

    RB_Node* newRoot = root->left;//�¸��ڵ㲻���ܳ���C_NULL���

    //�����¸��ڵ㸸��

    newRoot->parent = root->parent;

    //�����������ҽڵ�ĸ���

    root->left = newRoot->right;

    if (newRoot->right != C_NULL) {

        newRoot->right->parent = root;

    }

    //�����ɸ��ڵ㸸��

    newRoot->right = root;

    root->parent = newRoot;

    return newRoot;

}

//�������ĸ��ڵ�

RB_Node* getRoot(RB_Node* node) {

    if (node == C_NULL) return C_NULL;

    while (node->parent != C_NULL) {

        node = node->parent;

    }

    return node;

}

//��ȡ�ֵܽڵ�

RB_Node* brotherNode(RB_Node* node) {

    //У�鸸�׽ڵ����ȷ��

    if (node->parent == C_NULL) return C_NULL;

    //��ѯ�ֵܽڵ�

    if (node != node->parent->left) {

        return node->parent->left;

    }

    else {

        return node->parent->right;

    }

}

//��ȡ�ڵ���ɫ

RB_COLOR NodeColor(RB_Node* node) {

    return (node == C_NULL) ? RB_BLACK : node->color;

}
/*��һ���֣��ڵ����*/

//��̽�������

RB_Node* HandleNodeWithTwoChildren(RB_Node* node) {

    //У��

    if (node == C_NULL) return C_NULL;

    //����

    RB_Node* temp = node;

    node = node->right;

    while (node->left != C_NULL) {

        node = node->left;

    }

    temp->value = node->value;

    return node;

}

//Ѱ��ɾ���ڵ�

RB_Node* FindTargetNode(RB_Node* root, int data) {

    //�жϺ�����Ƿ����

    if (root == C_NULL) return C_NULL;

    //����һ�����ڱ����ı���

    RB_Node* cur = root;

    //����

    while (cur != C_NULL) {

        //���Ŀ��ڵ�С�ڵ�ǰ�ڵ��ֵ,��ô�������

        if (data < cur->value) {

            cur = cur->left;

        }

        //���Ŀ��ڵ���ڵ�ǰ�ڵ��ֵ,��ô���ұ���

        else if (data > cur->value) {

            cur = cur->right;

        }

        //�ҵ�Ŀ��ڵ�

        else {

            //�����Ϊ2��Ҫ���ж��⴦��

            if (cur->left != C_NULL && cur->right != C_NULL) {

                cur = HandleNodeWithTwoChildren(cur);

            }

            //�����ҵ��Ľڵ�

            return cur;

        }

    }

    //û���ҵ�Ŀ��ڵ�

    return C_NULL;

}

/*�ڶ����֣�ɾ���������*/

//ɾ����Ϊ1�ĵ�

RB_Node* DeleteNodeWithOneChild(RB_Node* target) {

    //��ȡ���ӽڵ�

    RB_Node* child = (target->left != C_NULL) ? (target->left) : (target->right);

    //���ӽڵ���

    child->color = RB_BLACK;

    //���ɾ�����Ǹ��ڵ�

    if (target->parent == C_NULL) {

        child->parent = C_NULL;
        free(target);

        return child;

    }

    //���ɾ���Ľڵ�����ͨ�ڵ�

    child->parent = target->parent;

    //���target�Ǹ��׽ڵ������

    if (target == target->parent->left) {

        target->parent->left = child;

    }

    //���target�Ǹ��׽ڵ���Һ���

    else {

        target->parent->right = child;

    }

    free(target);

    return child;

}



//��Ϊ0�Ĵ��ࣺ���ɾ���ڵ��Ǻ�ɫ

RB_Node* DeleteRedLeafNode(RB_Node* target) {

    RB_Node* father = target->parent;

    (father->left == target) ? (father->left = C_NULL) : (father->right = C_NULL);

    free(target);

    return father;

}



//����ֵܽڵ��Ǻ�ɫ���Ҵ��ں�ɫ�ڵ㣨���ص�����ľֲ��������ڵ㣩

RB_Node* DeleteWhenHasRedNephews(RB_Node* brother) {

    //��ȡ���Ƚڵ�

    RB_Node* ancester = brother->parent->parent;

    //��ȡ���׽ڵ�

    RB_Node* father = brother->parent;

    RB_Node* father_copy = father;



    //LL��

    if (brother == father->left && NodeColor(brother->left) == RB_RED) {

        //��ɫ�޸�

        brother->left->color = brother->color;

        brother->color = father->color;

        father->color = RB_BLACK;

        //��ת

        father = rotateRight(father);

    }

    //RR��

    else if (brother == father->right && NodeColor(brother->right) == RB_RED) {

        //��ɫ�޸�

        brother->right->color = brother->color;

        brother->color = father->color;

        father->color = RB_BLACK;

        //��ת

        father = rotateLeft(father);

    }

    //LR��

    else if (brother == father->left && NodeColor(brother->left) == RB_BLACK) {

        //�״ε�����ת��ΪLL��

        brother = rotateLeft(brother);

        father->left = brother;

        //������ת

        father = rotateRight(father);

    }

    //RL��

    else if (brother == father->right && NodeColor(brother->right) == RB_BLACK) {

        //�״ε�����ת��ΪRR��

        brother = rotateRight(brother);

        father->right = brother;

        //������ת

        father = rotateLeft(father);

    }

    //����father�ڵ������Ƚڵ�����������

    //������Ȳ����ڣ���ֱ�ӷ���

    if (ancester == C_NULL) {

        return father;

    }

    //������׽ڵ������ȵ�����

    if (ancester->left == father_copy) {

        ancester->left = father;

    }

    //������׽ڵ������ȵ��Һ���

    else {

        ancester->right = father;

    }

    return father;

}



//���ɾ�����Ǻ�ɫ�ڵ㣬���ֵ�Ϊ��ɫ�����ص�����ľֲ��������ڵ㣩

RB_Node* DeleteBlackLeafNodeWithRedSibling(RB_Node* brother) {

    //��������

    RB_Node* father = brother->parent;

    RB_Node* ancestor = father->parent;

    RB_Node* resultNode = father;

    //��ɫ����

    RB_COLOR temp = brother->color;

    brother->color = father->color;

    father->color = temp;



    //��ת����

    if (brother == father->left) {//����

        father = rotateRight(father);

    }

    else if (brother == father->right) {//����

        father = rotateLeft(father);

    }

    //���׽ڵ����������ĸ��ڵ�

    if (ancestor == C_NULL) {

        return resultNode;

    }

    //�������Ƚڵ��븸�׽ڵ�Ĺ�ϵ

    (ancestor->left == resultNode) ? (ancestor->left = father) : (ancestor->right = father);

    return resultNode;

}
//ɾ������

RB_Node* TreeDelete(RB_Node* root, int data) {

    //��ȡĿ��ڵ�

    RB_Node* cur = FindTargetNode(root, data);

    if (cur == C_NULL) return root;//�ڵ���ҳ���
	//ɾ���ڵ���Դ
	free(cur->resource);
    //���ݶȵ�������ദ��
    
    //�����Ϊ0

    if (cur->left == C_NULL && cur->right == C_NULL) {

        //���ɾ�����Ǹ��ڵ�

        if (cur->parent == C_NULL) {

            free(cur);

            return C_NULL;

        }

        //�۲�ɾ���Ľڵ��Ƿ�Ϊ��ɫ�����ɾ���ڵ��Ǻ�ɫ

        if (cur->color == RB_RED) {

            cur = DeleteRedLeafNode(cur);

            return getRoot(cur);

        }

        //���ɾ���ڵ��Ǻ�ɫ

        //��ȡ�ֵܽڵ�

        RB_Node* brother = brotherNode(cur);

        //ɾ������

        (cur == cur->parent->left) ? (cur->parent->left = C_NULL) : (cur->parent->right = C_NULL);

        free(cur);

        cur = C_NULL;

        //����

        while (1) {

            //�۲��ֵܽڵ��Ƿ�Ϊ��ɫ�����Ϊ��ɫ

            if (NodeColor(brother) == RB_BLACK) {

                //�ж��ֵܽڵ��Ƿ���ں�ɫ���ӣ���������ں�ɫ

                if (NodeColor(brother->left) == RB_BLACK && NodeColor(brother->right) == RB_BLACK) {

                    brother->color = RB_RED;//�ֵܽڵ��ɺ�ɫ

                    //��鸸�׽ڵ��Ƿ�Ϊ��ɫ

                    if (NodeColor(brother->parent) == RB_RED) {

                        brother->parent->color = RB_BLACK;//���׽ڵ���

                        return getRoot(brother);

                    }

                    //������׽ڵ�Ϊ��ɫ������������������ĸ��ڵ�ʱ

                    else if (NodeColor(brother->parent) == RB_BLACK && brother->parent->parent == C_NULL) {

                        return brother->parent;//���ڸ��׽ڵ��Ǹ��ڵ㣬ֱ�ӷ���

                    }

                    //������׽ڵ�Ϊ��ɫ�����Ҹ��׽ڵ�֮�ϴ��ڸ���ά�ȵ���

                    else {

                        //�����ֵܽڵ㣬ע������������߲�ε�ά����

                        RB_Node* father = brother->parent;

                        RB_Node* ancestor = father->parent;

                        brother = father != ancestor->left ? ancestor->left : ancestor->right;

                    }

                }

                //���Ϊ���ں�ɫ

                else {

                    //printf("�ҽ�����0����˫�ڵ�λ��\n");

                    cur = DeleteWhenHasRedNephews(brother);

                    return getRoot(cur);

                }

            }

            //����Ǻ�ɫ

            else {

                RB_Node* temp = DeleteBlackLeafNodeWithRedSibling(brother);

                brother = temp->left ? temp->left : temp->right;

            }

        }

    }

    //�����Ϊ1

    else {

        cur = DeleteNodeWithOneChild(cur);

        return getRoot(cur);

    }

}

//ɾ�����ⲿת�ӽӿڣ�
void rbDelete(RB_Node** root, int value) {
    *root = TreeDelete(*root, value);
}





//ɾ�����ú����
void RB_Tree_Free(RB_Node** root){
	if(root == C_NULL)	return;
	int value;
	while((*root) != C_NULL){
		value = (*root)->value;
		rbDelete(root, value);
	}
}
