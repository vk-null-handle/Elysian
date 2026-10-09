#pragma once
#include "defines.h"
#include <vulkan/vulkan_core.h>

typedef struct vulkan_context {
  // Instance and device stuff
  VkInstance instance;
  VkDebugUtilsMessengerEXT debug_messenger;
  VkAllocationCallbacks* allocator;
  
  const char** layers;
  const char** exts;
  u32 n_layers, n_ext;

} VulkanContext;
