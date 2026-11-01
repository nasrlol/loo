#ifndef BASE_PROFILE_X64_H
#define BASE_PROFILE_X64_H

#define StartProfile(arena, callback) internal Profile *x86_start_profile(Arena *arena, void *callback);
#define EndProfile(profile, scratch)  internal void x86_end_profile(Profile *profile, ScratchArena *scratch);


/* #define GetCycles(high, low) do \ { \
 asm volatile ("RDTSC\n\t" \
 "mov %%edx, %0\n\t" \
 "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low):: “%eax”, “%edx”); \
 while(0); */

#endif
