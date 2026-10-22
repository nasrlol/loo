#ifndef PLATFORM_H
#define PLATFORM_H

//- Types

typedef struct WindowParams WindowParams;
struct WindowParams {
 cstring name;
 int window_width;
 int window_height;
 int display_width;
 int display_height;

 };

 typedef struct PlatformWindow PlatformWindow;

 // NOTE: raw input as the window backend sees it. keys are indexed by linux evdev scancode
 // (KEY_W, KEY_ESC, ...), mouse position is already in framebuffer pixels.
 typedef struct PlatformButton PlatformButton;
 struct PlatformButton {
 s32 half_transition_count;
 b32 ended_down;
 };

 typedef struct PlatformInput PlatformInput;
 struct PlatformInput {
 PlatformButton keys[256];
 PlatformButton mouse[3]; // BTN_LEFT, BTN_RIGHT, BTN_MIDDLE
 s32 mouse_x, mouse_y;
 b32 mouse_in_window;
 };

 //- Linux
 #if PLATFORM_LINUX

 #define PlatformCreateWindow(arena, height, width, display_x, display_y)                           \
 LinuxCreateWindow(arena, height, width, display_x, display_y)

 #define PlatformGetDisplayWidth(display, screen_num)                                               \
 LinuxDisplayWdith(display, screen_num) // defined externally

 #define PlatformGetDisplayHeight(display, screen_num)                                              \
 LinuxDisplayHeight(display, screen_num) // defined externally

 #define PlatformLoadBitmapFromFile(display, window, path, width, height, out, hotspot_x,           \
 hotspot_y)                                                      \
 LinuxLoadBitMapFromFile(display, window, path, width, height, out, hotspot_x, hotspot_y)

 //- X11
 #if X11
 internal LinuxWindowHandle x11_create_window(MemArena *arena, u32 window_height, u32 window_width,
 u32 disp_x, u32 disp_y);
 internal void x11_draw(LinuxWindowHandle *s, s32 start_x, s32 end_x, s32 start_y, s32 end_y);
 internal s32 x11_cleanup(LinuxWindowHandle *s);
 internal s32 x11_read_input();

 #define LinuxCreateWindow(arena, height, width, x, y) x11_create_window(arena, height, width, x, y)
 #define LinuxGetDisplayWdith(display, screen_num) DisplayWidth(display, screen_num)
 #define LinuxGetDisplayHeight(display, screen_num) DisplayHeight(display, screen_num);
 #define LinuxLoadBitMapFromFile(display, window, path, width, height, out, hotspot_x, hotspot_y)   \
 XReadBitmapFile(display, window, )

 //- Wayland
 #elif WAYLAND

 #define PlatformWindowOpen(arena, params)                    wayland_window_open(arena, params)
 #define PlatformWindowPump(window, input)                    wayland_window_pump(window, input)
 #define PlatformWindowPresent(window, pixels, width, height) wayland_window_present(window, pixels, width, height)
 #define PlatformWindowRefreshHz(window)                      wayland_window_refresh_hz(window)
 #define PlatformWindowClose(window)                          wayland_window_close(window)

 #endif // X11 and Wayland -- set by build params

 //- Windows
 #elif PLATFORM_WINDOWS

 typedef struct Win32WindowHandle Win32WindowHandle;
 struct Win32WindowHandle {
 HWND window_handle;
 HINSTANCE instance;
 HDC dc;
 };

 global_variable b32 Win32Running = true;

 internal LRESULT CALLBACK win32_window_proc(HWND handle, UINT messages, WPARAM wparam,
 LPARAM lparam);
 internal HWND win32_create_window(HINSTANCE instance, int height, int width, int display_x,
 int display_y, const char *window_name);

 #define Win32CreateWindow(instance, height, width, display_x, display_y)                           \
 win32_create_window(instance, height, width, display_x, display_y)

 #define PlatformCreateWindow(instance, height, width, display_x, display_y)                        \
 Win32CreateWindow(instance, height, width, display_x, display_y)

#endif // PLATFORM_LINUX || PLATFORM_WINDOWS

#endif // PLATFORM_H
