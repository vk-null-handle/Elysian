#pragma once
#include "defines.h"
// TEMP
#include <vulkan/vulkan_core.h>

typedef struct window Window;

Window* win_init(u32 width, u32 height, char* title);
void win_shutdown(Window *window);

b8 win_should_close(const Window *window);
void win_poll_events(void);

VkSurfaceKHR win_create_vk_surface(Window *window, VkInstance instance);
const char **win_get_vk_instance_ext(u32 *count);
void win_get_framebuffer_size(Window *window, i32 *width, i32 *height);

