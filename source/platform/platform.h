#ifndef PLATFORM_H
#define PLATFORM_H

//- Types

typedef struct WindowParams WindowParams;
struct WindowParams {
 cstring name;
 int window_width;
 int window_height;
 int display_width;
 int display_height;

 };

 typedef struct PlatformWindow PlatformWindow;

 // NOTE: raw input as the window backend sees it. keys are indexed by linux evdev scancode
 // (KEY_W, KEY_ESC, ...), mouse position is already in framebuffer pixels.
 typedef struct PlatformButton PlatformButton;
 struct PlatformButton {
 s32 half_transition_count;
 b32 ended_down;
 };

 typedef struct PlatformInput PlatformInput;
 struct PlatformInput {
 PlatformButton keys[256];
 PlatformButton mouse[3]; // BTN_LEFT, BTN_RIGHT, BTN_MIDDLE
 s32 mouse_x, mouse_y;
 b32 mouse_in_window;
 };

#endif // PLATFORM_H
