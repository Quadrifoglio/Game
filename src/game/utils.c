#include "game/utils.h"

#include <stdio.h>
#include <stdlib.h>

char* load_file_str(char* path) {
	FILE* f = fopen(path, "r");
	if(!f) {
		return 0;
	}

	fseek(f, 0, SEEK_END);
	size_t s = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = malloc(s + 1);
	fread(buf, s, 1, f);
	buf[s] = 0;

	fclose(f);
	return buf;
}

float rand_float(float a, float b) {
	float random = ((float) rand()) / (float) RAND_MAX;
	float diff = b - a;
	float r = random * diff;

	return a + r;
}
