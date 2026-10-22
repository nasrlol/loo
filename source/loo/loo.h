#ifndef LOO_H
#define LOO_H


#define SHIFTRGB(color) (color.r << 16) | (color.g << 8) | color.b;

// NOTE: game side types, only included by state.cpp

typedef struct Entity Entity;
struct Entity {
 Vec2 pos; // -1..1 on both axes, y up
 s32 width;
 s32 height;
 };

 typedef struct Entities Entities;
 struct Entities {
 Entity *next;
 Entity *previous;
 };

 typedef struct EntityState EntityState;
 struct EntityState {
 Entities *entities;
 };

 // NOTE: the root of everything the game keeps, lives at GameMemory.state
 typedef struct GameState GameState;
 struct GameState {
 Entity player;
 EntityState entity_state;
};

#endif // LOO_H
