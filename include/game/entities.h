#pragma once

#include "game/renderer.h"

#define SHIP_WIDTH 1.f
#define SHIP_HEIGHT 2.f

#define BASE_WIDTH 2.5f
#define BASE_HEIGHT 2.5f

typedef struct {
	mesh_t mesh;

	v2_t position;
	v2_t velocity;
	float rotation;

	v2_t target;
	float speed;
} ship_t;

typedef struct {
	v2_t position;
	mesh_t mesh;
	float rotation;
} base_t;

ship_t ent_ship_create(c4_t color);
void ent_ship_update(ship_t* s, float dt);
void ent_ship_render(shaders_t* sh, ship_t* s);

base_t ent_base_create(c4_t color, v2_t position);
void ent_base_update(base_t* s, float dt);
void ent_base_render(shaders_t* sh, base_t* s);
