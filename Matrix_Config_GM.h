#pragma once
// GM Matrix Daughterboard Config
// Nothing to edit in here

#include <SD.h>
#include <SPI.h>

// Set Panel Width/Height if not using a Constants.h file
// to set display WIDTH/HEIGHT
#if !defined(M_WIDTH)
#define M_WIDTH 64
#endif
#if !defined(M_HEIGHT)
#define M_HEIGHT 32
#endif

// RPi, Pico / PicoW
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_W)

#define matrix_CLK 12
#define matrix_OE 14
#define matrix_LAT 13
#define matrix_A 2
#define matrix_B 3
#define matrix_C 4
#define matrix_D 5

#define matrix_R1 6
#define matrix_R2 9
#define matrix_G1 7
#define matrix_G2 10
#define matrix_B1 8
#define matrix_B2 11

    // Set default SPI Pins (SD Card)

#define SPI0_MISO   16
#define SPI0_MOSI   19
#define SPI0_CLK    18
#define SDCARD_CS   17

    // Reset default I2C Pins
#define I2C_SDA 20
#define I2C_SCK 21


// ESP32 Pinout
#elif defined(ARDUINO_ARCH_ESP32)
#define matrix_CLK 15
#define matrix_OE 33
#define matrix_LAT 32
#define matrix_A 12
#define matrix_B 16
#define matrix_C 17
#define matrix_D 18

#define matrix_R1 25
#define matrix_R2 21
#define matrix_G1 27
#define matrix_G2 23
#define matrix_B1 26
#define matrix_B2 22
    
    // Set default SPI Pins

#define SPI0_MISO 2
#define SPI0_MOSI 4
#define SPI0_CLK 5
#define SDCARD_CS 13

    // Reset default I2C Pins
#define I2C_SDA 20
#define I2C_SCK 21

#endif
// ESP32 Pinout V2
// #elif defined(ARDUINO_ARCH_ESP32)
// #define matrix_CLK 17
// #define matrix_OE 16
// #define matrix_LAT 15
// #define matrix_A 15
// #define matrix_B 2
// #define matrix_C 0
// #define matrix_D 4

// #define matrix_R1 12
// #define matrix_R2 13
// #define matrix_G1 14
// #define matrix_G2 25
// #define matrix_B1 26
// #define matrix_B2 27
    
//     // Set default SPI Pins

// #define SPI0_MISO 19
// #define SPI0_MOSI 23
// #define SPI0_CLK 18
// #define SDCARD_CS 5

//     // Reset default I2C Pins
// #define I2C_SDA 21
// #define I2C_SCK 22

// #endif

uint16_t Color333(uint8_t r, uint8_t g, uint8_t b)
{
    // RRRrrGGGgggBBBbb
    return ((r & 0x7) << 13) | ((r & 0x6) << 10) | ((g & 0x7) << 8) |
           ((g & 0x7) << 5) | ((b & 0x7) << 2) | ((b & 0x6) >> 1);
}

// #define bitDepth 6

uint8_t rgbPins[] = {matrix_R1, matrix_G1, matrix_B1, matrix_R2, matrix_G2, matrix_B2};
uint8_t addrPins[] = {matrix_A, matrix_B, matrix_C, matrix_D};
uint8_t clockPin = matrix_CLK;
uint8_t latchPin = matrix_LAT;
uint8_t oePin = matrix_OE;
uint8_t scanMode = (M_HEIGHT == 64) ? -2 : -1;
// standard colors
uint16_t myRED = Color333(2, 0, 0);
uint16_t myGREEN = Color333(0, 2, 0);
uint16_t myBLUE = Color333(0, 0, 2);
uint16_t myWHITE = Color333(2, 2, 2);
uint16_t myYELLOW = Color333(2, 2, 0);
uint16_t myCYAN = Color333(0, 2, 2);
uint16_t myMAGENTA = Color333(2, 0, 2);
uint16_t myShadow = Color333(1, 0, 2);
uint16_t myROSE = Color333(2, 0, 1);
uint16_t myBLACK = Color333(0, 0, 0);

#if defined(USESDCARD)
    if (!SD.begin(_CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
#endif