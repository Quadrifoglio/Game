#include "game/game.h"

#include <stdio.h>

void game_init(game_ctx_t* c, int w, int h) {
	printf("Game initializing\n");
	printf("OpenGL %s\n", glGetString(GL_VERSION));

	game_state_t* s = (game_state_t*)c->storage;
	s->width = (float)w / UNIT_SIZE;
	s->height = (float)h / UNIT_SIZE;
	render_set_viewport(w, h);

	s->shaders = render_load_shaders("shaders/basic.vertex", "shaders/basic.fragment");
	render_bind_shaders(&s->shaders);

	mat4_t projection = mat4_ortho_projection(0.f, s->width, 0.f, s->height, 1.f, -1.f);
	render_set_projection(&s->shaders, projection);

	s->camera.x = 0.f;
	s->camera.y = 0.f;
	render_set_view(&s->shaders, mat4_translate2(s->camera));

	c4_t color = {1.f, 1.f, 1.f, 1.f};
	s->ships[0]= ent_ship_create(color);
	s->ships[0].position = (v2_t){SHIP_WIDTH / 2.f, SHIP_HEIGHT / 2.f};

	color = (c4_t){1.f, 0.f, 0.f, 1.f};
	s->ships[1] = ent_ship_create(color);
	s->ships[1].position = (v2_t){SHIP_WIDTH / 2.f, s->height - (SHIP_HEIGHT / 2.f)};
	s->ships[1].rotation = PI;

	color = (c4_t){0.f, 1.f, 0.f, 1.f};
	s->ships[2] = ent_ship_create(color);
	s->ships[2].position = (v2_t){s->width - (SHIP_WIDTH / 2.f), s->height - (SHIP_HEIGHT / 2.f)};
	s->ships[2].rotation = PI;

	color = (c4_t){0.f, 0.f, 1.f, 1.f};
	s->ships[3] = ent_ship_create(color);
	s->ships[3].position = (v2_t){s->width - (SHIP_WIDTH / 2.f), SHIP_HEIGHT / 2.f};
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
	else if(ev.type == EVENT_CLICK) {
		for(int i = 0; i < (int)sizeof(s->ships); ++i) {
			s->ships[i].target = (v2_t){ev.clickX / UNIT_SIZE, s->height - ev.clickY / UNIT_SIZE};
		}
	}
}

void game_update(game_ctx_t* c, float dt) {
	game_state_t* s = (game_state_t*)c->storage;

	s->camera.x += s->cameraVel.x * 8.f * dt;
	s->camera.y += s->cameraVel.y * 8.f * dt;
	render_set_view(&s->shaders, mat4_translate2(s->camera));

	for(int i = 0; i < (int)sizeof(s->ships); ++i) {
		ent_ship_update(&s->ships[i], dt);
	}
}

void game_render(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	render_clear_screen();

	for(int i = 0; i < (int)sizeof(s->ships); ++i) {
		ent_ship_render(&s->ships[i], &s->shaders);
	}

	render_check_errors();
}
