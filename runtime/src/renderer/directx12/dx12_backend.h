#pragma once
#include "defines.h"
#include "renderer/renderer_types.h"

b8 dx12_backend_init(Platform* platform);
void dx12_backend_shutdown(void);

b8 dx12_backend_begin_frame(RenderPacket* packet);
b8 dx12_backend_end_frame(void);
