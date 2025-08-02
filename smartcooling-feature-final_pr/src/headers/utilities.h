#ifndef UTILITIES
#define UTILITIES

#include "scr1_timer.h"
// #include "mik32_memory_map.h"

#define SYSTEM_FREQ_HZ 32000000UL

#define SCR1_TIMER_GET_TIME()                                                  \
    (((uint64_t)(SCR1_TIMER->MTIMEH) << 32) | (SCR1_TIMER->MTIME))

// transfer func to ram for better performance
void delay(uint32_t us) __attribute__((section(".ram_func.special_delay"))); 

int16_t getAnalog();

int8_t scr1_timer_higher(uint64_t lastTime, uint64_t comparableTime);
int8_t scr1_timer_lower(uint64_t lastTime, uint64_t comparableTime);

#define SET_TWO_BITS(REG, PIN, VAL) (((REG) & ~(0b11 << ((PIN) << 1))) | ((VAL) << ((PIN) << 1)))

#define MILLIS(MS) (((SYSTEM_FREQ_HZ) / 1000UL) * (MS))

#endif /* UTILITIES */