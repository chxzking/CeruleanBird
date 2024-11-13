#ifndef __STACK_H__
#define __STACK_H__
#include "Stack_API.h"

//栈结构
typedef struct StackNode_t {
	void* data;
	struct StackNode_t* next;
}StackNode_t;

//栈
struct Stack_t {
	unsigned int count;//栈的成员数
	StackNode_t* stack_top;//栈顶
};


#endif
