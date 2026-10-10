#include "engine.h"
#include "core/logger/logger.h"
#include "core/memory/memory.h"
#include "game.h"
#include "renderer/renderer.h"

struct engine {
	Window* window;
};

Engine* engine_init(void) {
	mem_init();
	Engine* e = mem_alloc(sizeof(struct engine));

	e->window = win_init(600, 800, "Engine");
	if (!e->window) {
		LOG_FATAL(ENGINE, "Failed to create window");
	}

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
	mem_free(engine, sizeof(struct engine));
	mem_shutdown();
}
