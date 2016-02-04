#include "global.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef void(*init_func_t)(game_ctx_t*, int, int);
typedef void(*event_func_t)(game_ctx_t*);
typedef void(*update_func_t)(game_ctx_t*, float, double, double);
typedef void(*render_func_t)(game_ctx_t*);

typedef struct {
	int width;
	int height;

	void* app;
	time_t lastLoaded;

	init_func_t init;
	event_func_t event;
	update_func_t update;
	render_func_t render;

	game_ctx_t c;
} game_t;

bool load_game(game_t* g) {
	static const char* path = "build/game/game.so";

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
		fprintf(stderr, "%s\n", dlerror());
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

void key_pressed(GLFWwindow* w, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(w, true);
	}
	else if(key == GLFW_KEY_R && action == GLFW_RELEASE) {
		game_t* g = (game_t*)glfwGetWindowUserPointer(w);

		g->lastLoaded = 0;
		memset(g->c.storage, g->c.size, 0);

		if(!load_game(g)) {
			glfwSetWindowShouldClose(w, true);
			return;
		}

		g->init(&g->c, g->width, g->height);
	}
}

int main(void) {
	if(!glfwInit()) {
		fputs("Can not init GLFW !\n", stderr);
		return 1;
	}

	game_t g = {0};
	if(!load_game(&g)) {
		fputs("Can not load game\n", stderr);

		glfwTerminate();
		return 1;
	}

	// TODO: Proper resolution settings
	g.width = 640;
	g.height = 480;

	GLFWwindow* w = glfwCreateWindow(g.width, g.height, "Game", 0, 0);
	if(!w) {
		fputs("Can not create window", stderr);

		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(w);
	glfwSwapInterval(1);
	glfwSetKeyCallback(w, key_pressed);
	glfwSetWindowUserPointer(w, &g);

	if(glewInit() != GLEW_OK) {
		fputs("Can not initialize OpenGL !\n", stderr);

		glfwTerminate();
		return 1;
	}

	if(!GLEW_VERSION_2_0) {
		fputs("OpenGL 2.0 or higher required, upgrade your graphic card or it's driver\n", stderr);

		glfwTerminate();
		return 1;
	}

	g.c.size = 10000000; // 10MB
	g.c.storage = malloc(g.c.size);

	g.init(&g.c, g.width, g.height);

	while(!glfwWindowShouldClose(w)) {
		if(!load_game(&g)) {
			break;
		}

		double mx, my;
		glfwGetCursorPos(w, &mx, &my);

		g.update(&g.c, 1.f / 60.f, mx, my);
		g.render(&g.c);

		glfwSwapBuffers(w);
		glfwPollEvents();
	}

	free(g.c.storage);
	dlclose(g.app);
	glfwTerminate();

	return 0;
}
