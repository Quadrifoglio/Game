#include "game/renderer.h"

#include <stdlib.h>
#include <stdio.h>
#include "game/utils.h"

shaders_t load_shaders(char* vertexPath, char* fragmentPath) {
	shaders_t s;

	char* vcode = load_file_str(vertexPath);
	if(!vcode) {
		fprintf(stderr, "Can not read vertex shader source %s\n", vertexPath);
		return s;
	}

	char* fcode = load_file_str(fragmentPath);
	if(!fcode) {
		fprintf(stderr, "Can not read fragement shader source %s\n", fragmentPath);
		return s;
	}

	s.vertex = glCreateShader(GL_VERTEX_SHADER);
	s.fragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(s.vertex, 1, (const char**)&vcode, 0);
	glShaderSource(s.fragment, 1, (const char**)&fcode, 0);

	glCompileShader(s.vertex);

	GLint status;
	glGetShaderiv(s.vertex, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		char buf[512];
		glGetShaderInfoLog(s.vertex, 512, 0, buf);
		fprintf(stderr, "Vertex shader error: %s\n", buf);

		goto exit;
	}

	status = GL_TRUE;

	glCompileShader(s.fragment);
	glGetShaderiv(s.fragment, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		char buf[512];
		glGetShaderInfoLog(s.fragment, 512, 0, buf);
		fprintf(stderr, "Fragment shader error: %s\n", buf);

		goto exit;
	}

	status = GL_TRUE;

	s.program = glCreateProgram();
	glAttachShader(s.program, s.vertex);
	glAttachShader(s.program, s.fragment);

	glLinkProgram(s.program);
	glGetProgramiv(s.program, GL_LINK_STATUS, &status);
	if(status != GL_TRUE) {
		char buf[512];
		glGetProgramInfoLog(s.program, 512, 0, buf);
		fprintf(stderr, "Shaders link error: %s\n", buf);

		goto exit;
	}

	s.position = glGetAttribLocation(s.program, "position");
	s.color = glGetAttribLocation(s.program, "color");

	glEnableVertexAttribArray(s.position);
	glEnableVertexAttribArray(s.color);

exit:
	free(fcode);
	free(vcode);

	return s;
}

void clear_screen(void) {
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void bind_shaders(shaders_t* s) {
	glUseProgram(s->program);
}

void set_projection(shaders_t* s, mat4_t m) {
	GLint loc = glGetUniformLocation(s->program, "proj");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &m.m11);
}

mesh_t create_mesh(size_t count, float* vertices, float* colors) {
	mesh_t m;

	glGenBuffers(1, &m.vbuffer);
	glGenBuffers(1, &m.cbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m.vbuffer);
	glBufferData(GL_ARRAY_BUFFER, count * 2 * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m.cbuffer);
	glBufferData(GL_ARRAY_BUFFER, count * 4 * sizeof(float), colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return m;
}

void draw_mesh(shaders_t* s, mesh_t* m) {
	glBindBuffer(GL_ARRAY_BUFFER, m->vbuffer);
	glVertexAttribPointer(s->position, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m->cbuffer);
	glVertexAttribPointer(s->color, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void check_render_errors(void) {
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) {
		printf("OpenGL error: 0x%x\n", err);
	}
}
