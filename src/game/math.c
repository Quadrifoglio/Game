#include "game/math.h"

#include <string.h>
#include <stdio.h>

v2_t v2_add(v2_t v, v2_t vv) {
	return (v2_t){v.x + vv.x, v.y + vv.y};
}

v2_t v2_multiplyf(v2_t v, float k) {
	return (v2_t){v.x * k, v.y * k};
}

v2_t v2_normalize(v2_t v) {
	float r = sqrt(v.x * v.x + v.y * v.y);
	if(r == 0.f) {
		return v;
	}

	v.x /= r;
	v.y /= r;

	return v;
}

float v2_scalar(v2_t a, v2_t b) {
	return a.x * b.x + a.y * b.y;
}

float v2_distance(v2_t a, v2_t b) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	return sqrt(dx * dx + dy * dy);
}

float v2_angle(v2_t a, v2_t b) {
	float arc = sqrt((a.x * a.x + a.y * a.y) * (b.x * b.x + b.y * b.y));

	if(arc > 0.f) {
		arc = acos((a.x * b.x + a.y * b.y) / arc);

		if(a.x * b.y - a.y * b.x < 0.f) {
			arc *= -1;
		}
	}

	return arc;
}

v3_t v3_cross(v3_t a, v3_t b) {
	v3_t res;

	res.x = a.y * b.z - a.z * b.y;
	res.y = a.z * b.x - a.x * b.z;
	res.z = a.x * b.y - a.y * b.x;

	return res;
}

v3_t v3_normalize(v3_t v) {
	float r = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if(r == 0.f) {
		return v;
	}

	v.x /= r;
	v.y /= r;
	v.z /= r;

	return v;
}

float v3_scalar(v3_t a, v3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

mat4_t mat4_null() {
	mat4_t m;
	memset(&m, 0, sizeof(mat4_t));

	return m;
}

mat4_t mat4_identity() {
	mat4_t mat = mat4_null();
	mat.m11 = 1.f;
	mat.m22 = 1.f;
	mat.m33 = 1.f;
	mat.m44 = 1.f;

	return mat;
}

mat4_t mat4_multiply(mat4_t* m1, mat4_t* m2) {
	mat4_t res = mat4_null();

	res.m11 = m1->m11 * m2->m11 + m1->m12 * m2->m21 + m1->m13 * m2->m31 + m1->m14 * m2->m41;
	res.m12 = m1->m11 * m2->m12 + m1->m12 * m2->m22 + m1->m13 * m2->m32 + m1->m14 * m2->m42;
	res.m13 = m1->m11 * m2->m13 + m1->m12 * m2->m23 + m1->m13 * m2->m33 + m1->m14 * m2->m43;
	res.m14 = m1->m11 * m2->m14 + m1->m12 * m2->m24 + m1->m13 * m2->m34 + m1->m14 * m2->m44;

	res.m21 = m1->m21 * m2->m11 + m1->m22 * m2->m21 + m1->m23 * m2->m31 + m1->m24 * m2->m41;
	res.m22 = m1->m21 * m2->m12 + m1->m22 * m2->m22 + m1->m23 * m2->m32 + m1->m24 * m2->m42;
	res.m23 = m1->m21 * m2->m13 + m1->m22 * m2->m23 + m1->m23 * m2->m33 + m1->m24 * m2->m43;
	res.m24 = m1->m21 * m2->m14 + m1->m22 * m2->m24 + m1->m23 * m2->m34 + m1->m24 * m2->m44;

	res.m31 = m1->m31 * m2->m11 + m1->m32 * m2->m21 + m1->m33 * m2->m31 + m1->m34 * m2->m41;
	res.m32 = m1->m31 * m2->m12 + m1->m32 * m2->m22 + m1->m33 * m2->m32 + m1->m34 * m2->m42;
	res.m33 = m1->m31 * m2->m13 + m1->m32 * m2->m23 + m1->m33 * m2->m33 + m1->m34 * m2->m43;
	res.m34 = m1->m31 * m2->m14 + m1->m32 * m2->m24 + m1->m33 * m2->m34 + m1->m34 * m2->m44;

	res.m41 = m1->m41 * m2->m11 + m1->m42 * m2->m21 + m1->m43 * m2->m31 + m1->m44 * m2->m41;
	res.m42 = m1->m41 * m2->m12 + m1->m42 * m2->m22 + m1->m43 * m2->m32 + m1->m44 * m2->m42;
	res.m43 = m1->m41 * m2->m13 + m1->m42 * m2->m23 + m1->m43 * m2->m33 + m1->m44 * m2->m43;
	res.m44 = m1->m41 * m2->m14 + m1->m42 * m2->m24 + m1->m43 * m2->m34 + m1->m44 * m2->m44;

	return res;
}

mat4_t mat4_ortho_projection(float left, float right, float bot, float top, float zNear, float zFar) {
	mat4_t mat = mat4_identity();

	mat.m11 = 2.f / (right - left);
	mat.m22 = 2.f / (top - bot);
	mat.m33 = -2.f / (zFar - zNear);
	mat.m14 = -(right + left) / (right - left);
	mat.m24 = -(top + bot) / (top - bot);
	mat.m34 = -(zFar + zNear) / (zFar - zNear);

	return mat;
}

// TODO: FIX THE GODDAMN VIEW MATRIX
mat4_t mat4_look_at(v3_t eye, v3_t center, v3_t up) {
	v3_t fwd = { center.x - eye.x, center.y - eye.y, center.z - eye.z };
	fwd = v3_normalize(fwd);

	v3_t right = v3_normalize(v3_cross(fwd, up));
	up = v3_normalize(v3_cross(right, fwd));

	mat4_t v;

	v.m11 = right.x;
	v.m12 = right.y;
	v.m13 = right.z;
	v.m14 = -v3_scalar(right, eye);

	v.m21 = up.x;
	v.m22 = up.y;
	v.m23 = up.z;
	v.m24 = -v3_scalar(up, eye);

	v.m31 = -fwd.x;
	v.m32 = -fwd.y;
	v.m33 = -fwd.z;
	v.m34 = v3_scalar(fwd, eye);

	v.m41 = 0.f;
	v.m42 = 0.f;
	v.m43 = 0.f;
	v.m44 = 1.f;

	return v;
}

mat4_t mat4_translate2(v2_t v) {
	mat4_t res = mat4_identity();

	res.m14 = v.x;
	res.m24 = v.y;
	res.m34 = 0.f;
	res.m44 = 1.f;

	return res;
}

mat4_t mat4_transalte3(v3_t v) {
	mat4_t res = mat4_identity();

	res.m14 = v.x;
	res.m24 = v.y;
	res.m34 = v.z;
	res.m44 = 1.f;

	return res;
}

mat4_t mat4_scale2(v2_t v) {
	mat4_t res = mat4_null();

	res.m11 = v.x;
	res.m22 = v.y;
	res.m33 = 1.f;
	res.m44 = 1.f;

	return res;
}

mat4_t mat4_Scale3(v3_t v) {
	mat4_t res = mat4_null();

	res.m11 = v.x;
	res.m22 = v.y;
	res.m33 = v.z;
	res.m44 = 1.f;

	return res;
}

mat4_t mat4_rotate_x(float angle) {
	mat4_t res = mat4_identity();

	res.m22 = cos(angle);
	res.m23 = -sin(angle);
	res.m32 = sin(angle);
	res.m33 = cos(angle);

	return res;
}

mat4_t mat4_rotate_y(float angle) {
	mat4_t res = mat4_identity();

	res.m11 = cos(angle);
	res.m13 = sin(angle);
	res.m31 = -sin(angle);
	res.m33 = cos(angle);

	return res;
}

mat4_t mat4_rotate_z(float angle) {
	mat4_t res = mat4_identity();

	res.m11 = cos(angle);
	res.m12 = -sin(angle);
	res.m21 = sin(angle);
	res.m22 = cos(angle);

	return res;
}

void mat4_print(mat4_t* mat) {
	printf("| %f %f %f %f |\n", mat->m11, mat->m12, mat->m13, mat->m14);
	printf("| %f %f %f %f |\n", mat->m21, mat->m22, mat->m23, mat->m24);
	printf("| %f %f %f %f |\n", mat->m31, mat->m32, mat->m33, mat->m34);
	printf("| %f %f %f %f |\n", mat->m41, mat->m42, mat->m43, mat->m44);
}
