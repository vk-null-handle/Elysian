#include "vk_backend.h"
#include "core/logger/logger.h"
#include "vk_instance.h"
#include "vk_device.h"
#include "vk_swapchain.h"

#include <vulkan/vulkan_core.h>

// Static vulkan context
static VulkanContext vkcontext;
static i32 cached_framebuffer_width = 0;
static i32 cached_framebuffer_height = 0;

b8 vulkan_backend_init(Window* window) {
	// TODO: custom allocator
	vkcontext.allocator = 0;

	// Use window framebuffer extent unless is 0x0
	win_get_framebuffer_size(window, &cached_framebuffer_width, &cached_framebuffer_height);
	vkcontext.framebuffer_width = (cached_framebuffer_width != 0) ? cached_framebuffer_width : 800;
	vkcontext.framebuffer_height = (cached_framebuffer_height != 0) ? cached_framebuffer_height : 600;

	if (!vk_instance_create(&vkcontext)) {
		LOG_FATAL(VULKAN, "Failed to create instance");
		return FALSE;
	}
	LOG_INFO(VULKAN, "Created instance");

	vkcontext.surface = win_create_vk_surface(window, vkcontext.instance.handle);
	if (!vkcontext.surface) {
		LOG_FATAL(VULKAN, "Failed to create surface");
		return FALSE;
	}
	LOG_DEBUG(VULKAN, "Created surface");

	if (!vk_logical_dev_create(&vkcontext)) {
		LOG_FATAL(VULKAN, "Failed to create logical device");
		return FALSE;
	}
	LOG_DEBUG(VULKAN, "Created logical device");

	vk_swapchain_create(&vkcontext, &vkcontext.swapchain, vkcontext.framebuffer_width, vkcontext.framebuffer_height);
	LOG_DEBUG(VULKAN, "Created swapchain");

	return TRUE;
}

void vulkan_backend_shutdown(void) {
}

void vulkan_backend_render_frame(RenderPacket* packet) {
}
