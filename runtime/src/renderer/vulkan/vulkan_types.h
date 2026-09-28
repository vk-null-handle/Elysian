#pragma once
#include "core/logging/logger.h"
#include "defines.h"
#include "vulkan_utils.h"
#include <vulkan/vulkan.h>

#define MAX_FRAMES_IN_FLIGHT 2
#define VK_CHECK(expr)                                             \
	do {                                                           \
		VkResult result = (expr);                                  \
		if (result < 0) {                                          \
			LOG_FATAL("Vulkan: %s", vulkan_result_string(result)); \
		} else if (result != VK_SUCCESS) {                         \
			LOG_INFO("Vulkan: %s", vulkan_result_string(result));  \
		}                                                          \
	} while (0)

typedef enum render_pass_state {
	READY,
	RECORDING,
	IN_RENDER_PASS,
	RECORDING_ENDED,
	SUBMITTED,
	NOT_ALLOCATED
} RenderPassState;

typedef enum command_buffer_state {
	COMMAND_BUFFER_STATE_READY,
	COMMAND_BUFFER_STATE_RECORDING,
	COMMAND_BUFFER_STATE_IN_RENDER_PASS,
	COMMAND_BUFFER_STATE_RECORDING_ENDED,
	COMMAND_BUFFER_STATE_SUBMITTED,
	COMMAND_BUFFER_STATE_NOT_ALLOCATED
} CommandBufferState;

typedef struct render_pass {
	VkRenderPass handle;   // Handle of render pass
	RenderPassState state; // State of render pass

	f32 x, y, w, h; // Offset and width/height
	f32 r, g, b, a; // Clear color
	f32 depth;		// Depth clear value
	u32 stencil;	// Stencil clear value
} RenderPass;

typedef struct fence {
	VkFence handle;
	b8 is_signaled;
} Fence;

typedef struct command_buffer {
	VkCommandBuffer handle;	  // Handle of command buffer object
	CommandBufferState state; // Sate of command buffer
} CommandBuffer;

typedef struct framebuffer {
	VkFramebuffer handle;	  // Handle
	VkImageView* attachments; // Attachments
	u32 attachment_count;	  // Number of attachments
} Framebuffer;

typedef struct shader_data {
	size_t size; // Size of shader
	char* data;	 // Shader data
} ShaderData;

// For engine created images (Shadows, depth etc)
typedef struct image {
	VkImage handle;
	VkDeviceMemory memory;
	VkImageView view;
	u32 width;
	u32 height;
} Image;

typedef struct vulkan_swapchain {
	VkSwapchainKHR handle;

	u32 n_images; // Number of swapchain images
	VkImage* images;
	VkImageView* images_views;

	VkExtent2D extent; // Dimensions
	VkFormat image_format;

	VkSurfaceFormatKHR surface_format;
	VkPresentModeKHR surface_present_mode;
} VulkanSwapchain;

typedef struct swapchain_info {
	VkSurfaceCapabilitiesKHR capabilities;

	VkSurfaceFormatKHR* surface_formats;
	u32 n_formats;

	VkPresentModeKHR* surface_present_modes;
	u32 n_present_modes;
} SwapchainInfo;

typedef struct device {
	VkPhysicalDevice phys_dev; // Physical device
	VkDevice logical_dev;	   // Logical device

	i32 graphics_queue_family_index;
	i32 present_queue_family_index;
	i32 transfer_queue_family_index;
	VkQueue graphics_queue; // Graphics queue
	VkQueue present_queue;	// Present queue
	VkQueue transfer_queue; // Present queue

	SwapchainInfo swapchain_support;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memory;
} Device;

typedef struct renderer_frame {
	VkSemaphore image_available;
	VkSemaphore* render_finished;

	Fence in_flight_fence;

	VkCommandPool cmd_pool;
	CommandBuffer cmd_buffer;
} RendererFrame;

typedef struct vulkan_context {
	const char** layers;
	const char** exts;
	u32 n_layers, n_ext;

	VkSurfaceKHR surface;
	VkInstance instance;
	VkAllocationCallbacks* allocator;

	Device device;
	VulkanSwapchain swapchain;
	RenderPass render_pass;

	Framebuffer* framebuffers;
	u32 n_fbuffers;

	u32 current_frame; // Which frame-in-flight slot using
	RendererFrame frames[MAX_FRAMES_IN_FLIGHT];

	u32 image_index; // Which swapchain image

	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;
} VulkanContext;
