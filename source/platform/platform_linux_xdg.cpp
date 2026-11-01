// generated xdg implementation

struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;

//- interface tables

// NOTE: one entry per argument, for messages that don't create or take xdg objects (max 4 args: "iiii")
internal const struct wl_interface *xdg_types_none[] = { NULL, NULL, NULL, NULL };

internal const struct wl_message xdg_toplevel_requests[] = {
 { "destroy",    "",  xdg_types_none }, // 0
 { "set_parent", "?o", xdg_types_none }, // 1
 { "set_title",  "s", xdg_types_none }, // 2
 { "set_app_id", "s", xdg_types_none }, // 3
};

internal const struct wl_message xdg_toplevel_events[] = {
 { "configure", "iia", xdg_types_none }, // 0
 { "close",     "",    xdg_types_none }, // 1
};

internal const struct wl_interface xdg_toplevel_interface = {
 "xdg_toplevel", 1,
 ArrayCount(xdg_toplevel_requests), xdg_toplevel_requests,
 ArrayCount(xdg_toplevel_events), xdg_toplevel_events,
};

internal const struct wl_message xdg_surface_requests[] = {
 { "destroy",             "",     xdg_types_none }, // 0
 { "get_toplevel",        "n",    xdg_types_none }, // 1
 { "get_popup",           "n?oo", xdg_types_none }, // 2
 { "set_window_geometry", "iiii", xdg_types_none }, // 3
 { "ack_configure",       "u",    xdg_types_none }, // 4
};

internal const struct wl_message xdg_surface_events[] = {
 { "configure", "u", xdg_types_none }, // 0
};

internal const struct wl_interface xdg_surface_interface = {
 "xdg_surface", 1,
 ArrayCount(xdg_surface_requests), xdg_surface_requests,
 ArrayCount(xdg_surface_events), xdg_surface_events,
};

internal const struct wl_message xdg_wm_base_requests[] = {
 { "destroy",           "",   xdg_types_none }, // 0
 { "create_positioner", "n",  xdg_types_none }, // 1
 { "get_xdg_surface",   "no", xdg_types_none }, // 2
 { "pong",              "u",  xdg_types_none }, // 3
};

internal const struct wl_message xdg_wm_base_events[] = {
 { "ping", "u", xdg_types_none }, // 0
};

internal const struct wl_interface xdg_wm_base_interface = {
 "xdg_wm_base", 1,
 ArrayCount(xdg_wm_base_requests), xdg_wm_base_requests,
 ArrayCount(xdg_wm_base_events), xdg_wm_base_events,
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
};

//- requests

internal int xdg_wm_base_add_listener(struct xdg_wm_base *wm_base, const struct xdg_wm_base_listener *listener, void *data) {
 return wl_proxy_add_listener((struct wl_proxy *)wm_base, (void (**)(void))listener, data);
}

internal struct xdg_surface *xdg_wm_base_get_xdg_surface(struct xdg_wm_base *wm_base, struct wl_surface *surface) {
 struct wl_proxy *proxy = (struct wl_proxy *)wm_base;
 return (struct xdg_surface *)wl_proxy_marshal_flags(proxy, 2, &xdg_surface_interface, wl_proxy_get_version(proxy), 0, NULL, surface);
}

internal void xdg_wm_base_pong(struct xdg_wm_base *wm_base, u32 serial) {
 struct wl_proxy *proxy = (struct wl_proxy *)wm_base;
 wl_proxy_marshal_flags(proxy, 3, NULL, wl_proxy_get_version(proxy), 0, serial);
}

internal int xdg_surface_add_listener(struct xdg_surface *xdg_surface, const struct xdg_surface_listener *listener, void *data) {
 return wl_proxy_add_listener((struct wl_proxy *)xdg_surface, (void (**)(void))listener, data);
}

internal struct xdg_toplevel *xdg_surface_get_toplevel(struct xdg_surface *xdg_surface) {
 struct wl_proxy *proxy = (struct wl_proxy *)xdg_surface;
 return (struct xdg_toplevel *)wl_proxy_marshal_flags(proxy, 1, &xdg_toplevel_interface, wl_proxy_get_version(proxy), 0, NULL);
}

internal void xdg_surface_ack_configure(struct xdg_surface *xdg_surface, u32 serial) {
 struct wl_proxy *proxy = (struct wl_proxy *)xdg_surface;
 wl_proxy_marshal_flags(proxy, 4, NULL, wl_proxy_get_version(proxy), 0, serial);
}

internal int xdg_toplevel_add_listener(struct xdg_toplevel *toplevel, const struct xdg_toplevel_listener *listener, void *data) {
 return wl_proxy_add_listener((struct wl_proxy *)toplevel, (void (**)(void))listener, data);
}

internal void xdg_toplevel_set_title(struct xdg_toplevel *toplevel, const char *title) {
 struct wl_proxy *proxy = (struct wl_proxy *)toplevel;
 wl_proxy_marshal_flags(proxy, 2, NULL, wl_proxy_get_version(proxy), 0, title);
}

internal void xdg_toplevel_set_app_id(struct xdg_toplevel *toplevel, const char *app_id) {
 struct wl_proxy *proxy = (struct wl_proxy *)toplevel;
 wl_proxy_marshal_flags(proxy, 3, NULL, wl_proxy_get_version(proxy), 0, app_id);
}
