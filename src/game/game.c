#include "game/game.h"

void game_init(game_ctx_t* c, int w, int h) {
	game_state_t* s = (game_state_t*)c->storage;

	render_set_viewport(w, h);
	s->shaders = render_load_shaders("shaders/basic.vertex", "shaders/basic.fragment");

	float v[6 * 2] = {
		0.f, 0.f,
		0.f, 5.f,
		5.f, 0.f,

		0.f, 5.f,
		5.f, 5.f,
		5.f, 0.f
	};

	float cl[6 * 4] = {
		1.f, 0.f, 0.f, 1.f,
		0.f, 1.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f,

		0.f, 1.f, 0.f, 1.f,
		1.f, 0.f, 0.f, 1.f,
		0.f, 0.f, 1.f, 1.f,
	};

	s->test = render_create_mesh(6, v, cl);

	render_bind_shaders(&s->shaders);

	mat4_t projection = mat4_ortho_projection(0.f, (float)w / 16.f, 0.f, (float)h / 16.f, 1.f, -1.f);
	render_set_projection(&s->shaders, projection);

	render_set_view(&s->shaders, mat4_identity());
}

void game_event(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;
}

void game_update(game_ctx_t* c, float dt, double mx, double my) {
	game_state_t* s = (game_state_t*)c->storage;

	v3_t pos = {0.f, 0.f, -1.f};
	v3_t dir = {0.f, 0.f, 0.f};
	v3_t up = {0.f, 1.f, 0.f};

	// TODO: FIX THE GODDAMN VIEW MATRIX
	mat4_t view = mat4_look_at(pos, dir, up);
	render_set_view(&s->shaders, view);
}

void game_render(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	render_clear_screen();
	render_draw_mesh(&s->shaders, &s->test);

	render_check_errors();
}
