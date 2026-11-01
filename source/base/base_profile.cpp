#if ARCH_x86_64
internal Profile *x86_start_profile(Arena *arena, void (*callback)(), void *callback_arg)
{
 ScratchArena scratch = scratch_start(arena);
Unused(scratch);
 u64 start_high, start_low, end_high, end_low;

 // GetCycles(start_high, start_low);

 Profile *p = PushStruct(arena, Profile);
 p->start = start_high - start_low;
 callback(arg);

 // GetCycles(end_high, end_low);
 p->end = end_high - end_low;
 return p;
// do the logging inline
{
 Log("log the profile or something");
}
 scratch_end(scratch);
}

#elif ARCH_ARM

#else
#error platform not supported for profiling
#endif