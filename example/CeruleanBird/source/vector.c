#include "vector.h"
#include <math.h>

//����һ��������
vector vector_Create(double x, double y) {
    vector v;
    v.x = x;
    v.y = y;
    return v;
}
//�������������ĺ�
vector vector_Add(const vector* v1, const vector* v2) {
    return vector_Create(v1->x + v2->x, v1->y + v2->y);
}
//�������������Ĳ�
vector vector_Sub(const vector* v1, const vector* v2) {
    return vector_Create(v1->x - v2->x, v1->y - v2->y);
}
//�������������ĵ��
double vector_Dot(const vector* v1, const vector* v2) {
    return v1->x * v2->x + v1->y * v2->y;
}
//����һ��������һ�������Ļ�
vector vector_Scalar_Mul(const vector* v, double k) {
    return vector_Create(k * v->x, k * v->y);
}
//�����������������
vector vector_Scalar_Div(const vector* v, double k) {
    if (fabs(k) < 1e-9) {
        return vector_Create(0, 0);
    }
    return vector_Create(v->x / k, v->y / k);
}
//��������ģ����ƽ��
double vector_MagnitudeSquare(const vector* v) {
    return v->x * v->x + v->y * v->y;
}
//����������ģ��
double vector_Magnitude(const vector* v) {
    return sqrt(vector_MagnitudeSquare(v));
}
//��һ������
vector vector_Normalize(vector* v) {
	double mag = vector_Magnitude(v);
	if (fabs(mag) < 1e-9) {
		// �ж������Ƿ�ӽ�������
		if (fabs(v->x) < 1e-9 && fabs(v->y) < 1e-9) {
			return vector_Create(0, 0); // ����������һ��
		}
		// �ж������Ƿ�ֱx��y��
		if (fabs(v->x) < 1e-9) {
			return vector_Create(0, v->y > 0 ? 1 : -1); // ��ֱ��x��
		}
		return vector_Create(v->x > 0 ? 1 : -1, 0); // ��ֱ��y��
	}
	return vector_Scalar_Div(v, mag);
}
//��������ȡ��
vector vector_Negate(const vector* v) {
    return vector_Create(-v->x, -v->y);
}
