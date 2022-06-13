/*!
 * @file Adafruit_EPD.cpp
 *
 * @mainpage Adafruit EPD driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's EPD driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit EPD breakouts.
 *
 * These displays use SPI to communicate, 6 pins are required to
 * interface
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a
 * href="https://github.com/adafruit/Adafruit-GFX-Library"> Adafruit_GFX</a>
 * being present on your system. Please make sure you have installed the latest
 * version before using this library.
 *
 * @section author Author
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Adafruit_EPD.h"
#include <stdlib.h>

bool Adafruit_EPD::_isInTransaction = false;

/**************************************************************************/
/*!
    @brief constructor if using external SRAM chip and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param spi_mosi the SID pin to use
    @param spi_clock the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param spi_miso the MISO pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t spi_mosi,
                           int8_t spi_clock, int8_t DC, int8_t RST, int8_t CS,
                           int8_t SRCS, int8_t spi_miso, int8_t BUSY)
    : Adafruit_GFX(width, height), sram(spi_mosi, spi_miso, spi_clock, SRCS) {
  _cs_pin = CS;
  _reset_pin = RST;
  _dc_pin = DC;
  _busy_pin = BUSY;
  if (SRCS >= 0) {
    use_sram = true;
  } else {
    use_sram = false;
  }

  spi_dev = new Adafruit_SPIDevice(CS, spi_clock, spi_miso, spi_mosi,
                                   4000000,               // frequency
                                   SPI_BITORDER_MSBFIRST, // bit order
                                   SPI_MODE0              // data mode
  );

  singleByteTxns = false;
  buffer1_size = buffer2_size = 0;
  buffer1_addr = buffer2_addr = 0;
  colorbuffer_addr = blackbuffer_addr = 0;
  buffer1 = buffer2 = color_buffer = black_buffer = NULL;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param BUSY the busy pin to use
    @param spi the SPI bus to use
*/
/**************************************************************************/
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t DC, int8_t RST,
                           int8_t CS, int8_t SRCS, int8_t BUSY, SPIClass *spi)
    : Adafruit_GFX(width, height), sram(SRCS) {
  _cs_pin = CS;
  _reset_pin = RST;
  _dc_pin = DC;
  _busy_pin = BUSY;
  if (SRCS >= 0) {
    use_sram = true;
  } else {
    use_sram = false;
  }

  spi_dev = new Adafruit_SPIDevice(CS,
                                   4000000,               // frequency
                                   SPI_BITORDER_MSBFIRST, // bit order
                                   SPI_MODE0,             // data mode
                                   spi);

  singleByteTxns = false;
  buffer1_size = buffer2_size = 0;
  buffer1_addr = buffer2_addr = 0;
  colorbuffer_addr = blackbuffer_addr = 0;
  buffer1 = buffer2 = color_buffer = black_buffer = NULL;
}

/**************************************************************************/
/*!
    @brief default destructor
*/
/**************************************************************************/
Adafruit_EPD::~Adafruit_EPD() {
  if (buffer1 != NULL) {
    free(buffer1);
    buffer1 = NULL;
  }
  if (buffer2 != NULL) {
    free(buffer2);
    buffer2 = NULL;
  }
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_EPD::begin(bool reset) {
  setBlackBuffer(0, true);  // black defaults to inverted
  setColorBuffer(1, false); // red defaults to not inverted

  layer_colors[EPD_WHITE] = 0b00;
  layer_colors[EPD_BLACK] = 0b01;
  layer_colors[EPD_RED] = 0b10;
  layer_colors[EPD_GRAY] = 0b10;
  layer_colors[EPD_DARK] = 0b01;
  layer_colors[EPD_LIGHT] = 0b10;

  if (use_sram) {
    sram.begin();
    sram.write8(0, K640_SEQUENTIAL_MODE, MCPSRAM_WRSR);
  }

  // Serial.println("set pins");
  // set pin directions
  pinMode(_dc_pin, OUTPUT);
  pinMode(_cs_pin, OUTPUT);

#if defined(BUSIO_USE_FAST_PINIO)
  csPort = (BusIO_PortReg *)portOutputRegister(digitalPinToPort(_cs_pin));
  csPinMask = digitalPinToBitMask(_cs_pin);
  dcPort = (BusIO_PortReg *)portOutputRegister(digitalPinToPort(_dc_pin));
  dcPinMask = digitalPinToBitMask(_dc_pin);
#endif

  csHigh();

  if (!spi_dev->begin()) {
    return;
  }

  // Serial.println("hard reset");
  if (reset) {
    hardwareReset();
  }

  // Serial.println("busy");
  if (_busy_pin >= 0) {
    pinMode(_busy_pin, INPUT);
  }
  // Serial.println("done!");
}

/**************************************************************************/
/*!
    @brief reset Perform a hardware reset
*/
/**************************************************************************/
void Adafruit_EPD::hardwareReset(void) {
  if (_reset_pin >= 0) {
    // Setup reset pin direction
    pinMode(_reset_pin, OUTPUT);
    // VDD (3.3V) goes high at start, lets just chill for a ms
    digitalWrite(_reset_pin, HIGH);
    delay(10);
    // bring reset low
    digitalWrite(_reset_pin, LOW);
    // wait 10ms
    delay(10);
    // bring out of reset
    digitalWrite(_reset_pin, HIGH);
    delay(10);
  }
}

/**************************************************************************/
/*!
    @brief draw a single pixel on the screen
        @param x the x axis position
        @param y the y axis position
        @param color the color of the pixel
*/
/**************************************************************************/
void Adafruit_EPD::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  uint8_t *black_pBuf, *color_pBuf;

  // deal with non-8-bit heights
  uint16_t _HEIGHT = HEIGHT;
  if (_HEIGHT % 8 != 0) {
    _HEIGHT += 8 - (_HEIGHT % 8);
  }

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    EPD_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = _HEIGHT - y - 1;
    break;
  case 3:
    EPD_swap(x, y);
    y = _HEIGHT - y - 1;
    break;
  }
  uint16_t addr = ((uint32_t)(WIDTH - 1 - x) * (uint32_t)_HEIGHT + y) / 8;
  uint8_t black_c, color_c;

  if (use_sram) {
    black_c = sram.read8(blackbuffer_addr + addr);
    black_pBuf = &black_c;
    color_c = sram.read8(colorbuffer_addr + addr);
    color_pBuf = &color_c;
  } else {
    color_pBuf = color_buffer + addr;
    black_pBuf = black_buffer + addr;
  }

  bool color_bit, black_bit;

  black_bit = layer_colors[color] & 0x1;
  color_bit = layer_colors[color] & 0x2;

  if ((color_bit && colorInverted) || (!color_bit && !colorInverted)) {
    *color_pBuf &= ~(1 << (7 - y % 8));
  } else {
    *color_pBuf |= (1 << (7 - y % 8));
  }

  if ((black_bit && blackInverted) || (!black_bit && !blackInverted)) {
    *black_pBuf &= ~(1 << (7 - y % 8));
  } else {
    *black_pBuf |= (1 << (7 - y % 8));
  }

  if (use_sram) {
    sram.write8(colorbuffer_addr + addr, *color_pBuf);
    sram.write8(blackbuffer_addr + addr, *black_pBuf);
  }
}

/**************************************************************************/
/*!
    @brief Transfer the data stored in the buffer(s) to the display
*/
/**************************************************************************/
void Adafruit_EPD::display(bool sleep) {
  uint8_t c;

#ifdef EPD_DEBUG
  Serial.println("  Powering Up");
#endif

  powerUp();

#ifdef EPD_DEBUG
  Serial.println("  Set RAM address");
#endif

  // Set X & Y ram counters
  setRAMAddress(0, 0);

  if (use_sram) {
    sram.csLow();
    // send read command
    SPItransfer(MCPSRAM_READ);
    // send address
    SPItransfer(buffer1_addr >> 8);
    SPItransfer(buffer1_addr & 0xFF);

    // first data byte from SRAM will be transfered in at the same time
    // as the EPD command is transferred out
    c = writeRAMCommand(0);

    dcHigh();
    for (uint16_t i = 0; i < buffer1_size; i++) {
      c = SPItransfer(c);
      // Serial.print("0x"); Serial.print((byte)c, HEX); Serial.print(", ");
      // if (i % 32 == 31) Serial.println();
    }
    csHigh();
    sram.csHigh();
  } else {
    // write image
    writeRAMCommand(0);
    dcHigh();
    for (uint16_t i = 0; i < buffer1_size; i++) {
      SPItransfer(buffer1[i]);
    }
    csHigh();
  }

  if (buffer2_size == 0) {
    update();
    return;
  }

  // oh there's another buffer eh?
  delay(2);

  // Set X & Y ram counters
  setRAMAddress(0, 0);

  if (use_sram) {
    sram.csLow();
    // send read command
    SPItransfer(MCPSRAM_READ);
    // send address
    SPItransfer(buffer2_addr >> 8);
    SPItransfer(buffer2_addr & 0xFF);

    // first data byte from SRAM will be transfered in at the same time
    // as the EPD command is transferred out
    c = writeRAMCommand(1);

    dcHigh();
    for (uint16_t i = 0; i < buffer2_size; i++) {
      c = SPItransfer(c);
    }
    csHigh();
    sram.csHigh();
  } else {
    writeRAMCommand(1);
    dcHigh();

    for (uint16_t i = 0; i < buffer2_size; i++) {
      SPItransfer(buffer2[i]);
    }
    csHigh();
  }

#ifdef EPD_DEBUG
  Serial.println("  Update");
#endif
  update();

  if (sleep) {
#ifdef EPD_DEBUG
    Serial.println("  Powering Down");
#endif
    powerDown();
  }
}

/**************************************************************************/
/*!
    @brief Determine whether the black pixel data is the first or second buffer
    @param index 0 or 1, for primary or secondary value
    @param inverted Whether to invert the logical value
*/
/**************************************************************************/
void Adafruit_EPD::setBlackBuffer(int8_t index, bool inverted) {
  if (index == 0) {
    if (use_sram) {
      blackbuffer_addr = buffer1_addr;
    } else {
      black_buffer = buffer1;
    }
  }
  if (index == 1) {
    if (use_sram) {
      blackbuffer_addr = buffer2_addr;
    } else {
      black_buffer = buffer2;
    }
  }
  blackInverted = inverted;
}

/**************************************************************************/
/*!
    @brief Determine whether the color pixel data is the first or second buffer
    @param index 0 or 1, for primary or secondary value
    @param inverted Whether to invert the logical value
*/
/**************************************************************************/
void Adafruit_EPD::setColorBuffer(int8_t index, bool inverted) {
  if (index == 0) {
    if (use_sram) {
      colorbuffer_addr = buffer1_addr;
    } else {
      color_buffer = buffer1;
    }
  }
  if (index == 1) {
    if (use_sram) {
      colorbuffer_addr = buffer2_addr;
    } else {
      color_buffer = buffer2;
    }
  }
  colorInverted = inverted;
}

/**************************************************************************/
/*!
    @brief clear all data buffers
*/
/**************************************************************************/
void Adafruit_EPD::clearBuffer() {
  if (use_sram) {
    if (buffer1_size != 0) {
      if (blackInverted) {
        sram.erase(buffer1_addr, buffer1_size, 0xFF);
      } else {
        sram.erase(buffer1_addr, buffer1_size, 0x00);
      }
    }
    if (buffer2_size != 0) {
      if (colorInverted) {
        sram.erase(buffer2_addr, buffer2_size, 0xFF);
      } else {
        sram.erase(buffer2_addr, buffer2_size, 0x00);
      }
    }
  } else {
    if (buffer1) {
      if (blackInverted) {
        memset(buffer1, 0xFF, buffer1_size);
      } else {
        memset(buffer1, 0x00, buffer1_size);
      }
    }
    if (buffer2) {
      if (colorInverted) {
        memset(buffer2, 0xFF, buffer2_size);
      } else {
        memset(buffer2, 0x00, buffer2_size);
      }
    }
  }
}

/**************************************************************************/
/*!
    @brief clear the display twice to remove any spooky ghost images
*/
/**************************************************************************/
void Adafruit_EPD::clearDisplay() {
  clearBuffer();
  display();
  delay(100);
  display();
}

/**************************************************************************/
/*!
 */
/**************************************************************************/
void Adafruit_EPD::EPD_commandList(const uint8_t *init_code) {
  uint8_t buf[64];

  while (init_code[0] != 0xFE) {
    char cmd = init_code[0];
    init_code++;
    int num_args = init_code[0];
    init_code++;
    if (cmd == 0xFF) {
      busy_wait();
      delay(num_args);
      continue;
    }
    if (num_args > sizeof(buf)) {
      Serial.println("ERROR - buf not large enough!");
      while (1)
        delay(10);
    }

    for (int i = 0; i < num_args; i++) {
      buf[i] = init_code[0];
      init_code++;
    }
    EPD_command(cmd, buf, num_args);
  }
}

/**************************************************************************/
/*!
    @brief send an EPD command followed by data
    @param c the command to send
    @param buf the buffer of data to send
    @param len the length of the data buffer
*/
/**************************************************************************/
void Adafruit_EPD::EPD_command(uint8_t c, const uint8_t *buf, uint16_t len) {
  EPD_command(c, false);
  EPD_data(buf, len);
}

/**************************************************************************/
/*!
    @brief send an EPD command with no data
    @param c the command to send
    @param end if true the cs pin will be pulled high following the transaction.
   If false the cs pin will remain low.
    @returns the data byte read over the SPI bus
*/
/**************************************************************************/
uint8_t Adafruit_EPD::EPD_command(uint8_t c, bool end) {
  // SPI
  csHigh();
  dcLow();
  csLow();

  uint8_t data = SPItransfer(c);
#ifdef EPD_DEBUG
  Serial.print("\tCommand: 0x");
  Serial.println(c, HEX);
#endif

  if (end) {
    csHigh();
  }

  return data;
}

/**************************************************************************/
/*!
    @brief send data to the display
    @param buf the data buffer to send
    @param len the length of the data buffer
*/
/**************************************************************************/
void Adafruit_EPD::EPD_data(const uint8_t *buf, uint16_t len) {
  // SPI
  dcHigh();

#ifdef EPD_DEBUG
  Serial.print("\tData: ");
#endif
  for (uint16_t i = 0; i < len; i++) {
    SPItransfer(buf[i]);
#ifdef EPD_DEBUG
    Serial.print("0x");
    Serial.print(buf[i], HEX);
    Serial.print(", ");
#endif
  }

#ifdef EPD_DEBUG
  Serial.println();
#endif

  csHigh();
}

/**************************************************************************/
/*!
    @brief send data to the display
    @param data the data byte to send
*/
/**************************************************************************/
void Adafruit_EPD::EPD_data(uint8_t data) {
  // SPI
  csHigh();
  dcHigh();
  csLow();

#ifdef DEBUG
  Serial.print("Data: ");
  Serial.print("0x");
  Serial.println(data, HEX);
#endif
  SPItransfer(data);

  csHigh();
}

/**************************************************************************/
/*!
    @brief transfer a single byte over SPI.
    @param d the data to send
    @returns the data byte read
*/
/**************************************************************************/
uint8_t Adafruit_EPD::SPItransfer(uint8_t d) {
  // Serial.print("-> 0x"); Serial.println((byte)d, HEX);

  if (singleByteTxns) {
    uint8_t b;
    csLow();
    b = spi_dev->transfer(d);
    csHigh();
    return b;
  } else {
    return spi_dev->transfer(d);
  }
}

/**************************************************************************/
/*!
    @brief set chip select pin high
*/
/**************************************************************************/
void Adafruit_EPD::csHigh() {

#ifdef BUSIO_USE_FAST_PINIO
  *csPort |= csPinMask;
#else
  digitalWrite(_cs_pin, HIGH);
#endif

  spi_dev->endTransaction();
  _isInTransaction = false;
}

/**************************************************************************/
/*!
    @brief set chip select pin low
*/
/**************************************************************************/
void Adafruit_EPD::csLow() {
  spi_dev->beginTransaction();
  _isInTransaction = true;

#ifdef BUSIO_USE_FAST_PINIO
  *csPort &= ~csPinMask;
#else
  digitalWrite(_cs_pin, LOW);
#endif
}

/**************************************************************************/
/*!
    @brief set data/command pin high
*/
/**************************************************************************/
void Adafruit_EPD::dcHigh() {
#ifdef BUSIO_USE_FAST_PINIO
  *dcPort |= dcPinMask;
#else
  digitalWrite(_dc_pin, HIGH);
#endif
}

/**************************************************************************/
/*!
    @brief set data/command pin low
*/
/**************************************************************************/
void Adafruit_EPD::dcLow() {
#ifdef BUSIO_USE_FAST_PINIO
  *dcPort &= ~dcPinMask;
#else
  digitalWrite(_dc_pin, LOW);
#endif
}
