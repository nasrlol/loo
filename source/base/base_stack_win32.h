#ifndef BASE_STACK_WIN32_H
#define BASE_STACK_WIN32_H

#define StackCreate(size) win32_stack_create(size)
#define StackDestroy(stack) win32_stack_destroy(stack)

internal MemStack *win32_stack_create(u64 capacity);
internal void      win32_stack_destroy(MemStack *stack);

#endif
