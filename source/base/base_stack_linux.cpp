internal MemStack *linux_stack_create(u64 capacity)
{
 MemStack *stack = (MemStack *)mmap(0, capacity + sizeof(MemStack), PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

 if(stack == MAP_FAILED) {
  return NULL;
 }

 stack->capacity       = capacity;
 stack->base_position  = (u8 *)stack + sizeof(MemStack);
 stack->current_offset = 0;

 return stack;
}

internal void linux_stack_destroy(MemStack *stack)
{
 if(!stack) {
  return;
 }

 munmap(stack, stack->capacity + sizeof(MemStack));
}
