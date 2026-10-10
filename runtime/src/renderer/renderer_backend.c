#include "renderer_backend.h"
#include "core/logger/logger.h"
#include "core/memory/memory.h"

#include "directx12/dx12_backend.h"
#include "vulkan/vk_backend.h"

RendererBackend* renderer_backend_create(RendererBackendType type) {
	RendererBackend* backend = mem_alloc(sizeof(RendererBackend));

	switch (type) {
	case RENDERER_BACKEND_TYPE_VULKAN:
		backend->init = vulkan_backend_init;
		backend->shutdown = vulkan_backend_shutdown;
		backend->render_frame = vulkan_backend_render_frame;

		break;
	case RENDERER_BACKEND_TYPE_DIRECTX12:
		backend->init = dx12_backend_init;
		backend->shutdown = dx12_backend_shutdown;
		backend->render_frame = dx12_backend_render_frame;

		LOG_FATAL(RENDERER, "DirectX12 not implemented");
		break;
	};

	return backend;
}

void renderer_backend_destroy(RendererBackend* backend) {
	mem_free(backend, sizeof(RendererBackend));
}
