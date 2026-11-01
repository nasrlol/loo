#include "base_include.h"
#include "render_include.h"
#include "platform_include.h"
#include "loo_platform.h"


// NOTE: the platform layer. owns main, the window, memory and the loop.
// the game itself lives in state.so and is reloaded whenever it gets rebuilt.

#define FRAMEBUFFER_WIDTH  1920
#define FRAMEBUFFER_HEIGHT 1080

//- game code reloading

typedef struct GameCode GameCode;
struct GameCode {
 void *library;
 struct timespec last_write_time;
 GameUpdateAndRenderFn *update_and_render;

 // NOTE: next to the executable, so it doesn't matter where loo is started from
 char library_path[PATH_MAX];
 char temp_path[PATH_MAX];
 char lock_path[PATH_MAX];
 };

 internal void game_code_init_paths(GameCode *code) {
 char exe_path[PATH_MAX] = {};
  u64 length = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
 AssertLog((length > 0), "failed to read /proc/self/exe\n");

 {
 String8 path = str8(exe_path);
 u64 slash_position = 0;
if(str8_find_last(&path, str8("/"), &slash_position)) {
exe_path[slash_position] = 0;
}
 
 }

 s32 written = 0;
 written = snprintf(code->library_path, sizeof(code->library_path), "%s/loo_state.so", exe_path);
 AssertLog((written > 0 && written < (s32)sizeof(code->library_path)), "game code path too long\n");
 written = snprintf(code->temp_path, sizeof(code->temp_path), "%s/loo_state_temp.so", exe_path);
 AssertLog((written > 0 && written < (s32)sizeof(code->temp_path)), "game code path too long\n");
 written = snprintf(code->lock_path, sizeof(code->lock_path), "%s/loo_state.lock", exe_path);
 AssertLog((written > 0 && written < (s32)sizeof(code->lock_path)), "game code path too long\n");
 }

 internal void game_code_reload_if_changed(Arena *scratch_arena, GameCode *code) {
 // build.sh / reload.sh hold this while the compiler is writing state.so
 if (access(code->lock_path, F_OK) == 0) return;

 struct stat info = {};
 if (stat(code->library_path, &info) != 0) return;

 b32 changed = info.st_mtim.tv_sec  != code->last_write_time.tv_sec ||
 info.st_mtim.tv_nsec != code->last_write_time.tv_nsec;
 if (!changed) return;

 // NOTE: remember the time even if loading fails, so a broken build isn't retried every frame
 code->last_write_time = info.st_mtim;

 if (code->library) {
 dlclose(code->library);
 code->library           = NULL;
 code->update_and_render = NULL;
 }

 ScratchArena scratch = scratch_start(scratch_arena);
 String8 data = FileLoad(scratch.arena, str8(code->library_path));
 // NOTE:
 // unlink first so the copy is a new file, never a rewrite of one that's still mapped
 unlink(code->temp_path);
 b32 copied = data.size > 0 && FileWrite(data, str8(code->temp_path));
 scratch_end(&scratch);

 if (!copied) {
 Log("failed to copy %s\n", code->library_path);
 return;
 }

 // TODO: make this platform generic
 code->library = dlopen(code->temp_path, RTLD_NOW | RTLD_LOCAL);
 if (!code->library) {
 Log("failed to load game code: %s\n", dlerror());
 return;
 }

 code->update_and_render = (GameUpdateAndRenderFn *)dlsym(code->library, GAME_UPDATE_AND_RENDER_SYMBOL);
 if (!code->update_and_render) {
 Log("game code has no %s: %s\n", GAME_UPDATE_AND_RENDER_SYMBOL, dlerror());
 return;
 }

 Log("loaded game code\n");
 }

 //- input mapping

 internal ButtonState button_from_platform(PlatformButton a, PlatformButton b) {
 ButtonState result = {};
 result.ended_down            = a.ended_down || b.ended_down;
 result.half_transition_count = a.half_transition_count + b.half_transition_count;
 return result;
 }

 internal GameInput game_input_from_platform(PlatformInput *platform, f32 dt) {
 PlatformButton none = {};
 GameInput input = {};
 input.dt = dt;

 input.move_up    = button_from_platform(platform->keys[KEY_W], platform->keys[KEY_UP]);
 input.move_down  = button_from_platform(platform->keys[KEY_S], platform->keys[KEY_DOWN]);
 input.move_left  = button_from_platform(platform->keys[KEY_A], platform->keys[KEY_LEFT]);
 input.move_right = button_from_platform(platform->keys[KEY_D], platform->keys[KEY_RIGHT]);
 input.action     = button_from_platform(platform->keys[KEY_SPACE], platform->keys[KEY_ENTER]);
 input.back       = button_from_platform(platform->keys[KEY_BACKSPACE], none);

 for (u32 i = 0; i < ArrayCount(input.mouse); ++i) {
 input.mouse[i] = button_from_platform(platform->mouse[i], none);
 }
 input.mouse_x         = platform->mouse_x;
 input.mouse_y         = platform->mouse_y;
 input.mouse_in_window = platform->mouse_in_window;

 return input;
 }

 //- entrypoint

 ENTRYPOINT()
 {
 // NOTE: the game's memory, outlives every reload of state.so
 Arena *permanent = ArenaCreate(MiB(100));
 Arena *transient = ArenaCreate(MiB(100));
 // NOTE: the platform's own memory (window state, framebuffer, reload scratch)
 Arena *platform_arena = ArenaCreate(MiB(100));
 Assert(permanent && transient && platform_arena);

 WindowParams params = {};
 params.name = (cstring)"Loo";
 PlatformWindow *window = PlatformWindowOpen(platform_arena, params);

 FrameBuffer framebuffer = {};
 framebuffer.width  = FRAMEBUFFER_WIDTH;
 framebuffer.height = FRAMEBUFFER_HEIGHT;
 framebuffer.pitch  = FRAMEBUFFER_WIDTH;
 framebuffer.pixels = PushArrayZero(platform_arena, u32, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);

 GameMemory memory = {};
 memory.permanent = permanent;
 memory.transient = transient;

 GameCode code = {};
 game_code_init_paths(&code);

 // NOTE: fixed timestep from the monitor, we're vsynced to it through the frame callbacks
 f32 dt = 1.0f / PlatformWindowRefreshHz(window);
 Log("running at %.2f Hz\n", 1.0f / dt);

 PlatformInput platform_input = {};
 while (PlatformWindowPump(window, &platform_input)) {
 // NOTE: no decorations, so this is the only way to close the window besides the compositor
 if (platform_input.keys[KEY_ESC].ended_down || platform_input.keys[KEY_Q].ended_down) {
 Log("quit key pressed\n");
 break;
 }

 game_code_reload_if_changed(platform_arena, &code);

 GameInput input = game_input_from_platform(&platform_input, dt);
 arena_clear(memory.transient);
 if (code.update_and_render) {
 code.update_and_render(&memory, &input, &framebuffer);
 }

 PlatformWindowPresent(window, framebuffer.pixels, framebuffer.width, framebuffer.height);
 }

 PlatformWindowClose(window);
 return 0;
}
