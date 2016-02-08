#include "game/entities.h"

ship_t ent_ship_create(c4_t color) {
	ship_t s;

	float v[6 * 2] = {
		0.5f, 0.f,
		0.f, 0.5f,
		0.5f, 2.f,

		0.5f, 0.f,
		1.f, 0.5f,
		0.5f, 2.f
	};

	float cl[6 * 4] = {
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a
	};

	s.mesh = render_create_mesh(GL_TRIANGLES, 6, v, cl);
	s.position.x = 0.f;
	s.position.y = 0.f;
	s.velocity.x = 0.f;
	s.velocity.y = 0.f;
	s.target.x = 0.f;
	s.target.y = 0.f;
	s.speed = 4.f;
	s.rotation = 0.f;

	return s;
}

void ent_ship_update(ship_t* s, float dt) {
	float d = v2_distance(s->target, s->position);

	// TODO: Proper stop at target (now: 0.2 units error)
	if(s->target.x != 0 && s->target.y != 0 && d > 0.2f) {
		s->velocity.x = (s->target.x - s->position.x) / (v2_distance(s->position, s->target) / s->speed);
		s->velocity.y = (s->target.y - s->position.y) / (v2_distance(s->position, s->target) / s->speed);
		s->rotation = -atan2(s->velocity.x, s->velocity.y);
	}
	else {
		s->velocity.x = 0;
		s->velocity.y = 0;
	}

	s->position.x += s->velocity.x * s->speed * dt;
	s->position.y += s->velocity.y * s->speed * dt;
}

void ent_ship_render(shaders_t* sh, ship_t* s) {
	v2_t p = {s->position.x - SHIP_WIDTH / 2.f, s->position.y - SHIP_HEIGHT / 2.f};
	mat4_t model = mat4_identity();

	mat4_t t = mat4_translate2(p);
	model = mat4_multiply(&model, &t);

	if(s->rotation != 0.f && s->rotation != 2 * PI) {
		t = mat4_translate2((v2_t){SHIP_WIDTH / 2.f, SHIP_HEIGHT / 2.f});
		model = mat4_multiply(&model, &t);

		mat4_t r = mat4_rotate_z(s->rotation);
		model = mat4_multiply(&model, &r);

		t = mat4_translate2((v2_t){-SHIP_WIDTH / 2.f, -SHIP_HEIGHT / 2.f});
		model = mat4_multiply(&model, &t);
	}

	render_set_model(sh, model);
	render_draw_mesh(sh, &s->mesh);
}

base_t ent_base_create(c4_t color, v2_t position) {
	base_t b;

	float v[8 * 3 * 2] = {
		1.f / 1.5f, 0.f / 1.5f,
		1.f / 1.5f, 2.5 / 1.5f,
		0.f / 1.5f, 1.f / 1.5f,

		1.f / 1.5f, 2.5 / 1.5f,
		0.f / 1.5f, 2.5 / 1.5f,
		0.f / 1.5f, 1.f / 1.5f,

		0.f / 1.5f, 2.5 / 1.5f,
		1.f / 1.5f, 2.5 / 1.5f,
		1.f / 1.5f, 3.5 / 1.5f,

		1.f / 1.5f, 0.f / 1.5f,
		2.5f / 1.5f, 0.f / 1.5f,
		1.f / 1.5f, 3.5 / 1.5f,

		2.5 / 1.5f, 3.5 / 1.5f,
		1.f / 1.5f, 3.5 / 1.5f,
		2.5 / 1.5f, 0.f / 1.5f,

		2.5 / 1.5f, 2.5 / 1.5f,
		3.5 / 1.5f, 2.5 / 1.5f,
		2.5 / 1.5f, 3.5 / 1.5f,

		3.5 / 1.5f, 2.5 / 1.5f,
		2.5 / 1.5f, 2.5 / 1.5f,
		3.5 / 1.5f, 1.f / 1.5f,

		2.5 / 1.5f, 2.5 / 1.5f,
		2.5 / 1.5f, 0.f / 1.5f,
		3.5 / 1.5f, 1.f / 1.5f
	};

	float c[8 * 3 * 4] = {
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
		color.r, color.g, color.b, color.a,
	};

	b.mesh = render_create_mesh(GL_TRIANGLES, 8 * 3, v, c);
	b.position = position;
	b.rotation = 0;

	return b;
}

void ent_base_update(base_t* b, float dt) {
	//b->rotation = 0.111f;
}

void ent_base_render(shaders_t* sh, base_t* b) {
	v2_t p = {b->position.x - SHIP_WIDTH / 2.f, b->position.y - SHIP_HEIGHT / 2.f};
	mat4_t model = mat4_identity();

	mat4_t t = mat4_translate2(p);
	model = mat4_multiply(&model, &t);

	if(b->rotation != 0.f && b->rotation != 2 * PI) {
		t = mat4_translate2((v2_t){SHIP_WIDTH / 2.f, SHIP_HEIGHT / 2.f});
		model = mat4_multiply(&model, &t);

		mat4_t r = mat4_rotate_z(b->rotation);
		model = mat4_multiply(&model, &r);

		t = mat4_translate2((v2_t){-SHIP_WIDTH / 2.f, -SHIP_HEIGHT / 2.f});
		model = mat4_multiply(&model, &t);
	}

	render_set_model(sh, model);
	render_draw_mesh(sh, &b->mesh);
}
