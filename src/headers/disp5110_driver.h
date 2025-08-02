#ifndef DISP5110_DRIVER
#define DISP5110_DRIVER
#include "mik32_hal_spi.h"

#define FRAME_COUNT (20)  // animation[FRAME_COUNT][504]

#define DISP_TIMEOUT (20000)  // microseconds

#define MAX_VERT_LINES (6)
#define MAX_HORIZONTAL_PIXELS (84)

void DISP_PlayAnim(SPI_HandleTypeDef *hspi);
void DISP_ClearLine(SPI_HandleTypeDef *hspi, uint8_t line);
void DISP_Init(SPI_HandleTypeDef *hspi);
void DISP_DrawInt(SPI_HandleTypeDef *hspi, uint8_t number, uint8_t xCoord, uint8_t yCoord);
void DISP_DrawText(SPI_HandleTypeDef *hspi, char *word, uint8_t xCoord, uint8_t yCoord);

inline void DISP_Clear(SPI_HandleTypeDef *hspi);

inline void DISP_SendCommand(SPI_HandleTypeDef *hspi, uint8_t* commands,
                            uint8_t* receive_stub, uint32_t size);

inline void DISP_SendData(SPI_HandleTypeDef *hspi, uint8_t* data,
                            uint8_t* receive_stub, uint32_t size);


#endif /*DISP5110_DRIVER*/
