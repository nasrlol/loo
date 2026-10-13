#ifndef BASE_PLATFORM_H
#define BASE_PLATFORM_H

//- Linux
#if PLATFORM_LINUX || PLATFORM_UNIX

//- Types
typedef struct SharedLibrary SharedLibrary;
struct SharedLibrary {
    void *mem;
    const char *name;
};

typedef struct Directory Directory;
struct Directory {
    String8List *file_names;
};

#if 1 // TODO: @WAYLAND and @X11 implementation

typedef struct WindowParams WindowParams;
struct WindowParams {
     cstring name;
    int window_width;
    int window_height;
    int display_width;
    int display_height;

};

#define LinuxLoadSharedLibrary(arena, name) unix_load_shared_library(arena, name)
#define LoadSharedLibrary (arena, name) LinuxLoadSharedLibrary(arena, name)
#endif

//- Shared libraries
internal void *unix_shared_library_get_proc_from_name(SharedLibrary *library, const char *name);
internal void unix_load_shared_library(MemArena *arena);
internal int unix_shared_library_close(SharedLibrary *library);

internal String8 unix_get_env(const char *var);

//- File I/O
internal b32 unix_file_write(String8 data, String8 path);
internal String8 unix_file_load(MemArena *arena, String8 path);
internal b32 unix_file_exists(String8 path);
internal String8List *unix_open_dir(MemArena *arena,
                                    String8 *original_path); // TODO: change the return value

internal s64 unix_read(int fd, const void *buf, u64 count);
internal void unix_os_exit() __attribute__((noreturn));

internal s64 unix_log(int fd, const void *buf, u64 count);
internal void unix_log_with_args(const char *format, ...);

#define FileLoad(arena, path) unix_file_load(arena, path)
#define FileWrite(data, path) unix_file_write(data, path)
#define FileExists(path) unix_file_exists(path)
#define DirLoad(arena, path) unix_open_dir(arena, path)

#if DEBUG
#define OSExit(code) _exit(code)
#define Log(...) unix_log_with_args(__VA_ARGS__)
#define LogStr8(message) Log(Str8Fmt, Str8Arg(message))
#else
#define OSExit(code) Unused(code)
#define Log(...) ((void)0)
#define LogStr8(message) ((void)0)
#endif // disable comments when debug mode is disabled

//- Entrypoint and platform window layer
#define ENTRYPOINT() int main(int count, char **args)

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

#include "base_platform_linux_x11.cpp"

//- Wayland
#elif WAYLAND

typedef struct WaylandState WaylandState;
struct WaylandState {
    //- connection
    struct wl_display  *display;
    struct wl_registry *registry;

    struct wl_compositor *compositor;
    struct wl_shm        *shm;
    struct xdg_wm_base   *xdg_wm_base;
    struct wl_seat       *seat;

    //- window objects

    struct wl_surface   *surface;
    struct xdg_surface  *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    //- keyboard input
    struct wl_keyboard *keyboard;
    b32 keys_down[256]; // indexed by linux scancode

    //- mouse (pointer) input

    struct wl_pointer *pointer; // wl_pointer is a predefined <wayland-client.h> struct
    f32 mouse_x, mouse_y;

    b32 mouse_buttons[3];
    f32 x_offset, y_offset;
    b32 mouse_in_window;



    //- software framebuffer
    struct wl_buffer *buffer;
    u8 *pixels;
    u32 width;
    u32 height;
    u32 stride;
    u32 size;

    b32 should_close;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#include "xdg-shell-protocol.c"
#pragma GCC diagnostic pop


#include "base_platform_linux_wayland_mouse_input.cpp"
#include "base_platform_linux_wayland_keyboard_input.cpp"
#include "base_platform_linux_wayland.cpp"

internal void wayland_entrypoint(MemArena *arena, WindowParams *params);
#define PlatformWindowEntrypoint(arena, params) wayland_entrypoint(arena, params);

#endif // X11 and Wayland -- set by build params

//- Windows
#elif PLATFORM_WINDOWS

//- Types
typedef struct Win32WindowHandle Win32WindowHandle;
struct Win32WindowHandle {
    HWND window_handle;
    HINSTANCE instance;
    HDC dc;
};

global_variable b32 Win32Running = true;

#define STDIN 0
#define STDOUT 1
#define STDERR 2

//- Shared libraries
internal void *win32_load_library();
internal void win32_library_get_proc();

//- File I/O
internal b32 win32_file_exists(u8 *path);
internal b32 win32_file_write(String8 data, String8 path);
internal String8 win32_file_load(MemArena *arena, String8 file);
internal String8List win32_open_dir(MemArena *arena, String8 *original_path); // TODO: ...

internal b32 win32_os_exit();
internal void win32_handle_error(MemArena *arena, DWORD error);
internal u64 safe_truncate_64(u64 value);

internal void win32_log(String8List messages);

#define FileLoad(arena, path) win32_file_load(arena, path)
#define FileWrite(data, path) win32_file_write(data, path)
#define FileExists(path) win32_file_exists(path)
#define Win32FileExists(path) win32_file_exists(path)
#define DirLoad(arena, path) win32_open_dir(arena, original_path);

#define OSExit(code) win32_os_exit()

#if COMPILER_GCC || COMPILER_CLANG

#define PRINTF_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
internal void win32_logf(const char *fmt, ...) PRINTF_FORMAT;

#elif COMPILER_MSVC
internal void win32_logf(_Printf_format_string_ const char *fmt, ...);

#define Log(fmt, ...) win32_logf(fmt, ##__VA_ARGS__)

#endif // COMPILER_GCC || COMPILER_CLANG

//- Entrypoint and platform window layer
#if COMPILER_MSVC
#pragma warning(disable : 28251) // disabled the annotations warning that are defined in winbase.h
#define ENTRYPOINT(hinstance, prev_instance, cmd_line, show_cmd)                                   \
    int WINAPI WinMain(hinstance, prev_instance, cmd_line, show_cmd)
#endif // COMPIPILER_MSVC DISABLE WARNING

internal LRESULT CALLBACK win32_window_proc(HWND handle, UINT messages, WPARAM wparam,
                                            LPARAM lparam);
internal HWND win32_create_window(HINSTANCE instance, int height, int width, int display_x,
                                  int display_y, const char *window_name);

#define Win32CreateWindow(instance, height, width, display_x, display_y)                           \
    win32_create_window(instance, height, width, display_x, display_y)

#define PlatformCreateWindow(instance, height, width, display_x, display_y)                        \
    Win32CreateWindow(instance, height, width, display_x, display_y)

#endif // PLATFORM_LINUX || PLATFORM_WINDOWS

#endif // BASE_PLATFORM_H
