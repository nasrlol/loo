// TODO: in the reload function replace the linux implementatoin with a platform generic one

#include "base_include.h"
#include "loo_platform.h"
#include "platform_include.h"
#include "render_include.h"

#define FRAMEBUFFER_WIDTH  1920
#define FRAMEBUFFER_HEIGHT 1080

//- game code reloading

typedef struct GameCode GameCode;
struct GameCode {
	SharedLibrary library;
	struct timespec last_write_time;
	GameUpdateAndRenderFn *update_and_render;
	String8 path;
};

internal void game_code_init_path(Arena *arena, GameCode *code) {
	String8 self = ReadSelf(arena);
	u64 slash_position = 0;
	if (str8_find_last(&self, str8("/"), &slash_position)) {
		self.size = slash_position + 1;
	}
	
	const char *lib = "loo_state.so";
	
	String8 name = str8(lib);
	code->path      = PushString8(arena, self.size + name.size + 1);
	code->path.size = 0;
	str8_append(&code->path, self);
	str8_append(&code->path, name);
}

internal void game_code_reload_if_changed(Arena *arena, GameCode *code) {
	struct stat info = {};
	ScratchArena scratch = scratch_start(arena);
	int stat_result = stat(null_terminate(scratch.arena, &code->path), &info);
	scratch_end(&scratch);
	if (stat_result != 0) return;
	
	if (info.st_mtim.tv_sec  == code->last_write_time.tv_sec &&
	    info.st_mtim.tv_nsec == code->last_write_time.tv_nsec) return;
	code->last_write_time = info.st_mtim;
	
	if (code->library.mem) LibraryClose(&code->library);
	code->library = LibraryLoad(arena, code->path);
	code->update_and_render = (GameUpdateAndRenderFn *)LibraryProcLoad(&code->library, GAME_UPDATE_AND_RENDER_SYMBOL);
	Log("loaded game code\n");
}

//- input mapping
internal PlatformButton button_from_platform(PlatformButton a, PlatformButton b) {
	PlatformButton result = {};
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
	params.window_width  = FRAMEBUFFER_WIDTH;
	params.window_height = FRAMEBUFFER_HEIGHT;
	PlatformWindow *window = PlatformWindowOpen(platform_arena, params);
	
	FrameBuffer framebuffer = {};
	framebuffer.width  = FRAMEBUFFER_WIDTH;
	framebuffer.height = FRAMEBUFFER_HEIGHT;
	framebuffer.pixels = PushArrayZero(platform_arena, u32, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);
	
	GameMemory memory = {};
	memory.permanent = permanent;
	memory.transient = transient;
	
	GameCode code = {};
	game_code_init_path(platform_arena, &code);
	
	vk_render_entrypoint(memory.permanent, "engine");
	
	f32 dt = 1.0f / PlatformWindowRefreshHz(window);
	PlatformInput platform_input = {};
	while (PlatformWindowPump(window, &platform_input))
	{
		if (platform_input.keys[KEY_ESC].ended_down ||
			platform_input.keys[KEY_Q].ended_down)
		{
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
