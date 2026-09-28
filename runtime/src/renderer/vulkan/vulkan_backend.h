#pragma once
#include "core/platform/platform.h"
#include "defines.h"
#include "renderer/renderer_types.h"
#include "vulkan_types.h"

b8 vulkan_backend_init(Platform* platform);
void vulkan_backend_shutdown(void);

b8 vulkan_backend_begin_frame(RenderPacket* packet);
b8 vulkan_backend_end_frame(void);
