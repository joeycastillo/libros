#ifndef LIB_ADAFRUIT_IL0371
#define LIB_ADAFRUIT_IL0371

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define IL0371_PANEL_SETTING 0x00
#define IL0371_POWER_SETTING 0x01
#define IL0371_POWER_OFF 0x02
#define IL0371_POWER_OFF_SEQUENCE 0x03
#define IL0371_POWER_ON 0x04
#define IL0371_POWER_ON_MEASURE 0x05
#define IL0371_BOOSTER_SOFT_START 0x06
#define IL0371_DEEP_SLEEP 0x07
#define IL0371_DTM1 0x10
#define IL0371_DATA_STOP 0x11
#define IL0371_DISPLAY_REFRESH 0x12
#define IL0371_PDTM1 0x14
#define IL0371_PDTM2 0x15
#define IL0371_PDRF 0x16
#define IL0371_LUT1 0x20
#define IL0371_LUTWW 0x21
#define IL0371_LUTBW 0x22
#define IL0371_LUTWB 0x23
#define IL0371_LUTBB 0x24
#define IL0371_PLL 0x30
#define IL0371_CDI 0x50
#define IL0371_RESOLUTION 0x61
#define IL0371_VCM_DC_SETTING 0x82

/**************************************************************************/
/*!
    @brief  Class for interfacing with IL0371 EPD drivers
*/
/**************************************************************************/
class Adafruit_IL0371 : public Adafruit_EPD {
public:
#ifdef USE_EXTERNAL_SRAM
  Adafruit_IL0371(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                  int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                  int8_t BUSY = -1);
  Adafruit_IL0371(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                  int8_t SRCS, int8_t BUSY = -1);
#else
  Adafruit_IL0371(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                  int8_t RST, int8_t CS, int8_t BUSY = -1);
  Adafruit_IL0371(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                  int8_t BUSY = -1);
#endif

  void begin(bool reset = true);
  void powerUp();

  void drawPixel(int16_t x, int16_t y, uint16_t color);

  void display();
  void update();

  void clearBuffer();
  void clearDisplay();
};

#endif