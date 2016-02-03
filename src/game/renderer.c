#include "game/renderer.h"

#include <stdlib.h>
#include <stdio.h>
#include "game/utils.h"

shaders_t load_shaders(char* vertexPath, char* fragmentPath) {
	shaders_t s;

	char* vcode = load_file_str(vertexPath);
	if(!vcode) {
		fprintf(stderr, "Can not read shader source %s\n", vertexPath);
		return s;
	}

	char* fcode = load_file_str(fragmentPath);
	if(!fcode) {
		fprintf(stderr, "Can not read shader source %s\n", fragmentPath);
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
	glEnableVertexAttribArray(s.position);

exit:
	free(fcode);
	free(vcode);

	return s;
}

void clear_screen() {
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}
