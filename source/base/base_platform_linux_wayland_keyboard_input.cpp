
internal void wayland_keyboard_keymap(void *data, struct wl_keyboard *keyboard, u32 format, s32 fd, u32 size) {
    Log("<- wl_keyboard.keymap: format=%u fd=%d size=%u\n", format, fd, size);
    close(fd);
}

internal void wayland_keyboard_enter(void *data, struct wl_keyboard *keyboard, u32 serial,
                                     struct wl_surface *surface, struct wl_array *keys) {
    WaylandState *state = (WaylandState *)data;
    Log("<- wl_keyboard.enter\n");

    // wl_array is just { size_t size; size_t alloc; void *data; }. here data is a list of u32.
    u32 *key = (u32 *)keys->data;
    u32 key_count = keys->size / sizeof(u32);
    for (u32 i = 0; i < key_count; ++i) {
        if (key[i] < sizeof(state->keys_down) / sizeof(state->keys_down[0])) {
            state->keys_down[key[i]] = true;
        }
    }
}

// leave: we lost focus. after this we will NOT receive release events for keys that
// are still held, so we release everything ourselves. otherwise keys get "stuck".
internal void wayland_keyboard_leave(void *data, struct wl_keyboard *keyboard, u32 serial,
                                     struct wl_surface *surface) {
    WaylandState *state = (WaylandState *)data;
    Log("<- wl_keyboard.leave\n");

    MemSetZero(state->keys_down, sizeof(state->keys_down));
}

internal void wayland_keyboard_key(void *data, struct wl_keyboard *keyboard, u32 serial,
                                   u32 time, u32 key, u32 key_state) {
    WaylandState *state = (WaylandState *)data;
    b32 pressed = (key_state == WL_KEYBOARD_KEY_STATE_PRESSED);

    Log("<- wl_keyboard.key: key=%u %s\n", key, pressed ? "pressed" : "released");

    if (key < sizeof(state->keys_down) / sizeof(state->keys_down[0])) {
        state->keys_down[key] = pressed;
    }

    if ((key == KEY_ESC || key == KEY_Q) && pressed) {
        state->should_close = true;
    }
}

internal void wayland_keyboard_modifiers(void *data, struct wl_keyboard *keyboard, u32 serial, u32 mods_depressed, u32 mods_latched,
                                         u32 mods_locked, u32 group) {
}

internal void wayland_keyboard_repeat_info(void *data, struct wl_keyboard *keyboard,
                                           s32 rate, s32 delay) {
    Log("<- wl_keyboard.repeat_info: rate=%d delay=%d\n", rate, delay);
}

// the listener: a struct of function pointers, one per event, in the order the events
// are declared in wayland.xml (which is also their opcode). libwayland indexes into this
// struct with the opcode it reads from the message header.
global_variable const struct wl_keyboard_listener wayland_keyboard_listener = {
    wayland_keyboard_keymap,      // opcode 0
    wayland_keyboard_enter,       // opcode 1
    wayland_keyboard_leave,       // opcode 2
    wayland_keyboard_key,         // opcode 3
    wayland_keyboard_modifiers,   // opcode 4
    wayland_keyboard_repeat_info, // opcode 5
};





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

    if(has_pointer && state->pointer == NULL) {
        state->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(state->pointer, &wayland_pointer_listener, state);
    } else if(!has_pointer && state->pointer != NULL){
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

global_variable const struct wl_seat_listener wayland_seat_listener = {
    wayland_seat_capabilities, // opcode 0
    wayland_seat_name,         // opcode 1 (since version 2)
};
