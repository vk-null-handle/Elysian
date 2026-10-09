#include "vk_image.h"
#include "core/logger/logger.h"
#include "renderer/vulkan/vk_utils.h"
#include <vulkan/vulkan_core.h>

static i32 find_memory_index(VulkanContext* vkcontext, u32 type_filter, u32 property_flags) {
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(vkcontext->device.physical_dev, &memory_properties);

	for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
		// Check each memory type to see if its bit is set to 1.
		if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
			return i;
		}
	}

	LOG_WARN(VULKAN, "Unable to find suitable memory type");
	return -1;
}

void vk_image_create(
	VulkanContext* vkcontext,
	VkImageType image_type,
	u32 width,
	u32 height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags memory_flags,
	b32 create_view,
	VkImageAspectFlags view_aspect_flags,
	VulkanImage* out_image) {

	out_image->width = width;
	out_image->height = height;

	// https://docs.vulkan.org/refpages/latest/refpages/source/VkImageCreateInfo.html
	VkImageCreateInfo image_create_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = image_type,
		.extent = {
			.width = width,
			.height = height,
			.depth = 1,
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.format = format,
		.tiling = tiling,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage = usage,
		.samples = VK_SAMPLE_COUNT_1_BIT,
	};

	VK_CHECK(vkCreateImage(vkcontext->device.logical_dev, &image_create_info, vkcontext->allocator, &out_image->handle));

	// Query memory requirements
	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(vkcontext->device.logical_dev, out_image->handle, &memory_requirements);

	i32 memory_type = find_memory_index(vkcontext, memory_requirements.memoryTypeBits, memory_flags);
	if (memory_type == -1) {
		LOG_ERROR(VULKAN, "Required memory type not found, Image not valid");
	}

	// Allocate memory
	VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = memory_type;
	VK_CHECK(vkAllocateMemory(vkcontext->device.logical_dev, &memory_allocate_info, vkcontext->allocator, &out_image->memory));

	// Bind the memory
	VK_CHECK(vkBindImageMemory(vkcontext->device.logical_dev, out_image->handle, out_image->memory, 0));

	// Create view
	if (create_view) {
		out_image->view = 0;
		vk_image_view_create(vkcontext, format, out_image, view_aspect_flags);
	}
}

void vk_image_view_create(
	VulkanContext* context,
	VkFormat format,
	VulkanImage* image,
	VkImageAspectFlags aspect_flags) {

	// https://docs.vulkan.org/refpages/latest/refpages/source/VkImageViewCreateInfo.html
	VkImageViewCreateInfo view_create_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image->handle,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = {
			.aspectMask = aspect_flags,
			.baseMipLevel = 0,
			.baseArrayLayer = 0,
			.levelCount = 1,
			.layerCount = 1,
		},
	};

	VK_CHECK(vkCreateImageView(context->device.logical_dev, &view_create_info, context->allocator, &image->view));
}

void vk_image_destroy(VulkanContext* vkcontext, VulkanImage* image) {
	if (image->view) {
		vkDestroyImageView(vkcontext->device.logical_dev, image->view, vkcontext->allocator);
		image->view = 0;
	}
	if (image->memory) {
		vkFreeMemory(vkcontext->device.logical_dev, image->memory, vkcontext->allocator);
		image->memory = 0;
	}
	if (image->handle) {
		vkDestroyImage(vkcontext->device.logical_dev, image->handle, vkcontext->allocator);
		image->handle = 0;
	}
}
