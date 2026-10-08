#include "engine.h"
#include "core/logger.h"
#include "game.h"

void engine_init(Engine *engine) {
  LOG_DEBUG(ENGINE, "Initialized");
  game_init();
}

void engine_run(Engine *engine) {
  LOG_DEBUG(ENGINE, "Running");
  game_tick();
}

void engine_shutdown(Engine *engine) {
  LOG_DEBUG(ENGINE, "Shutdown");
  game_shutdown();
}
