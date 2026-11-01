#ifndef BASE_PROFILE_ARM_H
#define BASE_PROFILE_ARM_H

#define GetCurrentCPUTime(val) __asm__ volatile("isb; mrs %0, cntvct_el0" : "=r"(val) ::"memory");
#define GetCurrentCPUFreq(val) __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(val));

#endif
