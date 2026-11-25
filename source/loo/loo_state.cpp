#include "base_include.h"
#include "render_include.h"
#include "loo_platform.h"
#include "loo.h"

internal void draw_rect(FrameBuffer *fb, s32 x0, s32 y0, s32 x1, s32 y1, RGB color) {
	x0 = Clamp(x0, 0, fb->width);
	x1 = Clamp(x1, 0, fb->width);
	y0 = Clamp(y0, 0, fb->height);
	y1 = Clamp(y1, 0, fb->height);
	u32 c = rgb_to_u32(color);

	for (s32 y = y0; y < y1; ++y) {
		u32 *row = fb->pixels + (u64)y * fb->width;
		for (s32 x = x0; x < x1; ++x) row[x] = c;
	}
}

extern "C" GAME_UPDATE_AND_RENDER(game_update_and_render) {
	if (!memory->state) {
		GameState *game = PushStructZero(memory->permanent, GameState);
		game->player = { Vec2(100, 100), 80, 80 };
		memory->state = game;
	}

	GameState *game = (GameState *)memory->state;
	Entity *player = &game->player;

	f32 speed = 400.0f;
	if (input->move_left.ended_down)  player->pos.x -= speed * input->dt;
	if (input->move_right.ended_down) player->pos.x += speed * input->dt;
	if (input->move_up.ended_down)    player->pos.y -= speed * input->dt;
	if (input->move_down.ended_down)  player->pos.y += speed * input->dt;

	MemSetZero(fb->pixels, (u64)fb->width * fb->height * sizeof(u32));

	s32 x = (s32)player->pos.x;
	s32 y = (s32)player->pos.y;
	draw_rect(fb, x, y, x + player->width, y + player->height, GREEN);
}
