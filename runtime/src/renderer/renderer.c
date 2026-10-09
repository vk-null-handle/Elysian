#include "renderer.h"
#include "core/logger/logger.h"
#include "core/memory/memory.h"
#include "renderer_backend.h"

static RendererBackend* backend;

b8 renderer_init(Window* window) {
	backend = mem_alloc(sizeof(RendererBackend));

	renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, backend);

	if (!backend->init(window)) {
		LOG_FATAL(RENDERER, "Failed to initialize backend");
		return FALSE;
	}

	return TRUE;
}

void renderer_shutdown(void) {
	backend->shutdown();
	mem_free(backend, sizeof(RendererBackend));
}

void renderer_drawframe(Window* window) {
	backend->render_frame(backend->packet, window);
}
