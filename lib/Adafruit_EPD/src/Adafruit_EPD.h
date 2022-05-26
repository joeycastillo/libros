/*!
 * @file Adafruit_EPD.h
 *
 * This is a library for our EPD displays based on EPD drivers.
 * Designed specifically to work with Adafruit EPD displays.
 *
 * These displays use SPI to communicate, 4 or 5 pins are required to
 * interface
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef _ADAFRUIT_EPD_H_
#define _ADAFRUIT_EPD_H_

// #define EPD_DEBUG

#define RAMBUFSIZE 64 ///< size of the ram buffer

#include "Adafruit_MCPSRAM.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SPIDevice.h>

/**************************************************************************/
/*!
    @brief available EPD colors
*/
/**************************************************************************/
enum {
  EPD_WHITE, ///< white color
  EPD_BLACK, ///< black color
  EPD_RED,   ///< red color
  EPD_GRAY,  ///< gray color ('red' on grayscale)
  EPD_DARK,  ///< darker color
  EPD_LIGHT, ///< lighter color
  EPD_NUM_COLORS
};

#define EPD_swap(a, b)                                                         \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  } ///< simple swap function

/**************************************************************************/
/*!
    @brief  Class for interfacing with Adafruit EPD display breakouts.
*/
/**************************************************************************/
class Adafruit_EPD : public Adafruit_GFX {
public:
  Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
               int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
               int8_t BUSY = -1);
  Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS,
               int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI);
  ~Adafruit_EPD();

  void begin(bool reset = true);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void clearBuffer();
  void clearDisplay();
  void setBlackBuffer(int8_t index, bool inverted);
  void setColorBuffer(int8_t index, bool inverted);
  void display(bool sleep = false);

protected:
  /**************************************************************************/
  /*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color
    displays) Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the
    command
  */
  /**************************************************************************/
  virtual uint8_t writeRAMCommand(uint8_t index) = 0;

  /**************************************************************************/
  /*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
  */
  /**************************************************************************/
  virtual void setRAMAddress(uint16_t x, uint16_t y) = 0;

  virtual void busy_wait(void) = 0;

  /**************************************************************************/
  /*!
    @brief start up the display
  */
  /**************************************************************************/
  virtual void powerUp() = 0;

  /**************************************************************************/
  /*!
    @brief signal the display to update
  */
  /**************************************************************************/
  virtual void update(void) = 0;

  /**************************************************************************/
  /*!
    @brief wind down the display
  */
  /**************************************************************************/
  virtual void powerDown(void) = 0;
  void hardwareReset(void);

  int8_t _dc_pin,                     ///< data/command pin
      _reset_pin,                     ///< reset pin
      _cs_pin,                        ///< chip select pin
      _busy_pin;                      ///< busy pin
  Adafruit_SPIDevice *spi_dev = NULL; ///< SPI object
  static bool _isInTransaction;       ///< true if SPI bus is in trasnfer state
  bool singleByteTxns; ///< if true CS will go high after every data byte
                       ///< transferred

  const uint8_t *_epd_init_code = NULL;
  const uint8_t *_epd_lut_code = NULL;
  const uint8_t *_epd_partial_init_code = NULL;
  const uint8_t *_epd_partial_lut_code = NULL;

  uint16_t default_refresh_delay = 15000;

  Adafruit_MCPSRAM sram; ///< the ram chip object if using off-chip ram

  bool blackInverted; ///< is black channel inverted
  bool colorInverted; ///< is red channel inverted

  uint8_t layer_colors[EPD_NUM_COLORS];

  uint16_t buffer1_size; ///< size of the primary buffer
  uint16_t buffer2_size; ///< size of the secondary buffer
  uint8_t *buffer1; ///< the pointer to the primary buffer if using on-chip ram
  uint8_t
      *buffer2; ///< the pointer to the secondary buffer if using on-chip ram
  uint8_t
      *color_buffer; ///< the pointer to the color buffer if using on-chip ram
  uint8_t
      *black_buffer; ///< the pointer to the black buffer if using on-chip ram
  uint16_t buffer1_addr; ///< The SRAM address offsets for the primary buffer
  uint16_t buffer2_addr; ///< The SRAM address offsets for the secondary buffer
  uint16_t colorbuffer_addr; ///< The SRAM address offsets for the color buffer
  uint16_t blackbuffer_addr; ///< The SRAM address offsets for the black buffer

  void EPD_commandList(const uint8_t *init_code);
  void EPD_command(uint8_t c, const uint8_t *buf, uint16_t len);
  uint8_t EPD_command(uint8_t c, bool end = true);
  void EPD_data(const uint8_t *buf, uint16_t len);
  void EPD_data(uint8_t data);

  uint8_t SPItransfer(uint8_t c);

  bool use_sram; ///< true if we are using an SRAM chip as a framebuffer

#if defined(BUSIO_USE_FAST_PINIO)
  BusIO_PortReg *csPort, *dcPort;
  BusIO_PortMask csPinMask, dcPinMask;
#endif

  void csLow();
  void csHigh();
  void dcHigh();
  void dcLow();
};

#include "drivers/Adafruit_IL0373.h"
#include "drivers/Adafruit_IL0398.h"
#include "drivers/Adafruit_IL91874.h"
#include "drivers/Adafruit_SSD1608.h"
#include "drivers/Adafruit_SSD1619.h"
#include "drivers/Adafruit_SSD1675.h"
#include "drivers/Adafruit_SSD1675B.h"
#include "drivers/Adafruit_SSD1680.h"
#include "drivers/Adafruit_SSD1681.h"

#endif /* _ADAFRUIT_EPD_H_ */
