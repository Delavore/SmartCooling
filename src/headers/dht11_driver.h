#ifndef DHT11_DRIVER
#define DHT11_DRIVER
#include <stdint.h>
#include "mik32_hal_usart.h"

// Transfer func to ram for better performance
void DHT_Get(int8_t *) __attribute__((section(".ram_func.special_DHT_Init"))); 

// Transfer func to ram for better performance
void DHT_GetUsart(USART_HandleTypeDef* husart, char *, uint8_t *) __attribute__((section(".ram_func.special_DHT_Usart"))); 

#endif /* DHT11_DRIVER */