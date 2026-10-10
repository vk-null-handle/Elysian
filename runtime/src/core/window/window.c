#include "window.h"
#include "core/logger/logger.h"
#include "core/memory/memory.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct window {
	GLFWwindow* handle;
};

Window* win_init(u32 width, u32 height, char* title) {
	Window* window = mem_alloc(sizeof(struct window));

	glfwInit();
	if (!glfwVulkanSupported()) {
		LOG_FATAL(ENGINE, "Vulkan is not supported on this system");
		glfwTerminate();
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetWindowUserPointer(window->handle, window);

	return window;
}

void win_shutdown(Window* window) {
	if (!window) {
		return;
	}

	glfwDestroyWindow(window->handle);
	glfwTerminate();
	mem_free(window, sizeof(struct window));
}

b8 win_should_close(const Window* window) {
	return glfwWindowShouldClose(window->handle);
}

void win_poll_events(void) {
	glfwPollEvents();
}

VkSurfaceKHR win_create_vk_surface(Window* window, VkInstance instance) {
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instance, window->handle, NULL, &surface) !=
		VK_SUCCESS) {
		return VK_NULL_HANDLE;
	}
	return surface;
}

const char** win_get_vk_instance_ext(u32* count) {
	return glfwGetRequiredInstanceExtensions(count);
}

void win_get_framebuffer_size(Window* window, i32* width, i32* height) {
	glfwGetFramebufferSize(window->handle, width, height);
}
