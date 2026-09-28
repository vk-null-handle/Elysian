#include "renderer.h"
#include "core/logging/logger.h"
#include "core/memory/memory.h"
#include "renderer_backend.h"

static RendererBackend* backend = 0;

b8 renderer_init(Platform* platform) {
	backend = mem_alloc(sizeof(RendererBackend));

	renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, backend);

	if (!backend->init(platform)) {
		LOG_FATAL("Failed to init renderer backend");
		return FALSE;
	}

	return TRUE;
}

void renderer_shutdown(void) {
	backend->shutdown();
	mem_free(backend, sizeof(RendererBackend));
}

b8 renderer_begin_frame(RenderPacket* packet) {
	return backend->begin_frame(packet);
}

b8 renderer_end_frame(void) {
	b8 result = backend->end_frame();
	return result;
}

b8 renderer_drawframe(RenderPacket* packet) {
	if (renderer_begin_frame(packet)) {

		if (!renderer_end_frame()) {
			LOG_ERROR("renderer_end_frame failed");
			return FALSE;
		}
	}

	return TRUE;
}
