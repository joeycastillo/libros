#ifndef LIB_ADAFRUIT_SSD1619
#define LIB_ADAFRUIT_SSD1619

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define SSD1619_DRIVER_CONTROL 0x01
#define SSD1619_GATE_VOLTAGE 0x03
#define SSD1619_SOURCE_VOLTAGE 0x04
#define SSD1619_PROGOTP_INITIAL 0x08
#define SSD1619_PROGREG_INITIAL 0x09
#define SSD1619_READREG_INITIAL 0x0A
#define SSD1619_BOOST_SOFTSTART 0x0C
#define SSD1619_DEEP_SLEEP 0x10
#define SSD1619_DATA_MODE 0x11
#define SSD1619_SW_RESET 0x12
#define SSD1619_TEMP_CONTROL 0x18
#define SSD1619_TEMP_WRITE 0x1A
#define SSD1619_MASTER_ACTIVATE 0x20
#define SSD1619_DISP_CTRL1 0x21
#define SSD1619_DISP_CTRL2 0x22
#define SSD1619_WRITE_RAM1 0x24
#define SSD1619_WRITE_RAM2 0x26
#define SSD1619_WRITE_VCOM 0x2C
#define SSD1619_READ_OTP 0x2D
#define SSD1619_READ_STATUS 0x2F
#define SSD1619_WRITE_LUT 0x32
#define SSD1619_WRITE_BORDER 0x3C
#define SSD1619_SET_RAMXPOS 0x44
#define SSD1619_SET_RAMYPOS 0x45
#define SSD1619_SET_RAMXCOUNT 0x4E
#define SSD1619_SET_RAMYCOUNT 0x4F
#define SSD1619_SET_ANALOGBLOCK 0x74
#define SSD1619_SET_DIGITALBLOCK 0x7E

/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1619 EPD drivers
*/
/**************************************************************************/
class Adafruit_SSD1619 : public Adafruit_EPD {
public:
  Adafruit_SSD1619(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                   int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                   int8_t BUSY = -1);
  Adafruit_SSD1619(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                   int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void update(void);
  void powerDown();

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void busy_wait();
};

#endif
