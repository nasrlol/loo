#if 1
#include <arm_neon.h>
#endif // ARCH_ARM

#define u8x16 uint8x16_t

#define CheckSIMDNeon() do { CheckFPAdvSIMDEnabled64(); } while(0);
internal void neon_compare_u8(u8 *a,u8 *b, u64 *out) {

 if(!a || !b ) {

 u64 e = 0;

 umm size_a = sizeof(*a);
 umm size_b = sizeof(*b);

 b32 passed = false;

 u8x16 operand1 = vld1q_u8(a + e);   // V[n]

 }

}


#if 0
#include <intrin.h>
#endif