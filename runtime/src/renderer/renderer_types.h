#pragma once
#include "core/platform/platform.h"
#include "defines.h"

#define MAX_FRAMES_IN_FLIGHT 2

// Packets of data to send gpu each frame
typedef struct render_packet {
	f32 delta_time;
} RenderPacket;

// Backend type
typedef enum renderer_backend_type {
	RENDERER_BACKEND_TYPE_VULKAN,
	RENDERER_BACKEND_TYPE_OPENGL,
	RENDERER_BACKEND_TYPE_DIRECTX12
} RendererBackendType;

typedef struct renderer_backend {
	b8 (*init)(Platform* platform);
	void (*shutdown)(void);

	b8 (*begin_frame)(RenderPacket* packet);
	b8 (*end_frame)(void);
} RendererBackend;
