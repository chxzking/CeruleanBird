#include "vector.h"
#include <math.h>

//创建一个向量体
vector vector_Create(double x, double y) {
    vector v;
    v.x = x;
    v.y = y;
    return v;
}
//计算两个向量的和
vector vector_Add(const vector* v1, const vector* v2) {
    return vector_Create(v1->x + v2->x, v1->y + v2->y);
}
//计算两个向量的差
vector vector_Sub(const vector* v1, const vector* v2) {
    return vector_Create(v1->x - v2->x, v1->y - v2->y);
}
//计算两个向量的点积
double vector_Dot(const vector* v1, const vector* v2) {
    return v1->x * v2->x + v1->y * v2->y;
}
//计算一个向量和一个标量的积
vector vector_Scalar_Mul(const vector* v, double k) {
    return vector_Create(k * v->x, k * v->y);
}
//计算向量与标量的商
vector vector_Scalar_Div(const vector* v, double k) {
    if (fabs(k) < 1e-9) {
        return vector_Create(0, 0);
    }
    return vector_Create(v->x / k, v->y / k);
}
//计算向量模长的平方
double vector_MagnitudeSquare(const vector* v) {
    return v->x * v->x + v->y * v->y;
}
//计算向量的模长
double vector_Magnitude(const vector* v) {
    return sqrt(vector_MagnitudeSquare(v));
}
//归一化向量
vector vector_Normalize(vector* v) {
	double mag = vector_Magnitude(v);
	if (fabs(mag) < 1e-9) {
		// 判断向量是否接近零向量
		if (fabs(v->x) < 1e-9 && fabs(v->y) < 1e-9) {
			return vector_Create(0, 0); // 零向量不归一化
		}
		// 判断向量是否垂直x、y轴
		if (fabs(v->x) < 1e-9) {
			return vector_Create(0, v->y > 0 ? 1 : -1); // 垂直于x轴
		}
		return vector_Create(v->x > 0 ? 1 : -1, 0); // 垂直于y轴
	}
	return vector_Scalar_Div(v, mag);
}
//计算向量取反
vector vector_Negate(const vector* v) {
    return vector_Create(-v->x, -v->y);
}
