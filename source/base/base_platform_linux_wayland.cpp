
//- software framebuffer
// TODO: delete this section once vulkan is presenting. vkCreateWaylandSurfaceKHR only needs
// state->display and state->surface, the driver then creates and attaches buffers itself.

// creates a wl_buffer backed by shared memory that both we and the compositor can read.
internal struct wl_buffer *wayland_create_shm_buffer(WaylandState *state) {
    state->stride = state->width * 4; // 4 bytes per pixel (XRGB8888)
    state->size = state->stride * state->height;

    int fd = memfd_create("loo-shared-memory", MFD_CLOEXEC);
    AssertLog((fd != -1), "Failed to open buffer for shared memory.");
    AssertLog((ftruncate(fd, state->size) != -1), "Failed to truncate buffer.");

    state->pixels = (u8 *)mmap(NULL, state->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    AssertLog((state->pixels != MAP_FAILED), "Failed to mmap shared memory.");

    // a pool is the compositor-side view of our shared memory. libwayland sends the fd
    // over the socket for us (the SCM_RIGHTS dance, in the other direction).
    struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, fd, state->size);

    // a buffer is a typed slice of the pool: offset, size, stride and pixel format
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, state->width, state->height,
                                                         state->stride, WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);
    close(fd);

    return buffer;
}

internal void wayland_draw_gradient(WaylandState *state) {
    u32 *pixels = (u32 *)state->pixels;
    for (u32 y = 0; y < state->height; y++) {
        for (u32 x = 0; x < state->width; x++) {
            u8 r = (u8)(x * 255 / state->width);
            u8 g = (u8)(y * 255 / state->height);
            u8 b = 0x80;
            pixels[y * state->width + x] = (r << 16) | (g << 8) | b;
        }
    }
}

internal void draw_square(u32 *pixels, WaylandState *state, u32 x_position, u32 y_position) {

for(u32 y = y_position; y < y_position + 10; ++y) {

for(u32 x = x_position; x < x_position + 10; ++x) {

u8 r = 0xff;
 u8 g = 0xff;
u8 b = 0xff;
            pixels[y * state->width + x] = (r << 16) | (g << 8) | b;

}
}

}

internal void wayland_draw_weird_gradient(WaylandState *state) {

local_persist u32 frame = 0;
frame += 1;

	local_persist u32 start_x = state->width / 2;
	local_persist u32 start_y = state->width / 2;

start_x += frame;

draw_square((u32 *)state->pixels, state, start_x , start_y);


}


//- xdg_wm_base events

// ping: the compositor checks we're still alive. if we don't pong, it may show the
internal void wayland_xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, u32 serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

global_variable const struct xdg_wm_base_listener wayland_xdg_wm_base_listener = {
    wayland_xdg_wm_base_ping,
};


//- frame


internal void wayland_frame_done(void *data, struct wl_callback *callback, u32 time_ms);

global_variable const struct wl_callback_listener wayland_frame_listener = {
wayland_frame_done,

};


internal void wayland_frame_done(void *data, struct wl_callback *callback, u32 time_ms) {

WaylandState *state = (WaylandState *)data;
wl_callback_destroy(callback);

wayland_draw_weird_gradient(state);
    struct wl_callback *next = wl_surface_frame(state->surface);
    wl_callback_add_listener(next, &wayland_frame_listener, state);
	
    wl_surface_attach(state->surface, state->buffer, 0, 0);
    wl_surface_damage(state->surface, 0, 0, state->width, state->height);
    wl_surface_commit(state->surface);

}



//- xdg_surface events

internal void wayland_xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, u32 serial) {
    WaylandState *state = (WaylandState *)data;

    xdg_surface_ack_configure(xdg_surface, serial);

    if (state->buffer == NULL) {
        state->buffer = wayland_create_shm_buffer(state);
wayland_draw_weird_gradient(state);

        struct wl_callback *cb = wl_surface_frame(state->surface);
        wl_callback_add_listener(cb, &wayland_frame_listener, state);
		wl_surface_attach(state->surface, state->buffer, 0, 0);
		wl_surface_damage(state->surface, 0, 0, state->width, state->height);
		wl_surface_commit(state->surface);
    }

    // attach = "this buffer is the surface's content", damage = "this region changed",
    // commit = "apply everything I sent since the last commit, atomically".
    // a wl_surface has pending state and current state, nothing is visible until commit.
    wl_surface_attach(state->surface, state->buffer, 0, 0);
    wl_surface_damage(state->surface, 0, 0, state->width, state->height);
    wl_surface_commit(state->surface);
}

global_variable const struct xdg_surface_listener wayland_xdg_surface_listener = {
    wayland_xdg_surface_configure,
};


//- xdg_toplevel events

// TODO: handle resizing. with vulkan that means recreating the swapchain.
internal void wayland_xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
                                             s32 width, s32 height, struct wl_array *states) {

WaylandState *state = (WaylandState *)data;
    Log("<- xdg_toplevel.configure: width=%d height=%d\n", width, height);
state->height = height;
state->width  = width;

}

internal void wayland_xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel) {
    WaylandState *state = (WaylandState *)data;
    Log("<- xdg_toplevel.close\n");
    state->should_close = true;
}

internal void wayland_xdg_toplevel_resize_edge(void *data, struct xdg_toplevel *xdg_toplevel,
 s32 width, s32 height, struct wl_array *states) {


}
internal void wayland_xdg_toplevel_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel,
                                                    s32 width, s32 height) {
}

internal void wayland_xdg_toplevel_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel,
                                                   struct wl_array *capabilities) {
}

global_variable const struct xdg_toplevel_listener wayland_xdg_toplevel_listener = {
    wayland_xdg_toplevel_configure,
    wayland_xdg_toplevel_close,
    wayland_xdg_toplevel_configure_bounds,
    wayland_xdg_toplevel_wm_capabilities,
};


//- wl_registry events

internal void wayland_registry_global(void *data, struct wl_registry *registry, u32 name,
                                      const char *interface, u32 version) {
    WaylandState *state = (WaylandState *)data;

    Log("<- wl_registry.global: name=%u interface=%s version=%u\n", name, interface, version);

    if (CstringIsMatch(interface, wl_compositor_interface.name)) {
        state->compositor = (struct wl_compositor *)
            wl_registry_bind(registry, name, &wl_compositor_interface, Min(version, 4));
    } else if (CstringIsMatch(interface, wl_shm_interface.name)) {
        state->shm = (struct wl_shm *)
            wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if(CstringIsMatch(interface, xdg_wm_base_interface.name)) {
        state->xdg_wm_base = (struct xdg_wm_base *)
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(state->xdg_wm_base, &wayland_xdg_wm_base_listener, state);
    } else if (CstringIsMatch(interface, wl_seat_interface.name)) {
        state->seat = (struct wl_seat *)
            wl_registry_bind(registry, name, &wl_seat_interface, Min(version, 5));
        wl_seat_add_listener(state->seat, &wayland_seat_listener, state); // has the keyboard handler
    }
}

internal void wayland_registry_global_remove(void *data, struct wl_registry *registry, u32 name) {
}

global_variable const struct wl_registry_listener wayland_registry_listener = {
    wayland_registry_global,
    wayland_registry_global_remove,
};

//- entrypoint

internal void wayland_entrypoint(MemArena *arena, LinuxWindowParams *params) {

    WaylandState state = {};

    state.display = wl_display_connect(NULL);
    AssertLog((state.display != NULL), "failed to connect to wayland display\n");

    // ask for the registry and listen to its global events
    state.registry = wl_display_get_registry(state.display);
    wl_registry_add_listener(state.registry, &wayland_registry_listener, &state);

    wl_display_roundtrip(state.display);

    AssertLog((state.compositor != NULL), "compositor has no wl_compositor\n");
    AssertLog((state.shm != NULL), "compositor has no wl_shm\n");
    AssertLog((state.xdg_wm_base != NULL), "compositor has no xdg_wm_base\n");
    if (state.seat == NULL) {
        Log("no wl_seat: running without keyboard input\n");
    }

    // build the window: surface -> give it the xdg role -> make it a toplevel
    state.surface = wl_compositor_create_surface(state.compositor);

    state.xdg_surface = xdg_wm_base_get_xdg_surface(state.xdg_wm_base, state.surface);
    xdg_surface_add_listener(state.xdg_surface, &wayland_xdg_surface_listener, &state);

    state.xdg_toplevel = xdg_surface_get_toplevel(state.xdg_surface);
    xdg_toplevel_add_listener(state.xdg_toplevel, &wayland_xdg_toplevel_listener, &state);
    xdg_toplevel_set_title(state.xdg_toplevel, params->name);
    xdg_toplevel_set_app_id(state.xdg_toplevel, params->name);

    wl_surface_commit(state.surface);

    //
    // TODO: blocking is fine while the picture is static. for a game loop that renders
    // every frame we'll instead use wl_display_dispatch_pending + a non blocking read
    // (wl_display_prepare_read / wl_display_read_events with poll on wl_display_get_fd).
    while (!state.should_close) {

        if (wl_display_dispatch(state.display) == -1) {
            Log("wayland connection lost\n");
goto regular_exit;
        }

if(state.keys_down[KEY_W]) { Log("Move Forward"); }
if(state.keys_down[KEY_A]) { Log("Move Left"); }
 if(state.keys_down[KEY_D]) { Log("Move Right"); }
 if(state.keys_down[KEY_S]) { Log("Move Back"); }
if(state.keys_down[KEY_Q]) {
goto cleanup_exit;
}

}


	cleanup_exit:
	
    if (state.keyboard) wl_keyboard_destroy(state.keyboard);
    if (state.buffer) wl_buffer_destroy(state.buffer);
    if (state.pixels) munmap(state.pixels, state.size);
    xdg_toplevel_destroy(state.xdg_toplevel);
    xdg_surface_destroy(state.xdg_surface);
    wl_surface_destroy(state.surface);
    if (state.seat) wl_seat_destroy(state.seat);
    xdg_wm_base_destroy(state.xdg_wm_base);
    wl_shm_destroy(state.shm);
    wl_compositor_destroy(state.compositor);
    wl_registry_destroy(state.registry);
return;

regular_exit:
return;
}
