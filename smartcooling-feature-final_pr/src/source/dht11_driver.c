#include "dht11_driver.h"
#include "gpio.h"
#include "init_periphery.h"
#include "utilities.h"


/* Get data from DHT11
    Note: There is all information inside datasheet on DHT11
    size of info: 5
*/
void DHT_Get(int8_t *info) {
    
    int8_t flagError1 = 0;
    int8_t flagError2 = 0;

    DHT_PORT->DIRECTION_OUT = 1 << DHT_PIN_NUM;

    // Lower the line
    DHT_PORT->OUTPUT &= ~(1 << DHT_PIN_NUM);
    delay(20000);  // min 18ms 

    // Raise the line
    DHT_PORT->OUTPUT |= 1 << DHT_PIN_NUM;

    // Now we listening
    DHT_PORT->DIRECTION_IN = 1 << DHT_PIN_NUM;

    // First lower control signal from DHT11
    delay(40);
    if ((DHT_PORT->STATE & (1 << DHT_PIN_NUM)))
       flagError1++; 

    // Second raised control signal from DHT11
    delay(80);
    if (~(DHT_PORT->STATE & (1 << DHT_PIN_NUM)))
        flagError2++;
    
    /* Uncomment this line to see that the sensor is ready to send a packege
    if (flagError1 == 1 || flagError2 == 1)
        LED_PORT->OUTPUT |= 1 << LED_PIN_NUM; */

    while ((DHT_PORT->STATE & (1 << DHT_PIN_NUM)))
    ;

    // Get data
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 8; i++) {
            while (!(DHT_PORT->STATE & (1 << DHT_PIN_NUM))) 
            ;
            delay(40);
            info[j] <<= 1;
            if ((DHT_PORT->STATE & (1 << DHT_PIN_NUM))) {
                info[j]++;
                while ((DHT_PORT->STATE & (1 << DHT_PIN_NUM))) 
                ;
            }
        }
    }
}

/* Warning: this function doesnt work in flash, and tx on mik32 doest support
    open drain line (line with pull up resistor)
    This function get data from DHT11 using USART.
    You should set up USART like this: baudrate 115200, inverted rx, 7bit data
    size of receive: 44, size of info: 5
*/
void DHT_GetUsart(USART_HandleTypeDef* husart, char *receive, uint8_t *info) {

        /* Get bits from DHT11 */

        for (int i =0; i < 44; i++)
            HAL_USART_Transmit(husart, 0x0, USART_TIMEOUT_DEFAULT);

        for (int i = 0; i < 44; i++)
            HAL_USART_Receive(husart, receive + i, 20000);

        /* Parsing bits */

        // first 3 bytes (bits) are controls and last one too, so dont take them
        int k = -1; 
        for (int i = 3; i < 42; i++) {
            if ((i - 3) % 8 == 0)
                k++;

            info[k] <<= 1;
            if (receive[i] == 0)
                info[k]++;
        }

        // Its okey, transaction closed
        LED_PORT->OUTPUT |= (1 << LED_PIN_NUM);
}