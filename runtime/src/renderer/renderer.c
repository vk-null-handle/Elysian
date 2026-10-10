#include "renderer.h"
#include "renderer_backend.h"

// Replace
static RendererBackend* backend;

b8 renderer_init(Window* window) {
	backend = renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN);
	backend->init(window);
	return TRUE;
}

void renderer_shutdown(void) {
	backend->shutdown();
	renderer_backend_destroy(backend);
}

void renderer_drawframe(Window* window) {
	backend->render_frame(backend->packet, window);
}
