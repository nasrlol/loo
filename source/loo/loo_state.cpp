#include "base_include.h"
#include "render_include.h"
#include "loo_platform.h"
#include "loo.h"


internal void clamp_to_framebuffer(s32 width, s32 height, Vec2 *pos0, Vec2 *pos1) {
 if (pos1->x <= 0) pos0->x = 0;
 if (pos1->y <= 0) pos0->y = 0;
 if (pos1->x > width)  pos0->x = width;
 if (pos1->y > height) pos0->y = height;
 }


 internal void draw_line(FrameBuffer *fb, Vec2 pos0, Vec2 pos1, RGB color) {

 clamp_to_framebuffer(fb->width, fb->height, &pos0, &pos1);
 u32 c = SHIFTRGB(color);

 u32 *row = fb->pixels + 1 * fb->pitch;
 for(s32 x = pos0.x; x < pos1.x; ++x) row[x] = c;
 for(s32 y = pos0.y; y < pos1.y; ++y) row[y] = c;

 }

 internal void draw_rect(FrameBuffer *fb, Vec2 pos0, Vec2 pos1, RGB color) {
 clamp_to_framebuffer(fb->width, fb->height, &pos0, &pos1);
 u32 c = SHIFTRGB(color);

 for(s32 y = pos0.y; y < pos1.y; ++y)
 {
 u32 *row = fb->pixels + y * fb->pitch;

 for(s32 x = pos0.x; x < pos1.x; ++x) {

 row[x] = c;
 }

 }
 }


 internal void draw_game_player() {




 }


 extern "C" GAME_UPDATE_AND_RENDER(game_update_and_render) {

 if (!memory->is_initialized)
 {
 GameState *game = PushStructZero(memory->permanent, GameState);
 game->player = { Vec2(1.3, 0.4), 80, 80 };
 memory->state = game;
 memory->is_initialized = true;
 }
 GameState *game = (GameState *)memory->state;

 MemSetZero(fb->pixels, (u64)fb->pitch * fb->height * sizeof(u32));

 Vec2 start = Vec2(0.1, 0.1);
 Vec2 end = Vec2(0.9, 0.9);
 draw_line(fb, start, end, BLACK);

}
