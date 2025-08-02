#include "disp5110_wrapper.h"
#include "utilities.h"
#include "init_periphery.h"
#include "disp5110_driver.h"

/* Wrapper for sending commands */
void DISP_SendCommand(SPI_HandleTypeDef *hspi, uint8_t* commands,
                            uint8_t* receive_stub, uint32_t size) {

    DC_PORT->OUTPUT &= ~(1 << DC_PIN_NUM);  // DC = 0, command
    HAL_SPI_Exchange(hspi, commands, receive_stub, size, DISP_TIMEOUT);
}

/* Wrapper for sending data */
void DISP_SendData(SPI_HandleTypeDef *hspi, uint8_t* data,
                            uint8_t* receive_stub, uint32_t size) {

    DC_PORT->OUTPUT |= 1 << DC_PIN_NUM;  // DC = 1, data
    HAL_SPI_Exchange(hspi, data, receive_stub, size, DISP_TIMEOUT);
}

/* Wrapper to pull down RST to zero */
void DISP_RstRun() {
    RST_PORT->OUTPUT &= ~(1 << RST_PIN_NUM);
    delay(1000000);  // We need to wait some time
    RST_PORT->OUTPUT |= 1 << RST_PIN_NUM;
}