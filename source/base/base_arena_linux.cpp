internal Arena *
linux_arena_create(u64 capacity)
{
 Arena *arena = (Arena *)mmap(
                                    /* kernel decides where to throw the arena */
                                    NULL,
                                    capacity + sizeof(Arena),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS,
                                    -1,
                                    0);

 if(arena == MAP_FAILED) {
  return NULL;
 }

 arena->capacity          = capacity;
 arena->base_position     = (u8 *)arena + sizeof(Arena);
 arena->current_position  = 0;
 arena->previous_position = 0;

 return arena;
}

internal void
linux_arena_destroy(Arena *arena)
{
 if(!arena) {
  return;
 }

 munmap(arena, arena->capacity + sizeof(Arena));
}
