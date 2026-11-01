#ifndef PLATFORM_LINUX_X11_H
#define PLATFORM_LINUX_X11_H

 internal LinuxWindowHandle x11_create_window(Arena *arena, u32 window_height, u32 window_width,
 u32 disp_x, u32 disp_y);
 internal void x11_draw(LinuxWindowHandle *s, s32 start_x, s32 end_x, s32 start_y, s32 end_y);
 internal s32 x11_cleanup(LinuxWindowHandle *s);
 internal s32 x11_read_input();

 #define LinuxCreateWindow(arena, height, width, x, y) x11_create_window(arena, height, width, x, y)
 #define LinuxGetDisplayWdith(display, screen_num) DisplayWidth(display, screen_num)
 #define LinuxGetDisplayHeight(display, screen_num) DisplayHeight(display, screen_num);
 #define LinuxLoadBitMapFromFile(display, window, path, width, height, out, hotspot_x, hotspot_y)   \
 XReadBitmapFile(display, window, )

#endif
