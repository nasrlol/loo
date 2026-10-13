#define RENDER
#include "base_include.h"
#include "loo.h"

internal void reload_loolib_worker(void *arg) {

time_t original_mod_time = 0;
while(true) {

struct stat result = {0};
 int err = lstat("./librender.so", &result);
AssertLog(err != 0, "Failed to get lstat when hotreloading");
time_t current_mod_time = result.st_mtime;
if(current_mod_time >= original_mod_time) {
original_mod_time = current_mod_time; // NOTE: i know this isnt correct but there is no need to do an extra call for nothing
			dlopen("./librender.so", RTLD_GLOBAL | RTLD_NOW);
}

sleep(1);
}

}


internal void draw_square(WaylandState *state, Vec2 pos, s32 width, s32 height, RGB color) {

 u32 *pixels = (u32 *)state->pixels;

s32 center_x = (s32)((pos.x + 1.0f) * 0.5f * state->width);
    s32 center_y = (s32)((1.0f - pos.y) * 0.5f * state->height);

    s32 x0 = center_x - width / 2;
    s32 y0 = center_y - height / 2;
    s32 x1 = x0 + width;
    s32 y1 = y0 + height;

    // clip to the framebuffer
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > (s32)state->width)  x1 = state->width;
    if (y1 > (s32)state->height) y1 = state->height;

    u32 c = (color.r << 16) | (color.g << 8) | color.b;

    for (s32 y = y0; y < y1; ++y) {
        for (s32 x = x0; x < x1; ++x) {
            pixels[y * state->width + x] = c;
        }
    }

}


internal void render_frame(WaylandState *state) {
Vec2 pos = Vec2(0, 0);
      Entity o = { pos,  80, 80 };

o.pos.x += 0.1;
     draw_square(state, o.pos, o.width, o.height, RED);
}


ENTRYPOINT()
{
    MemArena *arena = ArenaCreate(MiB(100));
    Assert(arena);

     WindowParams params = {};
     params.name = (cstring)"Loo";

EntityState *es = PushStruct(arena, EntityState);
    PlatformWindowEntrypoint(arena, params);

    return 0;
}
