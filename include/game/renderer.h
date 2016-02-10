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
	GLint tex;
} shaders_t;

typedef struct {
	GLuint id;
} texture_t;

typedef struct {
	float* v;
	float* c;
	float* t;
	size_t count;
} vertices_t;

typedef struct {
	GLuint vbuffer;
	GLuint cbuffer;
	GLuint tbuffer;

	GLenum type;
	size_t vertexCount;
} mesh_t;

void render_clear_screen(void);

vertices_t render_vertices_load(char* path, c4_t defColor);
void render_vertices_dispose(vertices_t* v);

shaders_t render_shaders_load(char* vertexPath, char* fragmentPath);
void render_shaders_bind(shaders_t* s);

void render_set_viewport(int w, int h);
void render_set_projection(shaders_t* s, mat4_t m);
void render_set_view(shaders_t* s, mat4_t v);
void render_set_model(shaders_t* s, mat4_t m);

texture_t render_texture_create(u8* data, size_t w, size_t h);
void render_texture_bind(shaders_t*s, texture_t* t);

mesh_t render_mesh_create(GLenum t, vertices_t* v);
void render_mesh_draw(shaders_t* s, mesh_t* m);

void render_check_errors(void);
