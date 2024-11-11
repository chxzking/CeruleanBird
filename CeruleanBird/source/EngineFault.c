#include "EngineFault.h"	//引擎错误(故障)处理

/********************************************
【忠告】
	1、 引擎故障函数，应该只能在引擎状态处于了
		完全无法处理的错误下才能使用，因为这些
		函数的初衷是[应对无法处理的错误，防止
		硬件出现更大的不可预知的错误]
	2、引擎故障函数应该少用，因为在单片机环境下，
		引擎故障会导致机器卡死
		
*********************************************/


/**
	 @brief 检测到内存越界访问以及非法访问

	//【错误】<非法访问>
	EngineFault_IllegalAccess_Handle();
*/
void EngineFault_IllegalAccess_Handle(void){
	
	while(1){
		
	}
	
}

/**
	 @brief 检测到空指针访问

	//【错误】<空指针访问>
	EngineFault_NullPointerGuard_Handle();
*/
void EngineFault_NullPointerGuard_Handle(void){
	
	while(1){
		
	}
}


/**
	 @brief 多次释放同一个动态内存区、或者释放非动态内存地址
		@note 非动态内存定义：
				1、局部变量、全局变量等
				2、由于非法操作导致动态<内存控制块>结构被破坏以至于无法被正确识别为动态内存区。
	//【错误】<动态内存非法释放>
	EngineFault_IllegalFree_Handle();
*/
void EngineFault_IllegalFree_Handle(void){
	
	while(1){
		
	}
}


/**
	 @brief 由于引擎缺乏必要的内存等资源，导致无法正常运行
	//【错误】<资源不足>
	EngineFault_InadequateResources_Handle();
*/
void EngineFault_InadequateResources_Handle(void){
	
	while(1){
		
	}
}
