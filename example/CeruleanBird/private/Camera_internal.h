/*��ǰ�ļ�Ϊ������*/
#ifndef __CAMERA_INTERNAL_H__
#define __CAMERA_INTERNAL_H__
#include "Camera_API.h"


typedef struct FC_Node_t FC_Node_t;
typedef struct SSR_Frm_t SSR_Frm_t;
typedef struct SSR_Anim_t SSR_Anim_t;
	
//������ڲ�˽�����ݽӿ�
typedef struct Camera_PrivateData_t{
	void* data; 													//����
	void (*__FreeHandle)(void* ptr);			//���ݻ��սӿ�
}Camera_PrivateData_t;

//������ṹ�嶨��
typedef struct Camera_Struct{
	//����1
    double moveSpeed; 	// �ƶ��ٶ�
    double rotSpeed;  	// ��ת�ٶ� ÿ����תnPI��ֵ
	//����2
    vector position;    // ���λ��
    vector direction;   // �������
  	vector plane;		// �����ƽ��
	//����3
	Camera_PrivateData_t  Camera_PrivateData;	//�����˽������
	Camera_PrivateData_t	Engine_Common_Data;	//�����ڲ�����ͨ������
	//����4
	DeepBuffer_t* DeepBuffer;//��Ȼ�����
	//����5
	struct{//�����װ
		const unsigned char* floorDescriptArr;//���������������飨һάӳ��Ϊ��ά��  
		int Row;//�������������������
		int Col;//�������������������
		unsigned char renderType;//0ΪĬ����Ⱦ��������Ⱦ���棩��1Ϊ���ݵ���������Ⱦ
		void (*Floor_RenderHandle)(struct Camera_Struct* camera);
	}Floor;
	struct{//�컨���װ
		const unsigned char* ceilDescriptArr;//�컨�������������飨һάӳ��Ϊ��ά��  
		int Row;//�컨�����������������
		int Col;//�컨�����������������
		unsigned char renderType;//0ΪĬ����Ⱦ��������Ⱦ�컨�壩��1Ϊ�����컨��������Ⱦ
		void (*Ceil_RenderHandle)(struct Camera_Struct* camera);
	}Ceil;
	struct{//����������Ϣ������
		int index;//ִ�еĶ�������(����0��ֵ��0����ǰû��ִ���κζ���)
		SSR_Anim_t* Anim;//ִ�еĶ���
		SSR_Frm_t* ReadyFrm;//Ԥִ��֡
		unsigned char AnimType;//�������ͣ�0Ϊ���ζ�����1Ϊѭ�����Ͷ��������ڱ����������ǰִ�еĶ����л��������Ķ����飬�����Ͳ��䣬��Ҫ�ֶ����ø��£�
		int currentFrmIndex;//��ǰִ�е�֡������
		
		double accumulatedTime;//��ǰ�ۻ�ʱ��
		
		//������Ⱦλ��
		int ROW_Pixel;
		int COL_Pixel;
		void (*SSRAnim_Render)(Camera_Struct* camera);//������Ⱦ����
	}SSR_AnimData;
}Camera_Struct;

//�컨��/�ذ�ڵ�������ڵ�
struct FC_Node_t{
	int ROW;//�����ͼƬ���������
	int ROW_Logic;//�߼��У���λΪ��λ��
	int COL;//�����ͼƬ����Ŀ��
	const unsigned char* FC_texture;//ͼƬ
};
	

//����֡���������
struct SSR_Frm_t{
	int value;//������
	const unsigned char* frmArr;//֡��������
	const unsigned char* shadowFrmArr;//Ӱ����������
	struct SSR_Frm_t* next;//��һ֡
};

//һ����������������������ں�����ϣ�
struct SSR_Anim_t{
	int Row;//����֡�������
	int ROW_Logic;//�߼��У���λΪ��λ��
	int Col;//����֡�������
	int count;//֡����
	double time;//�˶����ܳ���ʱ��
	double frame_duration;//��֡��׼ʱ��
	SSR_Frm_t* FirstFrm;//��֡
	SSR_Frm_t* TailFrm;//β֡
};


//��Ⱦ
void Camera_Render(Camera_Struct* camera);

//��Ȼ���������
void Camera_DeepBuffer_Write(Camera_Struct* camera,int index,DeepBuffer_t value);
DeepBuffer_t Camera_DeepBuffer_Read(Camera_Struct* camera,int index);


#endif

