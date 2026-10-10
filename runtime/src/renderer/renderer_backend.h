#pragma once
#include "renderer_types.h"

RendererBackend* renderer_backend_create(RendererBackendType type);
void renderer_backend_destroy(RendererBackend* backend);
