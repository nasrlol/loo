// generated xdg implementation

struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

//- interface tables

// NOTE: one entry per argument, for messages that don't create or take xdg objects (max 4 args: "iiii")
internal const struct wl_interface *xdg_types_none[] = { NULL, NULL, NULL, NULL };

// NOTE: set_parent's self reference is NULL, libwayland only checks types on incoming objects
internal const struct wl_interface *xdg_toplevel_types_show_window_menu[] = { &wl_seat_interface, NULL, NULL, NULL };
internal const struct wl_interface *xdg_toplevel_types_seat[]             = { &wl_seat_interface, NULL, NULL };
internal const struct wl_interface *xdg_toplevel_types_set_fullscreen[]   = { &wl_output_interface };

internal const struct wl_message xdg_toplevel_requests[] = {
    { "destroy",          "",     xdg_types_none },                      // 0
    { "set_parent",       "?o",   xdg_types_none },                      // 1
    { "set_title",        "s",    xdg_types_none },                      // 2
    { "set_app_id",       "s",    xdg_types_none },                      // 3
    { "show_window_menu", "ouii", xdg_toplevel_types_show_window_menu }, // 4
    { "move",             "ou",   xdg_toplevel_types_seat },             // 5
    { "resize",           "ouu",  xdg_toplevel_types_seat },             // 6
    { "set_max_size",     "ii",   xdg_types_none },                      // 7
    { "set_min_size",     "ii",   xdg_types_none },                      // 8
    { "set_maximized",    "",     xdg_types_none },                      // 9
    { "unset_maximized",  "",     xdg_types_none },                      // 10
    { "set_fullscreen",   "?o",   xdg_toplevel_types_set_fullscreen },   // 11
    { "unset_fullscreen", "",     xdg_types_none },                      // 12
    { "set_minimized",    "",     xdg_types_none },                      // 13
};

internal const struct wl_message xdg_toplevel_events[] = {
    { "configure",        "iia",  xdg_types_none }, // 0
    { "close",            "",     xdg_types_none }, // 1
    { "configure_bounds", "4ii",  xdg_types_none }, // 2 (since v4)
    { "wm_capabilities",  "5a",   xdg_types_none }, // 3 (since v5)
};

internal const struct wl_interface xdg_toplevel_interface = {
    "xdg_toplevel", 7,
    14, xdg_toplevel_requests,
    4, xdg_toplevel_events,
};

// NOTE: get_popup's xdg_popup / xdg_positioner types are NULL since we don't have those interfaces
internal const struct wl_interface *xdg_surface_types_get_toplevel[] = { &xdg_toplevel_interface };
internal const struct wl_interface *xdg_surface_types_get_popup[]    = { NULL, NULL, NULL };

internal const struct wl_message xdg_surface_requests[] = {
    { "destroy",             "",     xdg_types_none },                 // 0
    { "get_toplevel",        "n",    xdg_surface_types_get_toplevel }, // 1
    { "get_popup",           "n?oo", xdg_surface_types_get_popup },    // 2
    { "set_window_geometry", "iiii", xdg_types_none },                 // 3
    { "ack_configure",       "u",    xdg_types_none },                 // 4
};

internal const struct wl_message xdg_surface_events[] = {
    { "configure", "u", xdg_types_none }, // 0
};

internal const struct wl_interface xdg_surface_interface = {
"xdg_surface", 7,
5, xdg_surface_requests,
1, xdg_surface_events,
};

internal const struct wl_interface *xdg_wm_base_types_create_positioner[] = { NULL };
internal const struct wl_interface *xdg_wm_base_types_get_xdg_surface[]   = { &xdg_surface_interface, &wl_surface_interface };

internal const struct wl_message xdg_wm_base_requests[] = {
{ "destroy",           "",   xdg_types_none },                      // 0
{ "create_positioner", "n",  xdg_wm_base_types_create_positioner }, // 1
{ "get_xdg_surface",   "no", xdg_wm_base_types_get_xdg_surface },   // 2
{ "pong",              "u",  xdg_types_none },                      // 3
};

internal const struct wl_message xdg_wm_base_events[] = {
{ "ping", "u", xdg_types_none }, // 0
};

internal const struct wl_interface xdg_wm_base_interface = {
"xdg_wm_base", 7,
4, xdg_wm_base_requests,
1, xdg_wm_base_events,
};

//- listeners
// NOTE: field order = event opcode order

struct xdg_wm_base_listener {
void (*ping)(void *data, struct xdg_wm_base *xdg_wm_base, u32 serial);
};

struct xdg_surface_listener {
void (*configure)(void *data, struct xdg_surface *xdg_surface, u32 serial);
};

struct xdg_toplevel_listener {
void (*configure)(void *data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height, struct wl_array *states);
void (*close)(void *data, struct xdg_toplevel *xdg_toplevel);
void (*configure_bounds)(void *data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height);
void (*wm_capabilities)(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities);
};

//- xdg_wm_base requests

internal int xdg_wm_base_add_listener(struct xdg_wm_base *wm_base, const struct xdg_wm_base_listener *listener, void *data) {
return wl_proxy_add_listener((struct wl_proxy *)wm_base, (void (**)(void))listener, data);
}

internal void xdg_wm_base_destroy(struct xdg_wm_base *wm_base) {
struct wl_proxy *proxy = (struct wl_proxy *)wm_base;
wl_proxy_marshal_flags(proxy, 0, NULL, wl_proxy_get_version(proxy), WL_MARSHAL_FLAG_DESTROY);
}

internal struct xdg_surface *xdg_wm_base_get_xdg_surface(struct xdg_wm_base *wm_base, struct wl_surface *surface) {
struct wl_proxy *proxy = (struct wl_proxy *)wm_base;
return (struct xdg_surface *)wl_proxy_marshal_flags(proxy, 2, &xdg_surface_interface, wl_proxy_get_version(proxy), 0, NULL, surface);
}

internal void xdg_wm_base_pong(struct xdg_wm_base *wm_base, u32 serial) {
struct wl_proxy *proxy = (struct wl_proxy *)wm_base;
wl_proxy_marshal_flags(proxy, 3, NULL, wl_proxy_get_version(proxy), 0, serial);
}

//- xdg_surface requests

internal int xdg_surface_add_listener(struct xdg_surface *xdg_surface, const struct xdg_surface_listener *listener, void *data) {
return wl_proxy_add_listener((struct wl_proxy *)xdg_surface, (void (**)(void))listener, data);
}

internal void xdg_surface_destroy(struct xdg_surface *xdg_surface) {
struct wl_proxy *proxy = (struct wl_proxy *)xdg_surface;
wl_proxy_marshal_flags(proxy, 0, NULL, wl_proxy_get_version(proxy), WL_MARSHAL_FLAG_DESTROY);
}

internal struct xdg_toplevel *xdg_surface_get_toplevel(struct xdg_surface *xdg_surface) {
struct wl_proxy *proxy = (struct wl_proxy *)xdg_surface;
return (struct xdg_toplevel *)wl_proxy_marshal_flags(proxy, 1, &xdg_toplevel_interface, wl_proxy_get_version(proxy), 0, NULL);
}

internal void xdg_surface_ack_configure(struct xdg_surface *xdg_surface, u32 serial) {
    struct wl_proxy *proxy = (struct wl_proxy *)xdg_surface;
    wl_proxy_marshal_flags(proxy, 4, NULL, wl_proxy_get_version(proxy), 0, serial);
}

//- xdg_toplevel requests

internal int xdg_toplevel_add_listener(struct xdg_toplevel *toplevel, const struct xdg_toplevel_listener *listener, void *data) {
    return wl_proxy_add_listener((struct wl_proxy *)toplevel, (void (**)(void))listener, data);
}

internal void xdg_toplevel_destroy(struct xdg_toplevel *toplevel) {
    struct wl_proxy *proxy = (struct wl_proxy *)toplevel;
    wl_proxy_marshal_flags(proxy, 0, NULL, wl_proxy_get_version(proxy), WL_MARSHAL_FLAG_DESTROY);
}

internal void xdg_toplevel_set_title(struct xdg_toplevel *toplevel, const char *title) {
    struct wl_proxy *proxy = (struct wl_proxy *)toplevel;
    wl_proxy_marshal_flags(proxy, 2, NULL, wl_proxy_get_version(proxy), 0, title);
}

internal void xdg_toplevel_set_app_id(struct xdg_toplevel *toplevel, const char *app_id) {
    struct wl_proxy *proxy = (struct wl_proxy *)toplevel;
    wl_proxy_marshal_flags(proxy, 3, NULL, wl_proxy_get_version(proxy), 0, app_id);
}
