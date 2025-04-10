#ifndef __VECTOR_H__
#define __VECTOR_H__

//�����ṹ��
typedef struct vector{
    double x, y;
}vector;

/**
*	@brief	����һ������
*	@param	ѡ������������
*			@arg	x -->	������
*			@arg	y -->	������
*	@retval	���������ṹ��
*/
vector vector_Create(double x, double y);
/**
*	@brief	�������������ĺ�
*	@param	ѡ��������
*			@arg	v1 -->	����1�ĵ�ַ
*			@arg	v2 -->	����2�ĵ�ַ
*	@retval	�����������ṹ��
*/
vector vector_Add(const vector* v1, const vector* v2);
/**
*	@brief	�������������Ĳ�
*	@param	ѡ��������
*			@arg	v1 -->	����������1�ĵ�ַ
*			@arg	v2 -->	��������2�ĵ�ַ
*	@note	���������ܷ���ֻ����v1-v2
*	@retval	�����������ṹ��
*/
vector vector_Sub(const vector* v1, const vector* v2);
/**
*	@brief	�������������ĵ��
*	@param	ѡ��������
*			@arg	v1 -->	����������1�ĵ�ַ
*			@arg	v2 -->	��������2�ĵ�ַ
*	@retval	���ص��
*/
double vector_Dot(const vector* v1, const vector* v2);
/**
*	@brief	����һ��������һ�������Ļ�
*	@param	ѡ��������
*			@arg	v -->	����
*			@arg	k -->	����
*	@retval	������������
*/
vector vector_Scalar_Mul(const vector* v, double k);
/**
*	@brief	�����������������
*	@param	ѡ��������
*			@arg	v -->	����
*			@arg	k -->	����
*	@retval	������������
*/
vector vector_Scalar_Div(const vector* v, double k);
/**
*	@brief	��������ģ����ƽ��
*	@param	ѡ��������
*			@arg	v -->	����
*	@retval	����ģ����ƽ��
*/
double vector_MagnitudeSquare(const vector* v);
/**
*	@brief	����������ģ��
*	@param	ѡ��������
*			@arg	v -->	����
*	@retval	��������ģ��
*/
double vector_Magnitude(const vector* v) ;
/**
*	@brief	��һ������
*	@param	ѡ��������
*			@arg	v -->	����
*	@retval	���ع�һ��������
*/
vector vector_Normalize(vector* v);
/**
*	@brief	��������ȡ��
*	@param	ѡ��������
*			@arg	v -->	����
*	@retval	���ط���������
*/
vector vector_Negate(const vector* v);

#endif
