internal Arena *
win32_arena_create(u64 capacity)
{
 Arena *arena = (Arena *)VirtualAlloc(NULL,
                                            capacity + sizeof(Arena),
                                            MEM_COMMIT | MEM_RESERVE,
                                            PAGE_READWRITE);
 if(!arena) {
  return NULL;
 }

 arena->capacity          = capacity;
 arena->base_position     = (u8 *)arena + sizeof(Arena);
 arena->current_position  = 0;
 arena->previous_position = 0;
 return arena;
}

internal void
win32_arena_destroy(Arena *arena)
{
 if(!arena) {
  return;
 }

 VirtualFree(arena, 0, MEM_RELEASE);
 return;
}
