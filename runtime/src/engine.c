#include "engine.h"
#include "core/logging/logger.h"
#include "renderer/renderer.h"

void engine_init(Engine* engine) {
	engine->delta_time = 0.0f;
	engine->start_time = 0.0f;
	platform_init(&engine->platform, "Test Engine", 1920, 1080);
	renderer_init(&engine->platform);
}

void engine_run(Engine* engine) {
	/*while (!platform_should_close(&engine->platform)) {
	  win_poll_events();
	  renderer_drawframe();

	  float end_time = glfwGetTime();
	  engine->delta_time = end_time - engine->start_time;
	  engine->start_time = end_time;
	}*/
}

void engine_shutdown(Engine* engine) {
	renderer_shutdown();
	platform_shutdown(&engine->platform);
}
