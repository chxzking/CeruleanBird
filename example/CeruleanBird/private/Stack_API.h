#ifndef __STACK_API_H__
#define __STACK_API_H__

//ջ
typedef struct Stack_t Stack_t;

/**
*	@brief	ѹջ
*	@param	
*		@arg	stack	ջ��ַ
*		@arg	data	ջ����
*	@retval	none��
*/
void Stack_Push(Stack_t* stack,void* data);
/**
*	@brief	��ջ
*	@param	
*		@arg	stack	ջ��ַ
*	@retval	ջ���ݣ�
*/
void* Stack_Pop(Stack_t* stack);
/**
*	@brief	��ȡջ��
*	@param	
*		@arg	stack	ջ��ַ
*	@retval	ջ�
*/
unsigned int Stack_Size(Stack_t* stack);
#endif
