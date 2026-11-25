internal Arena *
win32_arena_create(u64 capacity)
{
	u64    header = ArenaHeaderSize;
	Arena *arena = (Arena *)VirtualAlloc(NULL,
										 header + capacity,
										 MEM_COMMIT | MEM_RESERVE,
										 PAGE_READWRITE);
	if(!arena) {
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
win32_arena_destroy(Arena *arena)
{
	if(!arena) {
		return;
	}
	
	VirtualFree(arena, 0, MEM_RELEASE);
	return;
}
