#include "Canvas.h"

#include "bit_operate.h"

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

/********************************
���ڻ���������Ŀǰ�����ֲ���
����1��ʹ�������Դ��Ļ�������Ⱦ���
	   �����ݸ��Ƶ��ⲿ�����С�
����2����������ʹ���ض�λ�����ķ�
	   ʽ��������ֱ����Ⱦ���ⲿ����
*********************************/
int Canvas_Mode = DEFAULT_MODE;		//Ĭ��ʹ��<Ĭ��ģʽ>
//����������ָ��
ScreenType* Screen_Canvas = C_NULL;
/**
*	@brief	���ڳ�ʼ���������Ӷ�����ȡָ����С�Ŀռ�
*	@param	�޲�
*	@retval	�ռ�����ɹ�������0�����ʧ�ܷ���-1��
*/
int Canvas_Init(void){
	//�Ӷ�����ȡ�����ռ�
	#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1          //�������涯̬�ڴ�������
		//�˷�����Ҫ���ó�ʼ��������ʼ������
		memory_pool_init();
	#endif
	//Ĭ��ģʽ
	if(Canvas_Mode == DEFAULT_MODE){
		Screen_Canvas = (ScreenType*)malloc( (SCREEN_ROW/8) * SCREEN_COLUMN * sizeof(ScreenType));
		if(Screen_Canvas == NULL) return -1;
		//����ֵ��0
		for(int i = 0; i < (SCREEN_ROW/8) * SCREEN_COLUMN;i++){
			Screen_Canvas[i] = 0;
		}
		return 0;
	}
	//�ض���ģʽ
	return 0;//�ض���Ļ����ض����ʱ���ʼ��
}
/**
*	@brief	������������������һά������ά�߼���
*	@param	ROW --> ��������
*			COLUMN --> ��������
*			data	-->	д���ֵ��
*	@note ע��ROW��COLUMN����ѭ,������ʹ�������(1,2)��ʵ�ʷ��ʵ��ǵ�2�е�3��Ԫ�ء�
*	@retval	д�뻭���ɹ�������0�����ʧ�ܷ���-1����������飻
*/
int Canvas_Base(int ROW, int COLUMN, ScreenType data) {
	//У�������
	if (ROW >= SCREEN_ROW || ROW < 0)	return -1;
	//У�������
	if (COLUMN >= SCREEN_COLUMN || COLUMN < 0)	return -1;
	//��ά��д������
	Screen_Canvas[ROW * SCREEN_COLUMN + COLUMN] = data;
	//���سɹ���־
	return 0;
}
/**
*	@brief	��ȡ����һ��ָ��λ��Ԫ�صĵ�ַ
*	@param	ROW --> ��������
*			COLUMN --> ��������
*	@note ע��ROW��COLUMN����ѭ,������ʹ�������(1,2)��ʵ�ʷ��ʵ��ǵ�2�е�3��Ԫ�ء�
*	@retval	д�뻭���ɹ������ص�ַ�����ʧ�ܷ��ؿգ�
*/
ScreenType* Canvas_GetUnit(int ROW, int COLUMN){
	//У�������
	if (ROW >= SCREEN_ROW || ROW < 0)	return NULL;
	//У�������
	if (COLUMN >= SCREEN_COLUMN || COLUMN < 0)	return NULL;
	//����
	return &Screen_Canvas[ROW * SCREEN_COLUMN + COLUMN];
}
/**
*	@brief	�ڻ����Ͻ������ص㼶��Ĳ���
*	@param	Pixel_ROW --> ��������
*			Pixel_COL --> ��������
*			OperateNum -->	���ص��״̬
*	@retval	none��
*/
void Canvas_PixelOperate(int Pixel_ROW,int Pixel_COL,CANVAS_PIXEL_STATUS OperateNum){
	//������ص���з������
	if(Pixel_ROW < 0 || Pixel_ROW >= SCREEN_ROW)	return;
	//������ص���з������
	if(Pixel_COL < 0 || Pixel_COL >= SCREEN_COLUMN)		return;
	
	//���ص㶨λ
	int PosByteX = Pixel_ROW/8;//��ȡ��ʵ����
	int PosbitX =  Pixel_ROW%8;//��λ��λ��λ��
	//��ȡĿ���ֽڵĵ�ַ
	ScreenType* temp = Canvas_GetUnit(PosByteX, Pixel_COL);
	//�������ֽ�Ϊ��
	if(OperateNum == CANVAS_PIXEL_CLOSE){
		Bit_Modify_Within_Byte(temp, 7-PosbitX, 0);
	}
	//�������ֽ�Ϊ��
	else if(OperateNum == CANVAS_PIXEL_OPEN){
		Bit_Modify_Within_Byte(temp, 7-PosbitX, 1);
	}
	return;
}
/**
*	@brief	����ˢ��,��֮ǰ�Ļ�����Ϣȫ�����
*	@param	none
*	@retval	none
*/
void Canvas_Refresh(void){
	for(int i = 0;i<(SCREEN_ROW/8) * SCREEN_COLUMN;i++){
		Screen_Canvas[i] = 0;
	}
}
/**
*		@note	ע�⣺�ض�����뷢���ڻ�����ʼ��֮ǰ
*	@brief	���û���ģʽΪ�ض���ģʽ
*	@param	NewCanvasBase -->	�ض���Ļ���
*	@retval	����ڳ�ʼ��֮����ã��᷵�ش���ֵ-1�����ִ�гɹ���᷵��0
*/
int Canvas_SetCanvasModeToRedirect(unsigned char* NewCanvasBuffer){
	//��⻭���Ƿ��Ѿ���ʼ��
	if(Screen_Canvas != C_NULL){
		return -1;//����Ѿ���ʼ�����ˣ���ô�ͷ��ش���
	}
	Canvas_Mode = REDIRECT_MODE;
	Screen_Canvas = NewCanvasBuffer;
	return 0;
}
/**
*		@note	ע�⣬���ʹ�����ض�����ԣ��ض�����Ƕ�̬���棬����������ȥ�ͷ��������
*	@brief	��������ռ��ͷ�
*	@param	none
*	@retval	none
*/
void Canvas_Free(void){
	//Ĭ�ϲ���
	if(Canvas_Mode == DEFAULT_MODE){
		free(Screen_Canvas);
		Screen_Canvas = C_NULL;
		return;
	}
	//�ض������
	//���ò���
	Canvas_Mode = DEFAULT_MODE;
	return;
}
