#ifndef LOO_H
#define LOO_H

#endif // LOO_H

typedef struct Entity Entity;
struct Entity {
Vec2 pos;
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
