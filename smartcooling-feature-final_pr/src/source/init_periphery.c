#include "init_periphery.h"
#include "wakeup.h"
#include "mik32_hal_usart.h"
#include "mik32_memory_map.h"
#include "mik32_hal_usart.h"
#include "scr1_timer.h"
#include "epic.h"
#include "gpio.h"
#include "gpio_irq.h"
#include "mik32_memory_map.h"
#include "mik32_hal_adc.h"
#include "pad_config.h"
#include "power_manager.h"
#include "scr1_timer.h"
#include "timer32.h"
#include "wakeup.h"
#include "mik32_hal_spi.h"
#include "utilities.h"

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

void USART_Init(USART_HandleTypeDef* husart, uint8_t uart, uint8_t frame_size,
                            uint8_t rx_inv, uint32_t baudrate) // for DHT11
{
    if (uart)
        husart->Instance = UART_1;
    else
        husart->Instance = UART_0;

    if (rx_inv)
        husart->rx_inversion = Enable;
    else
        husart->rx_inversion = Disable;

    switch (frame_size){
        case (7):
            husart->frame = Frame_7bit;
            break;
        case (9):
            husart->frame = Frame_9bit;
            break;
        case (8):
        default:
            husart->frame = Frame_8bit;
            break;
    }

    husart->baudrate = baudrate;


    husart->transmitting = Enable;
    husart->receiving = Enable;
    husart->parity_bit = Disable;
    husart->parity_bit_inversion = Disable;
    husart->bit_direction = LSB_First;
    husart->data_inversion = Disable;
    husart->tx_inversion = Disable;
    husart->swap = Disable;
    husart->lbm = Disable;
    husart->stop_bit = StopBit_1;
    husart->mode = Asynchronous_Mode;
    husart->xck_mode = XCK_Mode3;
    husart->last_byte_clock = Disable;
    husart->overwrite = Disable;
    husart->rts_mode = AlwaysEnable_mode;
    husart->dma_tx_request = Disable;
    husart->dma_rx_request = Disable;
    husart->channel_mode = Duplex_Mode;
    husart->tx_break_mode = Disable;
    husart->Interrupt.ctsie = Disable;
    husart->Interrupt.eie = Disable;
    husart->Interrupt.idleie = Disable;
    husart->Interrupt.lbdie = Disable;
    husart->Interrupt.peie = Disable;
    husart->Interrupt.rxneie = Disable;
    husart->Interrupt.tcie = Disable;
    husart->Interrupt.txeie = Disable;
    husart->Modem.rts = Disable; //out
    husart->Modem.cts = Disable; //in
    husart->Modem.dtr = Disable; //out
    husart->Modem.dcd = Disable; //in
    husart->Modem.dsr = Disable; //in
    husart->Modem.ri = Disable;  //in
    husart->Modem.ddis = Disable;//out
    HAL_USART_Init(husart);
}


void SPI1_Init(SPI_HandleTypeDef *spi, USART_HandleTypeDef* husart) {
    spi->Instance = SPI_1;
    spi->Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    spi->Init.BaudRateDiv = SPI_BAUDRATE_DIV64;
    spi->Init.ManualCS = SPI_MANUALCS_OFF;

    spi->Init.CLKPhase = SPI_PHASE_OFF;
    spi->Init.CLKPolarity = SPI_POLARITY_LOW;

    spi->Init.Decoder = SPI_DECODER_NONE;
    
    spi->Init.ChipSelect = SPI_CS_1;

    if (HAL_SPI_Init(spi) == HAL_OK)
    {
        HAL_USART_Print(husart, "SPI0 init OK\r\n", USART_TIMEOUT_DEFAULT);
    }
}


void GPIO_Init() {
    // Set up clocking for GPIO_0, GPIO_1, GPIO_2
    PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_0_M;
    PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_1_M;
    PM->CLK_APB_P_SET = PM_CLOCK_APB_P_GPIO_2_M;

    // On board LED on ACE-UNO
    PAD_CONFIG->PORT_2_CFG  = SET_TWO_BITS(PAD_CONFIG->PORT_2_CFG, LED_PIN_NUM, 0b00);
    PAD_CONFIG->PORT_2_DS   = SET_TWO_BITS(PAD_CONFIG->PORT_2_DS, LED_PIN_NUM, 0b00);
    PAD_CONFIG->PORT_2_PUPD = SET_TWO_BITS(PAD_CONFIG->PORT_2_PUPD, LED_PIN_NUM, 0b00);
    LED_PORT->DIRECTION_OUT = 1 << LED_PIN_NUM;

    PAD_CONFIG->PORT_0_CFG  = SET_TWO_BITS(PAD_CONFIG->PORT_0_CFG, RST_PIN_NUM, 0b00);
    PAD_CONFIG->PORT_0_CFG  = SET_TWO_BITS(PAD_CONFIG->PORT_0_CFG, DC_PIN_NUM, 0b00);

    DC_PORT->DIRECTION_OUT = 1 << DC_PIN_NUM;
    RST_PORT->DIRECTION_OUT = 1 << RST_PIN_NUM;

    // Configure DHT11 
    PAD_CONFIG->PORT_0_CFG  = SET_TWO_BITS(PAD_CONFIG->PORT_0_CFG, DHT_PIN_NUM, 0b00);

    // Configure move sensor
    PAD_CONFIG->PORT_0_CFG  = SET_TWO_BITS(PAD_CONFIG->PORT_0_CFG, MOVE_PIN_NUM, 0b00);
    MOVE_PORT->DIRECTION_IN = 1 << MOVE_PIN_NUM;

    // Configure fan
    PAD_CONFIG->PORT_0_CFG  = SET_TWO_BITS(PAD_CONFIG->PORT_0_CFG, WIND_PIN_NUM, 0b00);
    WIND_PORT->DIRECTION_OUT = 1 << WIND_PIN_NUM;

    // Configure ANALOG_PIN
    PAD_CONFIG->PORT_1_CFG  = SET_TWO_BITS(PAD_CONFIG->PORT_1_CFG, ANALOG_PIN_NUM, 0b11);  // ANALOG SIGNAL
    PAD_CONFIG->PORT_1_DS   = SET_TWO_BITS(PAD_CONFIG->PORT_1_DS, ANALOG_PIN_NUM, 0b00);
    PAD_CONFIG->PORT_1_PUPD = SET_TWO_BITS(PAD_CONFIG->PORT_1_PUPD, ANALOG_PIN_NUM, 0b00);
 
}

/* Configure ADC module 
    Note: Also look at GPIO_Init() to see that the PORT1.7 is configured as an analog func
*/
void ADC_Init() {
    PM->CLK_APB_P_SET = PM_CLOCK_APB_P_ANALOG_REGS_M;  

    // Mux channel 1 (port 1.7)
    ANALOG_REG->ADC_CONFIG &= ~(0b111 << ADC_CONFIG_SEL_S);
    ANALOG_REG->ADC_CONFIG |= (0b1 << ADC_CONFIG_SEL_S);

    // Setup SAH (0x0C - standart value), 0x3F = 0b111111
    ANALOG_REG->ADC_CONFIG &= ~(0x3F << ADC_CONFIG_SAH_TIME_WRITE_S);
    ANALOG_REG->ADC_CONFIG |= (0x0C << ADC_CONFIG_SAH_TIME_WRITE_S);

    /*
    // Standart values. You dont need to set up them if values are standart
    ANALOG_REG->ADC_CONFIG &= ~(1 << ADC_CONFIG_EXTEN_S);
    ANALOG_REG->ADC_CONFIG &= ~(1 << ADC_CONFIG_EXTPAD_EN_S);
    */

    // Enable and reset to start
    ANALOG_REG->ADC_CONFIG |= ADC_CONFIG_RN_M;
    ANALOG_REG->ADC_CONFIG |= ADC_CONFIG_EN_M;
}

/*
    // Also check in main() that EPIC is clocking
    */
void TIMER32_0_Init() {
    PM->CLK_APB_M_SET = PM_CLOCK_APB_M_TIMER32_0_M;
    TIMER32_0->ENABLE = 0;
    TIMER32_0->TOP = SYSTEM_FREQ_HZ * 5;  // 5sec
    TIMER32_0->PRESCALER = 0;
    TIMER32_0->CONTROL =
        TIMER32_CONTROL_MODE_UP_M | TIMER32_CONTROL_CLOCK_PRESCALER_M;
    TIMER32_0->INT_MASK = 0;
    TIMER32_0->INT_CLEAR = 0xFFFFFFFF;
    TIMER32_0->ENABLE = 1;

    // Turn on overflow interrupt 
    TIMER32_0->INT_MASK = TIMER32_INT_OVERFLOW_M;

    // Turn on EPIC interrupts from TIMER32_0
    EPIC->MASK_LEVEL_SET = 1 << (EPIC_LINE_TIMER32_0_S);
}

void TIMER32_2_Init() {
    PM->CLK_APB_P_SET = PM_CLOCK_APB_P_TIMER32_2_M;
    TIMER32_2->ENABLE = 0;
    TIMER32_2->TOP = SYSTEM_FREQ_HZ*10;  // 10sec
    TIMER32_2->PRESCALER = 0;
    TIMER32_2->CONTROL =
        TIMER32_CONTROL_MODE_UP_M | TIMER32_CONTROL_CLOCK_PRESCALER_M;
    TIMER32_2->INT_MASK = 0;
    TIMER32_2->INT_CLEAR = 0xFFFFFFFF;

    // Period
    TIMER32_2->CHANNELS[0].OCR = (SYSTEM_FREQ_HZ*10) - 1;
    TIMER32_2->CHANNELS[0].CNTRL =
            TIMER32_CH_CNTRL_MODE_COMPARE_M | TIMER32_CH_CNTRL_ENABLE_M; 


    TIMER32_2->INT_MASK = TIMER32_INT_OC_M(0);
    EPIC->MASK_LEVEL_SET = 1 << (EPIC_LINE_TIMER32_2_S);
    TIMER32_2->ENABLE = 1;
}
