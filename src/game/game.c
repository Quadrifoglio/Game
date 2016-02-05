#include "game/game.h"

#include <stdio.h>

void game_init(game_ctx_t* c, int w, int h) {
	printf("Game initializing\n");
	printf("OpenGL %s\n", glGetString(GL_VERSION));

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

	s->camera.x = 0.f;
	s->camera.y = 0.f;
	render_set_view(&s->shaders, mat4_translate2(s->camera));
}

void game_event(game_ctx_t* c, game_event_t ev) {
	game_state_t* s = (game_state_t*)c->storage;

	if(ev.type == EVENT_KEY) {
		switch(ev.keyAction) {
			case DIRECTION_UP:
				s->cameraVel.y = (float)(ev.keyPressed) * -1.f;
				break;
			case DIRECTION_RIGHT:
				s->cameraVel.x = (float)(ev.keyPressed) * -1.f;
				break;
			case DIRECTION_DOWN:
				s->cameraVel.y = (float)(ev.keyPressed) * 1.f;
				break;
			case DIRECTION_LEFT:
				s->cameraVel.x = (float)(ev.keyPressed) * 1.f;
				break;
		}
	}
}

void game_update(game_ctx_t* c, float dt) {
	game_state_t* s = (game_state_t*)c->storage;

	s->camera.x += s->cameraVel.x * 8.f * dt;
	s->camera.y += s->cameraVel.y * 8.f * dt;
	render_set_view(&s->shaders, mat4_translate2(s->camera));
}

void game_render(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	render_clear_screen();
	render_draw_mesh(&s->shaders, &s->test);

	render_check_errors();
}
