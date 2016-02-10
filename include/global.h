#pragma once

#include <stdint.h>
#include <stdlib.h>

#define false 0
#define true 1

#define PI 3.14159265359f

#define UNIT_SIZE 16.f

#define DIRECTION_UP    1
#define DIRECTION_RIGHT 2
#define DIRECTION_DOWN  3
#define DIRECTION_LEFT  4

#define EVENT_KEY   1
#define EVENT_CLICK 2

typedef uint8_t bool;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef struct {
	void* storage;
	size_t size;
} game_ctx_t;

typedef struct {
	u8 type;

	// Key event
	bool keyPressed;
	int keyAction;

	// Click event
	float clickX;
	float clickY;
} game_event_t;
