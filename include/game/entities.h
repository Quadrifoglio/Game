#pragma once

#include "game/renderer.h"

#define SHIP_WIDTH 1.f
#define SHIP_HEIGHT 2.f

typedef struct {
	v2_t position;
	v2_t velocity;
	v2_t target;
	float speed;
	float rotation;

	mesh_t mesh;
} ship_t;

typedef struct {
	v2_t position;
	mesh_t mesh;
} base_t;

ship_t ent_ship_create(c4_t color);
void ent_ship_update(ship_t* s, float dt);
void ent_ship_render(ship_t* s, shaders_t* sh);

base_t ent_base_create(c4_t color, v2_t position);
void ent_base_update(base_t* s, float dt);
void ent_base_render(base_t* s, shaders_t* sh);
