internal void *linux_thread_entry(void *raw)
{
    ThreadStart start = *(ThreadStart *)raw;
	Arena *arena = ArenaCreate(MiB(64));
    start.proc(start.param);
    ArenaDestroy(arena);
    return 0;
}

internal Thread thread_launch(Arena *arena, ThreadProc *proc, void *param)
{
    Thread thread = {};
	
    ThreadStart *start = PushStruct(arena, ThreadStart);
    start->proc  = proc;
    start->param = param;
	
    pthread_t handle;
    int error = pthread_create(&handle, 0, linux_thread_entry, start);
    AssertLog(!error, "failed to launch thread");
	
    thread.data[0] = (u64)handle;
    return thread;
}


internal b32 thread_join(Thread thread)
{
    return pthread_join((pthread_t)thread.data[0], 0) == 0;
}

internal void thread_detach(Thread thread)
{
    pthread_detach((pthread_t)thread.data[0]);
}

internal void thread_set_name(String8 name)
{
    char buffer[16] = {0};
    MemCpy(buffer, name.data, Min(name.size, ArrayCount(buffer) - 1));
    pthread_setname_np(pthread_self(), buffer);
}

internal u64 thread_current_id(void)
{
    return (u64)gettid();
}

