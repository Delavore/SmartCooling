#include "mik32_hal_usart.h"
#include "mik32_hal_i2c.h"
#include "scr1_timer.h"
#include "xprintf.h"
#include "init_periphery.h"
#include "riscv-irq.h"
#include "epic.h"
#include "gpio.h"
#include "gpio_irq.h"
#include "mik32_memory_map.h"
#include "pad_config.h"
#include "power_manager.h"
#include "scr1_timer.h"
#include "timer32.h"
#include "wakeup.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_adc.h"
#include "analog_reg.h"
#include "dht11_driver.h"
#include "utilities.h"
#include "disp5110_driver.h"

#define SOUND_BORDER (1000)

void EPIC_trap_handler();

USART_HandleTypeDef husart0;  // for debug
USART_HandleTypeDef husart1;  // for DHT11
SPI_HandleTypeDef spi1;

volatile int8_t dhtReady = 0; 
volatile int8_t windReady = 0;

/*
void goSleep() {
    WU->SYS_MASK |= WU_MASK_WAKEUP_EXT_M;
    WU->SYS_LEVEL |= WU_SYS_LEVEL_EXT_LINE_M;
    WU->STOP = 1;
    __asm__ volatile ("wfi");
}
*/

typedef enum {
    WAIT_FIRST,
    WAIT_SECOND,
    WAIT_LAST_SILENCE,
    SIZE
} clap_detection;

int main()
{
    PM->CLK_APB_M_SET = PM_CLOCK_APB_M_EPIC_M;

    GPIO_Init();
    SystemClock_Config();
    
    riscv_irq_set_handler(RISCV_IRQ_MEI, EPIC_trap_handler);
    riscv_irq_enable(RISCV_IRQ_MEI);
    riscv_irq_global_enable();

    EPIC->MASK_LEVEL_SET  = 1 << (EPIC_LINE_GPIO_IRQ_S);

    /*
    GPIO_IRQ->EDGE =      (1 << MOVE_IRQ_LINE);
    GPIO_IRQ->LEVEL_SET = (1 << MOVE_IRQ_LINE);
    GPIO_IRQ->LINE_MUX =  GPIO_IRQ_LINE_MUX(MOVE_IRQ_MUX, MOVE_IRQ_LINE);
    GPIO_IRQ->ENABLE_SET =(1 << MOVE_IRQ_LINE);
    */

    TIMER32_0_Init();
    TIMER32_2_Init();
    USART_Init(&husart0, 0, 8, 0, 115200);

    SPI1_Init(&spi1, &husart0);
    DISP_Init(&spi1);
    DISP_Clear(&spi1);
    DISP_PlayAnim(&spi1);
    delay(5000000);

    DISP_Clear(&spi1);
    ADC_Init(); 


    int16_t curAnalog  = 0;
    int16_t lastAnalog  = 0;

    clap_detection state = WAIT_FIRST;

    uint64_t timestamp = 0;

    int8_t fl = 1;
    int8_t info[5];

    // WIND_PORT->OUTPUT |= 1 << WIND_PIN_NUM;
    while (1) {
        if (windReady) {
            windReady = 0;
            TIMER32_2->ENABLE |= (1 << 1);
            WIND_PORT->OUTPUT ^= 1 << WIND_PIN_NUM;
        }

        /* Update display information */

        if (dhtReady) {
                DHT_Get(info);

                DISP_ClearLine(&spi1, 0);
                DISP_DrawText(&spi1, "temp:", 0, 0);
                DISP_DrawInt(&spi1, info[2], 40, 0);

                DISP_ClearLine(&spi1, 2);
                DISP_DrawText(&spi1, "hum:", 0, 2);
                DISP_DrawInt(&spi1, info[0], 40, 2);
                
                dhtReady = 0;
        }

        curAnalog = getAnalog();

        /* FSM: detecting two claps */

        if (state == WAIT_FIRST && scr1_timer_higher(timestamp, MILLIS(3000)) 
            && lastAnalog < SOUND_BORDER && curAnalog > SOUND_BORDER ) {
            state++;

            timestamp = SCR1_TIMER_GET_TIME();
        } 
        else if (state == WAIT_SECOND && scr1_timer_higher(timestamp, MILLIS(100))
            && scr1_timer_lower(timestamp, MILLIS(300)) && curAnalog > SOUND_BORDER) {
            state = WAIT_FIRST;

        } 
        else if (state == WAIT_SECOND && scr1_timer_higher(timestamp, MILLIS(300)) 
            && scr1_timer_lower(timestamp, MILLIS(1500)) && curAnalog > SOUND_BORDER) {
            state++;

        } 
        else if (state == WAIT_LAST_SILENCE && scr1_timer_higher(timestamp, MILLIS(1500))
            && scr1_timer_lower(timestamp, MILLIS(2000)) && curAnalog > SOUND_BORDER) {
            state = WAIT_FIRST;

        } 
        else if (state == WAIT_LAST_SILENCE && scr1_timer_higher(timestamp, MILLIS(2000))) {
            state = WAIT_FIRST;

            LED_PORT->OUTPUT ^= 1 << LED_PIN_NUM;
            windReady = 1;
        }
        else if (state == WAIT_SECOND && scr1_timer_higher(timestamp, MILLIS(2000))) {
            state = WAIT_FIRST;

        }
                             

        if (curAnalog > 1000)
            xprintf("ADC %d\r\n", curAnalog);

        lastAnalog = curAnalog;
    }
}

void EPIC_trap_handler() {
    if (EPIC->RAW_STATUS & (1 << EPIC_LINE_GPIO_IRQ_S) && GPIO_IRQ->INTERRUPT & (1 << MOVE_IRQ_LINE)) {

        LED_PORT->OUTPUT |= 1 << LED_PIN_NUM;

        GPIO_IRQ->CLEAR = 1 << MOVE_IRQ_LINE;
        EPIC->CLEAR     = 1 << EPIC_LINE_GPIO_IRQ_S;

    } 

    if (EPIC->RAW_STATUS & (1 << EPIC_LINE_TIMER32_0_S)) {
        HAL_USART_Print(&husart0, "TIMER1->", 0);

        dhtReady = 1;

        TIMER32_0->INT_CLEAR = TIMER32_INT_OVERFLOW_M;
        EPIC->CLEAR = EPIC_LINE_TIMER32_0_S;
    } else if (EPIC->RAW_STATUS & (1 << EPIC_LINE_TIMER32_2_S) && (TIMER32_2->INT_FLAGS & TIMER32_INT_OC_M(0))) {
        windReady ^= 1;
        HAL_USART_Print(&husart0, "SEcond timer\r\n", 0);

        LED_PORT->OUTPUT ^= 1 << LED_PIN_NUM;

        TIMER32_2->INT_CLEAR = TIMER32_INT_OC_M(0);
        EPIC->CLEAR          = 1 << EPIC_LINE_TIMER32_2_S;
    }
}