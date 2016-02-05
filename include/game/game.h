#pragma once

#include "global.h"
#include "game/renderer.h"

typedef struct {
	shaders_t shaders;

	v2_t cameraVel;
	v2_t camera;

	mesh_t test;
} game_state_t;

extern void game_init(game_ctx_t* c, int w, int h);
extern void game_event(game_ctx_t* c, game_event_t ev);
extern void game_update(game_ctx_t* c, float dt);
extern void game_render(game_ctx_t* c);
