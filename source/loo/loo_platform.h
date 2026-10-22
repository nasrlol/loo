#ifndef LOO_PLATFORM_H
#define LOO_PLATFORM_H


//- framebuffer
typedef struct FrameBuffer FrameBuffer;
struct FrameBuffer {
 u32 *pixels;
 s32 width;
 s32 height;
 s32 pitch; // in pixels
 };

 //- input
 // half_transition_count = how many times the button flipped this frame, so a press + release
 // inside one frame still shows up. ended_down = state at the end of the frame.
 typedef struct ButtonState ButtonState;
 struct ButtonState {
 s32 half_transition_count;
 b32 ended_down;
 };

 typedef struct GameInput GameInput;
 struct GameInput {
 f32 dt; // fixed, 1 / monitor refresh rate

 ButtonState move_up;
 ButtonState move_down;
 ButtonState move_left;
 ButtonState move_right;
 ButtonState action;
 ButtonState back;

 ButtonState mouse[3]; // left, right, middle
 s32 mouse_x, mouse_y; // in framebuffer pixels
 b32 mouse_in_window;
 };

 internal b32 button_pressed(ButtonState b)  { return b.ended_down && b.half_transition_count > 0; }
 internal b32 button_released(ButtonState b) { return !b.ended_down && b.half_transition_count > 0; }

 struct GameMemory {
 b32 is_initialized;
 MemArena *permanent;
 MemArena *transient;
 void *state;
};

//- entry point
#define GAME_UPDATE_AND_RENDER(name) void name(GameMemory *memory, GameInput *input, FrameBuffer *fb)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRenderFn);

#define GAME_UPDATE_AND_RENDER_SYMBOL "game_update_and_render"

#endif // LOO_PLATFORM_H
