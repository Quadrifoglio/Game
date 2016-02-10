#include "game/renderer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "game/renderer.h"
#include "game/utils.h"

#define STB_TRUETYPE_IMPLEMENTATION 1
#include "lib/stb_truetype.h"

void render_clear_screen(void) {
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

shaders_t render_shaders_load(char* vertexPath, char* fragmentPath) {
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

void render_shaders_bind(shaders_t* s) {
	glUseProgram(s->program);
}

// TODO: Improve color management (now: one color per vertex file)
vertices_t render_vertices_load(char* path, c4_t defColor) {
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

void render_vertices_dispose(vertices_t* v) {
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

void render_set_viewport(int w, int h) {
	glViewport(0, 0, w, h);
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

texture_t render_texture_create(u8* data, size_t w, size_t h, GLenum format) {
	texture_t t;

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &t.id);
	glBindTexture(GL_TEXTURE_2D, t.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

	return t;
}

void render_texture_bind(shaders_t* s, texture_t* t) {
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

mesh_t render_mesh_create(GLenum t, vertices_t* v) {
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

void render_mesh_draw(shaders_t* s, mesh_t* m) {
	glBindBuffer(GL_ARRAY_BUFFER, m->vbuffer);
	glVertexAttribPointer(s->position, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m->cbuffer);
	glVertexAttribPointer(s->color, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m->tbuffer);
	glVertexAttribPointer(s->tex, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(m->type, 0, m->vertexCount);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

font_t render_font_load(char* path) {
	font_t f;
	f.bw = 512;
	f.bh = 512;

	char* data = load_file_str(path);
	if(!data) {
		return f;
	}

	int nchar = 96;
	u8* bitmap = malloc(f.bw * f.bh);
	f.cdata = malloc(nchar * sizeof(stbtt_bakedchar));
	f.firstChar = 32;

	int s = stbtt_BakeFontBitmap((const unsigned char*)data, 0, 32.f, bitmap, f.bw, f.bh, f.firstChar, nchar, f.cdata);

	f.tex = render_texture_create(bitmap, f.bw, f.bh, GL_RED);
	glBindTexture(GL_TEXTURE_2D, f.tex.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(bitmap);
	return f;
}

mesh_t render_font_text(font_t* f, unsigned char* str, float x, float y) {
	vertices_t v = {0};

	int vi = 0, ci = 0, ti = 0;
	int i = 1;

	while(*str) {
		if(*str >= 32 && *str < 128) {
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(f->cdata, f->bw , f->bh, *str - f->firstChar, &x, &y, &q, 1);

			v.v = realloc(v.v, i * 4 * 4 * 2 * sizeof(float));
			v.c = realloc(v.c, i * 4 * 4 * 4 * sizeof(float));
			v.t = realloc(v.t, i * 4 * 4 * 2 * sizeof(float));

			printf("(%f , %f) - (%f , %f)\n", q.x0, q.y0, q.s0, q.t1);

			// Vertex 1
			v.v[vi++] = q.x0;
			v.v[vi++] = q.y0;

			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;

			v.t[ti++] = q.s0;
			v.t[ti++] = q.t1;

			// Vertex 2
			v.v[vi++] = q.x1;
			v.v[vi++] = q.y0;

			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;

			v.t[ti++] = q.s1;
			v.t[ti++] = q.t1;

			// Vertex 3
			v.v[vi++] = q.x1;
			v.v[vi++] = q.y1;

			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;

			v.t[ti++] = q.s1;
			v.t[ti++] = q.t0;

			// Vertex 4
			v.v[vi++] = q.x0;
			v.v[vi++] = q.y1;

			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;
			v.c[ci++] = 1.f;

			v.t[ti++] = q.s0;
			v.t[ti++] = q.t0;

			++i;
			++str;

			v.count += 4;
		}
	}

	mesh_t m = render_mesh_create(GL_QUADS, &v);
	render_vertices_dispose(&v);

	return m;
}

void render_font_dispose(font_t* f) {
	free(f->cdata);
}

void render_check_errors(void) {
	GLenum err = glGetError();
	if(err != GL_NO_ERROR) {
		printf("OpenGL error: 0x%x\n", err);
	}
}
