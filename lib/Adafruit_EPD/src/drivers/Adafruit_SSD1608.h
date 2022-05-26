#ifndef LIB_ADAFRUIT_SSD1608
#define LIB_ADAFRUIT_SSD1608

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10

#define SSD1608_DRIVER_CONTROL 0x01
#define SSD1608_GATE_VOLTAGE 0x03
#define SSD1608_SOURCE_VOLTAGE 0x04
#define SSD1608_DISPLAY_CONTROL 0x07
#define SSD1608_NON_OVERLAP 0x0B
#define SSD1608_BOOSTER_SOFT_START 0x0C
#define SSD1608_GATE_SCAN_START 0x0F
#define SSD1608_DEEP_SLEEP 0x10
#define SSD1608_DATA_MODE 0x11
#define SSD1608_SW_RESET 0x12
#define SSD1608_TEMP_WRITE 0x1A
#define SSD1608_TEMP_READ 0x1B
#define SSD1608_TEMP_CONTROL 0x1C
#define SSD1608_TEMP_LOAD 0x1D
#define SSD1608_MASTER_ACTIVATE 0x20
#define SSD1608_DISP_CTRL1 0x21
#define SSD1608_DISP_CTRL2 0x22
#define SSD1608_WRITE_RAM 0x24
#define SSD1608_READ_RAM 0x25
#define SSD1608_VCOM_SENSE 0x28
#define SSD1608_VCOM_DURATION 0x29
#define SSD1608_WRITE_VCOM 0x2C
#define SSD1608_READ_OTP 0x2D
#define SSD1608_WRITE_LUT 0x32
#define SSD1608_WRITE_DUMMY 0x3A
#define SSD1608_WRITE_GATELINE 0x3B
#define SSD1608_WRITE_BORDER 0x3C
#define SSD1608_SET_RAMXPOS 0x44
#define SSD1608_SET_RAMYPOS 0x45
#define SSD1608_SET_RAMXCOUNT 0x4E
#define SSD1608_SET_RAMYCOUNT 0x4F
#define SSD1608_NOP 0xFF

/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1608 EPD drivers
*/
/**************************************************************************/
class Adafruit_SSD1608 : public Adafruit_EPD {
public:
  Adafruit_SSD1608(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                   int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                   int8_t BUSY = -1);
  Adafruit_SSD1608(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                   int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void powerDown();
  void update();

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();
};

#endif
