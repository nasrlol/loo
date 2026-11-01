#ifndef PLATFORM_WIN32_H
#define PLATFORM_WIN32_H

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

#endif
