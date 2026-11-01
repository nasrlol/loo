#ifndef BASE_THREAD_H
#define BASE_THREAD_H


// platform agnostic thread buffer
typedef struct Thread Thread;
struct Thread {
#if PLATFORM_LINUX
 pthread_t id;
#endif
 u64 data[2];
 s32 input;
 s32 result;
};


typedef struct ThreadPool ThreadPool;
struct ThreadPool {
 Thread *threads;
 u64     count;
};


//- os impementations for threading
// note: keeping the implementation linux specific for now
#if PLATFORM_LINUX

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

#elif PLATFORM_WINDOWS


#else
#error unsupported platform
#endif


#endif // BASE_THREAD_H
