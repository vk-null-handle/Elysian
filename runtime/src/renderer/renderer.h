#pragma once
#include "renderer_types.h"

b8 renderer_init(Window* window);
void renderer_shutdown(void);
void renderer_drawframe(void);
