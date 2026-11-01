#ifndef RENDER_VULKAN_H
#define RENDER_VULKAN_H

//- generic window description
typedef struct WinParams WinParams;
struct WinParams {
 void *window_handle;
 int width;
 int height;
 int display_x;
 int display_y;
 const char *name;
 };


 typedef struct Params Params;
 struct Params {
 const char *name;
 int width;
 int height;
 // VkExtensionProperties *vk_properties;
 // VkInstance            *vk_instance;
};

internal void vk_render_entrypoint(Arena *arena, WinParams *engine, Params *window);

#endif // RENDER_VULKAN_H
