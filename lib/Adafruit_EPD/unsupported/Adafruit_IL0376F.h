#ifndef LIB_ADAFRUIT_IL0376F
#define LIB_ADAFRUIT_IL0376F

#include "Adafruit_EPD.h"
#include <Arduino.h>

#if defined(IL0376F_200_200)
#define EPD_LCDWIDTH 200
#define EPD_LCDHEIGHT 200
#define EPD_BUFSIZE 10000
#define EPD_REDBUFSIZE 5000
#endif

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define IL0376F_PANEL_SETTING 0x00
#define IL0376F_POWER_SETTING 0x01
#define IL0376F_POWER_OFF 0x02
#define IL0376F_POWER_OFF_SEQUENCE 0x03
#define IL0376F_POWER_ON 0x04
#define IL0376F_POWER_ON_MEASURE 0x05
#define IL0376F_BOOSTER_SOFT_START 0x06
#define IL0376F_DTM1 0x10
#define IL0376F_DATA_STOP 0x11
#define IL0376F_DISPLAY_REFRESH 0x12
#define IL0376F_DTM2 0x13
#define IL0376F_VCOM1_LUT 0x20
#define IL0376F_WHITE_LUT 0x21
#define IL0376F_BLACK_LUT 0x22
#define IL0376F_GRAY1_LUT 0x23
#define IL0376F_GRAY2_LUT 0x24
#define IL0376F_VCOM2_LUT 0x25
#define IL0376F_RED0_LUT 0x26
#define IL0376F_RED1_LUT 0x27
#define IL0376F_PLL 0x30
#define IL0376F_CDI 0x50
#define IL0376F_RESOLUTION 0x61
#define IL0376F_VCM_DC_SETTING 0x82

/**************************************************************************/
/*!
    @brief  Class for interfacing with IL0376F EPD drivers
*/
/**************************************************************************/
class Adafruit_IL0376F : public Adafruit_EPD {
public:
#ifdef USE_EXTERNAL_SRAM
  Adafruit_IL0376F(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                   int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                   int8_t BUSY = -1);
  Adafruit_IL0376F(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                   int8_t SRCS, int8_t BUSY = -1);
#else
  Adafruit_IL0376F(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                   int8_t RST, int8_t CS, int8_t BUSY = -1);
  Adafruit_IL0376F(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                   int8_t BUSY = -1);
#endif

  void begin(bool reset = true);
  void powerUp();

  void drawPixel(int16_t x, int16_t y, uint16_t color);

  void display();
  void update();

  void clearBuffer();
  void clearDisplay();

protected:
  void busy_wait();
};

#endif