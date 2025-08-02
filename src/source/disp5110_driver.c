
#include "utilities.h"
#include "disp5110_driver.h"
#include "init_periphery.h"
#include "mik32_hal_spi.h"
#include "bitmap.h"
#include "disp5110_wrapper.h"



/* 
    Initialization of display. After it you can send data to display on drawing.
    After initialization you will see grey screen because it is lcd display.
    Dont forget to initialize SPI interface and configure DC and RST on output
*/
void DISP_Init(SPI_HandleTypeDef *hspi) {

    DISP_RstRun();

    // Look at datasheet on lcd 5110
    uint8_t command[] = {
        0x21, // 21 Function set: Power down control, entry mode, extended instruction set (H=1)
        0xBF, // Set VOP: Contrast control
        0x04, // Temperature coefficient
        0x14, // Bias system 
        0x20, // 20 Function set: Power down control, entry mode, basic instruction set (H=0)
        0x0C  // Display control: Normal mode
    };
    uint8_t receive_stub[sizeof(command)];

    DISP_SendCommand(hspi, command, receive_stub, sizeof(command));
}

#ifndef TEST
/* Play the animation on the display */
void DISP_PlayAnim(SPI_HandleTypeDef *hspi) {

    uint8_t transmit[1];  // used for sending coordinates x and y
    uint8_t receive [1];  // not used, because lcd has no miso

    // Upper left corner
    int8_t xCoord = 0;  
    int8_t yCoord = 0;

    for (int8_t frame = 0; frame < FRAME_COUNT; frame++) {
        int16_t k = 0;

        // Draw loop
        for (int8_t i = 0; i < MAX_VERT_LINES; i++) {
            for (int8_t j = 0; j < MAX_HORIZONTAL_PIXELS; j++) {
                
                // Set up x position
                transmit[0] = 0x80 | (xCoord + j);
                DISP_SendCommand(hspi, transmit, receive, 1);
                
                // Set up y position
                transmit[0] = 0x40 | (yCoord + i);
                DISP_SendCommand(hspi, transmit, receive, 1);

                // Move byte to rendering on disp
                DISP_SendData(hspi, *(animation + frame) + k, receive, 1);

                k++;
          }
      }
      
      // Delay for first and last frame
      if (frame == 0 || frame == FRAME_COUNT - 1) {
        delay(3000000);
      }
  }

}
#endif

/* Get str and print it. Working only on separated line with bitmap n*8 */
void DISP_DrawText(SPI_HandleTypeDef *hspi, char *word, uint8_t xCoord, uint8_t yCoord) {
    uint8_t transmit[1];  // used for sending coordinates x and y
    uint8_t receive [8];  // not used, because lcd has no miso

    // Set up x position
    transmit[0] = 0x80 | xCoord;
    DISP_SendCommand(hspi, transmit, receive, 1);
    
    // Set up y position
    transmit[0] = 0x40 | yCoord;
    DISP_SendCommand(hspi, transmit, receive, 1);
    

    int16_t i = 0;
    while (word[i]) {
        if (word[i] == ':')
            DISP_SendData(hspi, colon, receive, 8);
        else 
            DISP_SendData(hspi, *(alphabet + (word[i] - 'a')), receive, 8);

        i++;
    }
} 


/* Get int and print it. Working only on separated line with bitmap n*8 */
void DISP_DrawInt(SPI_HandleTypeDef *hspi, uint8_t number, uint8_t xCoord, uint8_t yCoord) {
    uint8_t transmit[1];  // used for sending coordinates x and y
    uint8_t receive [8];  // not used, because lcd has no miso

    // Set up x position
    transmit[0] = 0x80 | xCoord;
    DISP_SendCommand(hspi, transmit, receive, 1);
    
    // Set up y position
    transmit[0] = 0x40 | yCoord;
    DISP_SendCommand(hspi, transmit, receive, 1);
    
    uint8_t buf[10];  // for our revert number

    int8_t i = 0;
    if (number == 0)
        buf[i++] = 0;

    while (number) {
        uint8_t digit = number % 10;
        number /= 10;
        
        buf[i] = digit;
        i++;
    }

        // Move byte to rendering on disp
        for (i--; i >= 0; i--) 
            DISP_SendData(hspi, *(numbers + buf[i]), receive, 8);
} 

#ifndef TEST
/* Send zeros on whole screen to clear it */
void DISP_Clear(SPI_HandleTypeDef *hspi) {
    for (uint8_t y = 0; y < MAX_VERT_LINES; y++)
        DISP_ClearLine(hspi, y);
}
#endif

/* Send zeros on current line to clear it. Min line = 0, max line = 5 */
void DISP_ClearLine(SPI_HandleTypeDef *hspi, uint8_t line) {

        uint8_t transmit[1];  // used for sending coordinates x and y
        uint8_t receive [1];  // not used, because lcd has no miso

        // X coord always start at 0
        transmit[0] = 0x80;
        DISP_SendCommand(hspi, transmit, receive, 1);

        // Y coord equals to our line
        transmit[0] = 0x40 | line; 
        DISP_SendCommand(hspi, transmit, receive, 1);

        uint8_t zero_buffer[84] = {0};  // all line
        uint8_t receive_stub[84];

        DISP_SendData(hspi, zero_buffer, receive_stub, 84);
}