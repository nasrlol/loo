#ifndef BASE_THREAD_H

thread_local Arena *tls_scratch_arena = 0;
#define ScratchGet() scratch_start(tls_scratch_arena)

#define BASE_THREAD_H

typedef void ThreadProc(void *param);
typedef struct Thread Thread;
struct Thread {
    ScratchArena *memory;
    u64 data[1];
};

typedef struct ThreadStart ThreadStart;
struct ThreadStart {
    ThreadProc *proc;
    void       *param;
};

#endif // BASE_THREAD_H
