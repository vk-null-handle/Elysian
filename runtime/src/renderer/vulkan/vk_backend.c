#include "vk_backend.h"
#include "core/logger/logger.h"
#include "vk_utils.h"
#include "vk_instance.h"
#include "vk_device.h"
#include "vk_swapchain.h"
#include "vk_pipeline.h"
#include "vk_sync.h"
#include "vk_cmd_buffer.h"
#include "vk_shader.h"

#include <vulkan/vulkan_core.h>

// Static vulkan context
static VulkanContext vkcontext;

b8 vulkan_backend_init(Window* window) {
	// TODO: custom allocator
	vkcontext.allocator = 0;
	// Set initial signal semaphore value
	vkcontext.next_signal_value = MAX_FRAMES_IN_FLIGHT + 1;

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

	i32 width, height;
	win_get_framebuffer_size(window, &width, &height);
	if (width == 0 || height == 0) {
		width = 800;
		height = 600;
	}

	vk_swapchain_create(&vkcontext, &vkcontext.swapchain, width, height);
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

	vk_command_buffers_create(&vkcontext);
	LOG_DEBUG(VULKAN, "Created command buffers and pool");

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
		if (frame->command_pool) {
			vkDestroyCommandPool(vkcontext.device.logical_dev, frame->command_pool, vkcontext.allocator);
		}
	}

	if (vkcontext.pipeline.layout) {
		vkDestroyPipelineLayout(vkcontext.device.logical_dev, vkcontext.pipeline.layout, vkcontext.allocator);
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

void vulkan_backend_render_frame(RenderPacket* packet, Window* window) {
	// Recreate swapchain if needed
	if (vkcontext.swapchain.require_recreate) {
		i32 width, height;
		win_get_framebuffer_size(window, &width, &height);
		if (width == 0 || height == 0) {
			return; // Wait until the window is restored.
		}

		vkDeviceWaitIdle(vkcontext.device.logical_dev);
		vk_swapchain_destroy(&vkcontext, &vkcontext.swapchain);
		vk_swapchain_create(&vkcontext, &vkcontext.swapchain, width, height);
		vkcontext.swapchain.require_recreate = FALSE;
		LOG_INFO(VULKAN, "Swapchain resized");
	}

	// Which frame's synchronization resources to use
	u32 frame_res_index = vkcontext.frame_index++ % MAX_FRAMES_IN_FLIGHT;
	// Value current frame will set timeline semaphore when it completes,
	// so that future frames know what to wait on until resources are avaliable
	u64 signal_value = vkcontext.next_signal_value++;
	// Value current frame will wait until it begins using the resources
	const u64 wait_value = signal_value - MAX_FRAMES_IN_FLIGHT;

	// https://docs.vulkan.org/refpages/latest/refpages/source/VkSemaphoreWaitInfo.html
	VkSemaphoreWaitInfo wait_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
		// Only waiting for the one semaphore
		.semaphoreCount = 1,
		.pSemaphores = &vkcontext.timeline,
		.pValues = &wait_value,
	};
	// Wiat for timeline semaphore to reach signal value
	vkWaitSemaphores(vkcontext.device.logical_dev, &wait_info, UINT64_MAX);

	// Now its safe to start recording commands
	FrameResources* frame_res = &vkcontext.frame_resources[frame_res_index];
	// Reset command pool
	vkResetCommandPool(vkcontext.device.logical_dev, frame_res->command_pool, 0);

	// Sempaphore to singal when swapchain image is acquired
	VkSemaphore image_acquire = vkcontext.frame_resources[frame_res_index].image_acquired;
	// Get swapchain image for this frame
	u32 image_index = 0;
	VkResult acquire_result = vkAcquireNextImageKHR(vkcontext.device.logical_dev, vkcontext.swapchain.handle, UINT64_MAX, image_acquire, NULL, &image_index);

	// Handle resize and out-of-date images
	if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
		vkcontext.swapchain.require_recreate = TRUE;
		return;
	}
	if (acquire_result == VK_SUBOPTIMAL_KHR) {
		// Can render this frame but recreate next frame
		vkcontext.swapchain.require_recreate = TRUE;
	}

	// Begin recording commands
	// https://docs.vulkan.org/refpages/latest/refpages/source/VkCommandBufferBeginInfo.html
	VkCommandBufferBeginInfo cmd_begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		// Only be submited to queue once before needing to be reset
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	// Set state
	frame_res->command_buffer.state = COMMAND_BUFFER_STATE_RECORDING;
	vkBeginCommandBuffer(frame_res->command_buffer.handle, &cmd_begin_info);

	// Transition the color and depth images
	// https://docs.vulkan.org/refpages/latest/refpages/source/VkImageMemoryBarrier2.html
	VkImageMemoryBarrier2 layout_barriers[] = {
		// Color image barrier
		{
			// The swapchain image is prepared to receive rendered color output
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			// Dont care what old image format was
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			// Use image as color attachment
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.image = vkcontext.swapchain.images[image_index],
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		},
		// Depth image barrier
		// The swapchain depth image is prepared to receive depth output
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
			.srcAccessMask = 0,
			.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT, // Both specified to control memory access at both stages (write)
			.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			// Dont care what old image format was
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			// Use image as color attachment
			.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			.image = vkcontext.swapchain.depth_image.handle,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		},
	};
	u32 n_layout_barriers = sizeof(layout_barriers) / sizeof(layout_barriers[0]);

	// Record the two barriers into command buffer
	VkDependencyInfo dep_info = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = n_layout_barriers,
		.pImageMemoryBarriers = layout_barriers,
	};
	vkCmdPipelineBarrier2(frame_res->command_buffer.handle, &dep_info);

	// Setup the attachments (color and depth) and begin rendering (dynamic)
	VkRenderingAttachmentInfo color_attach_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = vkcontext.swapchain.image_views[image_index],
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,	 // clear the image
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE, // keep data for presentation
		// Clear color
		.clearValue = {
			.color = {0.01f, 0.01f, 0.01f, 1},
		},
	};
	VkRenderingAttachmentInfo depth_attach_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = vkcontext.swapchain.depth_image.view,
		.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,		 // clear the depth data
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, // don't care after rendering
		.clearValue = {
			.depthStencil = {1.0f, 0},
		},
	};

	VkRenderingInfo rendering_info = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = {
			.offset = {.x = 0, .y = 0},
			.extent = {
				.width = vkcontext.swapchain.width,
				.height = vkcontext.swapchain.height,
			},
		},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attach_info,
		.pDepthAttachment = &depth_attach_info,
	};

	// Set command buffer state
	frame_res->command_buffer.state = COMMAND_BUFFER_STATE_IN_RENDERING;
	// Begin dynamic rendering
	vkCmdBeginRendering(frame_res->command_buffer.handle, &rendering_info);
	{
		// Set viewport and scissor state
		VkViewport viewport = {
			.x = 0,
			.y = 0,
			.width = (float)vkcontext.swapchain.width,
			.height = (float)vkcontext.swapchain.height,
		};
		vkCmdSetViewport(frame_res->command_buffer.handle, 0, 1, &viewport);

		VkRect2D scissor = {
			.offset = {.x = 0, .y = 0},
			.extent = {
				.width = vkcontext.swapchain.width,
				.height = vkcontext.swapchain.height,
			},
		};
		vkCmdSetScissor(frame_res->command_buffer.handle, 0, 1, &scissor);

		// Draw triangle
		vkCmdBindPipeline(frame_res->command_buffer.handle, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext.pipeline.handle);
		vkCmdDraw(frame_res->command_buffer.handle, 3, 1, 0, 0);
	}
	frame_res->command_buffer.state = COMMAND_BUFFER_STATE_RENDERING_ENDED;
	// End dynamic rendering
	vkCmdEndRendering(frame_res->command_buffer.handle);

	// Transition the swapchain image from color attachment to presentation so we can show it
	VkImageMemoryBarrier2 present_layout_barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_2_NONE, // Nothing is waiting, but the cache is flushed and layout is transition
		.dstAccessMask = 0,
		// From color attachment to present layout
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.image = vkcontext.swapchain.images[image_index],
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	// Record the barrier into command buffer
	VkDependencyInfo present_dep_info = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &present_layout_barrier,
	};
	vkCmdPipelineBarrier2(frame_res->command_buffer.handle, &present_dep_info);

	frame_res->command_buffer.state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
	// End command buffer
	vkEndCommandBuffer(frame_res->command_buffer.handle);

	// Ensure swapchain image is actually vailable to start color output
	// https://docs.vulkan.org/refpages/latest/refpages/source/VkSemaphoreSubmitInfo.html
	VkSemaphoreSubmitInfo image_acquire_wait_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		.semaphore = image_acquire,
		.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, // Wait before drawing to image
	};
	// signal that the image can be presented
	VkSemaphoreSubmitInfo semaphore_signals[] = {
		{// Render work completion signal
		 .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
		 .semaphore = vkcontext.render_complete[image_index],
		 .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT},
		{
			// Entire frame is completed (timeline)
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = vkcontext.timeline,
			.value = signal_value,
			.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		},
	};
	u32 n_semaphore_signals = sizeof(semaphore_signals) / sizeof(semaphore_signals[0]);

	VkCommandBufferSubmitInfo cmd_submit_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = frame_res->command_buffer.handle,
	};

	VkSubmitInfo2 submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.waitSemaphoreInfoCount = 1,
		.pWaitSemaphoreInfos = &image_acquire_wait_info, // Ensure the image is ready
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &cmd_submit_info,
		.signalSemaphoreInfoCount = n_semaphore_signals,
		.pSignalSemaphoreInfos = semaphore_signals,
	};
	vkQueueSubmit2(vkcontext.device.gfx_queue, 1, &submit_info, NULL);

	// Present the image
	VkPresentInfoKHR present_info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &vkcontext.render_complete[image_index], // Render work completed semaphore to set
		.swapchainCount = 1,
		.pSwapchains = &vkcontext.swapchain.handle,
		.pImageIndices = &image_index,
		.pResults = NULL,
	};

	vkQueuePresentKHR(vkcontext.device.gfx_queue, &present_info);
}
