#pragma once
#include "core/platform/platform.h"
#include "defines.h"

typedef struct engine {
	f32 delta_time;
	f32 start_time;
	Platform platform;
} Engine;

void engine_init(Engine* engine);
void engine_shutdown(Engine* engine);
void engine_run(Engine* engine);
