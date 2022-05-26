#include "Adafruit_IL91874.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 500

const unsigned char lut_vcomDC[] = {
    0x00, 0x00, 0x00, 0x1A, 0x1A, 0x00, 0x00, 0x01, 0x00, 0x0A, 0x0A,
    0x00, 0x00, 0x08, 0x00, 0x0E, 0x01, 0x0E, 0x01, 0x10, 0x00, 0x0A,
    0x0A, 0x00, 0x00, 0x08, 0x00, 0x04, 0x10, 0x00, 0x00, 0x05, 0x00,
    0x03, 0x0E, 0x00, 0x00, 0x0A, 0x00, 0x23, 0x00, 0x00, 0x00, 0x01};

// R21H
const unsigned char lut_ww[] = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01, 0x40, 0x0A, 0x0A, 0x00, 0x00,
    0x08, 0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10, 0x80, 0x0A, 0x0A, 0x00,
    0x00, 0x08, 0x00, 0x04, 0x10, 0x00, 0x00, 0x05, 0x00, 0x03, 0x0E,
    0x00, 0x00, 0x0A, 0x00, 0x23, 0x00, 0x00, 0x00, 0x01};

// R22H	r
const unsigned char lut_bw[] = {
    0xA0, 0x1A, 0x1A, 0x00, 0x00, 0x01, 0x00, 0x0A, 0x0A, 0x00, 0x00,
    0x08, 0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10, 0x90, 0x0A, 0x0A, 0x00,
    0x00, 0x08, 0xB0, 0x04, 0x10, 0x00, 0x00, 0x05, 0xB0, 0x03, 0x0E,
    0x00, 0x00, 0x0A, 0xC0, 0x23, 0x00, 0x00, 0x00, 0x01};

// R23H	w
const unsigned char lut_bb[] = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01, 0x40, 0x0A, 0x0A, 0x00, 0x00,
    0x08, 0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10, 0x80, 0x0A, 0x0A, 0x00,
    0x00, 0x08, 0x00, 0x04, 0x10, 0x00, 0x00, 0x05, 0x00, 0x03, 0x0E,
    0x00, 0x00, 0x0A, 0x00, 0x23, 0x00, 0x00, 0x00, 0x01};

// R24H	b
const unsigned char lut_wb[] = {
    0x90, 0x1A, 0x1A, 0x00, 0x00, 0x01, 0x20, 0x0A, 0x0A, 0x00, 0x00,
    0x08, 0x84, 0x0E, 0x01, 0x0E, 0x01, 0x10, 0x10, 0x0A, 0x0A, 0x00,
    0x00, 0x08, 0x00, 0x04, 0x10, 0x00, 0x00, 0x05, 0x00, 0x03, 0x0E,
    0x00, 0x00, 0x0A, 0x00, 0x23, 0x00, 0x00, 0x00, 0x01};

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
Adafruit_IL91874::Adafruit_IL91874(int width, int height, int8_t SID,
                                   int8_t SCLK, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t MISO,
                                   int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {

  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }
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
*/
/**************************************************************************/
Adafruit_IL91874::Adafruit_IL91874(int width, int height, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t BUSY,
                                   SPIClass *spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {

  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_IL91874::busy_wait(void) {
  if (_busy_pin >= 0) {
    while (!digitalRead(_busy_pin)) {
      delay(1); // wait for busy low
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
void Adafruit_IL91874::begin(bool reset) {
  singleByteTxns = true;
  Adafruit_EPD::begin(reset);

  setBlackBuffer(0, true);  // black defaults to inverted
  setColorBuffer(1, false); // red defaults to not inverted

  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_IL91874::update() {
  EPD_command(IL91874_DISPLAY_REFRESH);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(16000);
  }
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_IL91874::powerUp() {
  uint8_t buf[5];

  hardwareReset();
  delay(200);

  EPD_command(IL91874_POWER_ON);
  busy_wait();

  buf[0] = 0xAF;
  EPD_command(IL91874_PANEL_SETTING, buf, 1);

  buf[0] = 0x3a;
  EPD_command(IL91874_PLL, buf, 1);

  buf[0] = 0x03;
  buf[1] = 0x00;
  buf[2] = 0x2b;
  buf[3] = 0x2b;
  buf[4] = 0x09;
  EPD_command(IL91874_POWER_SETTING, buf, 5);

  buf[0] = 0x07;
  buf[1] = 0x07;
  buf[2] = 0x017;
  EPD_command(IL91874_BOOSTER_SOFT_START, buf, 3);

  buf[0] = 0x60;
  buf[1] = 0xA5;
  EPD_command(0xF8, buf, 2);

  buf[0] = 0x89;
  buf[1] = 0xA5;
  EPD_command(0xF8, buf, 2);

  buf[0] = 0x90;
  buf[1] = 0x00;
  EPD_command(0xF8, buf, 2);

  buf[0] = 0x93;
  buf[1] = 0x2A;
  EPD_command(0xF8, buf, 2);

  buf[0] = 0x73;
  buf[1] = 0x41;
  EPD_command(0xF8, buf, 2);

  buf[0] = 0x12;
  EPD_command(IL91874_VCM_DC_SETTING, buf, 1);

  buf[0] = 0x87;
  EPD_command(IL91874_CDI, buf, 1);

  EPD_command(IL91874_LUT1, lut_vcomDC, 44);
  EPD_command(IL91874_LUTWW, lut_ww, 42);
  EPD_command(IL91874_LUTBW, lut_bw, 42);
  EPD_command(IL91874_LUTWB, lut_wb, 42);
  EPD_command(IL91874_LUTBB, lut_bb, 42);

  buf[0] = (HEIGHT >> 8) & 0xFF;
  buf[1] = HEIGHT & 0xFF;
  buf[2] = (WIDTH >> 8) & 0xFF;
  buf[3] = WIDTH & 0xFF;
  EPD_command(IL91874_RESOLUTION, buf, 4);

  buf[0] = 0x00;
  EPD_command(IL91874_PDRF, buf, 1);
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_IL91874::powerDown() {
  // power off
  uint8_t buf[1];

  EPD_command(IL91874_POWER_OFF);
  busy_wait();

  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    buf[0] = 0xA5;
    EPD_command(IL91874_DEEP_SLEEP, buf, 1);
  }
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color
   displays) Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the
   command
*/
/**************************************************************************/
uint8_t Adafruit_IL91874::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(EPD_RAM_BW, false);
  }
  if (index == 1) {
    return EPD_command(EPD_RAM_RED, false);
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void Adafruit_IL91874::setRAMAddress(uint16_t x, uint16_t y) {
  // on this chip we do nothing
}
