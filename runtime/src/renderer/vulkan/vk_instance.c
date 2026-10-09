#include "vk_instance.h"
#include "core/logger/logger.h"
#include "core/window/window.h"
#include "vk_utils.h"

b8 vk_instance_create(VulkanContext* vkcontext) {
	static const char* layers[] = {"VK_LAYER_KHRONOS_validation"};

	u32 n_instance_exts;
	const char** win_exts = win_get_vk_instance_ext(&n_instance_exts);
	const char* exts[100];
	for (u32 i = 0; i < n_instance_exts; i++) {
		exts[i] = win_exts[i];
	}

	// Debug messenger info
#ifdef DEBUG
	exts[n_instance_exts] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	n_instance_exts++;

	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
						   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					   VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
					   VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		.pfnUserCallback = debug_callback,
	};
#endif

	// https://docs.vulkan.org/refpages/latest/refpages/source/VkApplicationInfo.html
	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "TestEngine",
		.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
		.apiVersion = VK_API_VERSION_1_4,
	};

	// https://docs.vulkan.org/refpages/latest/refpages/source/VkInstanceCreateInfo.html
	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef DEBUG
		.pNext = &debug_create_info,
#endif
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 1,
		.enabledExtensionCount = n_instance_exts,
		.ppEnabledLayerNames = layers,
		.ppEnabledExtensionNames = exts,
	};

	VK_CHECK(
		vkCreateInstance(&inst_info, vkcontext->allocator, &vkcontext->instance.handle));

	if (!vk_load_instance_functions(vkcontext->instance.handle)) {
		LOG_FATAL(VULKAN, "Failed to load Vulkan debug functions");
		return FALSE;
	}

	// Create debug messenger
#ifdef DEBUG
	VK_CHECK(vk_funcs.create_debug_utils_messenger(
		vkcontext->instance.handle, &debug_create_info, vkcontext->allocator,
		&vkcontext->instance.debug_messenger));
	LOG_DEBUG(VULKAN, "Created debug messenger");
#endif
	return TRUE;
}

#ifdef DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			   VkDebugUtilsMessageTypeFlagsEXT message_types,
			   const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
			   void* user_data) {

	/*if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
					LOG_TRACE(VULKAN, "Validation Layer: %s",
	callback_data->pMessage);
	}
	if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
					LOG_INFO(VULKAN, "Validation Layer: %s",
	callback_data->pMessage);
	}*/

	if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		LOG_WARN(VULKAN, "Validation Layer: %s", callback_data->pMessage);
	}

	if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
		LOG_ERROR(VULKAN, "Validation Layers: %s", callback_data->pMessage);
	}
	return VK_FALSE;
}
#endif
