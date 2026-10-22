#ifndef BASE_ARENA_H
#define BASE_ARENA_H

//- arena
typedef struct MemArena     MemArena;
typedef struct ScratchArena ScratchArena;

// NOTE: used capacity can be calculated so we don't care
struct MemArena {
  u64       current_position;
  u64       previous_position;
  u64       capacity;
  u8       *base_position;
  MemArena *next;
  };

  struct ScratchArena {
  MemArena *arena;
  u64       start_position;
  };

  //-

  #if defined(PLATFORM_UNIX)

  internal MemArena *unix_arena_create(u64 capacity);
  internal void      unix_arena_destroy(MemArena *arena);

  #define ArenaCreate(capacity) unix_arena_create(capacity)
  #define ArenaDestroy(arena) unix_arena_destroy(arena)
  #elif defined(PLATFORM_WINDOWS)

  internal void      win32_arena_destroy(MemArena *arena);
  internal MemArena *win32_arena_create(u64 capacity);

  #define ArenaCreate(capacity) win32_arena_create(capacity)
  #define ArenaDestroy(arena) win32_arena_destroy(arena)
  #endif

  internal void        *arena_alloc(MemArena *arena, u64 size, b32 zero);
  internal void         arena_pop(MemArena *arena, u64 size);
  internal void         arena_pop_to(MemArena *arena, u64 pos);
  internal void         arena_clear(MemArena *arena);
  internal ScratchArena scratch_start(MemArena *arena);
  internal void         scratch_end(ScratchArena *scratch);
  internal MemArena    *arena_resize_block_align(MemArena *arena, void *old_memory, u64 new_size, u64 old_size,
  umm alignment);
  internal MemArena    *arena_resize_block(MemArena *arena, void *old_memory, u64 new_size, u64 old_size);
  internal u64          arena_calculate_used_capacity(MemArena *arena);

  #define Align(pointer, alignment) align((u64)(pointer), (umm)(alignment))
  #define PushStruct(arena, type) (type *)arena_alloc((arena), sizeof(type), 0)
  #define PushStructZero(arena, type) (type *)arena_alloc((arena), sizeof(type), 1)
  #define PushArray(arena, type, len) (type *)arena_alloc((arena), sizeof(type) * (len), 0)
  #define PushArrayZero(arena, type, len) (type *)arena_alloc((arena), sizeof(type) * (len), 1)
  #define ArenaResize(arena, old_memory, new_size, old_size) arena_resize_block(arena, old_memory, new_size, old_size) // takes the address of the allocated block in the arena and grows it with a new size
  #define ArenaPopLastPush(arena) arena_pop_to(arena, arena->previous_position)


  // claude magic HAHHAHAHAH
  #define LogArena(arena_expr)                                                                     \
  do {                                                                                         \
  MemArena *_a = (arena_expr);                                                             \
  if (!_a) {                                                                               \
  Log("[arena] %s = NULL (%s:%d)", #arena_expr, __FILE__, __LINE__);                   \
  break;                                                                               \
  }                                                                                        \
  unsigned long long _cap           = (unsigned long long)_a->capacity;                    \
  unsigned long long _cur           = (unsigned long long)_a->current_position;            \
  unsigned long long _prev          = (unsigned long long)_a->previous_position;           \
  unsigned long long _free          = _cur <= _cap ? _cap - _cur : 0;                      \
  unsigned long long _last          = _cur >= _prev ? _cur - _prev : 0;                    \
  double             _pct           = _cap ? (100.0 * (double)_cur / (double)_cap) : 0.0;  \
  u8                *_expected_base = (u8 *)_a + sizeof(MemArena);                         \
  Log("[arena] %s @ %p (%s:%d)\n"                                                          \
  "  base_position     : %p .. %p%s\n"                                                 \
  "  capacity          : %llu bytes\n"                                                 \
  "  current_position  : %llu (used %.2f%%)\n"                                         \
  "  previous_position : %llu\n"                                                       \
  "  last alloc size   : %llu bytes (current - previous)\n"                            \
  "  free              : %llu bytes (capacity - current)\n"                            \
  "  next write addr   : %p",                                                          \
  #arena_expr, (void *)_a, __FILE__, __LINE__,                                         \
  (void *)_a->base_position, (void *)(_a->base_position + _cap),                       \
  _a->base_position == _expected_base ? "" : "  <-- base != arena + sizeof(MemArena)", \
  _cap, _cur, _pct, _prev, _last, _free,                                               \
  (void *)(_a->base_position + _cur));                                                 \
  if (_cur > _cap)                                                                         \
  Log("  !! OVERFLOW: current_position exceeds capacity by %llu bytes", _cur - _cap);  \
  if (_prev > _cur)                                                                        \
  Log("  !! previous_position (%llu) > current_position (%llu)", _prev, _cur);         \
 } while (0)

#endif /* BASE_ARENA_H */
