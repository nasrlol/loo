#ifndef BASE_ARENA_WIN32_H
#define BASE_ARENA_WIN32_H

internal void      win32_arena_destroy(Arena *arena);
internal Arena *win32_arena_create(u64 capacity);

#define ArenaCreate(capacity) win32_arena_create(capacity)
#define ArenaDestroy(arena) win32_arena_destroy(arena)

#endif
