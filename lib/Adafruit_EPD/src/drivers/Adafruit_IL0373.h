#ifndef LIB_ADAFRUIT_IL0373
#define LIB_ADAFRUIT_IL0373

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define IL0373_PANEL_SETTING 0x00
#define IL0373_POWER_SETTING 0x01
#define IL0373_POWER_OFF 0x02
#define IL0373_POWER_OFF_SEQUENCE 0x03
#define IL0373_POWER_ON 0x04
#define IL0373_POWER_ON_MEASURE 0x05
#define IL0373_BOOSTER_SOFT_START 0x06
#define IL0373_DEEP_SLEEP 0x07
#define IL0373_DTM1 0x10
#define IL0373_DATA_STOP 0x11
#define IL0373_DISPLAY_REFRESH 0x12
#define IL0373_DTM2 0x13
#define IL0373_PDTM1 0x14
#define IL0373_PDTM2 0x15
#define IL0373_PDRF 0x16
#define IL0373_LUT1 0x20
#define IL0373_LUTWW 0x21
#define IL0373_LUTBW 0x22
#define IL0373_LUTWB 0x23
#define IL0373_LUTBB 0x24
#define IL0373_PLL 0x30
#define IL0373_CDI 0x50
#define IL0373_RESOLUTION 0x61
#define IL0373_VCM_DC_SETTING 0x82
#define IL0373_PARTIAL_WINDOW 0x90
#define IL0373_PARTIAL_ENTER 0x91
#define IL0373_PARTIAL_EXIT 0x92

#define EPD_RAM_BW IL0373_DTM1
#define EPD_RAM_RED IL0373_DTM2

/**************************************************************************/
/*!
    @brief  Class for interfacing with IL0373 EPD drivers
*/
/**************************************************************************/
class Adafruit_IL0373 : public Adafruit_EPD {
private:
public:
  Adafruit_IL0373(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                  int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                  int8_t BUSY = -1);
  Adafruit_IL0373(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                  int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void powerDown();
  void update();
  void displayPartial(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();
};

#endif
