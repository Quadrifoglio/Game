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

vertices_t render_load_vertices(char* path, c4_t defColor);
void render_dispose_vertices(vertices_t* v);

void render_set_viewport(int w, int h);
void render_clear_screen(void);

shaders_t render_load_shaders(char* vertexPath, char* fragmentPath);
void render_bind_shaders(shaders_t* s);

void render_set_projection(shaders_t* s, mat4_t m);
void render_set_view(shaders_t* s, mat4_t v);
void render_set_model(shaders_t* s, mat4_t m);

texture_t render_create_texture(u8* data, size_t w, size_t h);
void render_bind_texture(shaders_t*s, texture_t* t);

mesh_t render_create_mesh(GLenum t, vertices_t* v);
void render_draw_mesh(shaders_t* s, mesh_t* m);

void render_check_errors(void);
