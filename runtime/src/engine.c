#include "engine.h"
#include "core/logger/logger.h"
#include "core/memory/memory.h"
#include "game.h"
#include "renderer/renderer.h"

struct engine {
	Window* window;
};

Engine* engine_init(void) {
	Engine* e = mem_alloc(sizeof(struct engine));
	mem_init();
	win_init(e->window, 600, 800, "Engine");
	renderer_init(e->window);
	game_init();

	return e;
}

void engine_run(Engine* engine) {
	while (!win_should_close(engine->window)) {
		win_poll_events();
		renderer_drawframe(engine->window);
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
