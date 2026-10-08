#include "engine.h"
#include "core/logger/logger.h"
#include "core/memory/memory.h"
#include "game.h"

void engine_init(Engine *engine) {
  mem_init();
  win_init(engine->window, 600, 800, "Engine");

  LOG_DEBUG(ENGINE, "Initialized");
  game_init();
}

void engine_run(Engine *engine) {
  while (!win_should_close(engine->window)) {
    win_poll_events();
    game_tick();
  }
}

void engine_shutdown(Engine *engine) {
  mem_shutdown();
  win_shutdown(engine->window);

  LOG_DEBUG(ENGINE, "Shutdown");
  game_shutdown();
}
