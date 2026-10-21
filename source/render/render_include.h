#ifndef RENDER_INCLUDE_H
#define RENDER_INCLUDE_H

// NOTE: sits on top of base, doesn't know about the platform layer.

#ifndef VULKAN
#define VULKAN 0
#endif
#ifndef OPENGL
#define OPENGL 0
#endif

#if VULKAN
// #include <vulkan/vulkan.h>
#endif

//-
#include "render_2d.h"

#if VULKAN
#include "render_vulkan.h"
#include "render_vulkan.cpp"
#endif

#endif // RENDER_INCLUDE_H
