internal inline u64 ReadCPUTimer()
{
u64 val;
GetCurrentCPUTime(val);
return val;
}

internal inline u64 ReadCPUTimerFreq()
{
u64 val;
__asm__ volatile("mrs %0, cntfrq_el0" : "=r"(val));
return val;
}
