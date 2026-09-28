#pragma once
#include "defines.h"
#include <vulkan/vulkan_core.h>

typedef struct platform {
	void* internal;
} Platform;

b8 platform_init(Platform* platform, const char* title, i32 width, i32 height);
void platform_shutdown(Platform* platform);

b8 platform_should_close(Platform* platform);

// Vulkan stuff
b8 platform_create_vk_surface(Platform* platform, VkInstance* instance, VkSurfaceKHR* out_surface);
const char** platform_get_vk_instance_ext(u32* count);

// Memory stuff
void* platform_allocate(u64 size, b8 aligned);
void platform_free(void* block, b8 aligned);
void* platform_reallocate(void* block, u64 newsize, b8 aligned);
void* platform_zero_memory(void* block, u64 size);
void* platform_copy_memory(void* dest, const void* source, u64 size);
void* platform_set_memory(void* dest, i32 value, u64 size);
