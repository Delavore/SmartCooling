#ifndef INIT_PERIPHERY
#define INIT_PERIPHERY
#include "mik32_hal_spi.h"
#include "mik32_hal_usart.h"

// D3 on ACE-UNO
#define RST_PIN_NUM (0)
#define RST_PORT (GPIO_0)

// D7 on ACE-UNO
#define WIND_PIN_NUM (2)
#define WIND_PORT (GPIO_0)

// D2 on ACE-UNO
#define DC_PIN_NUM (10)
#define DC_PORT (GPIO_0)

// D5 on ACE-UNO
#define MOVE_PIN_NUM (1)
#define MOVE_PORT (GPIO_0)
#define MOVE_IRQ_LINE (5)
#define MOVE_IRQ_MUX (5)

// Onboard LED for debugging
#define LED_PIN_NUM (7)
#define LED_PORT (GPIO_2)

// D4 on ACE-UNO
#define DHT_PIN_NUM (8)
#define DHT_PORT (GPIO_0)

// A1/D15 on ACE-UNO
#define ANALOG_PIN_NUM (7)
#define ANALOG_PORT (GPIO_1)

void TIMER32_0_Init();
void TIMER32_2_Init();

void GPIO_Init();

void ADC_Init();

void SPI1_Init(SPI_HandleTypeDef *spi, USART_HandleTypeDef* husart);

void USART_Init(USART_HandleTypeDef* husart, uint8_t uart, uint8_t frame_size,
                            uint8_t rx_inv, uint32_t baudrate);

void SystemClock_Config(void);

#endif /* INIT_PERIPHERY */