internal void button_set(PlatformButton *b, b32 down) {
	if (b->ended_down != down) { b->ended_down = down; ++b->half_transition_count; }
}

internal void kb_keymap(void *d, struct wl_keyboard *k, u32 format, s32 fd, u32 size) { close(fd); }
internal void kb_enter (void *d, struct wl_keyboard *k, u32 serial, struct wl_surface *s, struct wl_array *keys) {}
internal void kb_mods  (void *d, struct wl_keyboard *k, u32 serial, u32 dep, u32 lat, u32 lock, u32 group) {}
internal void kb_leave (void *d, struct wl_keyboard *k, u32 serial, struct wl_surface *s) {
	WaylandState *st = (WaylandState *)d;
	for (u32 i = 0; i < ArrayCount(st->input.keys); ++i) button_set(&st->input.keys[i], 0);
}
internal void kb_key(void *d, struct wl_keyboard *k, u32 serial, u32 time, u32 key, u32 pressed) {
	WaylandState *st = (WaylandState *)d;
	if (key < ArrayCount(st->input.keys)) button_set(&st->input.keys[key], pressed);
}

internal void ptr_enter(void *d, struct wl_pointer *p, u32 serial, struct wl_surface *s, wl_fixed_t x, wl_fixed_t y) {
	WaylandState *st = (WaylandState *)d;
	st->input.mouse_in_window = 1;
	st->input.mouse_x = wl_fixed_to_int(x);
	st->input.mouse_y = wl_fixed_to_int(y);
}
internal void ptr_leave(void *d, struct wl_pointer *p, u32 serial, struct wl_surface *s) {
	((WaylandState *)d)->input.mouse_in_window = 0;
}
internal void ptr_motion(void *d, struct wl_pointer *p, u32 time, wl_fixed_t x, wl_fixed_t y) {
	WaylandState *st = (WaylandState *)d;
	st->input.mouse_x = wl_fixed_to_int(x);
	st->input.mouse_y = wl_fixed_to_int(y);
}
internal void ptr_button(void *d, struct wl_pointer *p, u32 serial, u32 time, u32 button, u32 pressed) {
	WaylandState *st = (WaylandState *)d;
	u32 i = button - BTN_LEFT;
	if (i < ArrayCount(st->input.mouse)) button_set(&st->input.mouse[i], pressed);
}
internal void ptr_axis(void *d, struct wl_pointer *p, u32 time, u32 axis, wl_fixed_t v) {}

global_variable const struct wl_keyboard_listener kb_listener  = { kb_keymap, kb_enter, kb_leave, kb_key, kb_mods };
global_variable const struct wl_pointer_listener  ptr_listener = { ptr_enter, ptr_leave, ptr_motion, ptr_button, ptr_axis };

internal void seat_caps(void *d, struct wl_seat *seat, u32 caps) {
	WaylandState *st = (WaylandState *)d;
	if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !st->keyboard) {
		st->keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(st->keyboard, &kb_listener, st);
	}
	if ((caps & WL_SEAT_CAPABILITY_POINTER) && !st->pointer) {
		st->pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(st->pointer, &ptr_listener, st);
	}
}
internal void wm_ping(void *d, struct xdg_wm_base *wm, u32 serial) { xdg_wm_base_pong(wm, serial); }
internal void xsurf_configure(void *d, struct xdg_surface *s, u32 serial) {
	xdg_surface_ack_configure(s, serial);
	((WaylandState *)d)->configured = 1;
}
internal void top_configure(void *d, struct xdg_toplevel *t, s32 w, s32 h, struct wl_array *states) {}
internal void top_close(void *d, struct xdg_toplevel *t) { ((WaylandState *)d)->should_close = 1; }
internal void buf_release(void *d, struct wl_buffer *b) { ((WaylandBuffer *)d)->busy = 0; }

global_variable const struct wl_seat_listener      seat_listener  = { seat_caps };
global_variable const struct xdg_wm_base_listener  wm_listener    = { wm_ping };
global_variable const struct xdg_surface_listener  xsurf_listener = { xsurf_configure };
global_variable const struct xdg_toplevel_listener top_listener   = { top_configure, top_close };
global_variable const struct wl_buffer_listener    buf_listener   = { buf_release };

internal void reg_global(void *d, struct wl_registry *r, u32 name, const char *iface, u32 version) {
	WaylandState *st = (WaylandState *)d;
	if (CstringIsMatch(iface, wl_compositor_interface.name)) {
		st->compositor = (struct wl_compositor *)wl_registry_bind(r, name, &wl_compositor_interface, 1);
	} else if (CstringIsMatch(iface, wl_shm_interface.name)) {
		st->shm = (struct wl_shm *)wl_registry_bind(r, name, &wl_shm_interface, 1);
	} else if (CstringIsMatch(iface, xdg_wm_base_interface.name)) {
		st->wm = (struct xdg_wm_base *)wl_registry_bind(r, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(st->wm, &wm_listener, st);
	} else if (CstringIsMatch(iface, wl_seat_interface.name)) {
		st->seat = (struct wl_seat *)wl_registry_bind(r, name, &wl_seat_interface, 1);
		wl_seat_add_listener(st->seat, &seat_listener, st);
	}
}
internal void reg_remove(void *d, struct wl_registry *r, u32 name) {}
global_variable const struct wl_registry_listener reg_listener = { reg_global, reg_remove };

internal PlatformWindow *wayland_window_open(Arena *arena, WindowParams params) {
	PlatformWindow *window = PushStructZero(arena, PlatformWindow);
	WaylandState *st = &window->wayland;
	st->width  = (u32)params.window_width;
	st->height = (u32)params.window_height;
	
	st->display = wl_display_connect(NULL);
	AssertLog(st->display, "no wayland display\n");
	wl_registry_add_listener(wl_display_get_registry(st->display), &reg_listener, st);
	wl_display_roundtrip(st->display);
	AssertLog(st->compositor && st->shm && st->wm, "missing wayland globals\n");
	
	st->surface = wl_compositor_create_surface(st->compositor);
	st->xsurf   = xdg_wm_base_get_xdg_surface(st->wm, st->surface);
	st->top     = xdg_surface_get_toplevel(st->xsurf);
	xdg_surface_add_listener(st->xsurf, &xsurf_listener, st);
	xdg_toplevel_add_listener(st->top, &top_listener, st);
	xdg_toplevel_set_title(st->top, params.name);
	wl_surface_commit(st->surface);
	while (!st->configured) wl_display_dispatch(st->display);
	
	u32 stride = st->width * 4;
	u32 frame  = stride * st->height;
	int fd = memfd_create("loo", MFD_CLOEXEC);
	AssertLog(fd != -1 && ftruncate(fd, frame * 2) != -1, "shm alloc failed\n");
	u8 *mem = (u8 *)mmap(NULL, frame * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	AssertLog(mem != MAP_FAILED, "shm mmap failed\n");
	struct wl_shm_pool *pool = wl_shm_create_pool(st->shm, fd, frame * 2);
	for (u32 i = 0; i < 2; ++i) {
		st->buffers[i].pixels = (u32 *)(mem + i * frame);
		st->buffers[i].buffer = wl_shm_pool_create_buffer(pool, i * frame, st->width, st->height,
		                                                  stride, WL_SHM_FORMAT_XRGB8888);
		wl_buffer_add_listener(st->buffers[i].buffer, &buf_listener, &st->buffers[i]);
	}
	wl_shm_pool_destroy(pool);
	close(fd);
	return window;
}

internal b32 wayland_window_pump(PlatformWindow *window, PlatformInput *input) {
	WaylandState *st = &window->wayland;
	
	wl_display_flush(st->display);
	if (wl_display_prepare_read(st->display) == 0) {
		struct pollfd fd = { wl_display_get_fd(st->display), POLLIN, 0 };
		if (poll(&fd, 1, 0) > 0) wl_display_read_events(st->display);
		else                     wl_display_cancel_read(st->display);
	}
	wl_display_dispatch_pending(st->display);
	
	*input = st->input;
	for (u32 i = 0; i < ArrayCount(st->input.keys);  ++i) st->input.keys[i].half_transition_count  = 0;
	for (u32 i = 0; i < ArrayCount(st->input.mouse); ++i) st->input.mouse[i].half_transition_count = 0;
	
	return !st->should_close && wl_display_get_error(st->display) == 0;
}

internal void wayland_window_present(PlatformWindow *window, u32 *pixels, u32 width, u32 height) {
	WaylandState *st = &window->wayland;
	Assert(width == st->width && height == st->height);
	
	WaylandBuffer *b = !st->buffers[0].busy ? &st->buffers[0] : &st->buffers[1];
	while (b->busy) {
		if (wl_display_dispatch(st->display) == -1) return;
		b = !st->buffers[0].busy ? &st->buffers[0] : &st->buffers[1];
	}
	
	MemCpy(b->pixels, pixels, (u64)width * height * 4);
	wl_surface_attach(st->surface, b->buffer, 0, 0);
	wl_surface_damage(st->surface, 0, 0, width, height);
	wl_surface_commit(st->surface);
	b->busy = 1;
	wl_display_flush(st->display);
}

internal f32  wayland_window_refresh_hz(PlatformWindow *window) { return 60.0f; }
internal void wayland_window_close(PlatformWindow *window)      { wl_display_disconnect(window->wayland.display); }
