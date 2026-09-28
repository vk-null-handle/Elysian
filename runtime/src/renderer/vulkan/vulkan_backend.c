#include "vulkan_backend.h"
#include "core/logging/logger.h"

b8 vulkan_backend_init(Platform* platform) {
	return TRUE;
}

void vulkan_backend_shutdown(void) {
}

b8 vulkan_backend_begin_frame(RenderPacket* packet) {
	return TRUE;
}
b8 vulkan_backend_end_frame(void) {
	return TRUE;
}
