#include "Adafruit_SSD1608.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 500

const unsigned char LUT_DATA[30] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69,
    0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00,
    0xF8, 0xB4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00};

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
Adafruit_SSD1608::Adafruit_SSD1608(int width, int height, int8_t SID,
                                   int8_t SCLK, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t MISO,
                                   int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {

  if ((width % 8) != 0) {
    width += 8 - (width % 8);
  }
  buffer1_size = (uint16_t)width * (uint16_t)height / 8;
  buffer2_size = 0;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = 0;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = NULL;
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
Adafruit_SSD1608::Adafruit_SSD1608(int width, int height, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t BUSY,
                                   SPIClass *spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {

  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }
  buffer1_size = (uint16_t)width * (uint16_t)height / 8;
  buffer2_size = 0;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = 0;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = buffer1;
  }
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_SSD1608::busy_wait(void) {
  if (_busy_pin >= 0) {
    while (digitalRead(_busy_pin)) { // wait for busy low
      delay(10);
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
void Adafruit_SSD1608::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true); // black defaults to inverted
  setColorBuffer(0, true); // no secondary buffer, so we'll just reuse index 0

  delay(100);
  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1608::update() {
  uint8_t buf[1];

  // display update sequence
  buf[0] = 0xC7;
  EPD_command(SSD1608_DISP_CTRL2, buf, 1);

  EPD_command(SSD1608_MASTER_ACTIVATE);

  busy_wait();
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_SSD1608::powerUp() {
  uint8_t buf[5];

  hardwareReset();
  busy_wait();

  // soft reset
  EPD_command(SSD1608_SW_RESET);

  busy_wait();

  // driver output control
  buf[0] = HEIGHT - 1;
  buf[1] = (HEIGHT - 1) >> 8;
  buf[2] = 0x00;
  EPD_command(SSD1608_DRIVER_CONTROL, buf, 3);

  // Set dummy line period
  buf[0] = 0x1B;
  EPD_command(SSD1608_WRITE_DUMMY, buf, 1);

  // Set gate line width
  buf[0] = 0x0B;
  EPD_command(SSD1608_WRITE_GATELINE, buf, 1);

  // Data entry sequence
  buf[0] = 0x03;
  EPD_command(SSD1608_DATA_MODE, buf, 1);

  // Set ram X start/end postion
  buf[0] = 0x00;
  buf[1] = WIDTH / 8 - 1;
  EPD_command(SSD1608_SET_RAMXPOS, buf, 2);

  // Set ram Y start/end postion
  buf[0] = 0x00;
  buf[1] = 0x00;
  buf[2] = HEIGHT - 1;
  buf[3] = (HEIGHT - 1) >> 8;
  EPD_command(SSD1608_SET_RAMYPOS, buf, 4);

  // Vcom Voltage
  buf[0] = 0x70;
  EPD_command(SSD1608_WRITE_VCOM, buf, 1);

  EPD_command(SSD1608_WRITE_LUT, LUT_DATA, 30);

  /*
  // border color
  buf[0] = 0x03;
  EPD_command(SSD1608_WRITE_BORDER, buf, 1);

  // Set gate voltage
  buf[0] = LUT_DATA[70];
  EPD_command(SSD1608_GATE_VOLTAGE, buf, 1);

  // Set source voltage
  buf[0] = LUT_DATA[71];
  buf[1] = LUT_DATA[72];
  buf[2] = LUT_DATA[73];
  EPD_command(SSD1608_SOURCE_VOLTAGE, buf, 3);
  */
  /*

  // set RAM x address count ;
  buf[0] = 0;
  EPD_command(SSD1608_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count;
  buf[0] = HEIGHT - 1;
  buf[1] = (HEIGHT - 1) >> 8;
  EPD_command(SSD1608_SET_RAMYCOUNT, buf, 2);
  */
  busy_wait();
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/

void Adafruit_SSD1608::powerDown(void) {
  uint8_t buf[2];
  // deep sleep
  buf[0] = 0x01;
  EPD_command(SSD1608_DEEP_SLEEP, buf, 1);
  delay(100);
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
uint8_t Adafruit_SSD1608::writeRAMCommand(uint8_t index) {
  return EPD_command(SSD1608_WRITE_RAM, false);
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void Adafruit_SSD1608::setRAMAddress(uint16_t x, uint16_t y) {
  uint8_t buf[2];

  // Set RAM X address counter
  buf[0] = x;
  EPD_command(SSD1608_SET_RAMXCOUNT, buf, 1);

  // Set RAM Y address counter
  buf[0] = y >> 8;
  buf[1] = y;
  EPD_command(SSD1608_SET_RAMYCOUNT, buf, 2);
}
