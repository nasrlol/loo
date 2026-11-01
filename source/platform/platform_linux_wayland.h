#ifndef BASE_PLATFORM_LINUX_WAYLAND_H
#define BASE_PLATFORM_LINUX_WAYLAND_H

//- state

// NOTE: one shm backed wl_buffer. we keep two and only draw into one the compositor released,
// writing into a buffer the compositor is still reading is what causes tearing.
typedef struct WaylandBuffer WaylandBuffer;
struct WaylandBuffer {
 struct wl_buffer *buffer;
 u32 *pixels;
 u32 width;
 u32 height;
 u32 size; // in bytes
 b32 busy; // attached and not released by the compositor yet
};

typedef struct WaylandState WaylandState;
struct WaylandState {
 //- connection
 struct wl_display    *display;
 struct wl_compositor *compositor;
 struct wl_shm        *shm;
 struct xdg_wm_base   *xdg_wm_base;
 struct wl_seat       *seat;

 //- window objects
 struct wl_surface   *surface;
 struct xdg_surface  *xdg_surface;
 struct xdg_toplevel *xdg_toplevel;

 //- input
 struct wl_keyboard *keyboard;
 struct wl_pointer  *pointer;
 PlatformInput input;

 //- window size
 u32 width;
 u32 height;

 //- presenting
 WaylandBuffer buffers[2];

 b32 configured;
 b32 should_close;
};

// NOTE: the platform agnostic handle, on wayland it's just the wayland state
struct PlatformWindow {
 WaylandState wayland;
};

//- public wayland  api

internal PlatformWindow *wayland_window_open(Arena *arena, WindowParams params);
// fills input. false once the window should close.
internal b32  wayland_window_pump(PlatformWindow *window, PlatformInput *input);
internal void wayland_window_present(PlatformWindow *window, u32 *pixels, u32 width, u32 height);
internal f32  wayland_window_refresh_hz(PlatformWindow *window);
internal void wayland_window_close(PlatformWindow *window);

#define PlatformWindowOpen(arena, params)                    wayland_window_open(arena, params)
#define PlatformWindowPump(window, input)                    wayland_window_pump(window, input)
#define PlatformWindowPresent(window, pixels, width, height) wayland_window_present(window, pixels, width, height)
#define PlatformWindowRefreshHz(window)                      wayland_window_refresh_hz(window)
#define PlatformWindowClose(window)                          wayland_window_close(window)

#endif // BASE_PLATFORM_LINUX_WAYLAND_H
