#if ARCH_ARM
#include <arm_neon.h>

typedef __u128 __u128;

#define u8x16 uint8x16_t

#define CheckSIMDNeon() do { CheckFPAdvSIMDEnabled64(); } while(0);
internal void neon_compare_u8(u8 *a,u8 *b) {

 // TODO: ...

}

#elif ARCH_X86 || ARCH_x86_64

#include <immintrin.h>
#if COMPILER_MSVC
#include <intrin.h>
#endif

internal u32 simd_ctz32(u32 x) {
#if COMPILER_MSVC
 unsigned long index;
 _BitScanForward(&index, x);
 return (u32)index;
#else
 return (u32)__builtin_ctz(x);
#endif
}

internal u64 simd_find_byte(u8 *data, u64 size, u8 byte) {
 u64 index = 0;
 m128i needle = _mm_set1_epi8((char)byte);

 for(; index + 16 <= size; index += 16) {
  m128i chunk = _mm_loadu_si128((m128i *)(data + index));
  m128i eq    = _mm_cmpeq_epi8(chunk, needle);
  u32 mask      = (u32)_mm_movemask_epi8(eq);
  if(mask != 0) {
   return index + simd_ctz32(mask); // form what i understand is that we check the lowest bit because that is the earliest thing we cann check? from the packed check that we did? or  something like tha?
  }
 }

 for(; index < size; ++index) {
  if(data[index] == byte) {
   return index;
  }
 }
 return size;
}

#endif // ARCH