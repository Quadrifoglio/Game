#include "game/game.h"

void game_init(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	s->shaders = load_shaders("shaders/basic.vertex", "shaders/basic.fragment");

	float v[3 * 2] = {
		-1.f, -1.f,
		0.f, 1.f,
		1.f, -1.f
	};

	float cl[3 * 4] = {
		1.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f
	};

	s->test = create_mesh(3, v, cl);

	bind_shaders(&s->shaders);
	set_projection(&s->shaders, mat4_identity());
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
	draw_mesh(&s->shaders, &s->test);

	check_render_errors();
}
