#ifndef BASE_STACK_WIN32_H
#define BASE_STACK_WIN32_H

#define StackCreate(size) win32_stack_create(size)
#define StackDestroy(stack) win32_stack_destroy(stack)

#endif
