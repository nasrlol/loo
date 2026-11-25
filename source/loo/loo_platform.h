#ifndef LOO_PLATFORM_H
#define LOO_PLATFORM_H

#include "platform/platform.h"

typedef struct FrameBuffer FrameBuffer;
struct FrameBuffer {
	u32 *pixels;
	s32 width;
	s32 height;
};

typedef struct GameInput GameInput;
struct GameInput {
	f32 dt;

	PlatformButton move_up;
	PlatformButton move_down;
	PlatformButton move_left;
	PlatformButton move_right;
	PlatformButton action;
	PlatformButton back;

	PlatformButton mouse[3];
	s32 mouse_x, mouse_y;
	b32 mouse_in_window;
};

internal b32 button_pressed(PlatformButton b)  { return  b.ended_down && b.half_transition_count > 0; }
internal b32 button_released(PlatformButton b) { return !b.ended_down && b.half_transition_count > 0; }

struct GameMemory {
	Arena *permanent;
	Arena *transient;
	void *state;
};

#define GAME_UPDATE_AND_RENDER(name) void name(GameMemory *memory, GameInput *input, FrameBuffer *fb)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRenderFn);

#define GAME_UPDATE_AND_RENDER_SYMBOL "game_update_and_render"

#endif // LOO_PLATFORM_H
