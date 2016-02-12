#include "game/entities.h"

ship_group_t ent_ship_group_create(c4_t color, int count, v2_t pos, float rotation) {
	ship_group_t g = {0};
	g.addIndex = 0;
	g.count = 0;
	g.speed = SHIP_BASE_SPEED;

	float spacingX = SHIP_WIDTH, spacingY = 0.f;

	for(int i = 0; i < count; ++i) {
		ent_ship_group_add(&g, color, pos, rotation);

		pos.x += spacingX;
		pos.y += spacingY;
	}

	return g;
}

void ent_ship_group_add(ship_group_t* g, c4_t color, v2_t pos, float rotation) {
	if(g->addIndex >= SHIP_MAX) {
		return;
	}

	vertices_t bv = render_vertices_load("meshes/ship.mesh", color);

	g->meshes[g->addIndex] = render_mesh_create(GL_TRIANGLES, &bv);
	g->positions[g->addIndex] = pos;
	g->velocities[g->addIndex] = (v2_t){0.f, 0.f};
	g->rotations[g->addIndex] = rotation;

	g->addIndex++;
	g->count++;

	render_vertices_dispose(&bv);
}

void ent_ship_group_update(ship_group_t* g, float dt) {
	for(int i = 0; i < g->count; ++i) {
		float d = v2_distance(g->target, g->positions[i]);

		// TODO: Proper stop at target (now: 0.2 units error)
		if(g->target.x != 0 && g->target.y != 0 && d > 0.2f) {
			g->velocities[i].x = (g->target.x - g->positions[i].x) / (v2_distance(g->positions[i], g->target) / g->speed);
			g->velocities[i].y = (g->target.y - g->positions[i].y) / (v2_distance(g->positions[i], g->target) / g->speed);
			g->rotations[i] = -atan2(g->velocities[i].x, g->velocities[i].y);
		}
		else {
			g->velocities[i].x = 0;
			g->velocities[i].y = 0;
		}

		g->positions[i].x += g->velocities[i].x * g->speed * dt;
		g->positions[i].y += g->velocities[i].y * g->speed * dt;
	}
}

void ent_ship_group_render(shaders_t* sh, ship_group_t* g) {
	for(int i = 0; i < g->count; ++i) {
		v2_t p = {g->positions[i].x - SHIP_WIDTH / 2.f, g->positions[i].y - SHIP_HEIGHT / 2.f};
		mat4_t model = mat4_identity();

		mat4_t t = mat4_translate2(p);
		model = mat4_multiply(&model, &t);

		if(g->rotations[i] != 0.f && g->rotations[i] != 2 * PI) {
			t = mat4_translate2((v2_t){SHIP_WIDTH / 2.f, SHIP_HEIGHT / 2.f});
			model = mat4_multiply(&model, &t);

			mat4_t r = mat4_rotate_z(g->rotations[i]);
			model = mat4_multiply(&model, &r);

			t = mat4_translate2((v2_t){-SHIP_WIDTH / 2.f, -SHIP_HEIGHT / 2.f});
			model = mat4_multiply(&model, &t);
		}

		render_set_model(sh, model);
		render_mesh_draw(sh, &g->meshes[i]);
	}
}

base_t ent_base_create(c4_t color, v2_t position) {
	base_t b;

	vertices_t vv = render_vertices_load("meshes/base.mesh", color);
	b.mesh = render_mesh_create(GL_TRIANGLES, &vv);

	b.position = position;
	b.rotation = 0;

	render_vertices_dispose(&vv);
	return b;
}

void ent_base_update(base_t* b, float dt) {
	b->rotation += dt * PI * 0.3f;
}

void ent_base_render(shaders_t* sh, base_t* b) {
	v2_t p = {b->position.x - BASE_WIDTH / 2.f, b->position.y - SHIP_HEIGHT / 2.f};
	mat4_t model = mat4_identity();

	mat4_t t = mat4_translate2(p);
	model = mat4_multiply(&model, &t);

	if(b->rotation != 0.f && b->rotation != 2 * PI) {
		t = mat4_translate2((v2_t){BASE_WIDTH / 2.f, BASE_HEIGHT / 2.f});
		model = mat4_multiply(&model, &t);

		mat4_t r = mat4_rotate_z(b->rotation);
		model = mat4_multiply(&model, &r);

		t = mat4_translate2((v2_t){-BASE_WIDTH / 2.f, -BASE_HEIGHT / 2.f});
		model = mat4_multiply(&model, &t);
	}

	render_set_model(sh, model);
	render_mesh_draw(sh, &b->mesh);
}
