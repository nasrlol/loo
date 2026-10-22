
internal ThreadPool thread_pool_create(MemArena *arena, u64 count)
{
 for (u64 index = 0; index < count; ++index) {
 pthread_t *out = PushStruct(arena, pthread_t);
 // pthread_create(out,  0, , 0);
 }

 return {};
 }


 internal Thread thread_launch(ThreadProc proc)
 {
 Thread thread = {};

 pthread_t out;
 int       error = pthread_create(&out, 0, (void *(*)(void *))proc, NULL);
 if (!error) {
 // TODO: the thread pool impelemtnation will follow in the future so i don't know how to handle this properly yet
 Assert(0);
 }

 thread.data[0] = (u64)out;

 // TODO: do i  put a referecne int the Thread struct to the created thread so we can destroy it later?

 return thread;
}