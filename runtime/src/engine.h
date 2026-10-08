#pragma once
#include "game.h"
#include "defines.h"
#include "core/window/window.h"

typedef struct engine {
  Window *window;
} Engine;

void engine_init(Engine* engine);
void engine_shutdown(Engine* engine);
void engine_run(Engine* engine);
