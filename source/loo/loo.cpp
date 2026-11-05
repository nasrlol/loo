#include "base_include.h"
#include "render_include.h"
#include "platform_include.h"
#include "loo_platform.h"


#define FRAMEBUFFER_WIDTH  1920
#define FRAMEBUFFER_HEIGHT 1080

//- game code reloading

typedef struct GameCode GameCode;
struct GameCode {
 SharedLibrary library;
 struct timespec last_write_time;
 GameUpdateAndRenderFn *update_and_render;
  String8 library_path;
  String8 temp_path;
  String8 lock_path;
 };

 internal void game_code_init_paths(Arena *arena, GameCode *code) {

String8 path = ReadSelf(arena);
 u64 slash_position = 0;

if(str8_find_last(&path, str8("/"), &slash_position)) {
path.size = slash_position + 1;
}

code->library_path      = PushString8(arena, path.size + sizeof("loo_state.so"));
code->library_path.size = 0;
code->temp_path         = PushString8(arena, path.size + sizeof("loo_state_temp.so"));
code->temp_path.size    = 0;
code->lock_path         = PushString8(arena, path.size + sizeof("loo_state.lock"));
code->lock_path.size    = 0;

	str8_append(&code->library_path, path);
	str8_append(&code->library_path, str8("loo_state.so"));
	
	str8_append(&code->temp_path, path);
	str8_append(&code->temp_path, str8("loo_state_temp.so"));
	
	str8_append(&code->lock_path, path);
	str8_append(&code->lock_path, str8("loo_state.lock"));
 }

 internal void game_code_reload_if_changed(Arena *arena, GameCode *code) {
b32 lock_file_exists = FileExists(arena, code->lock_path);
Assert(lock_file_exists);


 struct stat info = {};
 ScratchArena stat_scratch = scratch_start(arena);
 int stat_result = stat(null_terminate(stat_scratch.arena, &code->library_path), &info);
 scratch_end(&stat_scratch);
 if (stat_result != 0) return;

 b32 changed = info.st_mtim.tv_sec  != code->last_write_time.tv_sec ||
 info.st_mtim.tv_nsec != code->last_write_time.tv_nsec;
 if (!changed) return;

 code->last_write_time = info.st_mtim;

 if (code->library.mem) {
LibraryClose(&code->library);
 code->update_and_render = NULL;
 }

 ScratchArena scratch = scratch_start(arena);
 String8 data = FileLoad(scratch.arena, code->library_path);
FileDelete(scratch.arena, code->temp_path);
 b32 copied = data.size > 0 && FileWrite(data, code->temp_path);
 scratch_end(&scratch);

 if (!copied) {
 Log("failed to copy %s\n", code->library_path);
 return;
 }

 code->library = LibraryLoad(arena, code->temp_path);
 if (!code->library.mem) {
 Log("failed to load game code: %s\n", dlerror());
 return;
 }

 code->update_and_render = (GameUpdateAndRenderFn *)LibraryProcLoad(&code->library, GAME_UPDATE_AND_RENDER_SYMBOL);
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
 Arena *permanent = ArenaCreate(MiB(100));
 Arena *transient = ArenaCreate(MiB(100));
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
 game_code_init_paths(platform_arena, &code);

 f32 dt = 1.0f / PlatformWindowRefreshHz(window);
 PlatformInput platform_input = {};
 while (PlatformWindowPump(window, &platform_input)) {
 if (platform_input.keys[KEY_ESC].ended_down || platform_input.keys[KEY_Q].ended_down) {
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
