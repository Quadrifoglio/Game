#include "game/game.h"

#include <stdio.h>
#include "game/utils.h"

void game_init(game_ctx_t* c, int w, int h) {
	printf("Game initializing\n");
	printf("OpenGL %s\n", glGetString(GL_VERSION));

	game_state_t* s = (game_state_t*)c->storage;
	s->width = (float)w / UNIT_SIZE;
	s->height = (float)h / UNIT_SIZE;
	render_set_viewport(w, h);

	s->shaders = render_shaders_load("shaders/basic.vertex", "shaders/basic.fragment");
	render_shaders_bind(&s->shaders);

	mat4_t projection = mat4_ortho_projection(0.f, s->width, 0.f, s->height, 1.f, -1.f);
	render_set_projection(&s->shaders, projection);

	s->camera = (v2_t){0.f, 0.f};
	render_set_view(&s->shaders, mat4_translate2(s->camera));

	u8 pixels[2 * 2 * 3] = {
		255, 255, 255,    255, 255, 255,
		255, 255, 255,    255, 255, 255
	};
	s->defTexture = render_texture_create(pixels, 2, 2, GL_RGB);

	s->font = render_font_load("res/times.ttf");
	s->text = render_font_text(&s->font, "Pute", 0.f, -10.f);

	vertices_t bgv;
	int nstars = 500;
	int n = 0, nn = 0, nnn = 0;

	bgv.v = malloc(nstars * 2 * sizeof(float));
	bgv.c = malloc(nstars * 2 * 4 * sizeof(float));
	bgv.t = malloc(nstars * 2 * sizeof(float));
	bgv.count = nstars;

	for(int i = 0; i < nstars; ++i) {
		float x = rand_float(0.f, s->width);
		float y = rand_float(0.f, s->height);

		bgv.v[n++] = x;
		bgv.v[n++] = y;

		bgv.c[nn++] = 1.f;
		bgv.c[nn++] = 1.f;
		bgv.c[nn++] = 1.f;
		bgv.c[nn++] = 1.f;

		bgv.t[nnn++] = 0.f;
		bgv.t[nnn++] = 0.f;
	}

	s->bg = render_mesh_create(GL_POINTS, &bgv);
	render_vertices_dispose(&bgv);

	c4_t color = {1.f, 1.f, 1.f, 1.f};
	s->bases[0] = ent_base_create(color, (v2_t){40.f, 22.5f});
	s->baseCount = 1;

	color = (c4_t){1.f, 1.f, 1.f, 1.f};
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
	s->shipCount = 4;
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
		float x = (ev.clickX / UNIT_SIZE) - s->camera.x;
		float y = (s->height - ev.clickY / UNIT_SIZE) - s->camera.y;

		for(int i = 0; i < (int)s->shipCount; ++i) {
			s->ships[i].target = (v2_t){x, y};
		}
	}
}

void game_update(game_ctx_t* c, float dt) {
	game_state_t* s = (game_state_t*)c->storage;

	for(int i = 0; i < (int)s->baseCount; ++i) {
		ent_base_update(&s->bases[i], dt);
	}

	for(int i = 0; i < (int)s->shipCount; ++i) {
		ent_ship_update(&s->ships[i], dt);
	}

	s->camera.x += s->cameraVel.x * 8.f * dt;
	s->camera.y += s->cameraVel.y * 8.f * dt;
	render_set_view(&s->shaders, mat4_translate2(s->camera));
}

void game_render(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	render_clear_screen();
	render_shaders_bind(&s->shaders);

	v2_t v = {-s->camera.x, -s->camera.y};
	render_set_model(&s->shaders, mat4_translate2(v));

	render_texture_bind(&s->shaders, &s->font.tex);
	render_mesh_draw(&s->shaders, &s->text);
	render_texture_bind(&s->shaders, &s->defTexture);

	render_mesh_draw(&s->shaders, &s->bg);

	for(int i = 0; i < (int)s->baseCount; ++i) {
		ent_base_render(&s->shaders, &s->bases[i]);
	}

	for(int i = 0; i < (int)s->shipCount; ++i) {
		ent_ship_render(&s->shaders, &s->ships[i]);
	}

	render_check_errors();
}
