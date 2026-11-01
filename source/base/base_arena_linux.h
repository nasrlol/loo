#ifndef BASE_ARENA_LINUX_H
#define BASE_ARENA_LINUX_H

internal Arena *linux_arena_create(u64 capacity);
internal void      linux_arena_destroy(Arena *arena);

#define ArenaCreate(capacity) linux_arena_create(capacity)
#define ArenaDestroy(arena) linux_arena_destroy(arena)

#endif
