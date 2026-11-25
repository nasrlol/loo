internal Arena *
linux_arena_create(u64 capacity)
{
	u64    header = ArenaHeaderSize;
	Arena *arena = (Arena *)mmap(
								 /* kernel decides where to throw the arena */
								 NULL,
								 header + capacity,
								 PROT_READ | PROT_WRITE,
								 MAP_PRIVATE | MAP_ANONYMOUS,
								 -1,
								 0);
	
	if(arena == MAP_FAILED) {
		return NULL;
	}
	
	arena->capacity            = capacity;
	arena->base_position       = (u8 *)arena + header;
	arena->current_position    = 0;
	arena->last_alloc_position = 0;
	arena->block_position      = 0;
	arena->current             = arena;
	arena->previous            = NULL;
	
	return arena;
}

internal void
linux_arena_destroy(Arena *arena)
{
	if(!arena) {
		return;
	}
	
	munmap(arena, ArenaHeaderSize + arena->capacity);
}
