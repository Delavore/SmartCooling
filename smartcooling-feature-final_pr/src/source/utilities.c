#include "utilities.h"
#include "mik32_memory_map.h"
#include "analog_reg.h"


/* Delay func inside ram for better performance
  us - number of microseconds
*/
void delay(uint32_t us) {
    uint64_t end_mtimer = SCR1_TIMER_GET_TIME() + us * (SYSTEM_FREQ_HZ / 1000000UL);
    while (SCR1_TIMER_GET_TIME() < end_mtimer)
        ;
}

/* Func checks if the number of comparableTime seconds has passed */
int8_t scr1_timer_higher(uint64_t lastTime, uint64_t comparableTime) {
  uint64_t curTime = SCR1_TIMER_GET_TIME();
  uint64_t deltaTime = curTime - lastTime;

  if (deltaTime >= comparableTime)  
    return 1;
  return 0;
}

/* Func checks if the number of comparableTime seconds hasnt passed */
int8_t scr1_timer_lower(uint64_t lastTime, uint64_t comparableTime) {
  uint64_t curTime = SCR1_TIMER_GET_TIME();
  uint64_t deltaTime = curTime - lastTime;

  if (deltaTime <= comparableTime)  
    return 1;
  return 0;
}

/* Get analog value in single mode */
int16_t getAnalog() {
      ANALOG_REG->ADC_SINGLE = 1;
      while (ANALOG_REG->ADC_VALID != 1)
      ;
      return ANALOG_REG->ADC_VALUE;
}