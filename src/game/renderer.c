#include "game/renderer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "game/utils.h"

void render_set_viewport(int w, int h) {
	glViewport(0, 0, w, h);
}

shaders_t render_load_shaders(char* vertexPath, char* fragmentPath) {
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
	s.tex = glGetAttribLocation(s.program, "tex");

	glEnableVertexAttribArray(s.position);
	glEnableVertexAttribArray(s.color);
	glEnableVertexAttribArray(s.tex);

exit:
	free(fcode);
	free(vcode);

	return s;
}

// TODO: Improve color management (now: one color per vertex file)
vertices_t render_load_vertices(char* path, c4_t defColor) {
	vertices_t v = {0};

	FILE* f = fopen(path, "r");
	if(!f) {
		return v;
	}

	int n = 0;
	char line[1024];
	int vi = 0, ci = 0, ti = 0;

	while(fgets(line, 1024, f)) {
		if(strstr(line, ";") == 0) {
			continue;
		}

		float vv[2];

		for(int i = 1; i <= 2; ++i) {
			char* l = strdup(line);
			char* vs = file_get_csv_tok(l, i);
			vv[i - 1] = strtof(vs, 0);

			free(l);
		}

		n++;
		v.count++;

		v.v = realloc(v.v, n * 2 * sizeof(float));
		v.v[vi++] = vv[0];
		v.v[vi++] = vv[1];

		v.c = realloc(v.c, n * 4 * sizeof(float));
		v.c[ci++] = defColor.r;
		v.c[ci++] = defColor.g;
		v.c[ci++] = defColor.b;
		v.c[ci++] = defColor.a;

		v.t = realloc(v.t, n * 2 * sizeof(float));
		v.t[ti++] = 0.f;
		v.t[ti++] = 0.f;
	}

	fclose(f);
	return v;
}

void render_clear_screen(void) {
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void render_bind_shaders(shaders_t* s) {
	glUseProgram(s->program);
}

void render_set_projection(shaders_t* s, mat4_t m) {
	GLint loc = glGetUniformLocation(s->program, "proj");
	glUniformMatrix4fv(loc, 1, GL_TRUE, &m.m11);
}

void render_set_view(shaders_t* s, mat4_t m) {
	GLint loc = glGetUniformLocation(s->program, "view");
	glUniformMatrix4fv(loc, 1, GL_TRUE, &m.m11);
}

void render_set_model(shaders_t* s, mat4_t m) {
	GLint loc = glGetUniformLocation(s->program, "model");
	glUniformMatrix4fv(loc, 1, GL_TRUE, &m.m11);
}

texture_t render_create_texture(u8* data, size_t w, size_t h) {
	texture_t t;

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &t.id);
	glBindTexture(GL_TEXTURE_2D, t.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	return t;
}

void render_bind_texture(shaders_t* s, texture_t* t) {
	if(!t) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t->id);

		GLint l = glGetUniformLocation(s->program, "texSampler");
		glUniform1i(l, 0);
	}
}

mesh_t render_create_mesh(GLenum t, vertices_t* v) {
	mesh_t m;
	m.vertexCount = v->count;
	m.type = t;

	glGenBuffers(1, &m.vbuffer);
	glGenBuffers(1, &m.cbuffer);
	glGenBuffers(1, &m.tbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, m.vbuffer);
	glBufferData(GL_ARRAY_BUFFER, v->count * 2 * sizeof(float), v->v, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m.cbuffer);
	glBufferData(GL_ARRAY_BUFFER, v->count * 4 * sizeof(float), v->c, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m.tbuffer);
	glBufferData(GL_ARRAY_BUFFER, v->count * 2 * sizeof(float), v->t, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return m;
}

void render_draw_mesh(shaders_t* s, mesh_t* m) {
	glBindBuffer(GL_ARRAY_BUFFER, m->vbuffer);
	glVertexAttribPointer(s->position, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m->cbuffer);
	glVertexAttribPointer(s->color, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m->tbuffer);
	glVertexAttribPointer(s->tex, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(m->type, 0, m->vertexCount);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void render_check_errors(void) {
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) {
		printf("OpenGL error: 0x%x\n", err);
	}
}

void render_dispose_vertices(vertices_t* v) {
	if(v->v) {
		free(v->v);
	}
	if(v->c) {
		free(v->c);
	}
	if(v->t) {
		free(v->t);
	}
}
