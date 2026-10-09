#pragma once
#include "defines.h"
#include <vulkan/vulkan_core.h>

typedef struct vulkan_functions {
    PFN_vkCreateDebugUtilsMessengerEXT create_debug_utils_messenger;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messenger;
} VulkanFunctions;

extern VulkanFunctions vk_funcs;

b8 vk_load_instance_functions(VkInstance instance);


void vk_result_check(VkResult result, uint32_t lineNum, const char* funcName, const char* fileName);

#ifdef DEBUG
#define VK_CHECK(result) vk_result_check(result, __LINE__, __func__, __FILE__)
#else
#define VK_CHECK(result) ((void)0);
#endif
