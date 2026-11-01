#ifndef BASE_THREAD_LINUX_H
#define BASE_THREAD_LINUX_H

//- os impementations for threading
// note: keeping the implementation linux specific for now

typedef void (*ThreadProc)(void *arg);

internal b32  thread_launch(Thread *t, ThreadProc proc, void *arg);
internal void thread_join();
internal void thread_attach();
internal void thread_exit();
internal void thread_get_current();
internal void thread_compare();

internal void mutex_create();
internal void mutex_destroy();
internal void mutex_lock();
internal void mutex_unlock();

#endif
