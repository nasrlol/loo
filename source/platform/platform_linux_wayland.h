#ifndef BASE_PLATFORM_LINUX_WAYLAND_H
#define BASE_PLATFORM_LINUX_WAYLAND_H

typedef struct WaylandBuffer WaylandBuffer;
struct WaylandBuffer {
	struct wl_buffer *buffer;
	u32 *pixels;
	b32 busy;
};

typedef struct WaylandState WaylandState;
struct WaylandState {
	struct wl_display    *display;
	struct wl_compositor *compositor;
	struct wl_shm        *shm;
	struct xdg_wm_base   *wm;
	struct wl_seat       *seat;
	
	struct wl_surface   *surface;
	struct xdg_surface  *xsurf;
	struct xdg_toplevel *top;
	
	struct wl_keyboard *keyboard;
	struct wl_pointer  *pointer;
	PlatformInput input;
	
	u32 width;
	u32 height;
	WaylandBuffer buffers[2];
	
	b32 configured;
	b32 should_close;
};

struct PlatformWindow {
	WaylandState wayland;
};

internal PlatformWindow *wayland_window_open(Arena *arena, WindowParams params);
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
