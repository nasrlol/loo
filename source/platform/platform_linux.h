#ifndef PLATFORM_LINUX_H
#define PLATFORM_LINUX_H

 #define PlatformCreateWindow(arena, height, width, display_x, display_y)                           \
 LinuxCreateWindow(arena, height, width, display_x, display_y)

 #define PlatformGetDisplayWidth(display, screen_num)                                               \
 LinuxDisplayWdith(display, screen_num) // defined externally

 #define PlatformGetDisplayHeight(display, screen_num)                                              \
 LinuxDisplayHeight(display, screen_num) // defined externally

 #define PlatformLoadBitmapFromFile(display, window, path, width, height, out, hotspot_x,           \
 hotspot_y)                                                      \
 LinuxLoadBitMapFromFile(display, window, path, width, height, out, hotspot_x, hotspot_y)

#endif
