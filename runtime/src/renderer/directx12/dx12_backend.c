#include "dx12_backend.h"
#include "core/logging/logger.h"

b8 dx12_backend_init(Platform* platform) {
	return TRUE;
}

void dx12_backend_shutdown(void) {}

b8 dx12_backend_begin_frame(RenderPacket* packet) {
	return TRUE;
}

b8 dx12_backend_end_frame(void) {
	return TRUE;
}
