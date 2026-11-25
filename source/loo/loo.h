#ifndef LOO_H
#define LOO_H

internal u32 rgb_to_u32(RGB c) { return ((u32)c.r << 16) | ((u32)c.g << 8) | (u32)c.b; }

typedef struct Entity Entity;
struct Entity {
	Vec2 pos;
	s32 width;
	s32 height;
};

typedef struct GameState GameState;
struct GameState {
	Entity player;
};

#endif // LOO_H
