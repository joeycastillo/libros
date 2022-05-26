#include "Adafruit_IL0371.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 500

#ifdef USE_EXTERNAL_SRAM
/**************************************************************************/
/*!
    @brief constructor if using external SRAM chip and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param MISO the MISO pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_IL0371::Adafruit_IL0371(int width, int height, int8_t SID, int8_t SCLK,
                                 int8_t DC, int8_t RST, int8_t CS, int8_t SRCS,
                                 int8_t MISO, int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
#else
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_IL0371::Adafruit_IL0371(int width, int height, int8_t SID, int8_t SCLK,
                                 int8_t DC, int8_t RST, int8_t CS, int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY) {
  bw_buf = (uint8_t *)malloc(width * height / 8);
  red_buf = (uint8_t *)malloc(width * height / 8);
#endif
  bw_bufsize = width * height / 8;
  red_bufsize = bw_bufsize;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
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
*/
/**************************************************************************/
Adafruit_IL0371::Adafruit_IL0371(int width, int height, int8_t DC, int8_t RST,
                                 int8_t CS, int8_t SRCS, int8_t BUSY)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY) {
#else
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_IL0371::Adafruit_IL0371(int width, int height, int8_t DC, int8_t RST,
                                 int8_t CS, int8_t BUSY)
    : Adafruit_EPD(width, height, DC, RST, CS, BUSY) {
  bw_buf = (uint8_t *)malloc(width * height / 8);
  red_buf = (uint8_t *)malloc(width * height / 8);
#endif
  bw_bufsize = width * height / 8;
  red_bufsize = bw_bufsize;
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_IL0371::begin(bool reset) {
  uint8_t buf[5];
  Adafruit_EPD::begin(reset);

  buf[0] = 0x37;
  buf[1] = 0x00;
  // buf[2] = 0x0A;
  // buf[3] = 0x00;
  EPD_command(IL0371_POWER_SETTING, buf, 4);

  buf[0] = 0xc7;
  buf[1] = 0xcc;
  buf[2] = 0x28;
  EPD_command(IL0371_BOOSTER_SOFT_START, buf, 3);
}

/**************************************************************************/
/*!
    @brief update the display
*/
/**************************************************************************/
void Adafruit_IL0371::update() {
  EPD_command(IL0371_DISPLAY_REFRESH);

  delay(10000);

  // power off
  uint8_t buf[4];

  buf[0] = 0x17;
  EPD_command(IL0371_CDI, buf, 1);

  buf[0] = 0x1E;
  EPD_command(IL0371_VCM_DC_SETTING, buf, 0);

  buf[0] = 0x02;
  buf[1] = 0x00;
  buf[2] = 0x00;
  buf[3] = 0x00;
  EPD_command(IL0371_POWER_SETTING);

  EPD_command(IL0371_POWER_OFF);

  delay(10000);
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_IL0371::powerUp() {
  uint8_t buf[4];

  EPD_command(IL0371_POWER_ON);
  if (busy > -1)
    while (digitalRead(busy))
      ; // wait for busy low
  else
    delay(BUSY_WAIT);

  delay(200);

  buf[0] = 0xCF;
  buf[1] = 0x08;
  EPD_command(IL0371_PANEL_SETTING, buf, 2);

  buf[0] = 0x77;
  EPD_command(IL0371_CDI, buf, 1);

  buf[0] = 0x03;
  EPD_command(0xe5, buf, 1);

  buf[0] = 0x3c;
  EPD_command(IL0371_PLL, buf, 1);

  buf[0] = height() & 0xFF;
  buf[1] = (height() >> 8) & 0xFF;
  buf[2] = width() & 0xFF;
  buf[3] = (width() >> 8) & 0xFF;
  EPD_command(IL0371_RESOLUTION, buf, 4);

  buf[0] = 0x0A;
  EPD_command(IL0371_VCM_DC_SETTING, buf, 1);
  delay(20);
}

/**************************************************************************/
/*!
    @brief show the data stored in the buffer on the display
*/
/**************************************************************************/
void Adafruit_IL0371::display() {
  powerUp();

#ifdef USE_EXTERNAL_SRAM
  uint8_t c;

  sram.csLow();
  // send read command
  SPItransfer(MCPSRAM_READ);

  // send address
  SPItransfer(0x00);
  SPItransfer(0x00);

  // first data byte from SRAM will be transfered in at the same time as the EPD
  // command is transferred out
  c = EPD_command(EPD_RAM_BW, false);

  dcHigh();

  for (uint16_t i = 0; i < bw_bufsize; i++) {
    c = SPItransfer(c);
  }
  csHigh();
  sram.csHigh();

  delay(2);

  sram.csLow();
  // send write command
  SPItransfer(MCPSRAM_READ);

  uint8_t b[2];
  b[0] = (bw_bufsize >> 8);
  b[1] = (bw_bufsize & 0xFF);
  // send address
  SPItransfer(b[0]);
  SPItransfer(b[1]);

  // first data byte from SRAM will be transfered in at the same time as the EPD
  // command is transferred out
  c = EPD_command(EPD_RAM_RED, false);

  dcHigh();

  for (uint16_t i = 0; i < red_bufsize; i++) {
    c = SPItransfer(c);
  }
  csHigh();
  sram.csHigh();

#else
  // write image
  EPD_command(EPD_RAM_BW, false);
  dcHigh();

  for (uint16_t i = 0; i < bw_bufsize; i++) {
    SPItransfer(bw_buf[i]);
  }
  csHigh();

  EPD_command(EPD_RAM_RED, false);
  dcHigh();

  for (uint16_t i = 0; i < red_bufsize; i++) {
    SPItransfer(red_buf[i]);
  }
  csHigh();

#endif
  update();
}

/**************************************************************************/
/*!
    @brief draw a single pixel on the screen
        @param x the x axis position
        @param y the y axis position
        @param color the color of the pixel
*/
/**************************************************************************/
void Adafruit_IL0371::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  uint8_t *pBuf;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    EPD_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    EPD_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }
  // make our buffer happy
  x = (x == 0 ? 1 : x);

  uint16_t addr = ((width() - x) * height() + y) / 8;

#ifdef USE_EXTERNAL_SRAM
  if (color == EPD_RED) {
    // red is written after bw
    addr = addr + bw_bufsize;
  }
  uint8_t c = sram.read8(addr);
  pBuf = &c;
#else
  if (color == EPD_RED) {
    pBuf = red_buf + addr;
  } else {
    pBuf = bw_buf + addr;
  }
#endif
  // x is which column
  switch (color) {
  case EPD_WHITE:
    *pBuf |= (1 << (7 - y % 8));
    break;
  case EPD_RED:
  case EPD_BLACK:
    *pBuf &= ~(1 << (7 - y % 8));
    break;
  case EPD_INVERSE:
    *pBuf ^= (1 << (7 - y % 8));
    break;
  }
#ifdef USE_EXTERNAL_SRAM
  sram.write8(addr, *pBuf);
#endif
}

/**************************************************************************/
/*!
    @brief clear all data buffers
*/
/**************************************************************************/
void Adafruit_IL0371::clearBuffer() {
#ifdef USE_EXTERNAL_SRAM
  sram.erase(0x00, bw_bufsize + red_bufsize, 0xFF);
#else
  memset(bw_buf, 0xFF, bw_bufsize);
  memset(red_buf, 0xFF, red_bufsize);
#endif
}

/**************************************************************************/
/*!
    @brief clear the display twice to remove any spooky ghost images
*/
/**************************************************************************/
void Adafruit_IL0371::clearDisplay() {
  clearBuffer();
  display();
  delay(100);
  display();
}
