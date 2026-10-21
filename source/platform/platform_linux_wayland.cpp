//- listeners
// NOTE: a listener is a struct of function pointers, one per event, in the order the events are
// declared in the protocol xml (which is also their opcode). libwayland indexes into it with the
// opcode it reads from the message header.

global_variable const struct wl_registry_listener wayland_registry_listener = {
    wayland_registry_global,
    wayland_registry_global_remove,
};

// NOTE: bound at v2, name/description (v4) are never sent
global_variable const struct wl_output_listener wayland_output_listener = {
wayland_output_geometry,
wayland_output_mode,
wayland_output_done,  // since v2
wayland_output_scale, // since v2
};

global_variable const struct wl_seat_listener wayland_seat_listener = {
wayland_seat_capabilities, // opcode 0
wayland_seat_name,         // opcode 1 (since version 2)
};

global_variable const struct wl_keyboard_listener wayland_keyboard_listener = {
wayland_keyboard_keymap,      // opcode 0
wayland_keyboard_enter,       // opcode 1
wayland_keyboard_leave,       // opcode 2
wayland_keyboard_key,         // opcode 3
wayland_keyboard_modifiers,   // opcode 4
wayland_keyboard_repeat_info, // opcode 5
};

// NOTE: seat is bound at v5 at most, the v8+ events (axis_value120, ...) are never sent
global_variable const struct wl_pointer_listener wayland_pointer_listener = {
wayland_pointer_enter,
wayland_pointer_leave,
wayland_pointer_motion,
wayland_pointer_button,
wayland_pointer_axis,
wayland_pointer_frame,
wayland_pointer_axis_source,
wayland_pointer_axis_stop,
wayland_pointer_axis_discrete,
};

global_variable const struct xdg_wm_base_listener wayland_xdg_wm_base_listener = {
wayland_xdg_wm_base_ping,
};

global_variable const struct xdg_surface_listener wayland_xdg_surface_listener = {
wayland_xdg_surface_configure,
};

global_variable const struct xdg_toplevel_listener wayland_xdg_toplevel_listener = {
wayland_xdg_toplevel_configure,
wayland_xdg_toplevel_close,
wayland_xdg_toplevel_configure_bounds,
wayland_xdg_toplevel_wm_capabilities,
};

global_variable const struct wl_buffer_listener wayland_buffer_listener = {
wayland_buffer_release,
};

global_variable const struct wl_callback_listener wayland_frame_listener = {
wayland_frame_done,
};

//- input helpers

// NOTE: only counts a transition when the state actually flips
internal void wayland_button_update(PlatformButton *button, b32 down) {
if (button->ended_down != down) {
button->ended_down = down;
++button->half_transition_count;
}
}

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
    } else if (CstringIsMatch(interface, xdg_wm_base_interface.name)) {
    state->xdg_wm_base = (struct xdg_wm_base *)
    wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(state->xdg_wm_base, &wayland_xdg_wm_base_listener, state);
    } else if (CstringIsMatch(interface, wl_seat_interface.name)) {
    state->seat = (struct wl_seat *)
    wl_registry_bind(registry, name, &wl_seat_interface, Min(version, 5));
    wl_seat_add_listener(state->seat, &wayland_seat_listener, state);
    } else if (CstringIsMatch(interface, wl_output_interface.name) && state->output == NULL) {
    state->output = (struct wl_output *)
    wl_registry_bind(registry, name, &wl_output_interface, Min(version, 2));
    wl_output_add_listener(state->output, &wayland_output_listener, state);
    }
    }

    // NOTE: required listener slot. fill this in once we track globals that can come and go
    // (outputs being unplugged, seats disappearing).
    internal void wayland_registry_global_remove(void *data, struct wl_registry *registry, u32 name) {}

    //- wl_output events

    internal void wayland_output_geometry(void *data, struct wl_output *output, s32 x, s32 y,
    s32 physical_width, s32 physical_height, s32 subpixel,
    const char *make, const char *model, s32 transform) {}

    // NOTE: refresh is in mHz, 60Hz = 60000
    internal void wayland_output_mode(void *data, struct wl_output *output, u32 flags,
    s32 width, s32 height, s32 refresh) {
    WaylandState *state = (WaylandState *)data;
    if (flags & WL_OUTPUT_MODE_CURRENT) {
    Log("<- wl_output.mode: %dx%d @ %d mHz\n", width, height, refresh);
    state->refresh_mhz = (u32)refresh;
    }
    }

    internal void wayland_output_done(void *data, struct wl_output *output) {}
    internal void wayland_output_scale(void *data, struct wl_output *output, s32 factor) {}

    //- wl_seat events

    internal void wayland_seat_capabilities(void *data, struct wl_seat *seat, u32 capabilities) {
    WaylandState *state = (WaylandState *)data;
    b32 has_keyboard = (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) != 0;
    b32 has_pointer  = (capabilities & WL_SEAT_CAPABILITY_POINTER) != 0;

    Log("<- wl_seat.capabilities: %u\n", capabilities);

    if (has_keyboard && state->keyboard == NULL) {
    state->keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_add_listener(state->keyboard, &wayland_keyboard_listener, state);
    } else if (!has_keyboard && state->keyboard != NULL) {
    if (wl_keyboard_get_version(state->keyboard) >= WL_KEYBOARD_RELEASE_SINCE_VERSION) {
    wl_keyboard_release(state->keyboard);
    } else {
    wl_keyboard_destroy(state->keyboard);
    }
    state->keyboard = NULL;
    }

    if (has_pointer && state->pointer == NULL) {
    state->pointer = wl_seat_get_pointer(seat);
    wl_pointer_add_listener(state->pointer, &wayland_pointer_listener, state);
    } else if (!has_pointer && state->pointer != NULL) {
    if (wl_pointer_get_version(state->pointer) >= WL_POINTER_RELEASE_SINCE_VERSION) {
    wl_pointer_release(state->pointer);
    } else {
    wl_pointer_destroy(state->pointer);
    }
    state->pointer = NULL;
    }
    }

    // name: a human readable seat name like "seat0". only useful for multi-seat setups.
    internal void wayland_seat_name(void *data, struct wl_seat *seat, const char *name) {
    Log("<- wl_seat.name: %s\n", name);
    }

    //- wl_keyboard events
    // TODO: load the keymap with xkbcommon once we need text input
    internal void wayland_keyboard_keymap(void *data, struct wl_keyboard *keyboard, u32 format, s32 fd, u32 size) {
    close(fd);
    }

    // enter: we got focus. keys contains everything already held down.
    // NOTE: sets the down state without a transition, so holding a key while focusing isn't a "press"
    internal void wayland_keyboard_enter(void *data, struct wl_keyboard *keyboard, u32 serial,
    struct wl_surface *surface, struct wl_array *keys) {
    WaylandState *state = (WaylandState *)data;
    u32 *key = (u32 *)keys->data;
    u32 key_count = keys->size / sizeof(u32);
    for (u32 i = 0; i < key_count; ++i) {
    if (key[i] < ArrayCount(state->input.keys)) {
    state->input.keys[key[i]].ended_down = true;
    }
    }
    }

    // leave: we lost focus. after this we will NOT receive release events for keys that
    // are still held, so we release everything ourselves. otherwise keys get "stuck".
    internal void wayland_keyboard_leave(void *data, struct wl_keyboard *keyboard, u32 serial,
    struct wl_surface *surface) {
    WaylandState *state = (WaylandState *)data;
    for (u32 i = 0; i < ArrayCount(state->input.keys); ++i) {
    wayland_button_update(&state->input.keys[i], false);
    }
    }

    // NOTE: wayland never sends repeats, a held key is one press event until the release
    internal void wayland_keyboard_key(void *data, struct wl_keyboard *keyboard, u32 serial,
    u32 time, u32 key, u32 key_state) {
    WaylandState *state = (WaylandState *)data;
    if (key < ArrayCount(state->input.keys)) {
    wayland_button_update(&state->input.keys[key], key_state == WL_KEYBOARD_KEY_STATE_PRESSED);
    }
    }

    internal void wayland_keyboard_modifiers(void *data, struct wl_keyboard *keyboard, u32 serial,
    u32 mods_depressed, u32 mods_latched, u32 mods_locked, u32 group) {}

    // NOTE: only matters for text input, games don't want key repeat
    internal void wayland_keyboard_repeat_info(void *data, struct wl_keyboard *keyboard, s32 rate, s32 delay) {}

    //- wl_pointer events
    // NOTE: positions are in window coordinates here, pump converts them to framebuffer pixels

    internal void wayland_pointer_enter(void *data, struct wl_pointer *pointer, u32 serial,
    struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WaylandState *state = (WaylandState *)data;
    state->input.mouse_in_window = true;
    state->mouse_window_x = (f32)wl_fixed_to_double(surface_x);
    state->mouse_window_y = (f32)wl_fixed_to_double(surface_y);
    }

    internal void wayland_pointer_leave(void *data, struct wl_pointer *pointer, u32 serial,
    struct wl_surface *surface) {
    WaylandState *state = (WaylandState *)data;
    state->input.mouse_in_window = false;
    }

    internal void wayland_pointer_motion(void *data, struct wl_pointer *pointer, u32 time,
    wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WaylandState *state = (WaylandState *)data;
    state->mouse_window_x = (f32)wl_fixed_to_double(surface_x);
    state->mouse_window_y = (f32)wl_fixed_to_double(surface_y);
    }

    internal void wayland_pointer_button(void *data, struct wl_pointer *pointer,
    u32 serial, u32 time, u32 button, u32 button_state) {
    WaylandState *state = (WaylandState *)data;
    // BTN_LEFT, BTN_RIGHT, BTN_MIDDLE are consecutive, so this maps them to 0, 1, 2
    if (button >= BTN_LEFT && button <= BTN_MIDDLE) {
    wayland_button_update(&state->input.mouse[button - BTN_LEFT],
    button_state == WL_POINTER_BUTTON_STATE_PRESSED);
    }
    }

    // TODO: scroll wheel
    internal void wayland_pointer_axis(void *data, struct wl_pointer *pointer, u32 time, u32 axis, wl_fixed_t value) {}
    internal void wayland_pointer_frame(void *data, struct wl_pointer *pointer) {}
    internal void wayland_pointer_axis_source(void *data, struct wl_pointer *pointer, u32 axis_source) {}
    internal void wayland_pointer_axis_stop(void *data, struct wl_pointer *pointer, u32 time, u32 axis) {}
    internal void wayland_pointer_axis_discrete(void *data, struct wl_pointer *pointer, u32 axis, s32 discrete) {}

    //- xdg_wm_base events

    // ping: the compositor checks we're still alive. if we don't pong, it may show the
    // "application is not responding" dialog.
    internal void wayland_xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, u32 serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
    }

    //- xdg_toplevel events

    // NOTE: only proposes a size, it's applied when the xdg_surface.configure that follows arrives.
    // 0 means "client picks", so we keep what we have.
    internal void wayland_xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
    s32 width, s32 height, struct wl_array *states) {
    WaylandState *state = (WaylandState *)data;
    if (width  > 0) state->pending_width  = (u32)width;
    if (height > 0) state->pending_height = (u32)height;
    }

    internal void wayland_xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel) {
    WaylandState *state = (WaylandState *)data;
    Log("<- xdg_toplevel.close\n");
    state->should_close = true;
    }

    // NOTE: required listener slots, only sent when xdg_wm_base is bound at v4+ / v5+
    internal void wayland_xdg_toplevel_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height) {}
    internal void wayland_xdg_toplevel_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities) {}

    //- xdg_surface events

    // NOTE: the end of a configure sequence, ack it and make the pending size current.
    // the shm buffers get resized lazily the next time one is acquired.
    internal void wayland_xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, u32 serial) {
    WaylandState *state = (WaylandState *)data;
    xdg_surface_ack_configure(xdg_surface, serial);
    state->width      = state->pending_width;
    state->height     = state->pending_height;
    state->configured = true;
    }

    //- frame

    // NOTE: the compositor wants a new frame, pump returns once this fired
    internal void wayland_frame_done(void *data, struct wl_callback *callback, u32 time_ms) {
    WaylandState *state = (WaylandState *)data;
    wl_callback_destroy(callback);
    state->frame_ready = true;
    }

    //- shm buffers
    // TODO: delete this section once vulkan is presenting. vkCreateWaylandSurfaceKHR only needs
    // state->display and state->surface, the driver then creates and attaches buffers itself.

    internal void wayland_buffer_release(void *data, struct wl_buffer *wl_buffer) {
    WaylandBuffer *buffer = (WaylandBuffer *)data;
    buffer->busy = false;
    }

    internal void wayland_buffer_create(WaylandState *state, WaylandBuffer *buffer, u32 width, u32 height) {
    u32 stride   = width * 4; // 4 bytes per pixel (XRGB8888)
    buffer->size = stride * height;

    int fd = memfd_create("loo-shared-memory", MFD_CLOEXEC);
    AssertLog((fd != -1), "Failed to open buffer for shared memory.");
    AssertLog((ftruncate(fd, buffer->size) != -1), "Failed to truncate buffer.");

    buffer->pixels = (u32 *)mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    AssertLog((buffer->pixels != MAP_FAILED), "Failed to mmap shared memory.");

    // a pool is the compositor-side view of our shared memory. libwayland sends the fd
    // over the socket for us (the SCM_RIGHTS dance, in the other direction).
    struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, fd, buffer->size);

    // a buffer is a typed slice of the pool: offset, size, stride and pixel format
    buffer->buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_buffer_add_listener(buffer->buffer, &wayland_buffer_listener, buffer);

    wl_shm_pool_destroy(pool);
    close(fd);

    buffer->width  = width;
    buffer->height = height;
    buffer->busy   = false;
    }

    internal void wayland_buffer_destroy(WaylandBuffer *buffer) {
    if (buffer->buffer) wl_buffer_destroy(buffer->buffer);
    if (buffer->pixels) munmap(buffer->pixels, buffer->size);
    MemSetZero(buffer, sizeof(*buffer));
    }

    // NOTE: returns a buffer the compositor isn't reading, sized to the window
    internal WaylandBuffer *wayland_buffer_acquire(WaylandState *state) {
    for (;;) {
    for (u32 i = 0; i < ArrayCount(state->buffers); ++i) {
    WaylandBuffer *buffer = &state->buffers[i];
    if (buffer->busy) continue;

    if (buffer->width != state->width || buffer->height != state->height) {
    wayland_buffer_destroy(buffer);
    wayland_buffer_create(state, buffer, state->width, state->height);
    }
    return buffer;
    }

    // both are still held by the compositor, wait for a release
    if (wl_display_dispatch(state->display) == -1) {
    return NULL;
    }
    }
    }

    //- public api

    internal PlatformWindow *wayland_window_open(MemArena *arena, WindowParams params) {
    PlatformWindow *window = PushStructZero(arena, PlatformWindow);
    WaylandState *state = &window->wayland;

    state->pending_width  = params.window_width  > 0 ? (u32)params.window_width  : 960;
    state->pending_height = params.window_height > 0 ? (u32)params.window_height : 540;

    state->display = wl_display_connect(NULL);
    AssertLog((state->display != NULL), "failed to connect to wayland display\n");

    // ask for the registry and listen to its global events
    state->registry = wl_display_get_registry(state->display);
    wl_registry_add_listener(state->registry, &wayland_registry_listener, state);
    wl_display_roundtrip(state->display);

    AssertLog((state->compositor != NULL), "compositor has no wl_compositor\n");
    AssertLog((state->shm != NULL), "compositor has no wl_shm\n");
    AssertLog((state->xdg_wm_base != NULL), "compositor has no xdg_wm_base\n");
    if (state->seat == NULL) {
    Log("no wl_seat: running without keyboard input\n");
    }

    // NOTE: second roundtrip so the events the binds triggered (seat capabilities, output mode) arrive
    wl_display_roundtrip(state->display);

    // build the window: surface -> give it the xdg role -> make it a toplevel
    state->surface = wl_compositor_create_surface(state->compositor);

    state->xdg_surface = xdg_wm_base_get_xdg_surface(state->xdg_wm_base, state->surface);
    xdg_surface_add_listener(state->xdg_surface, &wayland_xdg_surface_listener, state);

    state->xdg_toplevel = xdg_surface_get_toplevel(state->xdg_surface);
    xdg_toplevel_add_listener(state->xdg_toplevel, &wayland_xdg_toplevel_listener, state);
    xdg_toplevel_set_title(state->xdg_toplevel, params.name);
    xdg_toplevel_set_app_id(state->xdg_toplevel, params.name);

    // NOTE: commit without a buffer, then wait for the first configure. attaching a buffer
    // before that is a protocol error.
    wl_surface_commit(state->surface);
    while (!state->configured) {
    AssertLog((wl_display_dispatch(state->display) != -1), "wayland connection lost before the first configure\n");
    }

    // NOTE: draw the first frame right away, after that frame callbacks pace us
    state->frame_ready = true;
    return window;
    }

    internal b32 wayland_window_pump(PlatformWindow *window, PlatformInput *input) {
    WaylandState *state = &window->wayland;

    // NOTE: blocks in dispatch until the frame callback fires, that's our vsync.
    // a hidden window gets no frame callbacks, so we just sleep here until it's visible again.
    while (!state->frame_ready && !state->should_close) {
    if (wl_display_dispatch(state->display) == -1) {
    Log("wayland connection lost\n");
    return false;
    }
    }
    state->frame_ready = false;

    *input = state->input;

    // window -> framebuffer pixels, undoing the letterbox from the last present
    if (state->view_width > 0 && state->view_height > 0) {
    input->mouse_x = (s32)((state->mouse_window_x - state->view_x) * state->framebuffer_width  / state->view_width);
    input->mouse_y = (s32)((state->mouse_window_y - state->view_y) * state->framebuffer_height / state->view_height);
    } else {
    input->mouse_x = (s32)state->mouse_window_x;
    input->mouse_y = (s32)state->mouse_window_y;
    }

    // NOTE: transitions are per frame, the down state carries over
    for (u32 i = 0; i < ArrayCount(state->input.keys); ++i)  state->input.keys[i].half_transition_count = 0;
    for (u32 i = 0; i < ArrayCount(state->input.mouse); ++i) state->input.mouse[i].half_transition_count = 0;

    return !state->should_close;
    }

    internal void wayland_window_present(PlatformWindow *window, u32 *pixels, u32 width, u32 height) {
    WaylandState *state = &window->wayland;

    WaylandBuffer *buffer = wayland_buffer_acquire(state);
    if (!buffer) return;

    // letterbox: biggest rect with the framebuffer's aspect ratio that fits the window
    if ((u64)state->width * height <= (u64)state->height * width) {
    state->view_width  = (s32)state->width;
    state->view_height = (s32)((u64)state->width * height / width);
    } else {
    state->view_height = (s32)state->height;
    state->view_width  = (s32)((u64)state->height * width / height);
    }
    state->view_x = ((s32)state->width  - state->view_width)  / 2;
    state->view_y = ((s32)state->height - state->view_height) / 2;
    state->framebuffer_width  = width;
    state->framebuffer_height = height;

    // nearest neighbour scale, 16.16 fixed point steps through the source
    u32 step_x = (width  << 16) / (u32)state->view_width;
    u32 step_y = (height << 16) / (u32)state->view_height;

    for (s32 y = 0; y < (s32)buffer->height; ++y) {
    u32 *row = buffer->pixels + (u64)y * buffer->width;

    if (y < state->view_y || y >= state->view_y + state->view_height) {
    MemSetZero(row, buffer->width * sizeof(u32));
    continue;
    }

    u32 *src_row = pixels + (u64)(((u32)(y - state->view_y) * step_y) >> 16) * width;
    for (s32 x = 0; x < (s32)buffer->width; ++x) {
    if (x < state->view_x || x >= state->view_x + state->view_width) {
    row[x] = 0;
    } else {
    row[x] = src_row[((u32)(x - state->view_x) * step_x) >> 16];
    }
    }
    }

    // NOTE: ask for the next frame callback before committing, it belongs to this commit
    struct wl_callback *callback = wl_surface_frame(state->surface);
    wl_callback_add_listener(callback, &wayland_frame_listener, state);

    // attach = "this buffer is the surface's content", damage = "this region changed",
    // commit = "apply everything I sent since the last commit, atomically".
    // a wl_surface has pending state and current state, nothing is visible until commit.
    wl_surface_attach(state->surface, buffer->buffer, 0, 0);
    wl_surface_damage(state->surface, 0, 0, buffer->width, buffer->height);
    wl_surface_commit(state->surface);
    buffer->busy = true;

    wl_display_flush(state->display);
    }

    internal f32 wayland_window_refresh_hz(PlatformWindow *window) {
    WaylandState *state = &window->wayland;
    return state->refresh_mhz ? (f32)state->refresh_mhz / 1000.0f : 60.0f;
    }

    internal void wayland_window_close(PlatformWindow *window) {
    WaylandState *state = &window->wayland;

    for (u32 i = 0; i < ArrayCount(state->buffers); ++i) wayland_buffer_destroy(&state->buffers[i]);

    if (state->keyboard) wl_keyboard_destroy(state->keyboard);
    if (state->pointer)  wl_pointer_destroy(state->pointer);
    xdg_toplevel_destroy(state->xdg_toplevel);
    xdg_surface_destroy(state->xdg_surface);
    wl_surface_destroy(state->surface);
    if (state->seat)   wl_seat_destroy(state->seat);
    if (state->output) wl_output_destroy(state->output);
    xdg_wm_base_destroy(state->xdg_wm_base);
    wl_shm_destroy(state->shm);
    wl_compositor_destroy(state->compositor);
    wl_registry_destroy(state->registry);
    wl_display_disconnect(state->display);
}
