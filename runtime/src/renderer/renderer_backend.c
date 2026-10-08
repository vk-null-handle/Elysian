#include "renderer_backend.h"
#include "core/logger/logger.h"
#include "directx12/dx12_backend.h"
#include "vulkan/vk_backend.h"

b8 renderer_backend_create(RendererBackendType type,
                           RendererBackend *out_backend) {

  if (type == RENDERER_BACKEND_TYPE_VULKAN) {
    out_backend->init = vulkan_backend_init;
    out_backend->shutdown = vulkan_backend_shutdown;
    out_backend->render_frame = vulkan_backend_render_frame;

    return TRUE;
  } else if (type == RENDERER_BACKEND_TYPE_DIRECTX12) {
    out_backend->init = dx12_backend_init;
    out_backend->shutdown = dx12_backend_shutdown;
    out_backend->render_frame = dx12_backend_render_frame;

    LOG_FATAL(RENDERER, "DirectX12 not implemented");
    return FALSE;
  }

  return FALSE;
}

void renderer_backend_destroy(RendererBackend *renderer_backend) {
  renderer_backend->init = 0;
  renderer_backend->shutdown = 0;
  renderer_backend->render_frame = 0;
}
