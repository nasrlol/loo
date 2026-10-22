#ifndef BASE_PROFILE_H
#define BASE_PROFILE_H


typedef struct Profile Profile;
struct Profile {
  f32 start, end;
  // TODO: is there a way to measure the cpu cycles?
  // frame time if graphics is enabled
  f32 frame_time;
  f32 cycles_sec;
  };

  #define GetCycles(high, low) do { \
  asm volatile ("RDTSC\n\t" \
  "mov %%edx, %0\n\t" \
  "mov %%eax, %1\n\t": "=r" (high), "=r" (low)); \
  while(0);


#endif // BASE_PROFILE_H