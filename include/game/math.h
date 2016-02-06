#pragma once

#include <math.h>

typedef struct {
	float x, y;
} v2_t;

typedef struct {
	float x, y, z;
} v3_t;

typedef struct {
	float r, g, b, a;
} c4_t;

typedef struct {
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;
} mat4_t;

v2_t v2_add(v2_t v, v2_t vv);
v2_t v2_multiplyf(v2_t v, float k);
v2_t v2_normalize(v2_t v);
float v2_scalar(v2_t a, v2_t b);
float v2_distance(v2_t a, v2_t b);
float v2_angle(v2_t a, v2_t b);

v3_t v3_cross(v3_t a, v3_t b);
v3_t v3_normalize(v3_t v);
float v3_scalar(v3_t a, v3_t b);

mat4_t mat4_null();
mat4_t mat4_identity();
mat4_t mat4_multiply(mat4_t* m1, mat4_t* m2);

mat4_t mat4_ortho_projection(float left, float right, float bot, float top, float zNear, float zFar);
mat4_t mat4_look_at(v3_t eye, v3_t center, v3_t up);

mat4_t mat4_translate2(v2_t v);
mat4_t mat4_translate3(v3_t v);
mat4_t mat4_scale2(v2_t v);
mat4_t mat4_scale3(v3_t v);
mat4_t mat4_rotate_x(float angle);
mat4_t mat4_rotate_y(float angle);
mat4_t mat4_rotate_z(float angle);

void mat4_print(mat4_t* mat);
