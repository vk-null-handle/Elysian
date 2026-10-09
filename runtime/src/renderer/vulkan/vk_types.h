#pragma once
#include "defines.h"
#include <vulkan/vulkan_core.h>

typedef struct vulkan_image {
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;
} VulkanImage;

typedef struct vulkan_swapchain {
  u32 width;
  u32 height;
  b8 require_recreate;

  VkSurfaceFormatKHR image_format;
  VkSwapchainKHR handle;

  u32 n_images;
  VkImage *images;
  VkImageView *image_views;

  VulkanImage depth_image;
} VulkanSwapchain;

typedef struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    
    u32 n_formats;
    VkSurfaceFormatKHR* formats;
   
    u32 n_present_modes;
    VkPresentModeKHR* present_modes;
} VulkanSwapchainSupportInfo;

typedef struct vulkan_device {
  VkDevice logical_dev;
  VkPhysicalDevice physical_dev;

  VkQueue gfx_queue;
  u32 gfx_queue_fam_idx;
  
  VkPhysicalDeviceProperties properties;
  VulkanSwapchainSupportInfo swapchain_info;
  
  VkFormat depth_format;
} VulkanDevice;

typedef struct vulkan_instance {
  VkInstance handle;
  VkDebugUtilsMessengerEXT debug_messenger;
  
  const char** layers;
  const char** exts;
  u32 n_layers, n_ext;
} VulkanInstance;

typedef struct vulkan_context {
  VkAllocationCallbacks* allocator;
  
  VkSurfaceKHR surface;
  u32 framebuffer_width, framebuffer_height;
  
  VkSemaphore *render_complete;
  
  VulkanDevice device;
  VulkanInstance instance;
  VulkanSwapchain swapchain;
} VulkanContext;
