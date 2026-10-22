// [] x amount of memory used through out the time
// [] delta time
// [] cpu cycles



internal Profile *
linux_start_profile(MemArena *arena, void *callback) {

 ScratchArena scratch = scratch_start(arena);
 Profile *profile = PushStruct(arena , Profile);
 u64 start, end = 0;

 u32 cylcles_low, cycles_high, clycles_low1, cycles_high1;
 GetCycles(cycles_high, cycles_low);

 &callback;
 GetCycles(cycles_high1, cycles_low1);

 start = ( ((u64)cycles_high << 32)   | cycles_low );
 end   = ( ((u64)cycles_high1 << 32)  | cycles_low1 );

 profile.start        = start;
 profile.end          = end;
 profile.cycles_sec   = end - start;

 return(profile);
 }


 internal void
 end_profile(Profile *profile, ScratchArena *scratch)
 {
 Log("outputting profiling results");
 scratch_end(scratch);
}

