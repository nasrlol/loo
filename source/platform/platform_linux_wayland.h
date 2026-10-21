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
    struct wl_display  *display;
    struct wl_registry *registry;

    struct wl_compositor *compositor;
    struct wl_shm        *shm;
    struct xdg_wm_base   *xdg_wm_base;
    struct wl_seat       *seat;
    struct wl_output     *output; // NOTE: only the first output, used for the refresh rate

    //- window objects
    struct wl_surface   *surface;
    struct xdg_surface  *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    //- input
    struct wl_keyboard *keyboard;
    struct wl_pointer  *pointer;
    PlatformInput input;            // mouse_x/y here are in window coordinates, pump converts them
    f32 mouse_window_x, mouse_window_y;

    //- window size
    // NOTE: width/height is the current window size. pending_* is what xdg_toplevel.configure
    // asked for, it becomes current on xdg_surface.configure.
    u32 width;
    u32 height;
    u32 pending_width;
    u32 pending_height;

    //- presenting
    WaylandBuffer buffers[2];
    u32 framebuffer_width;  // size of the last framebuffer that was presented
    u32 framebuffer_height;
    s32 view_x, view_y;     // where it ended up inside the window (letterboxed)
    s32 view_width, view_height;

    u32 refresh_mhz; // from wl_output.mode, 0 until known

    b32 configured;
    b32 frame_ready;
    b32 should_close;
};

// NOTE: the platform agnostic handle, on wayland it's just the wayland state
struct PlatformWindow {
    WaylandState wayland;
};

//- public wayland  api

internal PlatformWindow *wayland_window_open(MemArena *arena, WindowParams params);
// blocks until the compositor wants a new frame. fills input. false once the window should close.
internal b32  wayland_window_pump(PlatformWindow *window, PlatformInput *input);
// scales pixels (0x00RRGGBB, tightly packed) into the window and commits it
internal void wayland_window_present(PlatformWindow *window, u32 *pixels, u32 width, u32 height);
internal f32  wayland_window_refresh_hz(PlatformWindow *window);
internal void wayland_window_close(PlatformWindow *window);

//- shm buffers
internal void           wayland_buffer_create(WaylandState *state, WaylandBuffer *buffer, u32 width, u32 height);
internal void           wayland_buffer_destroy(WaylandBuffer *buffer);
internal WaylandBuffer *wayland_buffer_acquire(WaylandState *state);

//- event callbacks
// NOTE: the listener tables in the .cpp point at these, every slot must be filled (NULL slot = libwayland abort)

// wl_registry
internal void wayland_registry_global(void *data, struct wl_registry *registry, u32 name, const char *interface, u32 version);
internal void wayland_registry_global_remove(void *data, struct wl_registry *registry, u32 name);

// wl_output
internal void wayland_output_geometry(void *data, struct wl_output *output, s32 x, s32 y, s32 physical_width, s32 physical_height, s32 subpixel, const char *make, const char *model, s32 transform);
internal void wayland_output_mode(void *data, struct wl_output *output, u32 flags, s32 width, s32 height, s32 refresh);
internal void wayland_output_done(void *data, struct wl_output *output);
internal void wayland_output_scale(void *data, struct wl_output *output, s32 factor);

// wl_seat
internal void wayland_seat_capabilities(void *data, struct wl_seat *seat, u32 capabilities);
internal void wayland_seat_name(void *data, struct wl_seat *seat, const char *name);

// wl_keyboard
internal void wayland_keyboard_keymap(void *data, struct wl_keyboard *keyboard, u32 format, s32 fd, u32 size);
internal void wayland_keyboard_enter(void *data, struct wl_keyboard *keyboard, u32 serial, struct wl_surface *surface, struct wl_array *keys);
internal void wayland_keyboard_leave(void *data, struct wl_keyboard *keyboard, u32 serial, struct wl_surface *surface);
internal void wayland_keyboard_key(void *data, struct wl_keyboard *keyboard, u32 serial, u32 time, u32 key, u32 key_state);
internal void wayland_keyboard_modifiers(void *data, struct wl_keyboard *keyboard, u32 serial, u32 mods_depressed, u32 mods_latched, u32 mods_locked, u32 group);
internal void wayland_keyboard_repeat_info(void *data, struct wl_keyboard *keyboard, s32 rate, s32 delay);

// wl_pointer
internal void wayland_pointer_enter(void *data, struct wl_pointer *pointer, u32 serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
internal void wayland_pointer_leave(void *data, struct wl_pointer *pointer, u32 serial, struct wl_surface *surface);
internal void wayland_pointer_motion(void *data, struct wl_pointer *pointer, u32 time, wl_fixed_t surface_x, wl_fixed_t surface_y);
internal void wayland_pointer_button(void *data, struct wl_pointer *pointer, u32 serial, u32 time, u32 button, u32 button_state);
internal void wayland_pointer_axis(void *data, struct wl_pointer *pointer, u32 time, u32 axis, wl_fixed_t value);
internal void wayland_pointer_frame(void *data, struct wl_pointer *pointer);
internal void wayland_pointer_axis_source(void *data, struct wl_pointer *pointer, u32 axis_source);
internal void wayland_pointer_axis_stop(void *data, struct wl_pointer *pointer, u32 time, u32 axis);
internal void wayland_pointer_axis_discrete(void *data, struct wl_pointer *pointer, u32 axis, s32 discrete);

// xdg_wm_base
internal void wayland_xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, u32 serial);

// xdg_surface
internal void wayland_xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, u32 serial);

// xdg_toplevel
internal void wayland_xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height, struct wl_array *states);
internal void wayland_xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel);
internal void wayland_xdg_toplevel_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height);
internal void wayland_xdg_toplevel_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities);

// wl_buffer
internal void wayland_buffer_release(void *data, struct wl_buffer *buffer);

// wl_callback (frame)
internal void wayland_frame_done(void *data, struct wl_callback *callback, u32 time_ms);

#endif // BASE_PLATFORM_LINUX_WAYLAND_H
