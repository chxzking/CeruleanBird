#ifndef __VECTOR_H__
#define __VECTOR_H__

//向量结构体
typedef struct vector{
    double x, y;
}vector;

/**
*	@brief	创建一个向量
*	@param	选择向量的坐标
*			@arg	x -->	横坐标
*			@arg	y -->	纵坐标
*	@retval	返回向量结构体
*/
vector vector_Create(double x, double y);
/**
*	@brief	计算两个向量的和
*	@param	选择向量体
*			@arg	v1 -->	向量1的地址
*			@arg	v2 -->	向量2的地址
*	@retval	返回新向量结构体
*/
vector vector_Add(const vector* v1, const vector* v2);
/**
*	@brief	计算两个向量的差
*	@param	选择向量体
*			@arg	v1 -->	被减数向量1的地址
*			@arg	v2 -->	减数向量2的地址
*	@note	参数方向不能反，只能是v1-v2
*	@retval	返回新向量结构体
*/
vector vector_Sub(const vector* v1, const vector* v2);
/**
*	@brief	计算两个向量的点积
*	@param	选择向量体
*			@arg	v1 -->	被减数向量1的地址
*			@arg	v2 -->	减数向量2的地址
*	@retval	返回点积
*/
double vector_Dot(const vector* v1, const vector* v2);
/**
*	@brief	计算一个向量和一个标量的积
*	@param	选择向量体
*			@arg	v -->	向量
*			@arg	k -->	标量
*	@retval	返回新向量体
*/
vector vector_Scalar_Mul(const vector* v, double k);
/**
*	@brief	计算向量与标量的商
*	@param	选择向量体
*			@arg	v -->	向量
*			@arg	k -->	标量
*	@retval	返回新向量体
*/
vector vector_Scalar_Div(const vector* v, double k);
/**
*	@brief	计算向量模长的平方
*	@param	选择向量体
*			@arg	v -->	向量
*	@retval	返回模长的平方
*/
double vector_MagnitudeSquare(const vector* v);
/**
*	@brief	计算向量的模长
*	@param	选择向量体
*			@arg	v -->	向量
*	@retval	返回向量模长
*/
double vector_Magnitude(const vector* v) ;
/**
*	@brief	归一化向量
*	@param	选择向量体
*			@arg	v -->	向量
*	@retval	返回归一化的向量
*/
vector vector_Normalize(vector* v);
/**
*	@brief	计算向量取反
*	@param	选择向量体
*			@arg	v -->	向量
*	@retval	返回反方向向量
*/
vector vector_Negate(const vector* v);

#endif
