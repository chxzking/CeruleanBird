#include "Stack.h"
#include "EngineFault.h"	//�������(����)����
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



//ѹջ
void Stack_Push(Stack_t* stack,void* data) {
	if (stack == C_NULL) {
		//������<��ָ�����>
		EngineFault_NullPointerGuard_Handle();
	}
	//�����ڴ�
	StackNode_t* node = (StackNode_t*)malloc(sizeof(StackNode_t));
	if (node == C_NULL) {
		//������<��Դ����>
		EngineFault_InadequateResources_Handle();
	}
	
	node->data = data;
	node->next = stack->stack_top;
	stack->count++;
	return;
}

//��ջ
void* Stack_Pop(Stack_t* stack) {
	if (stack == C_NULL) {
		//������<��ָ�����>
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
