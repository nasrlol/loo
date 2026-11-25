#ifndef BASE_STACK_LINUX_H
#define BASE_STACK_LINUX_H

#define StackCreate(size) linux_stack_create(size)
#define StackDestroy(stack) linux_stack_destroy(stack)

internal MemStack *linux_stack_create(u64 capacity);
internal void      linux_stack_destroy(MemStack *stack);

#endif
