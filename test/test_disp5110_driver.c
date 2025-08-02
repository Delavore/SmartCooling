#include "unity.h"
#include "mock_disp5110_wrapper.h"
// #include "bitmap.h"
#include "disp5110_driver.h"
#include "mock_delayForTest.h"

static SPI_HandleTypeDef testSpi;
const uint8_t numbers[][8]  = {
    {0x00,0x7e,0x81,0x81,0x81,0x81,0x7e,0x00},  // 0
    {0x00,0x00,0x04,0x86,0xff,0x80,0x00,0x00},  // 1
    {0x00,0x00,0xc6,0xe3,0xb3,0x9e,0x80,0x00},  // 2
    {0x00,0x00,0x91,0x91,0x91,0xff,0x00,0x00},  // 3
    {0x00,0x0f,0x08,0x08,0x08,0xff,0x00,0x00},  // 4
    {0x00,0x00,0x8f,0x89,0x89,0x89,0x71,0x00},  // 5
    {0x00,0x00,0x7c,0x8a,0x89,0x89,0x70,0x00},  // 6
    {0x00,0x01,0x01,0xf1,0x09,0x07,0x00,0x00},  // 7
    {0x00,0xf6,0x89,0x89,0x89,0xf6,0x00,0x00},  // 8
    {0x00,0x0e,0x91,0x91,0x91,0x7e,0x00,0x00}   // 9
};

const uint8_t alphabet[][8] = {
    {0x10,0xfe,0x11,0x11,0x11,0xfe,0x10,0x00},  // A 
    {0x00,0xff,0x89,0x89,0x89,0x8e,0x70,0x00},  // B
    {0x00,0x00,0x7e,0x81,0x81,0x81,0x42,0x00},  // C
    {0x00,0x00,0xff,0x81,0x81,0x81,0x7e,0x00},  // D 
    {0x00,0x00,0xff,0x91,0x91,0x91,0x91,0x00},  // E
    {0x00,0x00,0xff,0x11,0x11,0x11,0x01,0x00},  // F
    {0x00,0x3e,0xc1,0x81,0x91,0x91,0x72,0x00},  // G
    {0x00,0xff,0x08,0x08,0x08,0xff,0x00,0x00},  // H
    {0x00,0x00,0x81,0xff,0x81,0x00,0x00,0x00},  // I
    {0x00,0x00,0x81,0x81,0x41,0x3f,0x00,0x00},  // J
    {0x00,0x00,0xff,0x08,0x14,0x22,0xc1,0x00},  // K
    {0x00,0x00,0xff,0x80,0x80,0x80,0x00,0x00},  // L
    {0x00,0xff,0x04,0x08,0x10,0x08,0x04,0xff},  // M
    {0x00,0xff,0x03,0x0c,0x38,0xc0,0xff,0x00},  // N
    {0x00,0x7e,0x81,0x81,0x81,0x81,0x7e,0x00},  // O
    {0x00,0xff,0x21,0x21,0x21,0x21,0x1e,0x00},  // P
    {0x00,0x7e,0x81,0x81,0x21,0x01,0x3e,0x00},  // Q
    {0x00,0xff,0x11,0x11,0x31,0xce,0x80,0x00},  // R
    {0x00,0x8e,0x9b,0x91,0x91,0x91,0x71,0x00},  // S
    {0x01,0x01,0x01,0xff,0x01,0x01,0x01,0x00},  // T
    {0x00,0x3f,0x40,0x80,0x80,0x40,0x3f,0x00},  // U
    {0x03,0x0c,0x30,0xc0,0xc0,0x30,0x0c,0x03},  // V
    {0x01,0x7e,0x80,0x60,0x60,0x80,0x7e,0x01},  // W
    {0x00,0xc1,0x22,0x14,0x18,0x14,0x22,0xc1},  // X
    {0x00,0x01,0x02,0x0c,0xf8,0x0c,0x02,0x01},  // Y
    {0x00,0xc1,0xe1,0xb1,0x99,0x8d,0x87,0x00}   // Z
};

const uint8_t colon[8] = {0x00,0x00,0x00,0xc3,0xc3,0x00,0x00,0x00};  // :
uint8_t animation [][4] = {{0, 0, 0, 0}, {0, 0, 0, 0} }; 

void setUp() {
}
void tearDown() {}

void test_DISP_Init_CallWrappers() {
    // Arrange and set up Assert
    uint8_t commands[] = { 0x21, 0xBF, 0x04, 0x14, 0x20, 0x0C };
    uint8_t receive_stub[sizeof(commands)];

    DISP_RstRun_Expect();
    DISP_SendCommand_Expect(&testSpi, commands, receive_stub, sizeof(commands));
    DISP_SendCommand_IgnoreArg_receive_stub();

    // Act
    DISP_Init(&testSpi);
}

// Cant call TEST_CASE with string arg
// TEST_CASE("without" 10, 4)
// TEST_CASE("without", 0, 0);
// TEST_CASE(":::::", 4, 3)
void test_DISP_DrawTextCol() {
    // Arrange
    char *word = "hello:";
    uint8_t xCoord = 10;
    uint8_t yCoord = 5;

    uint8_t xTransmit[1] = {0x80 | xCoord};
    uint8_t yTransmit[1] = {0x40 | yCoord};

    uint8_t receive[8];

    // Set expectations, Assert
    DISP_SendCommand_Expect(&testSpi, xTransmit, receive, 1);  
    DISP_SendCommand_IgnoreArg_receive_stub();

    DISP_SendCommand_Expect(&testSpi, yTransmit, receive, 1); 
    DISP_SendCommand_IgnoreArg_receive_stub();

    int i = 0;
    while (word[i]) {
        if (word[i] != ':') {
            DISP_SendData_Expect(&testSpi, alphabet[word[i] - 'a'], receive, 8); 
            DISP_SendData_IgnoreArg_receive_stub();
        } else {
            DISP_SendData_Expect(&testSpi, colon, receive, 8);  
            DISP_SendData_IgnoreArg_receive_stub();
        }
        i++;

    }


    // Act
    DISP_DrawText(&testSpi, word, xCoord, yCoord);
}

void test_DISP_DrawTextOnlyCol() {
    // Arrange
    char *word = ":::::";
    uint8_t xCoord = 0;
    uint8_t yCoord = 4;

    uint8_t xTransmit[1] = {0x80 | xCoord};
    uint8_t yTransmit[1] = {0x40 | yCoord};

    uint8_t receive[8];

    // Set expectations, Assert
    DISP_SendCommand_Expect(&testSpi, xTransmit, receive, 1);  
    DISP_SendCommand_IgnoreArg_receive_stub();

    DISP_SendCommand_Expect(&testSpi, yTransmit, receive, 1); 
    DISP_SendCommand_IgnoreArg_receive_stub();

    int i = 0;
    while (word[i]) {
        if (word[i] != ':') {
            DISP_SendData_Expect(&testSpi, alphabet[word[i] - 'a'], receive, 8); 
            DISP_SendData_IgnoreArg_receive_stub();
        } else {
            DISP_SendData_Expect(&testSpi, colon, receive, 8);  
            DISP_SendData_IgnoreArg_receive_stub();
        }
        i++;

    }

    // Act
    DISP_DrawText(&testSpi, word, xCoord, yCoord);
}

void test_DISP_DrawTextOnlyText() {
    // Arrange
    char *word = "dist";
    uint8_t xCoord = 0;
    uint8_t yCoord = 4;

    uint8_t xTransmit[1] = {0x80 | xCoord};
    uint8_t yTransmit[1] = {0x40 | yCoord};

    uint8_t receive[8];

    // Set expectations, Assert
    DISP_SendCommand_Expect(&testSpi, xTransmit, receive, 1);  
    DISP_SendCommand_IgnoreArg_receive_stub();

    DISP_SendCommand_Expect(&testSpi, yTransmit, receive, 1); 
    DISP_SendCommand_IgnoreArg_receive_stub();

    int i = 0;
    while (word[i]) {
        if (word[i] != ':') {
            DISP_SendData_Expect(&testSpi, alphabet[word[i] - 'a'], receive, 8); 
            DISP_SendData_IgnoreArg_receive_stub();
        } else {
            DISP_SendData_Expect(&testSpi, colon, receive, 8);  
            DISP_SendData_IgnoreArg_receive_stub();
        }
        i++;

    }

    // Act
    DISP_DrawText(&testSpi, word, xCoord, yCoord);
}

void test_DISP_DrawIntEcpectedFuncs() {
    // Arrange and setUp Asserts
    uint8_t xCoord = 0;
    uint8_t yCoord = 4;
    uint8_t number = 223;

    uint8_t xTransmit[1] = {0x80 | xCoord};  
    uint8_t yTransmit[1] = {0x40 | yCoord};
    uint8_t receive [8];  

    DISP_SendCommand_Expect(&testSpi, xTransmit, receive, 1);
    DISP_SendCommand_IgnoreArg_receive_stub();
    
    DISP_SendCommand_Expect(&testSpi, yTransmit, receive, 1);
    DISP_SendCommand_IgnoreArg_receive_stub();
    
    uint8_t buf[3] = {3, 2, 2};

    for (int i = 2; i >= 0; i--) {
        DISP_SendData_Expect(&testSpi, *(numbers + buf[i]), receive, 8);
        DISP_SendData_IgnoreArg_receive_stub();
    }

    // Act
    DISP_DrawInt(&testSpi, number, xCoord, yCoord);
}


void test_DISP_DrawIntExpectedFuncs2() {
    // Arrange and set up Asserts
    uint8_t xCoord = 0;
    uint8_t yCoord = 0;
    uint8_t number = 1;

    uint8_t xTransmit[1] = {0x80 | xCoord};  
    uint8_t yTransmit[1] = {0x40 | yCoord};
    uint8_t receive [8];  

    DISP_SendCommand_Expect(&testSpi, xTransmit, receive, 1);
    DISP_SendCommand_IgnoreArg_receive_stub();
    
    DISP_SendCommand_Expect(&testSpi, yTransmit, receive, 1);
    DISP_SendCommand_IgnoreArg_receive_stub();
    
    uint8_t buf[1] = {1};

    for (int i = 0; i >= 0; i--) {
        DISP_SendData_Expect(&testSpi, *(numbers + buf[i]), receive, 8);
        DISP_SendData_IgnoreArg_receive_stub();

    }
    
    // Act
    DISP_DrawInt(&testSpi, number, xCoord, yCoord);
}

TEST_CASE(0)
TEST_CASE(4)
void test_DISP_ClearLineExpected(uint8_t line) {
//     uint8_t line = 2;
    uint8_t xTransmit[1] = { 0x80 };  
    uint8_t yTransmit[1] = { 0x40 | line };  
    uint8_t receive [1];  

    DISP_SendCommand_Expect(&testSpi, xTransmit, receive, 1);

    DISP_SendCommand_Expect(&testSpi, yTransmit, receive, 1);

    uint8_t zero_buffer[84] = {0};  // all line
    uint8_t receive_stub[84];

    DISP_SendData_Expect(&testSpi, zero_buffer, receive_stub, 84);
    DISP_SendData_IgnoreArg_receive_stub();

    DISP_ClearLine(&testSpi, line);
}