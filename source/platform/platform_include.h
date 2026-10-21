#ifndef PLATFORM_INCLUDE_H
#define PLATFORM_INCLUDE_H

// NOTE: window, input and presenting. sits on top of base, base never includes this.

#ifndef X11
#define X11 0
#endif
#ifndef WAYLAND
#define WAYLAND 0
#endif

#if PLATFORM_LINUX

#include <linux/input-event-codes.h>

#if X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#elif WAYLAND
#include <wayland-client.h>
#endif

#endif // PLATFORM_LINUX

//-
#include "platform.h"

#if PLATFORM_LINUX

#if X11
#include "platform_linux_x11.cpp"
#elif WAYLAND
#include "platform_linux_wayland.h"
#include "platform_linux_xdg.cpp"
#include "platform_linux_wayland.cpp"
#endif

#elif PLATFORM_WINDOWS
#include "platform_win32.cpp"
#endif

#endif // PLATFORM_INCLUDE_H
