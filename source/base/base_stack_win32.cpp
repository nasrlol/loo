// request a page from the operating system
internal MemStack *win32_stack_create(u64 capacity)
{
 MemStack *stack = (MemStack *)VirtualAlloc(NULL, capacity + sizeof(MemStack),
                                            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

 if(!stack) {
  Log("Failed to receive a memory address from the operating system");
  return NULL;
 }

 stack->capacity       = capacity;
 stack->base_position  = (u8 *)stack + sizeof(MemStack);
 stack->current_offset = 0;
 return stack;
}

internal void win32_stack_destroy(MemStack *stack)
{
 b32 result = VirtualFree(stack,
                          0, // stands for the byte of committed pages
                          MEM_RELEASE);

 if(!result) {
  Log("Failed to receive a memory address from the operating system");
 }
}
