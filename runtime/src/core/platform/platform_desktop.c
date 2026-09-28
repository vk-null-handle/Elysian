#include "platform.h"

#include "core/logging/logger.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <string.h>

typedef struct desktop {
	GLFWwindow* window;
	VkSurfaceKHR surface;
} Desktop;

const char** platform_get_vk_instance_ext(u32* count) {
	return glfwGetRequiredInstanceExtensions(count);
}

b8 platform_should_close(Platform* platform) {
	Desktop* desktop = (Desktop*)platform->internal;
	return glfwWindowShouldClose(desktop->window);
}

b8 platform_init(Platform* platform, const char* title, i32 width, i32 height) {
	// Create internal struct
	platform->internal = platform_allocate(sizeof(Desktop), FALSE);
	Desktop* desktop = (Desktop*)platform->internal;

	// Create window
	if (!glfwInit()) {
		return FALSE;
	}
	if (!glfwVulkanSupported()) {
		glfwTerminate();
		return FALSE;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	desktop->window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (desktop->window == NULL) {
		glfwTerminate();
		return FALSE;
	}

	return TRUE;
}

b8 platform_create_vk_surface(Platform* platform, VkInstance* instance, VkSurfaceKHR* out_surface) {
	// Cold cast, since we know void ptr type
	Desktop* desktop = (Desktop*)platform->internal;
	if (glfwCreateWindowSurface(*instance, desktop->window, NULL, out_surface) != VK_SUCCESS) {
		return FALSE;
	}
	return TRUE;
}

void platform_shutdown(Platform* platform) {
	// Cold cast, since we know void ptr type
	Desktop* desktop = (Desktop*)platform->internal;

	if (desktop->window) {
		glfwDestroyWindow(desktop->window);
	}
	glfwTerminate();
	platform_free(platform->internal, FALSE);
}

void* platform_allocate(u64 size, b8 aligned) {
	return malloc(size);
}
void platform_free(void* block, b8 aligned) {
	free(block);
}
void* platform_reallocate(void* block, u64 newsize, b8 aligned) {
	return realloc(block, newsize);
}
void* platform_zero_memory(void* block, u64 size) {
	return memset(block, 0, size);
}
void* platform_copy_memory(void* dest, const void* source, u64 size) {
	return memcpy(dest, source, size);
}
void* platform_set_memory(void* dest, i32 value, u64 size) {
	return memset(dest, value, size);
}
