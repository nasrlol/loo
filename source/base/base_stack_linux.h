#ifndef BASE_STACK_LINUX_H
#define BASE_STACK_LINUX_H

#define StackCreate(size) linux_stack_create(size)
#define StackDestroy(stack) linux_stack_destroy(stack)

#endif
