//- input helpers

// NOTE: only counts a transition when the state actually flips
internal void wayland_button_update(PlatformButton *button, b32 down) {
 if (button->ended_down != down) {
  button->ended_down = down;
  ++button->half_transition_count;
 }
}

//- wl_keyboard events
// TODO: load the keymap with xkbcommon once we need text input
internal void wayland_keyboard_keymap(void *data, struct wl_keyboard *keyboard, u32 format, s32 fd, u32 size) {
 close(fd);
}

internal void wayland_keyboard_enter(void *data, struct wl_keyboard *keyboard, u32 serial,
                                     struct wl_surface *surface, struct wl_array *keys) {}

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

//- wl_pointer events

internal void wayland_pointer_enter(void *data, struct wl_pointer *pointer, u32 serial,
                                    struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
 WaylandState *state = (WaylandState *)data;
 state->input.mouse_in_window = true;
 state->input.mouse_x = wl_fixed_to_int(surface_x);
 state->input.mouse_y = wl_fixed_to_int(surface_y);
}

internal void wayland_pointer_leave(void *data, struct wl_pointer *pointer, u32 serial,
                                    struct wl_surface *surface) {
 WaylandState *state = (WaylandState *)data;
 state->input.mouse_in_window = false;
}

internal void wayland_pointer_motion(void *data, struct wl_pointer *pointer, u32 time,
                                     wl_fixed_t surface_x, wl_fixed_t surface_y) {
 WaylandState *state = (WaylandState *)data;
 state->input.mouse_x = wl_fixed_to_int(surface_x);
 state->input.mouse_y = wl_fixed_to_int(surface_y);
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

//- wl_seat events

global_variable const struct wl_keyboard_listener wayland_keyboard_listener = {
 wayland_keyboard_keymap,
 wayland_keyboard_enter,
 wayland_keyboard_leave,
 wayland_keyboard_key,
 wayland_keyboard_modifiers,
};

global_variable const struct wl_pointer_listener wayland_pointer_listener = {
 wayland_pointer_enter,
 wayland_pointer_leave,
 wayland_pointer_motion,
 wayland_pointer_button,
 wayland_pointer_axis,
};

internal void wayland_seat_capabilities(void *data, struct wl_seat *seat, u32 capabilities) {
 WaylandState *state = (WaylandState *)data;

 Log("<- wl_seat.capabilities: %u\n", capabilities);

 if ((capabilities & WL_SEAT_CAPABILITY_KEYBOARD) && state->keyboard == NULL) {
  state->keyboard = wl_seat_get_keyboard(seat);
  wl_keyboard_add_listener(state->keyboard, &wayland_keyboard_listener, state);
 }

 if ((capabilities & WL_SEAT_CAPABILITY_POINTER) && state->pointer == NULL) {
  state->pointer = wl_seat_get_pointer(seat);
  wl_pointer_add_listener(state->pointer, &wayland_pointer_listener, state);
 }
}

//- xdg events

internal void wayland_xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, u32 serial) {
 xdg_wm_base_pong(xdg_wm_base, serial);
}

internal void wayland_xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
                                             s32 width, s32 height, struct wl_array *states) {
 WaylandState *state = (WaylandState *)data;
 if (width  > 0) state->width  = (u32)width;
 if (height > 0) state->height = (u32)height;
}

internal void wayland_xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel) {
 WaylandState *state = (WaylandState *)data;
 Log("<- xdg_toplevel.close\n");
 state->should_close = true;
}

internal void wayland_xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, u32 serial) {
 WaylandState *state = (WaylandState *)data;
 xdg_surface_ack_configure(xdg_surface, serial);
 state->configured = true;
}

//- wl_registry events

global_variable const struct wl_seat_listener wayland_seat_listener = {
 wayland_seat_capabilities,
};

global_variable const struct xdg_wm_base_listener wayland_xdg_wm_base_listener = {
 wayland_xdg_wm_base_ping,
};

internal void wayland_registry_global(void *data, struct wl_registry *registry, u32 name,
                                      const char *interface, u32 version) {
 WaylandState *state = (WaylandState *)data;

 Log("<- wl_registry.global: name=%u interface=%s version=%u\n", name, interface, version);

 if (CstringIsMatch(interface, wl_compositor_interface.name)) {
  state->compositor = (struct wl_compositor *)wl_registry_bind(registry, name, &wl_compositor_interface, 1);
 } else if (CstringIsMatch(interface, wl_shm_interface.name)) {
  state->shm = (struct wl_shm *)wl_registry_bind(registry, name, &wl_shm_interface, 1);
 } else if (CstringIsMatch(interface, xdg_wm_base_interface.name)) {
  state->xdg_wm_base = (struct xdg_wm_base *)wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
  xdg_wm_base_add_listener(state->xdg_wm_base, &wayland_xdg_wm_base_listener, state);
 } else if (CstringIsMatch(interface, wl_seat_interface.name)) {
  state->seat = (struct wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, 1);
  wl_seat_add_listener(state->seat, &wayland_seat_listener, state);
 }
}

internal void wayland_registry_global_remove(void *data, struct wl_registry *registry, u32 name) {}

global_variable const struct wl_registry_listener wayland_registry_listener = {
 wayland_registry_global,
 wayland_registry_global_remove,
};

global_variable const struct xdg_surface_listener wayland_xdg_surface_listener = {
 wayland_xdg_surface_configure,
};

global_variable const struct xdg_toplevel_listener wayland_xdg_toplevel_listener = {
 wayland_xdg_toplevel_configure,
 wayland_xdg_toplevel_close,
};

//- shm buffers
// TODO: delete this section once vulkan is presenting. vkCreateWaylandSurfaceKHR only needs
// state->display and state->surface, the driver then creates and attaches buffers itself.

internal void wayland_buffer_release(void *data, struct wl_buffer *wl_buffer) {
 WaylandBuffer *buffer = (WaylandBuffer *)data;
 buffer->busy = false;
}

global_variable const struct wl_buffer_listener wayland_buffer_listener = {
 wayland_buffer_release,
};

//- public api

internal PlatformWindow *wayland_window_open(Arena *arena, WindowParams params) {
 PlatformWindow *window = PushStructZero(arena, PlatformWindow);
 WaylandState *state = &window->wayland;

 state->width  = params.window_width  > 0 ? (u32)params.window_width  : 960;
 state->height = params.window_height > 0 ? (u32)params.window_height : 540;

 state->display = wl_display_connect(NULL);
 AssertLog((state->display != NULL), "failed to connect to wayland display\n");

 // ask for the registry and listen to its global events
 struct wl_registry *registry = wl_display_get_registry(state->display);
 wl_registry_add_listener(registry, &wayland_registry_listener, state);
 wl_display_roundtrip(state->display);

 AssertLog((state->compositor != NULL), "compositor has no wl_compositor\n");
 AssertLog((state->shm != NULL), "compositor has no wl_shm\n");
 AssertLog((state->xdg_wm_base != NULL), "compositor has no xdg_wm_base\n");
 if (state->seat == NULL) {
  Log("no wl_seat: running without keyboard input\n");
 }

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

 return window;
}

internal b32 wayland_window_pump(PlatformWindow *window, PlatformInput *input) {
 WaylandState *state = &window->wayland;

 while (wl_display_prepare_read(state->display) != 0) {
  if (wl_display_dispatch_pending(state->display) == -1) {
   Log("wayland connection lost\n");
   return false;
  }
 }
 wl_display_flush(state->display);

 struct pollfd fd = { wl_display_get_fd(state->display), POLLIN, 0 };
 if (poll(&fd, 1, 0) > 0) {
  if (wl_display_read_events(state->display) == -1) {
   Log("wayland connection lost\n");
   return false;
  }
 } else {
  wl_display_cancel_read(state->display);
 }

 if (wl_display_dispatch_pending(state->display) == -1) {
  Log("wayland connection lost\n");
  return false;
 }

 *input = state->input;

 // NOTE: transitions are per frame, the down state carries over
 for (u32 i = 0; i < ArrayCount(state->input.keys); ++i)  state->input.keys[i].half_transition_count = 0;
 for (u32 i = 0; i < ArrayCount(state->input.mouse); ++i) state->input.mouse[i].half_transition_count = 0;

 return !state->should_close;
}

internal void wayland_window_present(PlatformWindow *window, u32 *pixels, u32 width, u32 height) {
 WaylandState *state = &window->wayland;

 // NOTE: find a buffer the compositor isn't reading
 WaylandBuffer *buffer = NULL;
 while (!buffer) {
  for (u32 i = 0; i < ArrayCount(state->buffers); ++i) {
   if (!state->buffers[i].busy) {
    buffer = &state->buffers[i];
    break;
   }
  }

  // both are still held by the compositor, wait for a release
  if (!buffer && wl_display_dispatch(state->display) == -1) {
   return;
  }
 }

 // NOTE: the shm buffers get resized lazily, here, when the window size changed
 if (buffer->width != state->width || buffer->height != state->height) {
  if (buffer->buffer) wl_buffer_destroy(buffer->buffer);
  if (buffer->pixels) munmap(buffer->pixels, buffer->size);

  u32 stride   = state->width * 4; // 4 bytes per pixel (XRGB8888)
  buffer->size = stride * state->height;

  int fd = memfd_create("loo-shared-memory", MFD_CLOEXEC);
  AssertLog((fd != -1), "Failed to open buffer for shared memory.");
  AssertLog((ftruncate(fd, buffer->size) != -1), "Failed to truncate buffer.");

  buffer->pixels = (u32 *)mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  AssertLog((buffer->pixels != MAP_FAILED), "Failed to mmap shared memory.");

  // a pool is the compositor-side view of our shared memory. libwayland sends the fd
  // over the socket for us (the SCM_RIGHTS dance, in the other direction).
  struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, fd, buffer->size);

  // a buffer is a typed slice of the pool: offset, size, stride and pixel format
  buffer->buffer = wl_shm_pool_create_buffer(pool, 0, state->width, state->height, stride, WL_SHM_FORMAT_XRGB8888);
  wl_buffer_add_listener(buffer->buffer, &wayland_buffer_listener, buffer);

  wl_shm_pool_destroy(pool);
  close(fd);

  buffer->width  = state->width;
  buffer->height = state->height;
 }

 u32 copy_width = Min(width, buffer->width);
 for (u32 y = 0; y < buffer->height; ++y) {
  u32 *row = buffer->pixels + (u64)y * buffer->width;
  if (y < height) {
   MemCpy(row, pixels + (u64)y * width, copy_width * sizeof(u32));
   MemSetZero(row + copy_width, (buffer->width - copy_width) * sizeof(u32));
  } else {
   MemSetZero(row, buffer->width * sizeof(u32));
  }
 }

 wl_surface_attach(state->surface, buffer->buffer, 0, 0);
 wl_surface_damage(state->surface, 0, 0, buffer->width, buffer->height);
 wl_surface_commit(state->surface);
 buffer->busy = true;

 wl_display_flush(state->display);
}

internal f32 wayland_window_refresh_hz(PlatformWindow *window) {
 return 60.0f;
}

internal void wayland_window_close(PlatformWindow *window) {
 WaylandState *state = &window->wayland;
 for (u32 i = 0; i < ArrayCount(state->buffers); ++i) {
  WaylandBuffer *buffer = &state->buffers[i];
  if (buffer->buffer) wl_buffer_destroy(buffer->buffer);
  if (buffer->pixels) munmap(buffer->pixels, buffer->size);
 }
 wl_display_disconnect(state->display);
}
