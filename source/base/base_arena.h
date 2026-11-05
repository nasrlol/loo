#ifndef BASE_ARENA_H

#define BASE_ARENA_H
typedef struct Arena     Arena;
typedef struct ScratchArena ScratchArena;

struct Arena {
 u64       current_position;
 u64       last_alloc_position;
 u64       capacity;
 u64       block_position;
 u8       *base_position;

 Arena    *current;
 Arena    *previous;
};

struct ScratchArena {
 Arena *arena;
 Arena *start_block;
 u64    start_position;
 u64    start_last_alloc_position;
};

internal void        *arena_alloc(Arena *arena, u64 size, b32 zero);
internal u64          arena_position(Arena *arena);
internal void         arena_pop(Arena *arena, u64 size);
internal void         arena_pop_to(Arena *arena, u64 pos);
internal void         arena_clear(Arena *arena);
internal ScratchArena scratch_start(Arena *arena);
internal void         scratch_end(ScratchArena *scratch);
internal void        *arena_resize_block(Arena *arena, void *block, u64 new_size, u64 old_size, b32 zero);

#define Align(pointer, alignment)                                align((u64)(pointer), (umm)(alignment))
#define ArenaHeaderSize                                          Align(sizeof(Arena), arena_align)
#define PushStruct(arena, type)                                  (type *)arena_alloc((arena), sizeof(type), 0)
#define PushStructZero(arena, type)                              (type *)arena_alloc((arena), sizeof(type), 1)
#define PushArray(arena, type, len)                              (type *)arena_alloc((arena), sizeof(type) * (len), 0)
#define PushArrayZero(arena, type, len)                          (type *)arena_alloc((arena), sizeof(type) * (len), 1)
#define ArenaResize(arena, block, new_size, old_size)            arena_resize_block((arena), (block), (new_size), (old_size), 0)
#define ArenaResizeZero(arena, block, new_size, old_size)        arena_resize_block((arena), (block), (new_size), (old_size), 1)
#define ArenaPopLastPush(arena)                                  arena_pop_to((arena), (arena)->current->block_position + (arena)->current->last_alloc_position)

#endif
