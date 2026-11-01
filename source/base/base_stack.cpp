// TODO: the previous position of the stack header never get's set
// TODO: renmae current_position in arena.cpp and arena.h to current_address or vice versa

internal u8 calculate_padding(u64 pointer, u8 alignment, u64 header_size)
{
 u8 modulo, padding;

 if(!is_pow(alignment)) {
  return 0;
 }

 modulo = pointer & (u8)(alignment - 1);

 padding = 0;

 if(0 != modulo) {
  padding = alignment - modulo;
 }

 if(padding < header_size) {
  header_size -= padding;

  if((header_size & (alignment - 1)) != 0) {
   padding += alignment * (1 + (u8)(header_size / alignment));
  } else {
   padding += alignment * ((u8)(header_size / alignment));
  }
 }

 return padding;
}

internal MemStack *stack_push_align(MemStack *stack, u64 size, u8 alignment)
{
 // NOTE: satifisying static analyzer
 AssertLog((stack != NULL), "STACK IS NULL");
 u8 padding = 0;

 // NOTE: this shouln't be possible because we have the allignment predefined
 // so it's bassically a useless check

 if(!is_pow(alignment)) {
  breakpoint();
  return (0);
 }

 if(alignment > 128) { // NOTE: where does the 128 come from?
  alignment = 128;
 }

 u64 current_address;
 current_address = (u64)stack->base_position + stack->current_offset; // NOTE: we already we're checking if the stack existed
 padding         = calculate_padding(current_address, alignment, sizeof(MemStackHeader));

 if(stack->current_offset + padding + size > stack->capacity) {
  breakpoint();
  return 0;
 }

 stack->current_offset += padding;

 u64             next_address = current_address + (u64)padding;
 MemStackHeader *header       = (MemStackHeader *)(next_address - sizeof(MemStackHeader));
 header->padding              = padding;

 stack->current_offset += size;

 return (MemStack *)MemSetZero((MemStack *)next_address, size);
}

internal void *stack_push(MemStack *stack, u64 size)
{
 return stack_push_align(stack, size, arena_align);
}

internal void stack_pop(MemStack *stack, void *pointer)
{
 if(pointer != NULL) {
  u64             start, end, current_address;
  MemStackHeader *header;
  u64             prev_offset;

  start           = (u64)stack->base_position;
  end             = start + (u64)stack->capacity;
  current_address = (u64)pointer;

  if(!(start <= current_address && current_address < end)) {
   if(0 && "Out of bounds memory address passed to stack allocator (free)") {
    return;
   }
   return;
  }

  if(current_address >= start + (u64)stack->current_offset) {
   return;
  }

  header                = (MemStackHeader *)(current_address - sizeof(MemStackHeader));
  prev_offset           = (size_t)(current_address - (u64)header->padding - start);
  stack->current_offset = prev_offset;
 }
}

internal MemStack *stack_resize_align(MemStack *stack, void *pointer, u64 old_size, u64 new_size,
                                      u8 alignment)
{
 if(pointer == NULL) {
  return stack_push_align(stack, new_size, alignment);
 } else if(new_size == 0) {
  stack_pop(stack, pointer);
  return NULL;
 }

 u64   start, end, current_address;
 u64   min_size = old_size < new_size ? old_size : new_size;
 void *new_pointer;

 start           = (u64)stack->base_position;
 end             = start + (u64)stack->capacity;
 current_address = (u64)pointer;
 if(!(start <= current_address && current_address < end)) {
  return NULL;
 }

 if(current_address >= start + (u64)stack->current_offset) {
  return NULL;
 }

 if(old_size == new_size) {
  return (MemStack *)pointer;
 }

 new_pointer = stack_push_align(stack, new_size, alignment);
 AssertLog((new_pointer != NULL), "stack_push_align failed in stack_resize_align");
 memmove(new_pointer, pointer, min_size);
 return (MemStack *)new_pointer;
}

internal MemStack *arena_to_stack(Arena *arena, u64 requested_size)
{
 Assert(arena);

 MemStack *stack  = PushStruct(arena, MemStack);
 u8       *buffer = PushArray(arena, u8, requested_size);

 stack->header         = 0;
 stack->base_position  = buffer;
 stack->capacity       = requested_size;
 stack->current_offset = 0;

 return stack;
}

#if 0
#if MEM_DEBUG
global_variable String8List *stack_log; // TODO: initialize this in the mem stack callback

#endif

internal void
stack_debug_view(Arena *global_arena, MemStack *stack)
{

 if(!stack_log)
 {
  stack_log = PushStruct(global_arena, String8List);
 }


}
#endif

internal void stack_clear(MemStack *stack)
{
 stack->current_offset = 0;
}
