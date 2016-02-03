#include "game/game.h"

void game_init(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	s->shaders = load_shaders("shaders/basic.vertex", "shaders/basic.fragment");
}

void game_event(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;
}

void game_update(game_ctx_t* c, float dt) {
	game_state_t* s = (game_state_t*)c->storage;
}

void game_render(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	clear_screen();
}
