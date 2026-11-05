internal void *
arena_alloc(Arena *arena, u64 size, b32 zero)
{
 Assert(arena);

 Arena *block   = arena->current;
 u64    aligned = Align(block->current_position, arena_align);

 if((aligned > block->capacity) || (size > block->capacity - aligned)) {
  Arena *next = ArenaCreate(Max(block->capacity, size));
  AssertLog(next, "failed to create a new arena block");

  next->block_position = block->block_position + block->capacity;
  next->previous       = block;
  arena->current       = next;

  block   = next;
  aligned = 0;
 }

 void *out = block->base_position + aligned;

 block->last_alloc_position = aligned;
 block->current_position    = aligned + size;

 if(zero) {
  MemSetZero(out, size);
 }

 return out;
}

internal u64
arena_position(Arena *arena)
{
 Arena *block = arena->current;
 return block->block_position + block->current_position;
}

internal void
arena_pop_to(Arena *arena, u64 pos)
{
 Arena *block = arena->current;

 while(block->previous && (block->block_position >= pos)) {
  Arena *previous = block->previous;
  ArenaDestroy(block);
  block = previous;
 }

 arena->current = block;

 u64 local_position         = pos - block->block_position;
 block->current_position    = Min(block->current_position, local_position);
 block->last_alloc_position = Min(block->last_alloc_position, block->current_position);
}

internal void
arena_pop(Arena *arena, u64 size)
{
 u64 pos = arena_position(arena);
 arena_pop_to(arena, pos - Min(size, pos));
}

internal void
arena_clear(Arena *arena)
{
 arena_pop_to(arena, 0);
}

internal void *
arena_resize_block(Arena *arena, void *block, u64 new_size, u64 old_size, b32 zero)
{
 if((block == NULL) || (old_size == 0)) {
  return arena_alloc(arena, new_size, zero);
 }

 u8 *old_mem = (u8 *)block;

 b32 owned = false;
 for(Arena *it = arena->current; it; it = it->previous) {
  if((old_mem >= it->base_position) && (old_mem < it->base_position + it->current_position)) {
   owned = true;
   break;
  }
 }
 AssertLog(owned, "Failed to resize block because it was not allocated in this arena");

 Arena *current = arena->current;
 u8    *result  = old_mem;
 b32    is_last = (old_mem == current->base_position + current->last_alloc_position);

 if(is_last && (new_size <= current->capacity - current->last_alloc_position)) {
  current->current_position = current->last_alloc_position + new_size;
 } else if(new_size > old_size) {
  result = (u8 *)arena_alloc(arena, new_size, 0);
  MemCpy(result, old_mem, old_size);
 }

 if(zero && (new_size > old_size)) {
  MemSetZero(result + old_size, new_size - old_size);
 }

 return result;
}

internal ScratchArena
scratch_start(Arena *arena)
{
 ScratchArena scratch;
 scratch.arena                     = arena;
 scratch.start_block               = arena->current;
 scratch.start_position            = arena_position(arena);
 scratch.start_last_alloc_position = arena->current->last_alloc_position;

 return scratch;
}

internal void
scratch_end(ScratchArena *scratch)
{
 arena_pop_to(scratch->arena, scratch->start_position);

 if(scratch->arena->current == scratch->start_block) {
  scratch->arena->current->last_alloc_position = scratch->start_last_alloc_position;
 }
}
