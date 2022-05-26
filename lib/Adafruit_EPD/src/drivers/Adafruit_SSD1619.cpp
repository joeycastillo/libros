#include "Adafruit_SSD1619.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 500

// clang-format off

const uint8_t ssd1619_default_init_code[] {
  SSD1619_SW_RESET, 0, // soft reset
    0xFF, 20,          // busy wait

    SSD1619_SET_ANALOGBLOCK, 1, 0x54,  // set analog block control
    SSD1619_SET_DIGITALBLOCK, 1, 0x3B, // set digital block control

    SSD1619_DATA_MODE, 1, 0x03, // Ram data entry mode

    SSD1619_WRITE_BORDER, 1, 0x01, // border color
    SSD1619_TEMP_CONTROL, 1, 0x80, // Temp control
    SSD1619_DISP_CTRL2, 1, 0xB1,

    SSD1619_MASTER_ACTIVATE, 0,
    0xFF, 20,          // busy wait
  
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
Adafruit_SSD1619::Adafruit_SSD1619(int width, int height, int8_t SID,
                                   int8_t SCLK, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t MISO,
                                   int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = width * height / 8;
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

  singleByteTxns = true;
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
Adafruit_SSD1619::Adafruit_SSD1619(int width, int height, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t BUSY,
                                   SPIClass *spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = width * height / 8;
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

  singleByteTxns = true;
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_SSD1619::busy_wait(void) {
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
void Adafruit_SSD1619::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true);  // black defaults to inverted
  setColorBuffer(1, false); // red defaults to un inverted
  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1619::update() {
  uint8_t buf[1];

  // display update sequence
  // buf[0] = 0x40;
  // EPD_command(SSD1619_DISP_CTRL1, buf, 1);
  buf[0] = 0xC7;
  EPD_command(SSD1619_DISP_CTRL2, buf, 1);

  EPD_command(SSD1619_MASTER_ACTIVATE);
  busy_wait();

  if (_busy_pin <= -1) {
    delay(1000);
  }
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_SSD1619::powerUp() {
  uint8_t buf[5];

  hardwareReset();
  delay(100);
  busy_wait();

  const uint8_t *init_code = ssd1619_default_init_code;

  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }
  EPD_commandList(init_code);

  // Set display size and driver output control
  buf[0] = (WIDTH - 1);
  buf[1] = (WIDTH - 1) >> 8;
  buf[2] = 0x00;
  EPD_command(SSD1619_DRIVER_CONTROL, buf, 3);

  setRAMWindow(0, 0, HEIGHT - 1, WIDTH - 1);
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_SSD1619::powerDown() {
  uint8_t buf[1];
  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    // deep sleep
    buf[0] = 0x01;
    EPD_command(SSD1619_DEEP_SLEEP, buf, 1);
    delay(100);
  } else {
    EPD_command(SSD1619_SW_RESET);
    busy_wait();
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
uint8_t Adafruit_SSD1619::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(SSD1619_WRITE_RAM1, false);
  }
  if (index == 1) {
    return EPD_command(SSD1619_WRITE_RAM2, false);
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
void Adafruit_SSD1619::setRAMAddress(uint16_t x, uint16_t y) {
  uint8_t buf[2];

  // set RAM x address count
  buf[0] = 0x00;
  EPD_command(SSD1619_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count
  buf[0] = 0x0;
  buf[1] = 0x0;
  EPD_command(SSD1619_SET_RAMYCOUNT, buf, 2);
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void Adafruit_SSD1619::setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2,
                                    uint16_t y2) {
  uint8_t buf[5];

  // Set ram X start/end postion
  buf[0] = x1 / 8;
  buf[1] = x2 / 8;
  EPD_command(SSD1619_SET_RAMXPOS, buf, 2);

  // Set ram Y start/end postion
  buf[0] = y1;
  buf[1] = y1 >> 8;
  buf[2] = y2;
  buf[3] = y2 >> 8;
  EPD_command(SSD1619_SET_RAMYPOS, buf, 4);
}
