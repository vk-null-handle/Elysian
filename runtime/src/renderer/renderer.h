#pragma once
#include "renderer_types.h"

b8 renderer_init(Platform* platform);
void renderer_shutdown(void);
b8 renderer_drawframe(RenderPacket* packet);
