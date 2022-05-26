#include "Adafruit_SSD1675.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 500

const unsigned char LUT_DATA[] = {
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, // LUT0: BB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, // LUT1: BW:     VS 0 ~7
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, // LUT2: WB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, // LUT3: WW:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT4: VCOM:   VS 0 ~7

    0x03, 0x03, 0x00, 0x00, 0x02, // TP0 A~D RP0
    0x09, 0x09, 0x00, 0x00, 0x02, // TP1 A~D RP1
    0x03, 0x03, 0x00, 0x00, 0x02, // TP2 A~D RP2
    0x00, 0x00, 0x00, 0x00, 0x00, // TP3 A~D RP3
    0x00, 0x00, 0x00, 0x00, 0x00, // TP4 A~D RP4
    0x00, 0x00, 0x00, 0x00, 0x00, // TP5 A~D RP5
    0x00, 0x00, 0x00, 0x00, 0x00, // TP6 A~D RP6

    0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

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
Adafruit_SSD1675::Adafruit_SSD1675(int width, int height, int8_t SID,
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
Adafruit_SSD1675::Adafruit_SSD1675(int width, int height, int8_t DC, int8_t RST,
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
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_SSD1675::busy_wait(void) {
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
void Adafruit_SSD1675::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true); // black defaults to inverted
  setColorBuffer(0, true); // no secondary buffer, so we'll just reuse index 0

  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1675::update() {
  uint8_t buf[1];

  // display update sequence
  buf[0] = 0xC7;
  EPD_command(SSD1675_DISP_CTRL2, buf, 1);

  EPD_command(SSD1675_MASTER_ACTIVATE);

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
void Adafruit_SSD1675::powerUp() {
  uint8_t buf[5];

  hardwareReset();
  delay(100);
  busy_wait();

  // soft reset
  EPD_command(SSD1675_SW_RESET);
  busy_wait();

  // set analog block control
  buf[0] = 0x54;
  EPD_command(SSD1675_SET_ANALOGBLOCK, buf, 1);

  // set digital block control
  buf[0] = 0x3B;
  EPD_command(SSD1675_SET_DIGITALBLOCK, buf, 1);

  // driver output control
  buf[0] = 0xFA; // 250-1
  buf[1] = 0x01;
  buf[2] = 0x00;
  EPD_command(SSD1675_DRIVER_CONTROL, buf, 3);

  // Data entry sequence
  buf[0] = 0x03;
  EPD_command(SSD1675_DATA_MODE, buf, 1);

  // Set ram X start/end postion
  buf[0] = 0x00;
  buf[1] = 0x0F; // (15+1) * 8 = 128
  EPD_command(SSD1675_SET_RAMXPOS, buf, 2);

  // Set ram Y start/end postion
  buf[0] = 0x00; // 0xF9-->(249+1)=250
  buf[1] = 0x00;
  buf[2] = 0xF9;
  buf[3] = 0x00;
  EPD_command(SSD1675_SET_RAMYPOS, buf, 4);

  // border color
  buf[0] = 0x03;
  EPD_command(SSD1675_WRITE_BORDER, buf, 1);

  // Vcom Voltage
  buf[0] = 0x70;
  EPD_command(SSD1675_WRITE_VCOM, buf, 1);

  // Set gate voltage
  buf[0] = LUT_DATA[70];
  EPD_command(SSD1675_GATE_VOLTAGE, buf, 1);

  // Set source voltage
  buf[0] = LUT_DATA[71];
  buf[1] = LUT_DATA[72];
  buf[2] = LUT_DATA[73];
  EPD_command(SSD1675_SOURCE_VOLTAGE, buf, 3);

  // Set dummy line period
  buf[0] = LUT_DATA[74];
  EPD_command(SSD1675_WRITE_DUMMY, buf, 1);

  // Set gate line width
  buf[0] = LUT_DATA[75];
  EPD_command(SSD1675_WRITE_GATELINE, buf, 1);

  EPD_command(SSD1675_WRITE_LUT, LUT_DATA, 70);

  // set RAM x address count to 0;
  buf[0] = 0;
  EPD_command(SSD1675_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count to 0X127;
  buf[0] = 0xF9;
  buf[1] = 0;
  EPD_command(SSD1675_SET_RAMYCOUNT, buf, 2);

  busy_wait();
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_SSD1675::powerDown() {
  uint8_t buf[1];
  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    // deep sleep
    buf[0] = 0x01;
    EPD_command(SSD1675_DEEP_SLEEP, buf, 1);
    delay(100);
  } else {
    EPD_command(SSD1675_SW_RESET);
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
uint8_t Adafruit_SSD1675::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(SSD1675_WRITE_RAM1, false);
  }
  if (index == 1) {
    return EPD_command(SSD1675_WRITE_RAM2, false);
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
void Adafruit_SSD1675::setRAMAddress(uint16_t x, uint16_t y) {
  uint8_t buf[2];

  // Set RAM X address counter
  buf[0] = 0;
  EPD_command(SSD1675_SET_RAMXCOUNT, buf, 1);

  // Set RAM Y address counter
  buf[0] = 0xF9;
  buf[1] = 0x00;
  EPD_command(SSD1675_SET_RAMYCOUNT, buf, 2);
}
