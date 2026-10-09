#include "vk_backend.h"
#include "core/logger/logger.h"
#include "vk_utils.h"
#include "vk_instance.h"
#include "vk_device.h"
#include "vk_swapchain.h"
#include "vk_pipeline.h"
#include "vk_sync.h"
#include "vk_shader.h"

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

	vkcontext.vert_shader = vk_shader_load(&vkcontext, "shader_vert.glsl", shaderc_vertex_shader);
	vkcontext.frag_shader = vk_shader_load(&vkcontext, "shader_frag.glsl", shaderc_fragment_shader);
	if (!vkcontext.vert_shader || !vkcontext.frag_shader) {
		LOG_FATAL(VULKAN, "Failed to compile shaders");
		return FALSE;
	}
	LOG_DEBUG(VULKAN, "Compiled and loaded shaders");

	vkcontext.pipeline.handle = vk_graphics_pipeline_create(&vkcontext);
	if (!vkcontext.pipeline.handle) {
		LOG_FATAL(VULKAN, "Failed to initialize the graphics pipeline");
		return FALSE;
	}
	LOG_DEBUG(VULKAN, "Created graphics pipeline");

	vk_sync_resources_create(&vkcontext);
	LOG_DEBUG(VULKAN, "Created sync resources");

	return TRUE;
}

void vulkan_backend_shutdown(void) {
	vkDeviceWaitIdle(vkcontext.device.logical_dev);

	if (vkcontext.timeline) {
		vkDestroySemaphore(vkcontext.device.logical_dev, vkcontext.timeline, vkcontext.allocator);
	}

	for (u8 i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		struct frame_resources* frame = &vkcontext.frame_resources[i];
		if (frame->image_acquired) {
			vkDestroySemaphore(vkcontext.device.logical_dev, frame->image_acquired, vkcontext.allocator);
		}
	}

	if (vkcontext.pipeline.pipeline_layout) {
		vkDestroyPipelineLayout(vkcontext.device.logical_dev, vkcontext.pipeline.pipeline_layout, vkcontext.allocator);
	}
	if (vkcontext.pipeline.handle) {
		vkDestroyPipeline(vkcontext.device.logical_dev, vkcontext.pipeline.handle, vkcontext.allocator);
	}

	if (vkcontext.vert_shader) {
		vkDestroyShaderModule(vkcontext.device.logical_dev, vkcontext.vert_shader, vkcontext.allocator);
	}
	if (vkcontext.frag_shader) {
		vkDestroyShaderModule(vkcontext.device.logical_dev, vkcontext.frag_shader, vkcontext.allocator);
	}

	if (vkcontext.swapchain.handle) {
		vk_swapchain_destroy(&vkcontext, &vkcontext.swapchain);
	}

	if (vkcontext.surface) {
		vkDestroySurfaceKHR(vkcontext.instance.handle, vkcontext.surface, vkcontext.allocator);
	};

	if (vkcontext.device.logical_dev) {
		vkDestroyDevice(vkcontext.device.logical_dev, vkcontext.allocator);
	}

	// Only happens if valid, so no gaurds needed
	if (vkcontext.instance.debug_messenger) {
		vk_funcs.destroy_debug_utils_messenger(vkcontext.instance.handle, vkcontext.instance.debug_messenger, vkcontext.allocator);
	}

	if (vkcontext.instance.handle) {
		vkDestroyInstance(vkcontext.instance.handle, vkcontext.allocator);
	}
}

void vulkan_backend_render_frame(RenderPacket* packet) {
}
