#include "Adafruit_IL3897.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 1000

// 0x32,	// command
const unsigned char LUTDefault_full[] = {
    0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E,
    0x1E, 0x1E, 0x1E, 0x1E, 0x01, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char LUTDefault_part[] = {
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

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
Adafruit_IL3897::Adafruit_IL3897(int width, int height, int8_t SID, int8_t SCLK,
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
Adafruit_IL3897::Adafruit_IL3897(int width, int height, int8_t SID, int8_t SCLK,
                                 int8_t DC, int8_t RST, int8_t CS, int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY) {
  bw_buf = (uint8_t *)malloc(width * height / 8);
#endif
  bw_bufsize = width * height / 8;
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
Adafruit_IL3897::Adafruit_IL3897(int width, int height, int8_t DC, int8_t RST,
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
Adafruit_IL3897::Adafruit_IL3897(int width, int height, int8_t DC, int8_t RST,
                                 int8_t CS, int8_t BUSY)
    : Adafruit_EPD(width, height, DC, RST, CS, BUSY) {
  bw_buf = (uint8_t *)malloc(width * height / 8);
#endif
  bw_bufsize = width * height / 8;
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_IL3897::busy_wait(void) {
  if (busy >= 0) {
    while (!digitalRead(busy)) {
      delay(1); // wait for busy high
    }
  } else {
    delay(BUSY_WAIT);
  }
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_IL3897::begin(bool reset) {
  uint8_t buf[5];
  Adafruit_EPD::begin(reset);
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_IL3897::update(bool partial) {
  uint8_t buf[1];

  if (partial) {
    buf[0] = 0x04;
  } else {
    buf[0] = 0xC7;
  }
  EPD_command(IL3897_DISPLAY_UPDATE_CONTROL2, buf, 1);
  EPD_command(IL3897_MASTER_ACTIVATE);
  EPD_command(0xFF); // ??
}

void Adafruit_IL3897::setRAMArea(uint8_t xstart, uint8_t xend, uint16_t ystart,
                                 uint16_t yend) {
  uint8_t buf[4];
  buf[0] = xstart;
  buf[1] = xend;
  EPD_command(IL3897_SET_RAM_X, buf, 2);
  buf[0] = ystart & 0xFF;
  buf[1] = ystart >> 8;
  buf[2] = yend & 0xFF;
  buf[3] = yend >> 8;
  EPD_command(IL3897_SET_RAM_Y, buf, 4);
}

void Adafruit_IL3897::setRAMPointer(uint8_t addrX, uint16_t addrY) {
  uint8_t buf[2];
  buf[0] = addrX;
  EPD_command(IL3897_SET_RAM_XADDR, buf, 1);
  buf[0] = addrY & 0xFF;
  buf[1] = addrY >> 8;
  EPD_command(IL3897_SET_RAM_YADDR, buf, 2);
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_IL3897::powerUp() {
  uint8_t buf[4];

  if (rst >= 0) {
    digitalWrite(rst, LOW);
    // wait 100ms
    delay(100);
    // bring out of reset
    digitalWrite(rst, HIGH);
    delay(100);
  }

  /*
  busy_wait();
  EPD_command(IL3897_SOFT_RESET);
  busy_wait();
  */

  // Gate control
  //{0x01,(yDot-1)%256,(yDot-1)/256,0x00}; //for 2.13inch
  buf[0] = (WIDTH - 1) & 0xFF;
  buf[1] = (WIDTH - 1) >> 8;
  buf[2] = 0x00;
  EPD_command(IL3897_DRIVER_OUT_CONTROL, buf, 3);

  // unsigned char softstart[]={0x0c,0xd7,0xd6,0x9d};
  buf[0] = 0xd7;
  buf[1] = 0xd6;
  buf[2] = 0x9d;
  EPD_command(0x0C, buf, 3);

  // VCOMVol[] = {0x2c,0xa8};	// VCOM 7c
  buf[0] = 0xa8;
  EPD_command(IL3897_VCOM_SET, buf, 1);

  // DummyLine[] = {0x3a,0x1a};	// 4 dummy line per gate
  buf[0] = 0x1A;
  EPD_command(IL3897_DUMMY_LINE, buf, 1);

  // {0x3b,0x08};	// 2us per line
  buf[0] = 0x08;
  EPD_command(IL3897_SET_GATE_WIDTH, buf, 1);

  // {0x11,0x01};	// Ram data entry mode
  buf[0] = 0x01;
  EPD_command(IL3897_DATA_ENTRY_MODE, buf, 1);

  // EPD_W21_SetRamArea(0x00,(xDot-1)/8,(yDot-1)%256,(yDot-1)/256,0x00,0x00);
  // // X-source area,Y-gage area
  setRAMArea(0, (HEIGHT - 1) / 8, WIDTH - 1, 0);

  // EPD_W21_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram
  setRAMPointer(0, WIDTH - 1);
}

/**************************************************************************/
/*!
    @brief show the data stored in the buffer on the display
*/
/**************************************************************************/
void Adafruit_IL3897::display() {
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
void Adafruit_IL3897::drawPixel(int16_t x, int16_t y, uint16_t color) {
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

  uint16_t addr = ((WIDTH - x) * HEIGHT + y) / 8;

#ifdef USE_EXTERNAL_SRAM
  uint8_t c = sram.read8(addr);
  pBuf = &c;
#else
  pBuf = bw_buf + addr;
#endif
  // x is which column
  switch (color) {
  case EPD_WHITE:
    *pBuf |= (1 << (7 - y % 8));
    break;
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
void Adafruit_IL3897::clearBuffer() {
#ifdef USE_EXTERNAL_SRAM
  sram.erase(0x00, bw_bufsize, 0xFF);
#else
  memset(bw_buf, 0xFF, bw_bufsize);
#endif
}

/**************************************************************************/
/*!
    @brief clear the display twice to remove any spooky ghost images
*/
/**************************************************************************/
void Adafruit_IL3897::clearDisplay() {
  clearBuffer();
  display();
  delay(100);
  display();
}

/**************************************************************************/
/*!
    @brief Go into deep sleep mode
*/
/**************************************************************************/
void Adafruit_IL3897::deepSleep() {
  uint8_t cmd_buf[1];
  cmd_buf[0] = 1;

  EPD_command(IL3897_DEEP_SLEEP, cmd_buf, 1);
  delay(100);
}
