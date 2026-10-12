
//- colors
// TODO: should go in a header file later

typedef struct RGB RGB;
struct RGB {
    u8 r;
    u8 g;
    u8 b;
};

#define BLACK   ((RGB){  0,   0,   0})
#define WHITE   ((RGB){255, 255, 255})
#define RED     ((RGB){255,   0,   0})
#define GREEN   ((RGB){  0, 255,   0})
#define BLUE    ((RGB){  0,   0, 255})
#define YELLOW  ((RGB){255, 255,   0})
#define CYAN    ((RGB){  0, 255, 255})
#define MAGENTA ((RGB){255,   0, 255})

#define GRAY       ((RGB){128, 128, 128})
#define LIGHT_GRAY ((RGB){192, 192, 192})
#define DARK_GRAY  ((RGB){ 64,  64,  64})

#define ORANGE ((RGB){255, 165,   0})
#define PURPLE ((RGB){128,   0, 128})
#define PINK   ((RGB){255, 192, 203})
#define BROWN  ((RGB){139,  69,  19})
#define NAVY   ((RGB){  0,   0, 128})
#define TEAL   ((RGB){  0, 128, 128})
#define OLIVE  ((RGB){128, 128,   0})
#define MAROON ((RGB){128,   0,   0})
#define LIME   ((RGB){ 50, 205,  50})
#define GOLD   ((RGB){255, 215,   0})

//-
typedef struct Object Object;
struct Object
{
Vec2 pos;

    f32 width;
    f32 height;
};

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

internal void draw_square(u32 *pixels, WaylandState *state, Vec2 pos, s32 width, s32 height, RGB color) {

    s32 center_x = (s32)((pos.x + 1.0f) * 0.5f * state->width);
    s32 center_y = (s32)((1.0f - pos.y) * 0.5f * state->height);

    s32 x0 = center_x - width / 2;
    s32 y0 = center_y - height / 2;
    s32 x1 = x0 + width;
    s32 y1 = y0 + height;

    // clip to the framebuffer
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (s32)state->width)  x1 = state->width;
    if (y1 > (s32)state->height) y1 = state->height;

    u32 c = (color.r << 16) | (color.g << 8) | color.b;

    for (s32 y = y0; y < y1; ++y) {
        for (s32 x = x0; x < x1; ++x) {
            pixels[y * state->width + x] = c;
        }
    }

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


internal Vec2 normalize_vec2_position_to_screen(Vec2 pos, s32 width, s32 height)
{
return Vec2(Clamp((pos.x + 1.0f) * 0.5f * width, 0, width - 1), Clamp((pos.y + 1.0f) * 0.5f * height, 0, width - 1));
}


internal void square_game(WaylandState *state, u32 time_ms) {


	
    local_persist Object object_a = { 0, 0, 80, 80 };
    local_persist Object object_b = { 0.2, 0.8, 80, 80 };
    local_persist Object object_c = { 0.3, -0.5, 80, 80};
	
    local_persist u32 previous_time_ms = 0;
    f32 dt = previous_time_ms ? (f32)(time_ms - previous_time_ms) / 1000.0f : 0.0f;
    previous_time_ms = time_ms;
	
    f32 speed = 1.0f;
	
    if(state->keys_down[KEY_W]) {
        Log("Moving forward");
        object_a.pos.y += speed * dt;
    }
    if(state->keys_down[KEY_A]) {
        object_a.pos.x -= speed * dt;
    }
    if(state->keys_down[KEY_D]) {
        object_a.pos.x += speed * dt;
    }
	
    if(state->keys_down[KEY_S]) {
        object_a.pos.y -= speed * dt;
    }
	
	
    MemSetZero(state->pixels, state->size);
    draw_square((u32 *)state->pixels, state, normalize_vec2_position_to_screen(object_a.pos, state->width, state->height), object_a.pos, object_a.width, object_a.height, RED);
    //-
	

}

internal void wayland_frame_done(void *data, struct wl_callback *callback, u32 time_ms) {

    WaylandState *state = (WaylandState *)data;
    wl_callback_destroy(callback);

//- start


for(u32 x_index = 0; x_index < state->width; x_index += state->width / 10)
{
for(u32 y_index = 0; y_index < state->height; y_index += state->height / 10)
{
draw_square((u32 *)state->pixels, state, Vec2((f32)x_index, (f32)y_index), state->width, state->height, WHITE);
}
}


//- end
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


//- xdg_

// TODO: handle resizing. with vulkan that means recreating the swapchain.
internal void wayland_xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
                                             s32 width, s32 height, struct wl_array *states) {

    WaylandState *state = (WaylandState *)data;
    Log("<- xdg_toplevel.configure: width=%d height=%d\n", width, height);
    if(width > 0 && height > 0)
    {
        state->height = height;
        state->width  = width;
    }
}

internal void wayland_xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel) {
    WaylandState *state = (WaylandState *)data;
    Log("<- xdg_toplevel.close\n");
    state->should_close = true;
}

internal void wayland_xdg_toplevel_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height) {
    // NOTE: empty for now...
}

internal void wayland_xdg_toplevel_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities) {
    // NOTE: seperating differnent frames (windows i guess) as toplevel or popup
    // https://wayland-book.com/xdg-shell-basics.html
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

    state.height = 800;
    state.width = 400;

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
            break;
        }

    }

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
}
