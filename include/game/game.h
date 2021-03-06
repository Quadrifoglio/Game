#pragma once

#include "global.h"
#include "game/renderer.h"
#include "game/entities.h"

typedef struct {
	float width, height;

	shaders_t shaders;
	texture_t defTexture;

	font_t font;
	mesh_t text;

	v2_t cameraVel;
	v2_t camera;

	mesh_t bg;

	ship_group_t groups[4];
	size_t groupCount;

	base_t bases[1];
	size_t baseCount;
} game_state_t;

extern void game_init(game_ctx_t* c, int w, int h);
extern void game_event(game_ctx_t* c, game_event_t ev);
extern void game_update(game_ctx_t* c, float dt);
extern void game_render(game_ctx_t* c);
