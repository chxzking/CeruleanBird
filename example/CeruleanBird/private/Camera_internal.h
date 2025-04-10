/*当前文件为二层塔*/
#ifndef __CAMERA_INTERNAL_H__
#define __CAMERA_INTERNAL_H__
#include "Camera_API.h"


typedef struct FC_Node_t FC_Node_t;
typedef struct SSR_Frm_t SSR_Frm_t;
typedef struct SSR_Anim_t SSR_Anim_t;
	
//摄像机内部私有数据接口
typedef struct Camera_PrivateData_t{
	void* data; 													//数据
	void (*__FreeHandle)(void* ptr);			//数据回收接口
}Camera_PrivateData_t;

//摄像机结构体定义
typedef struct Camera_Struct{
	//属性1
    double moveSpeed; 	// 移动速度
    double rotSpeed;  	// 旋转速度 每次旋转nPI的值
	//属性2
    vector position;    // 相机位置
    vector direction;   // 相机方向
  	vector plane;		// 相机法平面
	//属性3
	Camera_PrivateData_t  Camera_PrivateData;	//摄像机私有数据
	Camera_PrivateData_t	Engine_Common_Data;	//引擎内部公共通用数据
	//属性4
	DeepBuffer_t* DeepBuffer;//深度缓冲区
	//属性5
	struct{//地面封装
		const unsigned char* floorDescriptArr;//地面属性描述数组（一维映射为二维）  
		int Row;//地面描述数组的行总数
		int Col;//地面描述数组的列总数
		unsigned char renderType;//0为默认渲染（即不渲染地面），1为依据地面属性渲染
		void (*Floor_RenderHandle)(struct Camera_Struct* camera);
	}Floor;
	struct{//天花板封装
		const unsigned char* ceilDescriptArr;//天花板属性描述数组（一维映射为二维）  
		int Row;//天花板描述数组的行总数
		int Col;//天花板描述数组的列总数
		unsigned char renderType;//0为默认渲染（即不渲染天花板），1为依据天花板属性渲染
		void (*Ceil_RenderHandle)(struct Camera_Struct* camera);
	}Ceil;
	struct{//动画数据信息的描述
		int index;//执行的动画索引(大于0的值，0代表当前没有执行任何动画)
		SSR_Anim_t* Anim;//执行的动画
		SSR_Frm_t* ReadyFrm;//预执行帧
		unsigned char AnimType;//动画类型，0为单次动画，1为循环类型动画（长期保留，如果当前执行的动画切换到其他的动画组，该类型不变，需要手动设置更新）
		int currentFrmIndex;//当前执行的帧的索引
		
		double accumulatedTime;//当前累积时间
		
		//顶层渲染位置
		int ROW_Pixel;
		int COL_Pixel;
		void (*SSRAnim_Render)(Camera_Struct* camera);//顶层渲染函数
	}SSR_AnimData;
}Camera_Struct;

//天花板/地板节点的描述节点
struct FC_Node_t{
	int ROW;//保存的图片数组的行数
	int ROW_Logic;//逻辑行（以位为单位）
	int COL;//保存的图片数组的宽度
	const unsigned char* FC_texture;//图片
};
	

//动画帧链表的描述
struct SSR_Frm_t{
	int value;//排序编号
	const unsigned char* frmArr;//帧描述数组
	const unsigned char* shadowFrmArr;//影子描述数组
	struct SSR_Frm_t* next;//下一帧
};

//一个动画的描述（描述编号在红黑树上）
struct SSR_Anim_t{
	int Row;//动画帧数组的行
	int ROW_Logic;//逻辑行（以位为单位）
	int Col;//动画帧数组的列
	int count;//帧数量
	double time;//此动画总持续时间
	double frame_duration;//单帧标准时间
	SSR_Frm_t* FirstFrm;//首帧
	SSR_Frm_t* TailFrm;//尾帧
};


//渲染
void Camera_Render(Camera_Struct* camera);

//深度缓冲区操作
void Camera_DeepBuffer_Write(Camera_Struct* camera,int index,DeepBuffer_t value);
DeepBuffer_t Camera_DeepBuffer_Read(Camera_Struct* camera,int index);


#endif

