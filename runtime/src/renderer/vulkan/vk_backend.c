#include "vk_backend.h"
#include "core/logger/logger.h"
#include "vk_instance.h"

#include <vulkan/vulkan_core.h>

// Static vulkan context
static VulkanContext vkcontext;

b8 vulkan_backend_init(Window *window) {
  // TODO: custom allocator
  vkcontext.allocator = 0;

  if (!vk_instance_create(&vkcontext)) {
    LOG_FATAL(VULKAN, "Failed to create instance");
    return FALSE;
  }
  LOG_INFO(VULKAN, "Created instance");

  return TRUE;
}

void vulkan_backend_shutdown(void) {}

void vulkan_backend_render_frame(RenderPacket *packet) {}
