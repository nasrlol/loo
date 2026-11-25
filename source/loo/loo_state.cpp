#include "base_include.h"
#include "render_include.h"
#include "loo_platform.h"
#include "loo.h"

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
}
