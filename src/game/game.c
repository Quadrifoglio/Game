#include "game/game.h"
#include <GL/glew.h>

void game_init(game_ctx_t* c) {

}

void game_event(game_ctx_t* c) {

}

void game_update(game_ctx_t* c, float dt) {

}

void game_render(game_ctx_t* c) {
	glClearColor(0.f, 0.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}
