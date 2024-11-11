#ifndef __ENGINE_FAULT_H__
#define __ENGINE_FAULT_H__




//内存非法访问
void EngineFault_IllegalAccess_Handle(void);

//空指针访问
void EngineFault_NullPointerGuard_Handle(void);

//动态内存非法释放
void EngineFault_IllegalFree_Handle(void);

//资源不足
void EngineFault_InadequateResources_Handle(void);
#endif
