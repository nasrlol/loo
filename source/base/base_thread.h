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


#endif // BASE_THREAD_H
