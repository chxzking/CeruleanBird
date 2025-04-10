#include "CeruleanBird.h"

/**
*	@brief	������Ⱦ�����������һ����ѡ������������Ⱦ���߽����˶��η�װ�Է���ͳһ��Ⱦ��
*	@param	
*		@arg	camera �����
*	@retval	
*		���ر��浽�����ڲ��Ĺ���������
*/
void* CeruleanBird_EngineRender(Camera_Struct* camera){
	if(camera == ((void*)0)){
		return ((void*)0);
	}
	//����ȫ��ͳһ֡��ʱ������������ʱ��
	FrmRS_deltaTime_Trigger();
	
	//�����������Ⱦ	(��Ⱦ˳�򣺱�����Ⱦ->ǽ��������Ⱦ)
	Camera_Render(camera);	
	
	//����������Ⱦ(��ȡ���浽�����ڲ��Ĺ���������)
	void* data = Sprites_Exec(camera);
	
	//SSR��Ⱦ����
	SSRAnim_Exec(camera);
	
	return data;
}
