#include "engine.h"
#include "game.h"
#include <stdio.h>

void engine_init(Engine *engine) {
  printf("[Engine] Initialized\n");
  game_init();
}

void engine_run(Engine *engine) {
  printf("[Engine] Running\n");
  printf("[Engine] Renderer updated\n");
  game_tick();
}

void engine_shutdown(Engine *engine) {
  printf("[Engine] Shutdown\n");
  game_shutdown();
}
