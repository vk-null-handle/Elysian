#pragma once
#include "game.h"
#include "defines.h"

typedef struct engine {
  void (*game_init)(void);
  void (*game_run)(void);
  void (*game_shutdown)(void);
} Engine;

void engine_init(Engine* engine);
void engine_shutdown(Engine* engine);
void engine_run(Engine* engine);
