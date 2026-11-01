#if PLATFORM_UNIX

internal Arena *
unix_arena_create(u64 capacity)
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
unix_arena_destroy(Arena *arena)
{
 if(!arena) {
  return;
 }

 munmap(arena, arena->capacity + sizeof(Arena));
}

#elif PLATFORM_WINDOWS

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

#endif

internal void *
arena_alloc(Arena *arena, u64 size, b32 zero)
{
 Assert(arena);

 u64 aligned = Align(arena->current_position, arena_align);
 u64 new_pos = aligned + size;

 AssertLog((new_pos <= arena->capacity), "max arena capacity reached");


 void *out = arena->base_position + aligned;

 arena->previous_position = arena->current_position;
 arena->current_position  = aligned + size;

 if(zero) {
  MemSetZero(out, size);
 }

 return out;
}

internal void
arena_pop(Arena *arena, u64 size)
{
 size = Min(size, arena->current_position);
 arena->current_position -= size;
}

internal void
arena_pop_to(Arena *arena, u64 pos)
{
 u64 size = pos < arena->current_position ? arena->current_position - pos : 0;
 arena_pop(arena, size);
}

internal void
arena_clear(Arena *arena)
{
 arena->current_position = 0;
}

//- resizing an arena
internal Arena *
arena_resize_block_align(Arena *arena, void *old_memory, u64 new_size, u64 old_size, umm alignment)
{
 u8 *old_mem = (u8 *)old_memory;

 if(is_pow(alignment)) {
  arena->current_position = Align(arena->current_position, alignment);
 }


b32 used = (old_mem >= arena->base_position && old_mem < arena->base_position + arena->current_position);


 if(old_memory == NULL || old_size == 0) {
  return (Arena *)arena_alloc(arena, new_size, 0);
 } else if(used) {
  if((arena->base_position + arena->previous_position) == old_memory) {
   arena->current_position = arena->previous_position + new_size;
   if(new_size > old_size) {
    MemSetZero(old_mem + old_size, new_size - old_size);
   }

   return (Arena *)old_memory;
  } else {
   void *new_memory = arena_alloc(arena, new_size, 0);
   umm   copy_size  = old_size < new_size ? old_size : new_size;
   memmove(new_memory, old_mem, copy_size);
  }
 } else {
  AssertLog(false, "Failed to resize memory because the memory wasnt allocated yet.");
 }

 return NULL;
}

internal Arena *
arena_resize_block(Arena *arena, void *old_memory, u64 new_size, u64 old_size)
{
 return arena_resize_block_align(arena, old_memory, new_size, old_size, arena_align);
}

internal ScratchArena
scratch_start(Arena *arena)
{
 ScratchArena scratch;
 scratch.arena          = arena;
 scratch.start_position = arena->current_position;

 return scratch;
}

internal void
scratch_end(ScratchArena *scratch)
{
 scratch->arena->current_position = scratch->start_position;
}
