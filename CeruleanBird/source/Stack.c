#include "Stack.h"
#include "EngineFault.h"	//引擎错误(故障)处理
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



//压栈
void Stack_Push(Stack_t* stack,void* data) {
	if (stack == C_NULL) {
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	//申请内存
	StackNode_t* node = (StackNode_t*)malloc(sizeof(StackNode_t));
	if (node == C_NULL) {
		//【错误】<资源不足>
		EngineFault_InadequateResources_Handle();
	}
	
	node->data = data;
	node->next = stack->stack_top;
	stack->count++;
	return;
}

//出栈
void* Stack_Pop(Stack_t* stack) {
	if (stack == C_NULL) {
		//【错误】<空指针访问>
		EngineFault_NullPointerGuard_Handle();
	}
	StackNode_t* node = stack->stack_top;
	if (node == C_NULL) {
		return C_NULL;
	}
	stack->stack_top = stack->stack_top->next;
	void* data = node->data;
	free(node);
	stack->count--;
	return data;
}

unsigned int Stack_Size(Stack_t* stack){
	return stack->count;
}
