#ifndef __STACK_H__
#define __STACK_H__
#include "Stack_API.h"

//ջ�ṹ
typedef struct StackNode_t {
	void* data;
	struct StackNode_t* next;
}StackNode_t;

//ջ
struct Stack_t {
	unsigned int count;//ջ�ĳ�Ա��
	StackNode_t* stack_top;//ջ��
};


#endif
