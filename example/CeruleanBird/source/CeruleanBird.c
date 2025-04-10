#include "CeruleanBird.h"

/**
*	@brief	引擎渲染（这个函数是一个可选项，它将引擎的渲染总线进行了二次封装以方便统一渲染）
*	@param	
*		@arg	camera 摄像机
*	@retval	
*		返回保存到引擎内部的共享数据域
*/
void* CeruleanBird_EngineRender(Camera_Struct* camera){
	if(camera == ((void*)0)){
		return ((void*)0);
	}
	//触发全局统一帧耗时触发器，更新时间
	FrmRS_deltaTime_Trigger();
	
	//摄像机总线渲染	(渲染顺序：背景渲染->墙体纹理渲染)
	Camera_Render(camera);	
	
	//精灵总线渲染(获取保存到引擎内部的共享数据域)
	void* data = Sprites_Exec(camera);
	
	//SSR渲染（）
	SSRAnim_Exec(camera);
	
	return data;
}
