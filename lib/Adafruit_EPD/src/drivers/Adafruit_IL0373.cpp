#include "Adafruit_IL0373.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 100

// clang-format off

const uint8_t il0373_default_init_code[] {
  IL0373_POWER_SETTING, 5, 0x03, 0x00, 0x2b, 0x2b, 0x09,
    IL0373_BOOSTER_SOFT_START, 3, 0x17, 0x17, 0x17,
    IL0373_POWER_ON, 0,
    0xFF, 200,
    IL0373_PANEL_SETTING, 1, 0xCF,
    IL0373_CDI, 1, 0x37,
    IL0373_PLL, 1, 0x29,    
    IL0373_VCM_DC_SETTING, 1, 0x0A,
    0xFF, 20,
    0xFE};

// clang-format on

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
Adafruit_IL0373::Adafruit_IL0373(int width, int height, int8_t SID, int8_t SCLK,
                                 int8_t DC, int8_t RST, int8_t CS, int8_t SRCS,
                                 int8_t MISO, int8_t BUSY)
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
    @param spi the SPI bus to use
*/
/**************************************************************************/
Adafruit_IL0373::Adafruit_IL0373(int width, int height, int8_t DC, int8_t RST,
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
void Adafruit_IL0373::busy_wait(void) {
  Serial.print("Waiting...");
  if (_busy_pin >= 0) {
    while (!digitalRead(_busy_pin)) {
      delay(10); // wait for busy high
    }
  } else {
    delay(BUSY_WAIT);
  }
  Serial.println("OK!");
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_IL0373::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true); // black defaults to inverted
  setColorBuffer(1, true); // red defaults to inverted

  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_IL0373::update() {
  EPD_command(IL0373_DISPLAY_REFRESH);

  delay(100);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(default_refresh_delay);
  }
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_IL0373::powerUp(void) {
  uint8_t buf[5];

  hardwareReset();

  const uint8_t *init_code = il0373_default_init_code;

  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }
  EPD_commandList(init_code);

  if (_epd_lut_code) {
    EPD_commandList(_epd_lut_code);
  }

  buf[0] = HEIGHT & 0xFF;
  buf[1] = (WIDTH >> 8) & 0xFF;
  buf[2] = WIDTH & 0xFF;
  EPD_command(IL0373_RESOLUTION, buf, 3);
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_IL0373::powerDown() {
  // power off
  uint8_t buf[4];

  buf[0] = 0x17;
  EPD_command(IL0373_CDI, buf, 1);

  buf[0] = 0x00;
  EPD_command(IL0373_VCM_DC_SETTING, buf, 0);

  EPD_command(IL0373_POWER_OFF);
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
uint8_t Adafruit_IL0373::writeRAMCommand(uint8_t index) {
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
void Adafruit_IL0373::setRAMAddress(uint16_t x, uint16_t y) {
  // on this chip we do nothing
}

void Adafruit_IL0373::displayPartial(uint16_t x1, uint16_t y1, uint16_t x2,
                                     uint16_t y2) {
  uint8_t buf[7];
  uint8_t c;

  // check rotation, move window around if necessary
  switch (getRotation()) {
  case 0:
    EPD_swap(x1, y1);
    EPD_swap(x2, y2);
    y1 = WIDTH - y1;
    y2 = WIDTH - y2;
    break;
  case 1:
    break;
  case 2:
    EPD_swap(x1, y1);
    EPD_swap(x2, y2);
    x1 = HEIGHT - x1;
    x2 = HEIGHT - x2;
    break;
  case 3:
    y1 = WIDTH - y1;
    y2 = WIDTH - y2;
    x1 = HEIGHT - x1;
    x2 = HEIGHT - x2;
  }
  if (x1 > x2)
    EPD_swap(x1, x2);
  if (y1 > y2)
    EPD_swap(y1, y2);

  /*
  Serial.print("x: ");
  Serial.print(x1);
  Serial.print(" -> ");
  Serial.println(x2);
  Serial.print("y: ");
  Serial.print(y1);
  Serial.print(" -> ");
  Serial.println(y2);
  */

  // x1 and x2 must be on byte boundaries
  x1 -= x1 % 8;           // round down;
  x2 = (x2 + 7) & ~0b111; // round up

  // Serial.println("Partial update!");

  // backup & change init to the partial code
  const uint8_t *init_code_backup = _epd_init_code;
  const uint8_t *lut_code_backup = _epd_lut_code;
  _epd_init_code = _epd_partial_init_code;
  _epd_lut_code = _epd_partial_lut_code;

  // perform standard power up
  powerUp();

  EPD_command(IL0373_PARTIAL_ENTER);
  buf[0] = x1;
  buf[1] = x2 - 1;
  buf[2] = y1 >> 8;
  buf[3] = y1 & 0xFF;
  buf[4] = (y2 - 1) >> 8;
  buf[5] = (y2 - 1) & 0xFF;
  buf[6] = 0x28;
  EPD_command(IL0373_PARTIAL_WINDOW, buf, 7);

  // display....

  // write image
  writeRAMCommand(0);
  dcHigh();
  for (uint16_t y = y1; y < y2; y++) {
    for (uint16_t x = x1; x < x2; x += 8) {
      uint16_t i = (x / 8) + y * 16;
      SPItransfer(black_buffer[i]);
      // SPItransfer(0);
    }
  }
  csHigh();

  delay(2);

  writeRAMCommand(1);
  dcHigh();

  // Serial.print("Transfering: ");

  for (uint16_t y = y1; y < y2; y++) {
    for (uint16_t x = x1; x < x2; x += 8) {
      uint16_t i = (x / 8) + y * 16;
      /*
      Serial.print(i);
      Serial.print(" (0x");
      Serial.print(buffer2[i]);
      Serial.print("), ");
      if (i % 16 == 15) Serial.println();
      */
      SPItransfer(~black_buffer[i]);
      // SPItransfer(0xFF);
    }
  }
  Serial.println();
  csHigh();

#ifdef EPD_DEBUG
  Serial.println("  Update");
#endif

  update();

  EPD_command(IL0373_PARTIAL_EXIT);

#ifdef EPD_DEBUG
  Serial.println("  Powering Down");
#endif

  powerDown();
  // change init back
  _epd_lut_code = lut_code_backup;
  _epd_init_code = init_code_backup;
}
