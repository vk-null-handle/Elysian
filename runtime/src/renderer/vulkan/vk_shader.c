
#include "vk_shader.h"
#include "core/memory/memory.h"
#include "core/logger/logger.h"

#include <stdio.h>
#include <vulkan/vulkan_core.h>

VkShaderModule vk_shader_load(VulkanContext* vkcontext, const char* file_name, shaderc_shader_kind kind) {
	// read shader file from disk
	char shader_path[512];
	snprintf(shader_path, sizeof(shader_path), "../../runtime/assets/shaders/%s", file_name);

	FILE* file = fopen(shader_path, "rb");
	if (!file) {
		LOG_FATAL(VULKAN, "Specified shader file doesn't exist");
		return VK_NULL_HANDLE;
	}

	fseek(file, 0, SEEK_END);
	u32 file_size = ftell(file);
	rewind(file);

	char* src = mem_alloc(file_size + 1);
	if (!src) {
		fclose(file);
		return VK_NULL_HANDLE;
	}

	fread(src, 1, file_size, file);
	fclose(file);
	src[file_size] = '\0';

	// Compile the shader to SPIR-V
	LOG_DEBUG(VULKAN, "Compiling shader: %s", shader_path);

	shaderc_compiler_t compiler = shaderc_compiler_initialize();

	shaderc_compile_options_t opts = shaderc_compile_options_initialize();

	shaderc_compile_options_set_target_env(opts, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
	shaderc_compile_options_set_target_spirv(opts, shaderc_spirv_version_1_6);
	shaderc_compile_options_set_optimization_level(opts, shaderc_optimization_level_performance);

	shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, src, file_size, kind, file_name, "main", opts);

	mem_free(src, file_size + 1);

	if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
		LOG_FATAL(VULKAN, "Shader Compilation Error: %s", shaderc_result_get_error_message(result));

		shaderc_result_release(result);
		shaderc_compile_options_release(opts);
		shaderc_compiler_release(compiler);

		return VK_NULL_HANDLE;
	}

	// Pass SPIR-V to Vulkan and create shader-module
	VkShaderModuleCreateInfo module_create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = shaderc_result_get_length(result),
		.pCode = (const u32*)shaderc_result_get_bytes(result),
	};

	VkShaderModule shader_module = VK_NULL_HANDLE;

	if (vkCreateShaderModule(vkcontext->device.logical_dev, &module_create_info, vkcontext->allocator, &shader_module) != VK_SUCCESS) {
		LOG_FATAL(VULKAN, "Error creating shader module");

		shaderc_result_release(result);
		shaderc_compile_options_release(opts);
		shaderc_compiler_release(compiler);

		return VK_NULL_HANDLE;
	}

	shaderc_result_release(result);
	shaderc_compile_options_release(opts);
	shaderc_compiler_release(compiler);

	return shader_module;
}
