#ifndef __CPUTIME_H
#define __CPUTIME_H

#include "stdint.h"

float    clock_cpu_getres(void);
uint32_t clock_cpu_gettime(void);
uint32_t clock_cpu_microsecond(uint32_t cpu_tick);
uint32_t clock_cpu_millisecond(uint32_t cpu_tick);
uint32_t clock_cpu_microsecond_diff(uint32_t start, uint32_t end);
uint32_t clock_cpu_millisecond_diff(uint32_t start, uint32_t end);
uint32_t clock_cpu_delay(uint32_t delay_ns);
int      cortexm_cputime_init(void);
#endif