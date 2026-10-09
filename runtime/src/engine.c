#include "engine.h"
#include "core/logger/logger.h"
#include "core/memory/memory.h"
#include "game.h"
#include "renderer/renderer.h"

void engine_init(Engine* engine) {
	mem_init();
	win_init(engine->window, 600, 800, "Engine");
	renderer_init(engine->window);
	game_init();

	LOG_DEBUG(ENGINE, "Initialized");
}

void engine_run(Engine* engine) {
	while (!win_should_close(engine->window)) {
		win_poll_events();
		renderer_drawframe();
		game_tick();
	}
}

void engine_shutdown(Engine* engine) {
	game_shutdown();
	renderer_shutdown();
	win_shutdown(engine->window);
	mem_shutdown();
	LOG_DEBUG(ENGINE, "Shutdown");
}
