#pragma once
#include "core/window/window.h"
#include "defines.h"

// Packets of data to send gpu each frame
typedef struct render_packet {
	f32 delta_time;
} RenderPacket;

// Backend type
typedef enum renderer_backend_type {
	RENDERER_BACKEND_TYPE_VULKAN,
	RENDERER_BACKEND_TYPE_DIRECTX12,
} RendererBackendType;

typedef struct renderer_backend {
	b8 (*init)(Window* window);
	void (*shutdown)(void);
	void (*render_frame)(RenderPacket* packet);
  
  RenderPacket *packet;
} RendererBackend;
