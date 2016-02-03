#pragma once

#include "global.h"
#include "game/math.h"
#include <GL/glew.h>

typedef struct {
	GLuint vertex;
	GLuint fragment;
	GLuint program;

	GLint position;
	GLint color;
} shaders_t;

typedef struct {
	GLuint vbuffer;
	GLuint cbuffer;
} mesh_t;

shaders_t load_shaders(char* vertexPath, char* fragmentPath);

void clear_screen(void);
void bind_shaders(shaders_t* s);
void set_projection(shaders_t* s, mat4_t m);

mesh_t create_mesh(size_t count, float* vertices, float* colors);
void draw_mesh(shaders_t* s, mesh_t* m);

void check_render_errors(void);
