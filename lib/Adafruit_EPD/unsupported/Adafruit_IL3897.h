#ifndef LIB_ADAFRUIT_IL3897
#define LIB_ADAFRUIT_IL3897

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10

#define IL3897_DRIVER_OUT_CONTROL 0x01
#define IL3897_GATE_DRIVE_VOLTAGE 0x03
#define IL3897_GATE_SOURCE_VOLTAGE 0x04
#define IL3897_GATE_SCAN_START 0x0F
#define IL3897_DEEP_SLEEP 0x10
#define IL3897_DATA_ENTRY_MODE 0x11
#define IL3897_SOFT_RESET 0x12
#define IL3897_MASTER_ACTIVATE 0x20
#define IL3897_DISPLAY_UPDATE_CONTROL1 0x21
#define IL3897_DISPLAY_UPDATE_CONTROL2 0x22
#define IL3897_DISPLAY_WRITE_RAM 0x24
#define IL3897_VCOM_SENSE 0x28
#define IL3897_VCOM_DURATION 0x29
#define IL3897_VCOM_OTP 0x2A
#define IL3897_VCOM_SET 0x2C
#define IL3897_STATUS_READ 0x2F
#define IL3897_WRITE_LUT 0x32
#define IL3897_DUMMY_LINE 0x3A
#define IL3897_SET_GATE_WIDTH 0x3B
#define IL3897_BORDER_WAVEFORM 0x3C
#define IL3897_SET_RAM_X 0x44
#define IL3897_SET_RAM_Y 0x45
#define IL3897_SET_RAM_XADDR 0x4E
#define IL3897_SET_RAM_YADDR 0x4F
#define IL3897_SET_ANALOG_BLOCK 0x74
#define IL3897_SET_DIGITAL_BLOCK 0x7E

/**************************************************************************/
/*!
    @brief  Class for interfacing with IL0373 EPD drivers
*/
/**************************************************************************/
class Adafruit_IL3897 : public Adafruit_EPD {
public:
#ifdef USE_EXTERNAL_SRAM
  Adafruit_IL3897(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                  int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                  int8_t BUSY = -1);
  Adafruit_IL3897(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                  int8_t SRCS, int8_t BUSY = -1);
#else
  Adafruit_IL3897(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                  int8_t RST, int8_t CS, int8_t BUSY = -1);
  Adafruit_IL3897(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                  int8_t BUSY = -1);
#endif

  void begin(bool reset = true);
  void powerUp();

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void setRAMArea(uint8_t xstart, uint8_t xend, uint16_t ystart, uint16_t yend);
  void setRAMPointer(uint8_t addrX, uint16_t addrY);

  void display();
  void update(bool partial = false);

  void deepSleep(void);

  void clearBuffer();
  void clearDisplay();

protected:
  void busy_wait();
};

#endif
