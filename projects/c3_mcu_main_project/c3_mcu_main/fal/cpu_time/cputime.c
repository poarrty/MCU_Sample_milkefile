#include "cputime.h"
#include "fal.h"
#include "stm32f4xx.h"

static float cortexm_cputime_getres(void) {
    float ret = 1000 * 1000 * 1000;

    ret = ret / SystemCoreClock;
    return ret;
}

static uint32_t cortexm_cputime_gettime(void) {
    return DWT->CYCCNT;
}

int cortexm_cputime_init(void) {
    /* check support bit */
    if ((DWT->CTRL & (1UL << DWT_CTRL_NOCYCCNT_Pos)) == 0) {
        /* enable trace*/
        CoreDebug->DEMCR |= (1UL << CoreDebug_DEMCR_TRCENA_Pos);

        /* whether cycle counter not enabled */
        if ((DWT->CTRL & (1UL << DWT_CTRL_CYCCNTENA_Pos)) == 0) {
            /* enable cycle counter */
            DWT->CTRL |= (1UL << DWT_CTRL_CYCCNTENA_Pos);
        }
    }

    return 0;
}
FAL_MODULE_INIT(cortexm_cputime_init);

/**
 * The clock_cpu_getres() function shall return the resolution of CPU time, the
 * number of nanosecond per tick.
 *
 * @return the number of nanosecond per tick
 */
float clock_cpu_getres(void) {
    return cortexm_cputime_getres();
}

/**
 * The clock_cpu_gettime() function shall return the current value of cpu time
 * tick.
 *
 * @return the cpu tick
 */
uint32_t clock_cpu_gettime(void) {
    return cortexm_cputime_gettime();
}

/**
 * The clock_cpu_microsecond() fucntion shall return the microsecond according
 * to cpu_tick parameter.
 *
 * @param cpu_tick the cpu tick
 *
 * @return the microsecond
 */
uint32_t clock_cpu_microsecond(uint32_t cpu_tick) {
    float unit = clock_cpu_getres();

    return (uint32_t)((cpu_tick * unit) / 1000);
}

/**
 * The clock_cpu_microsecond() fucntion shall return the millisecond according
 * to cpu_tick parameter.
 *
 * @param cpu_tick the cpu tick
 *
 * @return the millisecond
 */
uint32_t clock_cpu_millisecond(uint32_t cpu_tick) {
    float unit = clock_cpu_getres();

    return (uint32_t)((cpu_tick * unit) / (1000 * 1000));
}

/**
 * The clock_cpu_microsecond_diff() fucntion shall return the microsecond
 * according to start and end time parameter.
 *
 * @param start and end time
 *
 * @return the microsecond
 */
uint32_t clock_cpu_microsecond_diff(uint32_t start, uint32_t end) {
    uint32_t diff = 0;
    float    unit = clock_cpu_getres();

    if (start <= end) {
        diff = end - start;
    } else {
        diff = (0xffffffff - start + end);
    }

    return (uint32_t)((diff * unit) / 1000);
}

/**
 * The clock_cpu_microsecond_diff() fucntion shall return the millisecond
 * according to start and end time parameter.
 *
 * @param start and end time
 *
 * @return the millisecond
 */
uint32_t clock_cpu_millisecond_diff(uint32_t start, uint32_t end) {
    uint32_t diff = 0;
    float    unit = clock_cpu_getres();

    if (start <= end) {
        diff = end - start;
    } else {
        diff = (0xffffffff - start + end);
    }

    return (uint32_t)((diff * unit) / (1000 * 1000));
}

/**
 * The clock_cpu_microsecond_diff() fucntion shall return the millisecond
 * according to start and end time parameter.
 *
 * @param start and end time
 *
 * @return the millisecond
 */
uint32_t clock_cpu_delay(uint32_t delay_ns) {
    uint32_t diff  = 0;
    uint32_t count = 0;
    uint32_t end   = 0;
    float    unit  = clock_cpu_getres();
    uint32_t start = clock_cpu_gettime();

    if (unit > 0) {
        count = delay_ns / unit;

        while (1) {
            end = clock_cpu_gettime();

            if (start <= end) {
                diff = end - start;
            } else {
                diff = (0xffffffff - start + end);
            }

            if (diff >= count) {
                break;
            }
        }
    }

    return 0;
}
