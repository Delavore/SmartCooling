#ifndef WRAPPER5110
#define WRAPPER5110
#include <stdint.h>
#include "mik32_hal_spi.h"

void DISP_SendCommand(SPI_HandleTypeDef *hspi, uint8_t* commands,
                            uint8_t* receive_stub, uint32_t size);

void DISP_SendData(SPI_HandleTypeDef *hspi, uint8_t* data,
                            uint8_t* receive_stub, uint32_t size);

void DISP_RstRun();

#endif /* WRAPPER5110 */