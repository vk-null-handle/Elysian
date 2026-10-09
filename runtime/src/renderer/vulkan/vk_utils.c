#include "vk_utils.h"
#include "core/logger/logger.h"
#include <vulkan/vk_enum_string_helper.h>

void vk_result_check(VkResult result, uint32_t lineNum, const char* funcName,
					 const char* fileName) {
	if (result != VK_SUCCESS) {
		char* msg = "VkResult is %s (line: %d, function: %s, file: %s)";
		LOG_FATAL(VULKAN, msg, string_VkResult(result), lineNum, funcName, fileName);
	}
}

// Vulkan function loader
VulkanFunctions vk_funcs;
b8 vk_load_instance_functions(VkInstance instance) {
	vk_funcs.create_debug_utils_messenger =
		(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	vk_funcs.destroy_debug_utils_messenger =
		(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (!vk_funcs.create_debug_utils_messenger ||
		!vk_funcs.destroy_debug_utils_messenger) {
		return FALSE;
	}

	return TRUE;
}
