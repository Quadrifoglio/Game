#include "game/game.h"

#include <stdio.h>
#include "game/utils.h"

void game_init(game_ctx_t* c, int w, int h) {
	printf("Game initializing\n");
	printf("OpenGL %s\n", glGetString(GL_VERSION));

	game_state_t* s = (game_state_t*)c->storage;
	s->width = (float)w / UNIT_SIZE;
	s->height = (float)h / UNIT_SIZE;
	render_init(w, h);

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

	s->font = render_font_load("res/font.png");

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

	v2_t pos = (v2_t){0.f, 0.f};
	color = (c4_t){1.f, 0.f, 0.f, 1.f};
	s->groups[0]= ent_ship_group_create(color, 50, pos, 0);

	pos = (v2_t){(s->width / 2.f) - 5 * SHIP_WIDTH, s->height / 2.f};
	color = (c4_t){0.f, 0.f, 1.f, 1.f};
	s->groups[1]= ent_ship_group_create(color, 10, pos, PI);

	s->groupCount = 2;
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

		s->groups[0].target = (v2_t){x, y};
	}
}

void game_update(game_ctx_t* c, float dt) {
	game_state_t* s = (game_state_t*)c->storage;

	for(int i = 0; i < (int)s->baseCount; ++i) {
		ent_base_update(&s->bases[i], dt);
	}

	for(int i = 0; i < (int)s->groupCount; ++i) {
		ent_ship_group_update(&s->groups[i], dt);
	}

	s->camera.x += s->cameraVel.x * 8.f * dt;
	s->camera.y += s->cameraVel.y * 8.f * dt;
	render_set_view(&s->shaders, mat4_translate2(s->camera));

	c4_t color = {1.f, 0.f, 0.f, 1.f};
	unsigned char str[18];
	sprintf((char*)str, "%d fps, %f s", (int)(1.f / dt), dt);
	s->text = render_font_text(&s->font, str, color);
}

void game_render(game_ctx_t* c) {
	game_state_t* s = (game_state_t*)c->storage;

	render_clear_screen();
	render_shaders_bind(&s->shaders);

	v2_t v = {-s->camera.x, -s->camera.y};
	mat4_t m = mat4_translate2(v);
	render_set_model(&s->shaders, m);

	mat4_t uitr = mat4_translate2((v2_t){0.f, s->height - 2.f});
	render_set_model(&s->shaders, mat4_multiply(&m, &uitr));
	render_texture_bind(&s->shaders, &s->font.tex);
	render_mesh_draw(&s->shaders, &s->text);

	render_texture_bind(&s->shaders, &s->defTexture);
	render_set_model(&s->shaders, m);
	render_mesh_draw(&s->shaders, &s->bg);

	for(int i = 0; i < (int)s->baseCount; ++i) {
		ent_base_render(&s->shaders, &s->bases[i]);
	}

	for(int i = 0; i < (int)s->groupCount; ++i) {
		ent_ship_group_render(&s->shaders, &s->groups[i]);
	}


	render_check_errors();
}
