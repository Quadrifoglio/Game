#include "global.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef void(*init_func_t)(game_ctx_t*);
typedef void(*event_func_t)(game_ctx_t*);
typedef void(*update_func_t)(game_ctx_t*, float);
typedef void(*render_func_t)(game_ctx_t*);

typedef struct {
	void* app;
	time_t lastLoaded;

	init_func_t init;
	event_func_t event;
	update_func_t update;
	render_func_t render;
} game_t;

bool load_game(game_t* g) {
	const char* path = "build/game/game.so";

	struct stat s = {0};
	stat(path, &s);

	if(s.st_mtime == g->lastLoaded || s.st_size == 0) {
		return true;
	}

	if(g->app) {
		dlclose(g->app);
	}

	g->app = dlopen(path, RTLD_NOW);
	if(!g->app) {
		return false;
	}

	g->init = 0;
	g->event = 0;
	g->update = 0;
	g->render = 0;

	g->init = (init_func_t)dlsym(g->app, "game_init");
	g->event = (event_func_t)dlsym(g->app, "game_event");
	g->update = (update_func_t)dlsym(g->app, "game_update");
	g->render = (render_func_t)dlsym(g->app, "game_render");

	if(!g->init || !g->event || !g->update || !g->render) {
		return false;
	}

	stat(path, &s);
	g->lastLoaded = s.st_mtime;

	return true;
}

int main(void) {
	if(!glfwInit()) {
		fputs("Can not init GLFW !", stderr);
		return 1;
	}

	game_t g = {0};
	if(!load_game(&g)) {
		fputs("Can not load game", stderr);

		glfwTerminate();
		return 1;
	}

	GLFWwindow* w = glfwCreateWindow(1280, 720, "Game", 0, 0);
	if(!w) {
		fputs("Can not create window", stderr);

		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(w);

	if(glewInit() != GLEW_OK) {
		fputs("Can not initialize OpenGL !", stderr);

		glfwTerminate();
		return 1;
	}

	if(!GLEW_VERSION_2_0) {
		fputs("OpenGL 2.0 or better required, upgrade your graphic card or it's driver", stderr);

		glfwTerminate();
		return 1;
	}

	game_ctx_t c;
	c.size = 10000000; // 10MB
	c.storage = malloc(c.size);

	g.init(&c);

	while(!glfwWindowShouldClose(w)) {
		if(!load_game(&g)) {
			break;
		}

		g.update(&c, 1.f);
		g.render(&c);

		glfwSwapBuffers(w);
		glfwPollEvents();
	}

	free(c.storage);
	dlclose(g.app);
	glfwTerminate();

	return 0;
}
