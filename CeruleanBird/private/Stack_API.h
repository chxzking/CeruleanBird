#ifndef __STACK_API_H__
#define __STACK_API_H__

//栈
typedef struct Stack_t Stack_t;

/**
*	@brief	压栈
*	@param	
*		@arg	stack	栈地址
*		@arg	data	栈数据
*	@retval	none；
*/
void Stack_Push(Stack_t* stack,void* data);
/**
*	@brief	出栈
*	@param	
*		@arg	stack	栈地址
*	@retval	栈数据；
*/
void* Stack_Pop(Stack_t* stack);
/**
*	@brief	获取栈深
*	@param	
*		@arg	stack	栈地址
*	@retval	栈深；
*/
unsigned int Stack_Size(Stack_t* stack);
#endif
