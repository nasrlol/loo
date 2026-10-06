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

#if 0 // TODO: @WAYLAND and @X11 implementation
typedef struct LinuxWindowHandle LinuxWindowHandle;
struct LinuxWindowHandle {
    Display *display;
    Window window;
};

#define LinuxLoadSharedLibrary(library, name) unix_
#endif

//- Shared libraries
internal void *unix_shared_library_get_proc_from_name(SharedLibrary *library, const char *name);
internal void unix_load_shared_library(MemArena *arena, SharedLibrary *library);
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

#define OSExit() unix_os_exit();

#if COMPILER_GCC || COMPILER_CLANG
// TODO: __VA_ARGS__ CLANG
#define Log(...) unix_log_with_args(__VA_ARGS__)
#define LogStr8(message)                                                                           \
    Log(Str8Fmt, Str8Arg(message)) // NOTE: for this to wwork we need the /analyze argument
#endif                             // COMPILER_GCC || COMPILER_CLANG

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

//- protocol values defined in the wayland library. it's a library load less i guess
global_variable const u32 wayland_display_object_id = 1;
global_variable const u16 wayland_wl_registry_event_global = 0;
global_variable const u16 wayland_shm_pool_event_format = 0;
global_variable const u16 wayland_wl_buffer_event_release = 0;
global_variable const u16 wayland_xdg_wm_base_event_ping = 0;
global_variable const u16 wayland_xdg_toplevel_event_configure = 0;
global_variable const u16 wayland_xdg_toplevel_event_close = 1;
global_variable const u16 wayland_xdg_surface_event_configure = 0;
global_variable const u16 wayland_wl_display_get_registry_opcode = 1;
global_variable const u16 wayland_wl_registry_bind_opcode = 0;
global_variable const u16 wayland_wl_compositor_create_surface_opcode = 0;
global_variable const u16 wayland_xdg_wm_base_pong_opcode = 3;
global_variable const u16 wayland_xdg_surface_ack_configure_opcode = 4;
global_variable const u16 wayland_wl_shm_create_pool_opcode = 0;
global_variable const u16 wayland_xdg_wm_base_get_xdg_surface_opcode = 2;
global_variable const u16 wayland_wl_shm_pool_create_buffer_opcode = 0;
global_variable const u16 wayland_wl_surface_attach_opcode = 1;
global_variable const u16 wayland_xdg_surface_get_toplevel_opcode = 1;
global_variable const u16 wayland_wl_surface_commit_opcode = 6;
global_variable const u16 wayland_wl_display_error_event = 0;
global_variable const u32 wayland_format_xrgb8888 = 1;
global_variable const u32 wayland_header_size = 8;
global_variable const u32 color_channels = 4;

enum state_state_t: u16 {
	STATE_NONE,
	STATE_SURFACE_ACKED_CONFIGURE,
	STATE_SURFACE_ATTACHED,
};

typedef struct state_t state_t;
struct state_t {
	uint32_t wl_registry;
	uint32_t wl_shm;
	uint32_t wl_shm_pool;
	uint32_t wl_buffer;
	uint32_t xdg_wm_base;
	uint32_t xdg_surface;
	uint32_t wl_compositor;
	uint32_t wl_surface;
	uint32_t xdg_toplevel;
	uint32_t stride;
	uint32_t w;
	uint32_t h;
	uint32_t shm_pool_size;
	int shm_fd;
	uint8_t *shm_pool_data;
	
	state_state_t state;
};


#include "base_platform_linux_wayland.cpp"

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
#define OSExit() win32_os_exit();

#if COMPILER_GCC || COMPILER_CLANG

#define PRINTF_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
internal void win32_logf(const char *fmt, ...) PRINTF_FORMAT;

#elif COMPILER_MSVC
internal void win32_logf(_Printf_format_string_ const char *fmt, ...);

#define Log(fmt, ...) win32_logf(fmt, __VA_ARGS__)

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
