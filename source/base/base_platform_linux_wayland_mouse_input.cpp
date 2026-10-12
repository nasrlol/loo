//- mouse (pointer) input

enum wayland_mouse_button_pressed: b32 {

    POINTER_PRESSED,
    POINTER_RELEASED,

};

internal void wayland_pointer_button(void *data, struct wl_pointer *pointer,
                                     u32 serial, u32 time, u32 button, u32 button_state) {

    WaylandState *state = (WaylandState *)data;
    b32 pressed = (button_state == WL_POINTER_BUTTON_STATE_PRESSED);

    // BTN_LEFT, BTN_RIGHT, BTN_MIDDLE are consecutive, so this maps them to 0, 1, 2
    if (button >= BTN_LEFT && button <= BTN_MIDDLE) {
        state->mouse_buttons[button - BTN_LEFT] = pressed;
    }

}


internal void wayland_pointer_enter(void *data, struct wl_pointer *pointer,
                                    u32 serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WaylandState *state = (WaylandState *)data;
    Log("<- pointer enteirng window");
    state->mouse_in_window = true;
    state->x_offset = surface_x;
    state->y_offset = surface_y;


}
internal void wayland_pointer_motion(void *data, struct wl_pointer *pointer, u32 time,
                                     wl_fixed_t surface_x, wl_fixed_t surface_y) {
    WaylandState *state = (WaylandState *)data;
    state->x_offset = (f32)wl_fixed_to_double(surface_x);
    state->y_offset = (f32)wl_fixed_to_double(surface_y);
}

internal void wayland_pointer_leave(void *data, struct wl_pointer *pointer, u32 serial, wl_surface *surface) {
    WaylandState *state = (WaylandState *)data;
    state->mouse_in_window = false;
    Log("<- pointer leaving window");
}

internal void wayland_pointer_axis(void *data, struct wl_pointer *pointer, u32 time, u32 axis, wl_fixed_t value) {

}

internal void wayland_pointer_frame(void *data, struct wl_pointer *pointer) {

}

internal void wayland_pointer_axis_source(void *data, struct wl_pointer *pointer, u32 axis_source) {

}

internal void wayland_pointer_axis_stop(void *data, struct wl_pointer *pointer, u32 time, u32 axis) {

}

internal void wayland_pointer_axis_discrete(void *data, struct wl_pointer *pointer, u32 axis, s32 discrete) {

}
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
