#pragma once

#include <GL/glew.h>

typedef struct {
	GLuint vertex;
	GLuint fragment;
	GLuint program;

	GLint position;
} shaders_t;

shaders_t load_shaders(char* vertexPath, char* fragmentPath);

void clear_screen();
