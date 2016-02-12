#pragma once

#include "game/renderer.h"

#define SHIP_WIDTH  1.f
#define SHIP_HEIGHT 2.f

#define BASE_WIDTH  2.5f
#define BASE_HEIGHT 2.5f

#define SHIP_MAX        50
#define SHIP_BASE_SPEED 4.f

typedef struct {
	int addIndex;
	int count;

	mesh_t meshes[SHIP_MAX];
	v2_t positions[SHIP_MAX];
	v2_t velocities[SHIP_MAX];
	float rotations[SHIP_MAX];

	v2_t target;
	float speed;
} ship_group_t;

typedef struct {
	v2_t position;
	mesh_t mesh;
	float rotation;
} base_t;

ship_group_t ent_ship_group_create(c4_t color, int count, v2_t pos, float rotation);
void ent_ship_group_add(ship_group_t* s, c4_t color, v2_t pos, float rotation);
void ent_ship_group_update(ship_group_t* s, float dt);
void ent_ship_group_render(shaders_t* sh, ship_group_t* s);

base_t ent_base_create(c4_t color, v2_t position);
void ent_base_update(base_t* s, float dt);
void ent_base_render(shaders_t* sh, base_t* s);
